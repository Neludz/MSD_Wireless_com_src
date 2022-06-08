#ifndef MODBUS_X_H_INCLUDED
#define MODBUS_X_H_INCLUDED

#include "modbus_reg.h"
#include "modbus_hard.h"
#include "main.h"

//-----------------------------------------------------------------------
// define
//-----------------------------------------------------------------------
#define LIMIT_REG	//check limit
#define EEPROM_REG	//use eeprom

#define OFFSET_FOR_ROUTER_DATA          100

//-----------------------------------------------------------------------
// Modbus registers X macros
//-----------------------------------------------------------------------
#define REG_END_REGISTER                Reg_End
/*
#define READ_R			(0)
#define WRITE_R			(0x01)	// 0 bit
#define EESAVE_R		(0x02)	// 1 bit
#define LIM_SIGN		(0x04)	// 2 bit for limit     	  <--|
#define LIM_UNSIGN		(0x08)  // 3 bit for limit	  <--|----------------
#define LIM_MASK	    (0x0C)	// 2 and 3 bit for limit  <--|	              |
*///										      |
//	Number  Name for enum		     Default   Min    Max   	__________Permission_______
//		    			     Value    Level    Level   |  R/W     EEPROM    LIMIT  |
//		    				              or Mask  | 			   |
#define MAIN_BUF_TABLE\
    X_BUF(0,	Reg_Start,		        0,	0, 	0,      READ_R)\
    X_BUF(1,	Reg_T1_Value,			0,	0, 	0,	READ_R)\
    X_BUF(2,	Reg_T2_Value,			0,	0, 	0,	READ_R)\
    X_BUF(3,	Reg_T3_Value,			0,	0, 	0,	READ_R)\
    X_BUF(4,	Reg_T4_Value,			0,	0, 	0,	READ_R)\
    X_BUF(5,	Reg_T5_Value,			0,	0, 	0,	READ_R)\
    X_BUF(6,	Reg_T6_Value,			0,	0, 	0,	READ_R)\
    X_BUF(7,	Reg_T7_Value,			0,	0, 	0,	READ_R)\
    X_BUF(8,	Reg_T8_Value,			0,	0, 	0,	READ_R)\
    X_BUF(9,	Reg_T9_Value,			0,	0, 	0,	READ_R)\
    X_BUF(10,	Reg_T10_Value,			0,	0, 	0,	READ_R)\
    X_BUF(11,	Reg_T11_Value,			0,	0, 	0,	READ_R)\
    X_BUF(12,	Reg_T12_Value,			0,	0, 	0,	READ_R)\
    X_BUF(13,	Reg_T13_Value,			0,	0, 	0,	READ_R)\
    X_BUF(14,	Reg_T14_Value,			0,	0, 	0,	READ_R)\
    X_BUF(15,	Reg_T15_Value,			0,	0, 	0,	READ_R)\
    X_BUF(16,	Reg_T16_Value,			0,	0, 	0,	READ_R)\
    X_BUF(18,	Reg_DI_Status,			0,	0, 	0,	READ_R)\
    X_BUF(19,	Reg_DO_Status,			0,	0, 	0,	READ_R)\
    X_BUF(20,	Reg_Zigbee_Connect_Status,      0,  	0, 	0,      READ_R)\
    X_BUF(26,	Reg_DO_On,			0,	0, 	0,	WRITE_R)\
    X_BUF(27,	Reg_DO_Off,			0,	0, 	0,	WRITE_R)\
    X_BUF(29,	Reg_Send_Command,		0,      0, 	0,      WRITE_R)\
    X_BUF(30,	Reg_DO_1_Mode,		        DO_MODE_OFF,\
                                                        DO_MODE_OFF,\
                                                                DO_MODE_ALARM,\
                                                                        WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(31,	Reg_DO_2_Mode,		        DO_MODE_OFF,\
                                                        DO_MODE_OFF,\
                                                                DO_MODE_ALARM,\
                                                                        WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(32,	Reg_DO_1_Alarm_Mask,            DO_ALARM_BIT_DEFAULT,\
                                                        0,      0,      WRITE_R | EESAVE_R)\
    X_BUF(33,	Reg_DO_2_Alarm_Mask,            DO_ALARM_BIT_DEFAULT,\
                                                        0,      0,      WRITE_R | EESAVE_R)\
    X_BUF(34,	Reg_DO_Alarm_Temperature,       70,     0,	0,      WRITE_R | EESAVE_R)\
    X_BUF(35,	Reg_DO_Warning_Temperature,     60,     0,	0,      WRITE_R | EESAVE_R)\
    X_BUF(37,	Reg_Rs485_Baud_Rate,		BAUD_DEFAULT_MB,\
                                                    	0x00, 	BAUD_MAX_MB,\
                                                                        WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(38,	Reg_Rs485_Parity_Stop_Bits,	PARITY_DEFAULT_MB,\
                                                    	0x00, 	PARITY_MAX_MB,\
                                                                        WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(39,	Reg_Rs485_Address,		1,      01, 	250,    WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(40,	Reg_Zigbee_Set_Mode,            ZI_SET_MODE_BIT_DEFAULT,\
                                                        0,  	0,      WRITE_R | EESAVE_R)\
    X_BUF(41,	Reg_Zigbee_Set_Channel,		ZI_SET_CHANNEL_DEFAULT,\
                                                        11,	26,     WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(43,	Reg_Zi_Max_Children,		ZI_DEFAULT_CHILDREN_MB,\
                                                        0,	ZI_MAX_CHILDREN_MB,\
                                                                        WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(44,	Reg_Zi_Sensor_Collection_Mask,  0,      0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(45,	Reg_Zi_Router_Collection_Mask_1,0,      0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(46,	Reg_Zi_Router_Collection_Mask_2,0,      0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(50,	Reg_Zi_Sensor_Code_Mask,        0x1FF,  0,      0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(51,	Reg_Zi_Router_Code_Mask_1,      0x3,    0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(52,	Reg_Zi_Router_Code_Mask_2,      0,      0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(60,	Reg_Zigbee_FICR_Addr_0,		0,      0,	0,      READ_R)\
    X_BUF(61,	Reg_Zigbee_FICR_Addr_1,         0,      0,	0,      READ_R)\
    X_BUF(62,	Reg_Zigbee_FICR_Addr_2,         0,      0,	0,      READ_R)\
    X_BUF(63,	Reg_Zigbee_FICR_Addr_3,         0,      0,	0,      READ_R)\
    X_BUF(64,	Reg_Zigbee_FICR_Addr_4,         0,      0,	0,      READ_R)\
    X_BUF(65,	Reg_Zigbee_FICR_Addr_5,         0,      0,	0,      READ_R)\
    X_BUF(66,	Reg_Zigbee_FICR_Addr_6,         0,      0,	0,      READ_R)\
    X_BUF(67,	Reg_Zigbee_FICR_Addr_7,         0,      0,	0,      READ_R)\
    X_BUF(70,	Reg_Zigbee_Addr_0_N_Panel,      0,      0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(71,	Reg_Zigbee_Addr_1_N_Sens_Point, 100,    0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(72,	Reg_Zigbee_Addr_2,              0xF4,   0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(73,	Reg_Zigbee_Addr_3,              0xCE,   0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(74,	Reg_Zigbee_Addr_4,              0x47,   0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(75,	Reg_Zigbee_Addr_5,              0xCA,   0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(76,	Reg_Zigbee_Addr_6,              0x36,   0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(77,	Reg_Zigbee_Addr_7,              0x5B,   0,	0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(80,	Reg_Zigbee_Code_0,              0x0102, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(81,	Reg_Zigbee_Code_1,              0x0304, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(82,	Reg_Zigbee_Code_2,              0x0506, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(83,	Reg_Zigbee_Code_3,              0x0708, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(84,	Reg_Zigbee_Code_4,              0x090A, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(85,	Reg_Zigbee_Code_5,              0x0B0C, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(86,	Reg_Zigbee_Code_6,              0x0D0E, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(87,	Reg_Zigbee_Code_7,              0x0F11, 0,	0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(100,	Reg_Router_1_Status,            0,	0,	0, 	READ_R)\
    X_BUF(101,	Reg_Router_1_Data,              0,	0,	0, 	READ_R)\
    X_BUF(3200,	Reg_Router_32_Status,           0,	0,	0, 	READ_R)\
    X_BUF(3201,	Reg_Router_32_Data,             0,	0,	0, 	READ_R)\
    X_BUF(3499,	Reg_End,			0,	0,	0,	READ_R)\


#endif /* MODBUS_X_H_INCLUDED */
