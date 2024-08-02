#include "RX8025T.h"
#include "Define.h"
#include "RTC.h"
#include "4G_Module.h"
#include "main.h"

#define TRUE                1
#define FALSE               0
#define RX8025T_DEV_ADDRESS 0x64 

RTCTimeFormat Real_Time;

unsigned char RTC_RX8025T_READ[7];
unsigned char RTC_RX8025T_WRITE[7];
unsigned char EEprom_System_Time_Receive[7];


void RX8025T_IIC_Start(void) {
    RX8025T_SDA_SET_OUT();
    RX8025T_SDA_HIGH;
    RX8025T_SCL_HIGH;
    FL_DelayUs(7);
    RX8025T_SDA_LOW;
    FL_DelayUs(7);
    RX8025T_SCL_LOW;
    FL_DelayUs(7);
}

void RX8025T_IIC_Stop(void) {
    RX8025T_SCL_LOW;
    RX8025T_SDA_SET_OUT();
    RX8025T_SDA_LOW;
    FL_DelayUs(7);
    RX8025T_SCL_HIGH;
    FL_DelayUs(7);
    RX8025T_SDA_HIGH;
    FL_DelayUs(7);
    RX8025T_SCL_LOW;
}

unsigned char RX8025T_IIC_Read(unsigned char ack) {
    unsigned char i, rc;
    RX8025T_SDA_SET_IN();
    rc = 0;
    for (i = 0; i < 8; i++) {
        RX8025T_SCL_HIGH;
        FL_DelayUs(7);
        if (RX8025T_SDA_VALUE) rc |= 1;
        RX8025T_SCL_LOW;
        FL_DelayUs(7);
        rc <<= 1;
    }
    rc >>= 1;
    RX8025T_SDA_SET_OUT();
    if (ack)  RX8025T_SDA_LOW;
    else RX8025T_SDA_HIGH;
    FL_DelayUs(7);
    RX8025T_SCL_HIGH;
    FL_DelayUs(7);
    RX8025T_SCL_LOW;
    return rc;
}

int RX8025T_IIC_Write(unsigned char ch) {
    int i;
    RX8025T_SDA_SET_OUT();
    for (i = 0; i < 8; i++) {
        if (ch & 0x80) RX8025T_SDA_HIGH;
        else RX8025T_SDA_LOW;
        FL_DelayUs(7);
        RX8025T_SCL_HIGH;
        FL_DelayUs(7);
        RX8025T_SCL_LOW;
        FL_DelayUs(7);
        ch <<= 1;
    }
    RX8025T_SDA_SET_IN();
    RX8025T_SCL_HIGH;
    FL_DelayUs(7);
    if (RX8025T_SDA_VALUE) {
        RX8025T_SCL_LOW;
        FL_DelayUs(7);
        return 1;
    }
    RX8025T_SCL_LOW;
    FL_DelayUs(7);
    return 0;
}

unsigned char RX8025T_IIC_WriteByte(unsigned char addr, unsigned char * buf, unsigned char len) {
    int i;
    RX8025T_IIC_Start();
    if (RX8025T_IIC_Write(RX8025T_DEV_ADDRESS)) goto OP_FAIL;
    if (RX8025T_IIC_Write(addr)) goto OP_FAIL;
    for (i = 0; i < len; i++) {
        if (RX8025T_IIC_Write(buf[i])) goto OP_FAIL;
    }
    RX8025T_IIC_Stop();
    return 1;
OP_FAIL:
    RX8025T_IIC_Stop();
    return 0;
}

unsigned char RX8025T_IIC_ReadByte(unsigned char addr, unsigned char * buf, unsigned char len) {
    unsigned char i;
    RX8025T_IIC_Start();
    if (RX8025T_IIC_Write(RX8025T_DEV_ADDRESS)) goto OP_FAIL;
    if (RX8025T_IIC_Write(addr)) goto OP_FAIL;
    RX8025T_IIC_Start();
    if (RX8025T_IIC_Write(RX8025T_DEV_ADDRESS | 0x01)) goto OP_FAIL;
    for (i = 0; i < len - 1; i++) buf[i] = RX8025T_IIC_Read(1);
    buf[len - 1] = RX8025T_IIC_Read(0);
    RX8025T_IIC_Stop();
    return 1;
OP_FAIL:
    RX8025T_IIC_Stop();
    return 0;
}

void RX8025T_Close(void) {
    int i;
    RX8025T_SDA_SET_IN();
    FL_DelayUs(4);
    for (i = 0; i < 10; i++)  RX8025T_SCL_HIGH;
    FL_DelayUs(4);
    RX8025T_SDA_SET_OUT();
    FL_DelayUs(4);
    RX8025T_SDA_HIGH;
}

void RX8025T_Read_RTC(void) {
    RX8025T_IIC_ReadByte(0, RTC_RX8025T_READ, 7);
    RX8025T_Close();
}

void RX8025T_Write_RTC(void) {
    RX8025T_IIC_WriteByte(0, RTC_RX8025T_WRITE, 7);
    RX8025T_Close();
}

void RX8025T_Init(void) {
    unsigned char data[2];
    data[0] = RTC_24H_EN;			/*- 24 hour bit -*/
    data[1] = 0x20;
    RX8025T_IIC_WriteByte(RTC8025T_CON_ADR, data, 2);
    RX8025T_Read_RTC();
    RX8025T_Close();
}

unsigned char Check_Date_ByHEX(unsigned char H_year1, unsigned char H_year2, unsigned char H_month, unsigned char H_date) {
    unsigned int w_year;
    unsigned char w_month, w_date;
    char Month_buf[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    w_year = (H_year1 / 0x10) * 1000 + (H_year1 % 0x10) * 100 + (H_year2 / 0x10) * 10 + (H_year2 % 0x10);
    w_month = (H_month / 0x10) * 10 + (H_month % 0x10);
    w_date = (H_date / 0x10) * 10 + (H_date % 0x10);
    if (w_month == 2)(((w_year % 4 == 0) && (w_year % 100 != 0)) || (w_year % 400 == 0)) ? Month_buf[1] += 1 : Month_buf[1];
    if (w_month > 12) return 0;
    if (w_date > 31) return 0;
    else if (w_month<1 || w_date>Month_buf[w_month - 1] || w_date < 1) return 0;
    return 1;
}

unsigned char Check_Time_ByHEX(unsigned char H_hour, unsigned char H_min, unsigned char H_second) {
    unsigned char w_hour, w_min, w_second;
    w_hour = (H_hour >> 4) * 10 + (H_hour & 0x0F);
    w_min = (H_min >> 4) * 10 + (H_min & 0x0F);
    w_second = (H_second >> 4) * 10 + (H_second & 0x0F);
    if ((w_hour == 0) || ((w_hour > 0) && (w_hour < 24))) {
        if ((w_min == 0) || ((w_min > 0) && (w_min < 60))) {
            if ((w_second == 0) || ((w_second > 0) && (w_second < 60))) {
                return 1;
            } else return 0;
        } else return 0;
    } else return 0;
}

void SystemDateTimeUTC_to_BJTime(void) {
    unsigned int w_year;
    unsigned char w_month, w_date;
    unsigned char w_hour;
    unsigned int lastday = 0;//last day of this month
    //  unsigned int lastlastday = 0;//last day of last month
    w_year = (0x20 / 0x10) * 1000 + (0x20 % 0x10) * 100 + (EEprom_System_Time_Receive[0] / 0x10) * 10 + (EEprom_System_Time_Receive[0] % 0x10);
    w_month = (EEprom_System_Time_Receive[1] / 0x10) * 10 + (EEprom_System_Time_Receive[1] % 0x10);
    w_date = (EEprom_System_Time_Receive[2] / 0x10) * 10 + (EEprom_System_Time_Receive[2] % 0x10);
    w_hour = (EEprom_System_Time_Receive[4] / 0x10) * 10 + (EEprom_System_Time_Receive[4] % 0x10);
    w_hour = w_hour + 8;
    if (w_month == 1 || w_month == 3 || w_month == 5 || w_month == 7 || w_month == 8 || w_month == 10 || w_month == 12) {
        lastday = 31;
        if (w_month == 3) {
            //      if((w_year%400 == 0)||(w_year%4 == 0 && w_year%100 != 0))//if this is lunar year
            //        lastlastday = 29;
            //      else
            //        lastlastday = 28;
        } else if (w_month == 8) {
            //      lastlastday = 31;
        }
    } else if (w_month == 4 || w_month == 6 || w_month == 9 || w_month == 11) {
        lastday = 30;
        //    lastlastday = 31;
    } else {
        //    lastlastday = 31;
        if ((w_year % 400 == 0) || (w_year % 4 == 0 && w_year % 100 != 0))
            lastday = 29;
        else
            lastday = 28;
    }

    if (w_hour >= 24) {// if >24, day+1
        w_hour = w_hour - 24;
        w_date += 1;
        if (w_date > lastday) { // next month,  day-lastday of this month
            w_date = w_date - lastday;
            w_month += 1;
            if (w_month > 12) {//next year , month-12
                w_month = w_month - 12;
                w_year += 1;
            }
        }
    }
    EEprom_System_Time_Receive[0] = (((w_year % 100) / 10) << 4) + (w_year % 10);
    EEprom_System_Time_Receive[1] = ((w_month / 10) << 4) + (w_month % 10);
    EEprom_System_Time_Receive[2] = ((w_date / 10) << 4) + (w_date % 10);
    EEprom_System_Time_Receive[4] = ((w_hour / 10) << 4) + (w_hour % 10);
}

unsigned char get_Week_In_Date(unsigned char XX, unsigned char YY, unsigned char ZZ) {
    unsigned int x = 0, y = 0, z = 0;
    unsigned char w = 0;
    x = 2000 + ((XX & 0xF0) >> 4) * 10 + (XX & 0x0F);
    y = ((YY & 0xF0) >> 4) * 10 + (YY & 0x0F);
    z = ((ZZ & 0xF0) >> 4) * 10 + (ZZ & 0x0F);
    if (y == 0x01 || y == 0x02) {
        y += 12;
        x -= 1;
    }
    w = (z + 2 * y + 3 * (y + 1) / 5 + x + x / 4 - x / 100 + x / 400) % 7;
    w++;
    return w;
}

void Init_SysTime_To_EEprom(void) {
    unsigned char i = 0;
    unsigned char SAVE_EEprom_System_Time[7] = {0};
    SAVE_EEprom_System_Time[0] = 0x23;
    SAVE_EEprom_System_Time[1] = 0x08;
    SAVE_EEprom_System_Time[2] = 0x09;
    SAVE_EEprom_System_Time[4] = 0x07;
    SAVE_EEprom_System_Time[5] = 0x50;
    SAVE_EEprom_System_Time[6] = 0x02;
    SAVE_EEprom_System_Time[3] = get_Week_In_Date(SAVE_EEprom_System_Time[0], SAVE_EEprom_System_Time[1], SAVE_EEprom_System_Time[2]);
    for (i = 0;i < 7;i++) {
        EEprom_Byte1Write(EEPROM_SYSTIME_ADDRESS + i, SAVE_EEprom_System_Time[i]);
    }
}

void Write_RX8025T_AndRTC_By_UTC8(void) {
    unsigned char i = 0;
    if ((EC20T_CCLK[0] == 0x22) && (EC20T_CCLK[3] == 0x2F) && (EC20T_CCLK[6] == 0x2F) && (EC20T_CCLK[9] == 0x2C) && (EC20T_CCLK[12] == 0x3A) && (EC20T_CCLK[15] == 0x3A) && (EC20T_CCLK[18] == 0x2B)) {
        EEprom_System_Time_Receive[0] = ((EC20T_CCLK[1] & 0x0F) << 4) + (EC20T_CCLK[2] & 0x0F);
        EEprom_System_Time_Receive[1] = ((EC20T_CCLK[4] & 0x0F) << 4) + (EC20T_CCLK[5] & 0x0F);
        EEprom_System_Time_Receive[2] = ((EC20T_CCLK[7] & 0x0F) << 4) + (EC20T_CCLK[8] & 0x0F);
        EEprom_System_Time_Receive[4] = ((EC20T_CCLK[10] & 0x0F) << 4) + (EC20T_CCLK[11] & 0x0F);
        EEprom_System_Time_Receive[5] = ((EC20T_CCLK[13] & 0x0F) << 4) + (EC20T_CCLK[14] & 0x0F);
        EEprom_System_Time_Receive[6] = ((EC20T_CCLK[16] & 0x0F) << 4) + (EC20T_CCLK[17] & 0x0F);
        EEprom_System_Time_Receive[3] = get_Week_In_Date(EEprom_System_Time_Receive[0], EEprom_System_Time_Receive[1], EEprom_System_Time_Receive[2]);
        if (HasRX8025T != 0) {
            SystemDateTimeUTC_to_BJTime();
            RTC_RX8025T_WRITE[0] = EEprom_System_Time_Receive[6];  //
            RTC_RX8025T_WRITE[1] = EEprom_System_Time_Receive[5];  //
            RTC_RX8025T_WRITE[2] = EEprom_System_Time_Receive[4];  //
            RTC_RX8025T_WRITE[3] = EEprom_System_Time_Receive[3];  //
            RTC_RX8025T_WRITE[4] = EEprom_System_Time_Receive[2];  //
            RTC_RX8025T_WRITE[5] = EEprom_System_Time_Receive[1];  //
            RTC_RX8025T_WRITE[6] = EEprom_System_Time_Receive[0];  //
            RX8025T_Write_RTC();
        }
        RTC_Data.year = EEprom_System_Time_Receive[0];
        RTC_Data.month = EEprom_System_Time_Receive[1];
        RTC_Data.day = EEprom_System_Time_Receive[2];
        RTC_Data.week = EEprom_System_Time_Receive[3];
        RTC_Data.hour = EEprom_System_Time_Receive[4];
        RTC_Data.minute = EEprom_System_Time_Receive[5];
        RTC_Data.second = EEprom_System_Time_Receive[6];
        FL_RTC_ConfigTime(RTC, &RTC_Data);
        for (i = 0;i < 7;i++)
            EEprom_Byte1Write(EEPROM_SYSTIME_ADDRESS + i, EEprom_System_Time_Receive[i]);
        execute_Adjust_Meter_1Second_Count = 1;
    }
}

void Read_EEprom_Or_RX8025T_To_Set_RTC(void) {
    unsigned char i = 0;
    unsigned char SAVE_EEprom_System_Time[7] = {0};
    if (HasRX8025T != 0) {
        RX8025T_Read_RTC();
        SAVE_EEprom_System_Time[0] = RTC_RX8025T_READ[6];
        SAVE_EEprom_System_Time[1] = RTC_RX8025T_READ[5];
        SAVE_EEprom_System_Time[2] = RTC_RX8025T_READ[4];
        SAVE_EEprom_System_Time[3] = RTC_RX8025T_READ[3];
        SAVE_EEprom_System_Time[4] = RTC_RX8025T_READ[2];
        SAVE_EEprom_System_Time[5] = RTC_RX8025T_READ[1];
        SAVE_EEprom_System_Time[6] = RTC_RX8025T_READ[0];
    } else {
        for (i = 0;i < 7;i++) {
            SAVE_EEprom_System_Time[i] = EEprom_Byte1Read(EEPROM_SYSTIME_ADDRESS + i);
        }
    }
    SAVE_EEprom_System_Time[3] = get_Week_In_Date(SAVE_EEprom_System_Time[0], SAVE_EEprom_System_Time[1], SAVE_EEprom_System_Time[2]);
    EEprom_Byte1Write(EEPROM_SYSTIME_ADDRESS + 3, SAVE_EEprom_System_Time[3]);
    if (Check_Date_ByHEX(0x20, SAVE_EEprom_System_Time[0], SAVE_EEprom_System_Time[1], SAVE_EEprom_System_Time[2]) == 0) {
        Init_SysTime_To_EEprom();
    }
    if (Check_Time_ByHEX(SAVE_EEprom_System_Time[4], SAVE_EEprom_System_Time[5], SAVE_EEprom_System_Time[6]) == 0) {
        Init_SysTime_To_EEprom();
    }

    if (HasRX8025T != 0) {
        RTC_RX8025T_WRITE[0] = SAVE_EEprom_System_Time[6];
        RTC_RX8025T_WRITE[1] = SAVE_EEprom_System_Time[5];
        RTC_RX8025T_WRITE[2] = SAVE_EEprom_System_Time[4];
        RTC_RX8025T_WRITE[3] = SAVE_EEprom_System_Time[3];
        RTC_RX8025T_WRITE[4] = SAVE_EEprom_System_Time[2];
        RTC_RX8025T_WRITE[5] = SAVE_EEprom_System_Time[1];
        RTC_RX8025T_WRITE[6] = SAVE_EEprom_System_Time[0];
        RX8025T_Write_RTC();
        FL_DelayMs(10);
    }
    RTC_Data.year = SAVE_EEprom_System_Time[0];
    RTC_Data.month = SAVE_EEprom_System_Time[1];
    RTC_Data.day = SAVE_EEprom_System_Time[2];
    RTC_Data.week = SAVE_EEprom_System_Time[3];
    RTC_Data.hour = SAVE_EEprom_System_Time[4];
    RTC_Data.minute = SAVE_EEprom_System_Time[5];
    RTC_Data.second = SAVE_EEprom_System_Time[6];
    FL_RTC_ConfigTime(RTC, &RTC_Data);
}
