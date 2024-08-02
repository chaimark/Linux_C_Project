#ifndef __ADC_H
#define __ADC_H

#define ADC_VREF (*((uint16_t *)(0x1FFFFB08)))
#define ADC_CHANNEL_NUM 5
#define ADC_RES_VALUE 100
#define ADC_START_MV 400 // 4 * ADC_RES_VALUE
#define ADC_PA_1MV 1	 // 1k (1600000 / 16 / ADC_RES_VALUE)

extern unsigned int Test_Current_Voltge_mV(void);
extern unsigned int Test_Current_MBUS_mA(void);

#endif
