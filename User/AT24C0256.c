#include "AT24C0256.h"
#include "main.h"
#include "Define.h"
#include "StrLib.h"

#define TRUE true
#define FALSE false

unsigned char EEprom_Write_Start(unsigned char * data, unsigned short int addr, unsigned char len);

void IIC_start(void) {
    EEprom_SDA_OUT();
    FL_DelayUs(3);
    EEprom_SDA_HIGH;
    FL_DelayUs(3);
    EEprom_SCL_HIGH;
    FL_DelayUs(3);
    EEprom_SDA_LOW;
    FL_DelayUs(5);
    EEprom_SCL_LOW;
    FL_DelayUs(3);
}

void IIC_stop(void) {
    EEprom_SDA_OUT();
    EEprom_SDA_LOW;
    FL_DelayUs(2);
    EEprom_SCL_HIGH;
    FL_DelayUs(2);
    EEprom_SDA_LOW;
    FL_DelayUs(2);
    EEprom_SDA_HIGH;
}

void IIC_SendAck(void) {
    EEprom_SDA_OUT();
    EEprom_SDA_LOW;
    FL_DelayUs(2);
    EEprom_SCL_LOW;
    FL_DelayUs(2);
    EEprom_SCL_HIGH;
    FL_DelayUs(2);
    EEprom_SCL_LOW;
    EEprom_SDA_HIGH;
}

void IIC_SendNoAck(void) {
    EEprom_SDA_OUT();
    EEprom_SDA_HIGH;
    FL_DelayUs(3);
    EEprom_SCL_LOW;
    FL_DelayUs(3);
    EEprom_SCL_HIGH;
    FL_DelayUs(3);
    EEprom_SCL_LOW;
}

unsigned char IIC_Check_ACK(void) {
    unsigned char AckStatus;
    EEprom_SDA_IN();
    EEprom_SCL_HIGH;
    FL_DelayUs(3);
    if (EEprom_SDA_VALUE) {
        AckStatus = FALSE;
    } else {
        AckStatus = TRUE;
    }
    EEprom_SCL_LOW;
    FL_DelayUs(3);
    EEprom_SDA_OUT();
    return AckStatus;
}

void IIC_SendByte(unsigned char data) {
    unsigned char tmp;
    EEprom_SDA_OUT();
    for (tmp = 0; tmp < 8; tmp++) {
        if (data & 0x80) {
            EEprom_SDA_HIGH;
        } else {
            EEprom_SDA_LOW;
        }
        FL_DelayUs(3);
        EEprom_SCL_HIGH;
        FL_DelayUs(3);
        EEprom_SCL_LOW;
        FL_DelayUs(3);
        data <<= 1;
    }
    FL_DelayUs(15);
}

unsigned char IIC_RevByte(void) {
    unsigned char tmp;
    unsigned char DATA = 0;
    EEprom_SDA_IN();
    EEprom_SCL_LOW;
    FL_DelayUs(3);
    for (tmp = 0; tmp < 8; tmp++) {
        EEprom_SCL_HIGH;
        FL_DelayUs(3);
        DATA <<= 1;
        if (EEprom_SDA_VALUE) {
            DATA |= 0x01;
        } else {
            DATA &= 0xfe;
        }
        EEprom_SCL_LOW;
    }
    EEprom_SDA_OUT();
    return DATA;
}

unsigned char EEprom_Byte1Write_NO_CHECK(unsigned int addr, unsigned char data) {
    unsigned char Dev_addr;
    char AddrLow;
    char AddrHigh;
    AddrLow = addr % 256;
    AddrHigh = addr / 256;
    Dev_addr = 0xa0;
    IIC_start();
    IIC_SendByte(Dev_addr);
    if (IIC_Check_ACK() == FALSE) {
        goto AAA;
    }

    IIC_SendByte(AddrHigh);
    if (IIC_Check_ACK() == FALSE) {
        goto AAA;
    }
    IIC_SendByte(AddrLow);
    if (IIC_Check_ACK() == FALSE) {
        goto AAA;
    }
    IIC_SendByte(data);
    if (IIC_Check_ACK() == FALSE) {
        goto AAA;
    }
    IIC_stop();
    FL_DelayMs(5);
    return TRUE;
AAA:
    IIC_stop();
    FL_DelayMs(5);
    return FALSE;
}

unsigned char EEprom_Byte1Read_NO_CHECK(unsigned int addr) {
    unsigned char data;
    unsigned char Dev_addr;
    unsigned char AddrLow;
    unsigned char AddrHigh;
    AddrLow = addr % 0x100;
    AddrHigh = addr / 0x100;
    Dev_addr = 0xa0;
    IIC_start();
    IIC_SendByte(Dev_addr);
    if (IIC_Check_ACK() == FALSE) {
        goto BBB;
    }
    FL_DelayUs(1);
    IIC_SendByte(AddrHigh);
    if (IIC_Check_ACK() == FALSE) {
        goto BBB;
    }
    IIC_SendByte(AddrLow);
    if (IIC_Check_ACK() == FALSE) {
        goto BBB;
    }
    FL_DelayUs(1);
    IIC_start();
    Dev_addr = 0xa1;
    IIC_SendByte(Dev_addr);
    if (IIC_Check_ACK() == FALSE) {
        goto BBB;
    }
    FL_DelayUs(1);
    data = IIC_RevByte();
    IIC_SendNoAck();
    IIC_stop();
    FL_DelayUs(100);
    return data;
BBB:
    IIC_stop();
    FL_DelayMs(5);
    return FALSE;
}

unsigned char EEprom_Byte1Read(unsigned int addr) {
    unsigned char data[3] = {0};
    unsigned char i;
    for (i = 0; i < 2; i++) {
        FL_DelayMs(10);
        data[i] = EEprom_Byte1Read_NO_CHECK(addr);
    }
    if (data[0] == data[1])
        return data[0];
    else {
        data[2] = EEprom_Byte1Read_NO_CHECK(addr);
        FL_DelayMs(10);
        if (data[0] == data[2])
            return data[0];
        else if (data[1] == data[2])
            return data[1];
        else
            return FALSE;
    }
}

unsigned char EEprom_Byte1Write(unsigned int addr, unsigned char data) {
    unsigned char temp[3] = {0};
    EEprom_Byte1Write_NO_CHECK(addr, data);
    FL_DelayMs(2);
    temp[0] = EEprom_Byte1Read(addr);
    if (temp[0] == data)
        return TRUE;
    else {
        FL_DelayMs(2);
        EEprom_Byte1Write_NO_CHECK(addr, data);
        temp[1] = EEprom_Byte1Read(addr);
        if (temp[1] == data)
            return TRUE;
        else {
            EEprom_Byte1Write_NO_CHECK(addr, data);
            temp[2] = EEprom_Byte1Read(addr);
            if (temp[2] == data)
                return TRUE;
            else
                return FALSE;
        }
    }
}

unsigned int EEprom_Byte2Read(unsigned int addr) {
    unsigned int EE_Data2Byte;
    EE_Data2Byte = EEprom_Byte1Read(addr);
    EE_Data2Byte = EE_Data2Byte * 256;
    EE_Data2Byte = EE_Data2Byte + EEprom_Byte1Read(addr + 1);
    return EE_Data2Byte;
}

void EEprom_Byte2Write(unsigned int addr, unsigned int data) {
    unsigned char i, j;
    i = data / 256;
    j = data % 256;
    EEprom_Byte1Write(addr, i);
    EEprom_Byte1Write(addr + 1, j);
}

unsigned long int EEprom_Byte4Read(unsigned long int addr) {
    unsigned long int EE_Data2Byte, ee_;
    EE_Data2Byte = EEprom_Byte1Read(addr);
    EE_Data2Byte = EE_Data2Byte << 24;

    ee_ = EEprom_Byte1Read(addr + 1);
    ee_ = ee_ << 16;
    EE_Data2Byte = EE_Data2Byte + ee_;

    ee_ = EEprom_Byte1Read(addr + 2);
    ee_ = ee_ << 8;
    EE_Data2Byte = EE_Data2Byte + ee_;

    EE_Data2Byte = EE_Data2Byte + EEprom_Byte1Read(addr + 3);
    return EE_Data2Byte;
}

void EEprom_Byte4Write(unsigned long int addr, unsigned long int data) {
    unsigned char i;
    i = data;
    EEprom_Byte1Write(addr + 3, i);

    i = data >> 8;
    EEprom_Byte1Write(addr + 2, i);

    i = data >> 16;
    EEprom_Byte1Write(addr + 1, i);

    i = data >> 24;
    EEprom_Byte1Write(addr, i);
}

// public
AT24CXX_MANAGER_T AT24CXX_Manager = {
    0xB2,								  // 初始化标志
    {0x00, 0x23, 0x45, 0x67, 0x89, 0x03}, // 设备地址 02345678903
    "admin",
    "njhy1234",
    "20XX-09-01",
    "20XX-08-29",
    "0123465789",
    "192.168.0.105",
    "255.255.255.0",
    "192.168.0.1",
    "hysmartmetering.com",
    1883,
    1440,
    10, // 主表周期
    60, // 副表周期
    1,
    0x89,
    false,
    false,
};

// 初始化参数
unsigned char EEprom_MQTT_Parameter_Init(void) {
    unsigned char ret;
    int len = sizeof(AT24CXX_Manager);
    unsigned char * p = (unsigned char *)(&AT24CXX_Manager);
    if (EEprom_Byte1Read(0) != AT24CXX_Manager.sign) {
        for (int i = 1; i < len; i++) {
            ret = EEprom_Byte1Write(i, p[i]);
            if (ret == 0) {
                printf("e2 err1\r\n");
                return ret;
            }
        }
        ret = EEprom_Byte1Write(0, p[0]);
        if (ret == 0) {
            printf("e2 err2\r\n");
            return ret;
        }
    } else {
        for (int i = 1; i < len; i++) {
            p[i] = EEprom_Byte1Read(i);
        }
    }
    printf("e2 ok\r\n");
    return 0;
}

// 写参数
unsigned char EEprom_JSON_Write(unsigned char * data, unsigned char len) {
    int addr = (int)data - (int)(&AT24CXX_Manager); // 数据地址
    return EEprom_Write_Start(data, addr, len);
}
// 写参数 位置
unsigned char EEprom_Write_Start(unsigned char * data, unsigned short int addr, unsigned char len) {
    int i;
    unsigned char ret;
    for (i = 0; i < len; i++) {
        ret = EEprom_Byte1Write(addr + i, data[i]);
        if (ret == 0) {
            printf("e2 err3\r\n");
            return ret;
        }
    }
    return 0;
}
/*start*******************************配置NET******************************************/
void Read_MBUS_Buad_Local_From_AT256_To_ARM(void) {
    unsigned long int temp0, temp1;
    temp0 = EEprom_Byte4Read(EEPROM_MBUS_UART1_BAUDRATE_ADDR);
    if (temp0 == 1200)
        MBUS_UART1_BaudRate = temp0;
    else if (temp0 == 2400)
        MBUS_UART1_BaudRate = temp0;
    else if (temp0 == 4800)
        MBUS_UART1_BaudRate = temp0;
    else if (temp0 == 9600)
        MBUS_UART1_BaudRate = temp0;
    else
        MBUS_UART1_BaudRate = 2400;

    temp1 = EEprom_Byte1Read(EEPROM_MBUS_UART1_PARITY_ADDR);
    if (temp1 == 0)
        MBUS_UART1_Parity = temp1;
    else if (temp1 == 1)
        MBUS_UART1_Parity = temp1;
    else if (temp1 == 2)
        MBUS_UART1_Parity = temp1;
    else
        MBUS_UART1_Parity = 2;
}

void Read_RS4851_Buad_Local_From_AT256_To_ARM(void) {
    unsigned long int temp2, temp3;
    temp2 = EEprom_Byte4Read(EEPROM_RS4851_UART4_BAUDRATE_ADDR);
    if (temp2 == 1200)
        RS4851_UART4_BaudRate = temp2;
    else if (temp2 == 2400)
        RS4851_UART4_BaudRate = temp2;
    else if (temp2 == 4800)
        RS4851_UART4_BaudRate = temp2;
    else if (temp2 == 9600)
        RS4851_UART4_BaudRate = temp2;
    else
        RS4851_UART4_BaudRate = 1200;

    temp3 = EEprom_Byte1Read(EEPROM_RS4851_UART4_PARITY_ADDR);
    if (temp3 == 0)
        RS4851_UART4_Parity = temp3;
    else if (temp3 == 1)
        RS4851_UART4_Parity = temp3;
    else if (temp3 == 2)
        RS4851_UART4_Parity = temp3;
    else
        RS4851_UART4_Parity = 2;
}

void Read_RS4852_Buad_Local_From_AT256_To_ARM(void) {
    unsigned long int temp2, temp3;
    temp2 = EEprom_Byte4Read(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR);
    if (temp2 == 1200)
        RS4852_LPUART0_BaudRate = temp2;
    else if (temp2 == 2400)
        RS4852_LPUART0_BaudRate = temp2;
    else if (temp2 == 4800)
        RS4852_LPUART0_BaudRate = temp2;
    else if (temp2 == 9600)
        RS4852_LPUART0_BaudRate = temp2;
    else
        RS4852_LPUART0_BaudRate = 1200;

    temp3 = EEprom_Byte1Read(EEPROM_RS4852_LPUART0_PARITY_ADDR);
    if (temp3 == 0)
        RS4852_LPUART0_Parity = temp3;
    else if (temp3 == 1)
        RS4852_LPUART0_Parity = temp3;
    else if (temp3 == 2)
        RS4852_LPUART0_Parity = temp3;
    else
        RS4852_LPUART0_Parity = 2;
}
