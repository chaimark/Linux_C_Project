#ifndef __Define_H__
#define __Define_H__
#include "main.h"

extern const unsigned char SOFT_VERSION; // 12:V1.2
#define SHOW_EC20_PROGRAMMR_HY "HuiYun EC20 Programme  \r\n"

#define SHOW_UP_MODE_EC20 "UP_EC20  ON\r\n"
#define SHOW_UP_MODE_NET "UP_NET  ON\r\n"
#define SHOW_UP_MODE_MBUS "UP_USB_MBUS  ON\r\n"

#define SHOW_DOWN_MODE_WIR_ON "DOWN_WIR  ON\r\n"
#define SHOW_DOWN_MODE_MBUS "DOWN_MBUS  ON\r\n"
#define SHOW_DOWN_MODE_RS4851 "DOWN_RS4851  ON\r\n"
#define SHOW_DOWN_MODE_RS4852 "DOWN_RS4852  ON\r\n"
#define SHOW_DEFINE_MODE_SETUSB "DOWN_USB_SET ON\r\n"

// FLASH
#define FLASH_SSN_HIGH FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_9)  // PC9
#define FLASH_SSN_LOW FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_9)

#define FLASH_RST_HIGH FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_8)  // PC8
#define FLASH_RST_LOW FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_8)

#define FLASH_SCLK_HIGH FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_7)  // PC7
#define FLASH_SCLK_LOW FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_7)

#define FLASH_MOSI_HIGH FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_6)  // PC6
#define FLASH_MOSI_LOW FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_6)

#define FLASH_SDA_VALUE (FL_GPIO_ReadInputPort(GPIOC) & FL_GPIO_PIN_5)  // PC5

// EC20
#define EC20_PWR_KEY_L FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_4) // PA4
#define EC20_PWR_KEY_H FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_4)

#define EC20_STATUS_VALUE (FL_GPIO_ReadInputPort(GPIOA) & FL_GPIO_PIN_1) // PA1

#define EC20_RST_ON() FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_7) // PB7 (time > 300ms)
#define EC20_RST_OFF() FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_7)

#define EC20_AP_READY_VALUE (FL_GPIO_ReadInputPort(GPIOB) & FL_GPIO_PIN_6) // PB6

#define EC20_WAKEUP_IN_L FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_15)
#define EC20_WAKEUP_IN_H FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_15) // PA15

// LED显示
#define Heart_LED_H FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_0) // PC0
#define Heart_LED_L FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_0)

#define EC20_LED_H FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_5) // PB5
#define EC20_LED_L FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_5)

#define NET_LED_H FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_11) // PC11
#define NET_LED_L FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_11)

#define CH340_To_LED_H FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_12) // PC12
#define CH340_To_LED_L FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_12)

#define WIR_LED_H FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_11) // PB11
#define WIR_LED_L FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_11)

#define RS485_LED_H FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_10) // PB10
#define RS485_LED_L FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_10)

#define ERR_LED_H FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_10) // PC10
#define ERR_LED_L FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_10)

// 电源
#define VBAT_29302_CTL_ON FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_9) // PA9
#define VBAT_29302_CTL_OFF FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_9)

// 5v 电源控制 AO3401A
#define V50_PWR_CTL_ON FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_0) // PA0
#define V50_PWR_CTL_OFF FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_0)

// 3.3v 电源控制 AO3401A
#define V33_PWR_CTL_ON FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_6) // PA6
#define V33_PWR_CTL_OFF FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_6)

// NET 复位脚
#define NET_RST_ON FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_5) // PA5
#define NET_RST_OFF FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_5)

#define EEprom_SDA_HIGH FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_9) // PD9
#define EEprom_SDA_LOW FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_9)
#define EEprom_SDA_VALUE (FL_GPIO_ReadInputPort(GPIOB) & FL_GPIO_PIN_9)

#define EEprom_SCL_HIGH FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_8) // PD8
#define EEprom_SCL_LOW FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_8) 

#define RX8025T_SDA_HIGH FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_6) // PD6
#define RX8025T_SDA_LOW FL_GPIO_ResetOutputPin(GPIOD, FL_GPIO_PIN_6)
#define RX8025T_SDA_VALUE (FL_GPIO_ReadInputPort(GPIOD) & FL_GPIO_PIN_6)

#define RX8025T_SCL_HIGH FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_5) // PD5
#define RX8025T_SCL_LOW FL_GPIO_ResetOutputPin(GPIOD, FL_GPIO_PIN_5)

// 模拟开关
#define Select_ON_EC20_To_UART0 FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_3) // PD3    UART0_TO_EC20
#define Select_OFF_EC20_To_UART0 FL_GPIO_ResetOutputPin(GPIOD, FL_GPIO_PIN_3)

#define Select_ON_NET_To_UART0 FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_8) // PA8    UART0_TO_NET
#define Select_OFF_NET_To_UART0 FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_8)

#define Select_ON_MBUS_To_UART1 FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_1) // PC1    UART1_TO_MBUS
#define Select_OFF_MBUS_To_UART1 FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_1)

#define Select_ON_WIR_To_UART1 FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_12) // PA12    UART1_TO_LORA
#define Select_OFF_WIR_To_UART1 FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_12)

#define Select_ON_RS4852_To_LPUART0 FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_10) // PA10  LPUART0_TO_RS485_2
#define Select_OFF_RS4852_To_LPUART0 FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_10)

#define Select_ON_CH340_To_LPUART1 FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_4) // PB4    LPUART1_TO_CH340
#define Select_OFF_CH340_To_LPUART1 FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_4)

#define Select_ON_RS4851_To_UART4 FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_12) // PB12   UART4_TO_RS485_1
#define Select_OFF_RS4851_To_UART4 FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_12)

#define Select_ON_CH340_To_MBUS FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_4) // PC4    CH340_TO_MUSB
#define Select_OFF_CH340_To_MBUS FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_4)

#define Select_ON_CH340_To_RS4851 FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_0) // PB0    RS4851_TO_CH340
#define Select_OFF_CH340_To_RS4851 FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_0)

#define Select_ON_CH340_To_RS4852 FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_11) // PA11  RS4852_TO_CH340
#define Select_OFF_CH340_To_RS4852 FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_11)

#define MBUS_OUT_ON FL_GPIO_ResetOutputPin(GPIOD, FL_GPIO_PIN_4) // PD4 MUBS_CTRL
#define MBUS_OUT_OFF FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_4)

#define EEPROM_SYSTIME_ADDRESS 370
#define EEPROM_MBUS_UART1_BAUDRATE_ADDR 400
#define EEPROM_MBUS_UART1_PARITY_ADDR 405
#define EEPROM_RS4851_UART4_BAUDRATE_ADDR 410
#define EEPROM_RS4851_UART4_PARITY_ADDR 415
#define EEPROM_RS4852_LPUART0_BAUDRATE_ADDR 420
#define EEPROM_RS4852_LPUART0_PARITY_ADDR 425
#define EEPROM_COPY_METER_TOTAL_ADDR 490   // 辅表  数量
#define EEPROM_COPY_METER_START_ADDR 500   // 辅北砗彭  仪表号码存储开始地址  每个预留60字节   前4字节为仪表号码  后两字节为仪表协议类型  后一字节为端口号，每个仪表预留60字节   每组1200字节   12组14400字节+500
#define EEPROM_MAIN_METER_TOTAL_ADDR 14990 // 主表  数量
#define EEPROM_MAIN_METER_START_ADDR 15000 // 主表表号  地址15000
#define READINGMETER_DATA_ADDR 20000	   // 表数据存储开始位置 每只表预留100字节，前30字节存储其他信息，后70字节存储抄表数据，2000+200*100=22000位置

extern int UART0_BAND;

extern unsigned char DOWN_Mode_MBUS_ON;
extern unsigned char DOWN_Mode_RS4851_ON;
extern unsigned char DOWN_Mode_RS4852_ON;
extern unsigned int Current_MBUS_MV_Value;
extern unsigned int Current_MBUS_MA_Value;
extern unsigned char Default_Working_Mode;
extern const unsigned char HasRX8025T;


extern unsigned int MBUS_UART1_BaudRate;
extern unsigned char MBUS_UART1_Parity;
extern unsigned int RS4851_UART4_BaudRate;
extern unsigned char RS4851_UART4_Parity;
extern unsigned int RS4852_LPUART0_BaudRate;
extern unsigned char RS4852_LPUART0_Parity;

extern unsigned char UP_Mode_MBUSTOUSB_ON;
extern unsigned char UP_Mode_WIR_ON;
extern unsigned char DOWN_Mode_WIR_ON;
extern unsigned char  DUBG_Mode_WIFI_ON;
extern unsigned char DEFINE_Mode_SETUSB_ON;

extern unsigned char UP_Mode_EC20_ON;
extern unsigned char UP_Mode_NET_ON;

extern void RX8025T_SDA_SET_IN(void);
extern void RX8025T_SDA_SET_OUT(void);
extern void EEprom_SDA_IN(void);
extern void EEprom_SDA_OUT(void);

#endif
