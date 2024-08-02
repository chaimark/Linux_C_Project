#include "BSTim.h"
#include "4G_Module.h"
#include "main.h"


void BSTIM_10mS_Enable(void) {
    FL_BSTIM32_Enable(BSTIM32);
}

void BSTIM_10mS_Disable(void) {
    FL_BSTIM32_Disable(BSTIM32);
}

void BSTIM_IRQHandler(void) {
    if (FL_BSTIM32_IsActiveFlag_Update(BSTIM32)) {
        FL_BSTIM32_ClearFlag_Update(BSTIM32);
        // UpDataProtocol_Timer_Add();
        MQTT_4G_10mS_Timers_Add();
        MQTT_NET_10mS_Timers_Add();
    }
}
