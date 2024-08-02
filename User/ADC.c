#include "ADC.h"
#include "main.h"

static unsigned int GetVREF1P2Sample_POLL(void) {
    unsigned short int ADCRdresult;
    unsigned int i = 0;
    unsigned int time = 0;
    if (FL_ADC_IsEnabledOverSampling(ADC)) {
        time = (1600 * 2) << (FL_ADC_GetOverSamplingMultiplier(ADC) >> 17);
    } else time = 1600;
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV2);
    FL_VREF_EnableVREFBuffer(VREF);//使能VREF BUFFER
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_ALL_CHANNEL);//清空打开的通道
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);//通道选择VREF
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志
    FL_ADC_Enable(ADC);      // 启动ADC
    FL_ADC_EnableSWConversion(ADC);  // 开始转换
    // 等待转换完成
    while (FL_ADC_IsActiveFlag_EndOfConversion(ADC) == FL_RESET) {
        if (i >= time) {
            break;
        }
        i++;
        FL_DelayUs(1);
    }
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志
    ADCRdresult = FL_ADC_ReadConversionData(ADC); //获取采样值
    FL_ADC_Disable(ADC);    // 关闭ADC
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);//通道关闭VREF
    FL_VREF_DisableVREFBuffer(VREF);//关闭VREF BUFFER
    // 转换结果
    return ADCRdresult;
}

static unsigned int GetSingleChannelSample_POLL(unsigned int channel) {
    unsigned short int ADCRdresult;
    unsigned int i = 0;
    unsigned int time = 0;
    if (FL_ADC_IsEnabledOverSampling(ADC)) {
        time = (1600 * 2) << (FL_ADC_GetOverSamplingMultiplier(ADC) >> 17);
    } else time = 1600;
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV1);
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_ALL_CHANNEL);//清空打开的通道
    FL_ADC_EnableSequencerChannel(ADC, channel);//通道选择
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志
    FL_ADC_Enable(ADC);      // 启动ADC
    FL_ADC_EnableSWConversion(ADC);  // 开始转换
    // 等待转换完成
    while (FL_ADC_IsActiveFlag_EndOfConversion(ADC) == FL_RESET) {
        if (i >= time) {
            break;
        }
        i++;
        FL_DelayUs(1);
    }
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志
    ADCRdresult = FL_ADC_ReadConversionData(ADC); //获取采样值
    FL_ADC_Disable(ADC);    // 关闭ADC
    FL_ADC_DisableSequencerChannel(ADC, channel);//通道
    // 转换结果
    return ADCRdresult;
}

unsigned int GetSingleChannelVoltage_POLL(unsigned int channel) {
    unsigned int Get122VSample, GetChannelVoltage;
    uint64_t GetVSample;
    Get122VSample = GetVREF1P2Sample_POLL();
    GetVSample = GetSingleChannelSample_POLL(channel);
    GetChannelVoltage = (GetVSample * 3000 * (ADC_VREF)) / (Get122VSample * 4095);
    // 转换结果
    return GetChannelVoltage;
}
//输出值 mV
unsigned int Test_Current_Voltge_mV(void) {
    unsigned int ADC_DATA;
    unsigned char i;
    ADC_DATA = 0;
    for (i = 0;i < 20;i++) ADC_DATA += GetSingleChannelVoltage_POLL(FL_ADC_EXTERNAL_CH2);
    ADC_DATA = ADC_DATA / 20;
    return ADC_DATA * 11;
}
//输出值 为mA
unsigned int Test_Current_MBUS_mA(void) {
    unsigned int ADC_DATA;
    unsigned char i;
    ADC_DATA = 0;
    for (i = 0;i < 20;i++)ADC_DATA += GetSingleChannelVoltage_POLL(FL_ADC_EXTERNAL_CH5);
    ADC_DATA = ADC_DATA / 20;
    return ADC_DATA / 2.5;
}
