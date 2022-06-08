#ifndef MODBUS_HARD_H_INCLUDED
#define MODBUS_HARD_H_INCLUDED

#include "modbus.h"

//-----------------------------------------------------------------------
// define
//-----------------------------------------------------------------------
#define MB_STACKSIZE                    2048
#define MB_PRIORITY                     -2

#define MB_RX_BUF_RS485_SIZE     		MB_FRAME_MAX
#define MB_RX_DELAY_US     		        10000

#define DELAY_AFTER_EEPROM_WRITE_MS     1500

#define PARITY_MAX_MB       3
#define PARITY_DEFAULT_MB   0

enum {
    NO_PARITY_1_STOP	= 0x00,
    NO_PARITY_2_STOP	= 0x01,
    EVEN_PARITY_1_STOP	= 0x02,
    ODD_PARITY_1_STOP	= 0x03,
};

#define BAUD_NUMBER         4
#define BAUD_MAX_MB         3
#define BAUD_DEFAULT_MB     1       

#define	 BAUD_9600          9600
#define	 BAUD_19200         19200
#define	 BAUD_57600         57600
#define	 BAUD_115200        115200

#define	 RS_485_BAUD_LIST   {BAUD_9600, BAUD_19200, BAUD_57600, BAUD_115200}

//-----------------------------------------------------------------------
// type
//-----------------------------------------------------------------------

typedef struct {					
	uint32_t        kernel_data;
    mb_struct       MB_data;
} mb_queue_struct;

//-----------------------------------------------------------------------
// prototype
//-----------------------------------------------------------------------

void mh_Write_Eeprom (void *mbb);
void mh_USB_Transmit_Start (void *mbb);
void mh_RS485_Transmit_Start (void *mbb);
void mh_usb_init (void);
void mh_rs485_init (void);
void mh_Factory (void);
void mh_Buf_Init (void);
void mh_Modbus_Init(void);

#endif /* MODBUS_HARD_H_INCLUDED */
