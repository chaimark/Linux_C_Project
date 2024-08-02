
#include "LPTim32.h"
#include "fm33lc0xx_fl.h"
#include "MQTT_JSON.h"
#include "Uart.h"
#include "LPUart.h"
#include "ImmediatelyReading.h" 
#include "TimerMainReading.h" 
#include "TimerCopyReading.h"
#include "main.h"

void LPTIM32_250mS_Start(void) {
    //使能LPTIM32
    FL_LPTIM32_Enable(LPTIM32);
}

void LPTIM32_250mS_Stop(void) {
    //不使能LPTIM32
    FL_LPTIM32_Disable(LPTIM32);
}

void LPTIM_IRQHandler(void) {
    if (FL_LPTIM32_IsEnabledIT_Update(LPTIM32) && FL_LPTIM32_IsActiveFlag_Update(LPTIM32)) {
        FL_LPTIM32_ClearFlag_Update(LPTIM32);

        if (mainTimerRead.readMeter_250mS_Delay > 50) mainTimerRead.readMeter_250mS_Delay = 50;
        if (copyTimerRead.readMeter_250mS_Delay > 50) copyTimerRead.readMeter_250mS_Delay = 50;
        if (imm_Read.immediately_250mS_Delay > 50) imm_Read.immediately_250mS_Delay = 50;

        if (mainTimerRead.readMeter_250mS_Delay > 0) mainTimerRead.readMeter_250mS_Delay--;
        if (copyTimerRead.readMeter_250mS_Delay > 0) copyTimerRead.readMeter_250mS_Delay--;
        if (imm_Read.immediately_250mS_Delay > 0) imm_Read.immediately_250mS_Delay--;
    }
}
