#ifndef __RX8025T_H
#define __RX8025T_H


#define TURE_RTC                1
#define FALSE_RTC               0
#define AckError_RTC            0x55
#define OutOfRang_RTC           0xaa
#define OutOfAddr_RTC           0xbb

#define RTC8025T_SLV_ADR        0x64	        	    
#define RTC8025T_CON_ADR        0xe0	       
#define RTC8025T_ALARMD_ADR     0xb0             
#define ADJ_ADR                 0x70				      

#define RTC_24H_EN              0x20   
#define RTC_ALARMD_EN           0x40      


typedef struct {
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char sec;
    unsigned char date;
}RTCTimeFormat;

extern void RX8025T_Init(void);
extern void RX8025T_Close(void);
extern void Write_RX8025T_AndRTC_By_UTC8(void);
extern void Read_EEprom_Or_RX8025T_To_Set_RTC(void);
#endif


