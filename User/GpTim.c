#include "fm33lc0xx_fl.h"

void GPTIM0_Start(void) {
    //使能LGPTIM
    FL_GPTIM_Enable(GPTIM0);
}

void GPTIM0_Stop(void) {
    //使能LGPTIM
    FL_GPTIM_Disable(GPTIM0);
}

void GPTIM0_IRQHandler() {
    if (FL_GPTIM_IsEnabledIT_Update(GPTIM0) && FL_GPTIM_IsActiveFlag_Update(GPTIM0)) {
        FL_GPTIM_ClearFlag_Update(GPTIM0);
    }
}

void GPTIM1_Start(void) {
    //使能LGPTIM
    FL_GPTIM_Enable(GPTIM1);
}

void GPTIM1_Stop(void) {
    //使能LGPTIM
    FL_GPTIM_Disable(GPTIM1);
}

void GPTIM1_IRQHandler() {
    if (FL_GPTIM_IsEnabledIT_Update(GPTIM1) && FL_GPTIM_IsActiveFlag_Update(GPTIM1)) {
        FL_GPTIM_ClearFlag_Update(GPTIM1);
    }
}
