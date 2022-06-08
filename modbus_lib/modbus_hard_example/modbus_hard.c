
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>
#include <sys/ring_buffer.h>
#include <drivers/gpio.h>
#include <drivers/usb/usb_dc.h>

#include <usb/usb_device.h>
#include <logging/log.h>

#include "modbus_hard.h"
#include "modbus_reg.h"
#include "modbus.h"
#include "modbus_x_macros.h"

#include "user_mem.h"

LOG_MODULE_REGISTER(modbus_hard, LOG_LEVEL_DBG);  

//-----------------------------------------------------------------------
// Variable
//-----------------------------------------------------------------------

uint16_t MBbuf_main[NUM_BUF]={VERSION};

extern const t_default_state default_state[];

const struct device *dev_usb_acm;
const struct device *dev_rs485;

mb_queue_struct MB_USB;
mb_queue_struct MB_RS485;

uint8_t USB_MB_Buf[MB_FRAME_MAX];
uint8_t RS485_MB_Buf[MB_FRAME_MAX];

uint8_t RS485_Buf[MB_RX_BUF_RS485_SIZE];

const uint32_t Baud_rate[BAUD_NUMBER] = RS_485_BAUD_LIST;

#define TEST_NODE	DT_NODELABEL(uart1)
#if !DT_NODE_HAS_STATUS(TEST_NODE, okay)
#error "Unsupported board: test devicetree alias is not defined"
#endif

static struct gpio_dt_spec rs_rede = GPIO_DT_SPEC_GET(DT_ALIAS(rs485rede), gpios);
struct k_work work;


//-----------------------------------------------------------------------
// Define task & queue
//-----------------------------------------------------------------------
static struct k_thread mh_MB_thread_data;
static k_tid_t mh_MB_tid;

struct k_queue mh_queue;
K_THREAD_STACK_DEFINE(mh_MB_stack_area, MB_STACKSIZE);

//-----------------------------------------------------------------------
// Task function
//-----------------------------------------------------------------------
void mh_task_Modbus (void *arg1, void *arg2, void *arg3)
{
  	int ret;
  	mb_queue_struct *st_mb;
  	uart_irq_rx_enable(dev_usb_acm);
  	ret=uart_rx_enable(dev_rs485, RS485_Buf, MB_RX_BUF_RS485_SIZE, MB_RX_DELAY_US);
  	LOG_INF("uart_rx_enable: %d", ret);
    while (1) 
    {	
      	st_mb = k_queue_get(&mh_queue, K_FOREVER);
      	MBparsing((mb_struct*) &(st_mb->MB_data));
	}
}

//-----------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------
// Callback for rs485
void uart_rs485_callback (const struct device*dev, struct uart_event*evt, void*user_data)
{
  	switch (evt->type)
  	{
    case UART_RX_RDY:
    	//LOG_INF("RX: len - %d;" , evt->data.rx.len);   
    	uart_rx_disable(dev_rs485);
      	if ((evt->data.rx.len>=MB_FRAME_MIN) && (All_Idle_Check(&MB_RS485.MB_data)==REG_OK)) 
      	{
        	MB_RS485.MB_data.mb_index=(evt->data.rx.len);
        	MB_RS485.MB_data.mb_state=STATE_PARS;
			memcpy (MB_RS485.MB_data.p_mb_buff, evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len);
        	k_queue_append(&mh_queue, &MB_RS485);
      	}
   		break;

    case UART_RX_DISABLED:
    	k_work_submit(&work); 
    	break;

    case UART_RX_STOPPED:
    	break;

  	case UART_RX_BUF_REQUEST:
    	break;

  	case UART_RX_BUF_RELEASED:
    	break;

  	case UART_TX_DONE:
      	gpio_pin_set_dt(&rs_rede,0);
      	MB_RS485.MB_data.mb_state=STATE_IDLE;
    	break;

  	case UART_TX_ABORTED:
      	gpio_pin_set_dt(&rs_rede,0);
      	MB_RS485.MB_data.mb_state=STATE_IDLE;
    	break;

  	default: 
    	break;
  	}
}

// Callback for usb com
void uart_usb_interrupt_handler (const struct device *dev, void*user_data)
{
	uint8_t buf[64];
	int recv_len;
  	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) 
  	{
		if (uart_irq_rx_ready(dev)) 
    	{
      		recv_len = uart_fifo_read(dev, buf, sizeof(buf));  
      		if(All_Idle_Check(&MB_USB.MB_data)==REG_OK) 
      		{
			    if (recv_len>=MB_FRAME_MIN) 
          		{
            		MB_USB.MB_data.mb_index=(recv_len);
            		MB_USB.MB_data.mb_state=STATE_PARS;
				    memcpy (MB_USB.MB_data.p_mb_buff, buf, recv_len);	
            		k_queue_append(&mh_queue, &MB_USB);
			    }
      		}    
		}
		if (uart_irq_tx_ready(dev)&&(MB_USB.MB_data.mb_state==STATE_SEND)) 
    	{
			int wrote_len;
			wrote_len = uart_fifo_fill(dev, MB_USB.MB_data.p_mb_buff, MB_USB.MB_data.response_size);
      		uart_irq_tx_disable(dev);
      		MB_USB.MB_data.mb_state=STATE_IDLE;
		}
	}
}

// Init modbus 
void mh_Modbus_Init(void)
{
	mh_usb_init();
    mh_rs485_init();

	k_queue_init(&mh_queue);

	mh_MB_tid = k_thread_create(&mh_MB_thread_data,
				    mh_MB_stack_area,
				    K_THREAD_STACK_SIZEOF(mh_MB_stack_area),
				    mh_task_Modbus,
				    NULL, NULL, NULL,
				    MB_PRIORITY,
				    0, K_MSEC(1500));
	//k_thread_name_set(&zboss_thread_data, "modbus");
}

// Enable rx 
void mh_work_handler(struct k_work *item)
{
  	uart_rx_enable(dev_rs485, RS485_Buf, MB_RX_BUF_RS485_SIZE, MB_RX_DELAY_US);
}


void mh_usb_init (void)
{
  	dev_usb_acm = device_get_binding("CDC_ACM_0");
	if (!dev_usb_acm) 
	{
		LOG_DBG("CDC_ACM_0 device not found");
		return;
	} 
    MB_USB.MB_data.p_write = MBbuf_main;
    MB_USB.MB_data.p_read = MBbuf_main;
    MB_USB.MB_data.reg_read_last=NUM_BUF-1;
    MB_USB.MB_data.reg_write_last=NUM_BUF-1;
    MB_USB.MB_data.eep_state=EEP_FREE;
    MB_USB.MB_data.er_frame_bad=EV_NOEVENT;
    MB_USB.MB_data.slave_address=MB_ANY_ADDRESS;	//0==any address
    MB_USB.MB_data.mb_state=STATE_IDLE;
    MB_USB.MB_data.p_mb_buff=&USB_MB_Buf[0];
    MB_USB.MB_data.f_save = mh_Write_Eeprom;
    MB_USB.MB_data.f_start_trans = mh_USB_Transmit_Start;

    uart_irq_callback_set(dev_usb_acm, uart_usb_interrupt_handler);   
}

void mh_rs485_init (void)
{
 	struct uart_config uart_rs485_cfg;
 	int ret;

  	dev_rs485 = device_get_binding(DT_LABEL(DT_ALIAS(uart_rs485)));
	if (!dev_rs485) 
    {
		LOG_INF("UART1 device not found");
		return;
	} 

 	 ret = uart_config_get(dev_rs485, &uart_rs485_cfg);
  	LOG_INF("uart_config_get == %d", ret);
    if (MBbuf_main[Reg_Rs485_Baud_Rate]<BAUD_NUMBER)
    {
    	uart_rs485_cfg.baudrate = Baud_rate[MBbuf_main[Reg_Rs485_Baud_Rate]];
    }

    switch (MBbuf_main[Reg_Rs485_Parity_Stop_Bits])
    {
    case NO_PARITY_1_STOP:
      	uart_rs485_cfg.data_bits = UART_CFG_DATA_BITS_8;
      	uart_rs485_cfg.parity = UART_CFG_PARITY_NONE;
      	uart_rs485_cfg.stop_bits = UART_CFG_STOP_BITS_1;
    	break;

    case NO_PARITY_2_STOP:
      	uart_rs485_cfg.data_bits = UART_CFG_DATA_BITS_8;
      	uart_rs485_cfg.parity = UART_CFG_PARITY_NONE;
      	uart_rs485_cfg.stop_bits = UART_CFG_STOP_BITS_2;
    	break;

    case EVEN_PARITY_1_STOP:
      	uart_rs485_cfg.data_bits = UART_CFG_DATA_BITS_8;
      	uart_rs485_cfg.parity = UART_CFG_PARITY_EVEN;
      	uart_rs485_cfg.stop_bits = UART_CFG_STOP_BITS_1;
      	break;

    case ODD_PARITY_1_STOP:
      	uart_rs485_cfg.data_bits = UART_CFG_DATA_BITS_8;
      	uart_rs485_cfg.parity = UART_CFG_PARITY_EVEN;
      	uart_rs485_cfg.stop_bits = UART_CFG_STOP_BITS_1;
      	break;

    default:
      	uart_rs485_cfg.data_bits = UART_CFG_DATA_BITS_8;
      	uart_rs485_cfg.parity = UART_CFG_PARITY_NONE;
      	uart_rs485_cfg.stop_bits = UART_CFG_STOP_BITS_1;
      	break;
    }

    ret = uart_configure(dev_rs485, &uart_rs485_cfg);
    LOG_INF("uart_configure == %d", ret);
    MB_RS485.MB_data.p_write = MBbuf_main;
    MB_RS485.MB_data.p_read = MBbuf_main;
    MB_RS485.MB_data.reg_read_last=NUM_BUF-1;
    MB_RS485.MB_data.reg_write_last=NUM_BUF-1;
    MB_RS485.MB_data.eep_state=EEP_FREE;
    MB_RS485.MB_data.er_frame_bad=EV_NOEVENT;
    MB_RS485.MB_data.slave_address=MBbuf_main[Reg_Rs485_Address];	//0==any address
    MB_RS485.MB_data.mb_state=STATE_IDLE;
    MB_RS485.MB_data.p_mb_buff=&RS485_MB_Buf[0];
    MB_RS485.MB_data.f_save = mh_Write_Eeprom;
    MB_RS485.MB_data.f_start_trans = mh_RS485_Transmit_Start;
 
    ret = gpio_pin_configure_dt(&rs_rede, 
                    (rs_rede.dt_flags & GPIO_ACTIVE_LOW) ? GPIO_OUTPUT_HIGH : GPIO_OUTPUT_LOW);

	ret = uart_callback_set(dev_rs485, uart_rs485_callback, NULL);
    LOG_INF("uart_callback_set == %d", ret);
    k_work_init(&work, mh_work_handler);
}

void mh_Write_Eeprom (void *mbb)
{
	uint32_t err;
	mb_struct *st_mb = (void*) mbb;
	for (int32_t i = 0; i < (st_mb->eep_indx); i++)
	{
		if(EESave_Check(i+(st_mb->eep_start_save))==REG_OK)
		{
      		err = user_mem_update_reg16 (((st_mb->eep_start_save)+i), (uint16_t*) &(st_mb->p_write[i+(st_mb->eep_start_save)]));
		}
	}
	k_sleep(K_MSEC(DELAY_AFTER_EEPROM_WRITE_MS));
	st_mb->eep_state = EEP_FREE;
}

void mh_USB_Transmit_Start (void *mbb)
{
  	uart_irq_tx_enable(dev_usb_acm);
}

void mh_RS485_Transmit_Start (void *mbb)
{
  	gpio_pin_set_dt(&rs_rede, 1);
  	uart_tx(dev_rs485, MB_RS485.MB_data.p_mb_buff, MB_RS485.MB_data.response_size, 1000);
}

void mh_Factory (void)
{
 	int err;
	for (int32_t i=0; i< NUM_BUF; i++)
	{
		if (EESave_Check(i)==REG_OK)
		{
		  	MBbuf_main[i] = default_state[i].Default_Value;
      		err = user_mem_update_reg16 (i,  &MBbuf_main[i]);
		  	LOG_INF("mh_Factory, id= %d, err =%d \n", i, err);
		}
	}
}

void mh_Buf_Init (void)
{
	int err;

	for (int32_t i=0; i< NUM_BUF; i++)
	{
		if(EESave_Check(i)==REG_OK)
		{
      		err = user_mem_read_reg16 (i, &MBbuf_main[i]);
        	if(err==RET_MEM_NOT_FOUND)
			{
			   	MBbuf_main[i] = default_state[i].Default_Value;
       	 		err = user_mem_update_reg16 (i,  &MBbuf_main[i]);	
			}		
			if(Limit_Check(i, MBbuf_main[i])==REG_ERR)
			{
			   	MBbuf_main[i]=default_state[i].Default_Value;
			    err = user_mem_update_reg16 (i,  &MBbuf_main[i]);
			}
		}       
	}
}
