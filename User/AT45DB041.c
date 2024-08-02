#include "main.h"

/******************************************************************************/
/*正常操作电压为2.7~3.6V，实验中发现当电压超过4.25V后读出的状态字节为9A（正常 */
/*的状态字节值为9D），并且读写数据均不准确，所以应当保证卡片的供电电压不超过  */
/*4.25V。                                                                     */
/*SPI规范：Data is always clocked into the device on the rising edge of SCK a-*/
/*    nd clocked out of the device on the falling edge of SCK.All instruction-*/
/*    s,addresses and data are transferred with the most significant bit(MSB) */
/*    first.                                                                  */
/*                                                                  2008-08-23*/
/******************************************************************************/

unsigned char SPI_HostReadByte(void) {
    unsigned char i, rByte = 0;
    for (i = 0;i < 8;i++) {
        FLASH_SCLK_LOW;
        rByte = rByte << 1;
        if (FLASH_SDA_VALUE) {
            rByte |= 1;
        } else {
            rByte |= 0;
        }
        FLASH_SCLK_HIGH;
    }
    return rByte;
}

void SPI_HostWriteByte(unsigned char val) {
    unsigned char i;
    for (i = 0;i < 8;i++) {
        FLASH_SCLK_LOW;
        if (val & 0x80)FLASH_MOSI_HIGH;
        else FLASH_MOSI_LOW;
        val = val << 1;
        FLASH_SCLK_HIGH;
    }
}

void AT45DB041_Initial(void) {
    FLASH_RST_HIGH;
    FL_DelayMs(300);
    FLASH_RST_LOW;
    FL_DelayMs(300);
    FLASH_RST_HIGH;
    FLASH_SSN_HIGH;
}
/******************************************************************************/
//读状态寄存器   
/*Status Register Format:                                                     */
/*   -----------------------------------------------------------------------  */
/*  |  bit7  |  bit6  |  bit5  |  bit4  |  bit3  |  bit2  |  bit1  |  bit0  | */
/*  |--------|--------|--------|--------|--------|--------|--------|--------| */
/*  |RDY/BUSY|  COMP  |   0    |   1    |   1    |   1    |   X    |   X    | */
/*   -----------------------------------------------------------------------  */
/*  bit7 - 忙标记，0为忙1为不忙。                                             */
/*         当Status Register的位0移出之后，接下来的时钟脉冲序列将使SPI器件继续*/
/*         将最新的状态字节送出。                                             */
/*  bit6 - 标记最近一次Main Memory Page和Buffer的比较结果，0相同，1不同。     */
/*  bit5                                                                      */
/*  bit4                                                                      */
/*  bit3                                                                      */
/*  bit2 - 这4位用来标记器件密度，对于AT45DB041B，这4位应该是0111，一共能标记 */
/*         16种不同密度的器件。                                               */
/*  bit1                                                                      */
/*  bit0 - 这2位暂时无效                                                      */
/******************************************************************************/
unsigned char AT45DB041_Status_Read(void) {
    unsigned char i;
    FLASH_SSN_LOW;
    __NOP();
    SPI_HostWriteByte(0xD7);
    i = SPI_HostReadByte();
    FLASH_SSN_HIGH;
    __NOP();
    return i;
}
/******************************************************************************/
/*描述：                                                                      */
/*    When the last bit in the main memory array has been read,the device will*/
/*    continue reading back at the beginning of the first page of memory.As w-*/
/*    ith crossing over page boundaries,no delays will be incurred when wrapp-*/
/*    ing around from the end of the array to the beginning of the array.     */
/*参数：                                                                      */
/*    PA      - 页地址，0~2047                                                */
/*    BFA     - 指定BUFFER中的起始写入地址                                    */
/*    pHeader - 指定数据的首地址                                              */
/*    len     - 指定数据的长度                                                */
/******************************************************************************/
void AT45DB041_Continu_Read(unsigned short int PA, unsigned short int BFA, unsigned char * pHeader, unsigned short int len) {
    unsigned short int i = 0;
    while (i++ < 765) { if (AT45DB041_Status_Read() & 0x80) { break; } }
    FLASH_SSN_LOW;
    __NOP();
    SPI_HostWriteByte(0xe8);
    SPI_HostWriteByte((unsigned char)(PA >> 7));
    SPI_HostWriteByte((unsigned char)((PA << 1) | (BFA >> 8)));
    SPI_HostWriteByte((unsigned char)BFA);
    for (i = 0;i < 4;i++) { SPI_HostWriteByte(0x00); }
    for (i = 0;i < len;i++) { pHeader[i] = SPI_HostReadByte(); }
    FLASH_SSN_HIGH;
    __NOP();
}

/******************************************************************************/
/*描述：
    //读数据到Buffer                                                          */
    /*参数：                                                                      */
    /*    PA      - 页地址，0~2047                                                */
    /*    addr    - 指定BUFFER中的起始写入地址                                    */
    /*    buf_n   - 指定BUFFER是1还是2                                            */
    /*    len     - 指定数据的长度                                                */
    /******************************************************************************/
void AT45DB041_PageToBuffer(uint32_t addr, unsigned char buf_n) {
    unsigned short int i = 0;
    unsigned short int PA;
    PA = addr / 264;      //页地址，0~2047   
    if (PA > 2047) PA = 0;
    while (i++ < 765) { if (AT45DB041_Status_Read() & 0x80) { break; } }
    FLASH_SSN_LOW;
    __NOP();
    if (buf_n == 1) SPI_HostWriteByte(0x53);
    else SPI_HostWriteByte(0x55);
    SPI_HostWriteByte((unsigned char)(PA >> 7));
    SPI_HostWriteByte((unsigned char)(PA << 1));
    SPI_HostWriteByte(0x00);
    FLASH_SSN_HIGH;
    __NOP();
}

/******************************************************************************/
/*描述：                                                                      */
/*    将指定数据写入从某个地址（0~263）开始的BUFFER中。                       */
/*参数：                                                                      */
/*    buffer  - 选择BUFFER，01H选择BUFFER 1，02H选择BUFFER 2                  */
/*              在该指令序列中，操作码84H选择BUFFER 1，87H选择BUFFER 2        */
/*    BFA     - BUFFER中的起始地址，0~263                                     */
/*    pHeader - 待存数据的头指针                                              */
/*    len     - 待存数据的长度1~264                                           */
/******************************************************************************/
void AT45DB041_BufferWrite(unsigned char buffer, unsigned short int BFA, unsigned char * pHeader, unsigned short int len) {
    unsigned short int i = 0;
    while (i++ < 765) { if (AT45DB041_Status_Read() & 0x80) { break; } }
    FLASH_SSN_LOW;
    __NOP();
    switch (buffer) {
        case 1:SPI_HostWriteByte(0x84);break;
        case 2:SPI_HostWriteByte(0x87);break;
    }
    SPI_HostWriteByte(0x00);
    SPI_HostWriteByte((unsigned char)(BFA >> 8));
    SPI_HostWriteByte((unsigned char)BFA);

    for (i = 0;i < len;i++) { SPI_HostWriteByte(pHeader[i]); }
    FLASH_SSN_HIGH;
    __NOP();
}
/******************************************************************************/
/*描述：                                                                      */
/*    将指定数据写入从某个地址（0~263）开始的页中：包含2个动作，首先将指定数据*/
/*    写入到BUFFER 1或者BUFFER 2中，其中可以指定BUFFER中的起始写入地址，此写入*/
/*    动作不影响BUFFER中其它地址中的数据，然后再将BUFFER中的整个数据写入到某指*/
/*    定页中(带预擦除)。                                                      */
/*参数：                                                                      */
/*    buffer  - 选择BUFFER，01H选择BUFFER 1，02H选择BUFFER 2                  */
/*    PA      - 页地址，0~2047                                                */
/*    BFA     - 指定BUFFER中的起始写入地址                                    */
/*    pHeader - 指定数据的首地址                                              */
/*    len     - 指定数据的长度                                                */
/******************************************************************************/
void AT45DB041_Built_With_Erase(unsigned char buffer, unsigned short int PA, unsigned short int BFA, unsigned char * pHeader, unsigned short int len) {
    unsigned short int i = 0;
    AT45DB041_BufferWrite(buffer, BFA, pHeader, len);
    while (i++ < 3000) { if (AT45DB041_Status_Read() & 0x80) { break; } }

    FLASH_SSN_LOW;
    switch (buffer) {
        case 1:SPI_HostWriteByte(0x83);break;
        case 2:SPI_HostWriteByte(0x86);break;
    }
    SPI_HostWriteByte((unsigned char)(PA >> 7));
    SPI_HostWriteByte((unsigned char)(PA << 1));
    SPI_HostWriteByte(0x00);
    FLASH_SSN_HIGH;
}
/******************************************************************************/
/*描述：                                                                      */
/*    与上一个函数的唯一区别是不带预擦除。                                    */
/******************************************************************************/
void AT45DB041_Built_Without_Erase(unsigned char buffer, unsigned short int PA, unsigned short int BFA, unsigned char * pHeader, unsigned short int len) {
    unsigned short int i = 0;

    AT45DB041_BufferWrite(buffer, BFA, pHeader, len);
    while (i++ < 3000) { if (AT45DB041_Status_Read() & 0x80) { break; } }

    FLASH_SSN_LOW;
    switch (buffer) {
        case 1:SPI_HostWriteByte(0x88);break;
        case 2:SPI_HostWriteByte(0x89);break;
    }
    SPI_HostWriteByte((unsigned char)(PA >> 7));
    SPI_HostWriteByte((unsigned char)(PA << 1));
    SPI_HostWriteByte(0x00);
    FLASH_SSN_HIGH;
}

void Read_AT45DB041_Data(uint32_t addr, unsigned char * pHeader, unsigned short int len) {
    AT45DB041_Continu_Read(addr / 264, addr % 264, pHeader, len);
}
/******************************************************************************/
//写数据到AT45DB041B   
/*描述：                                                                      */
/*    将指定数据写入从某个地址（0~263）开始的页中：包含2个动作，首先将指定数据*/
/*    写入到BUFFER 1或者BUFFER 2中，其中可以指定BUFFER中的起始写入地址，此写入*/
/*    动作不影响BUFFER中其它地址中的数据，然后再将BUFFER中的整个数据写入到某指*/
/*    定页中(带预擦除)。                                                      */
/*参数：                                                                      */
/*    addr      - 地址，0~540408                         */
/*    pHeader - 指定数据的首地址                                              */
/*    len     - 指定数据的长度                                                */
/******************************************************************************/
void Write_AT45DB041_Data(unsigned char buf, uint32_t addrl, unsigned char * pHeader, unsigned short int len) {
    AT45DB041_Built_With_Erase(buf, addrl / 264, addrl % 264, pHeader, len);
}
void Write_AT45DB041_Data_NO_ERASE(unsigned char buf, uint32_t addrl, unsigned char * pHeader, unsigned short int len) {
    AT45DB041_Built_Without_Erase(buf, addrl / 264, addrl % 264, pHeader, len);
}
