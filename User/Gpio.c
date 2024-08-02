#include "Gpio.h"
#include "main.h"

static unsigned int gpio_run_dealy;
static unsigned int gpio_err_dealy;
static unsigned int gpio_gsm_dealy;
static unsigned int gpio_check_dealy;
static unsigned int gpio_inv_dealy;
static unsigned int gpio_stop_dealy;
static unsigned int gpio_err = 0;
static unsigned int gpio_gsm = 0;

void EC20_Dealy_GPIO_Init(void) {
    //	LED_GSM_OFF();
    //	LED_GSM_ON();
    //	LED_ERR_OFF();
    //	LED_RUN_OFF();
}

void EC20_Dealy_GPIO_Timer_Add(void) {
    gpio_run_dealy++;
    gpio_err_dealy--;
    gpio_gsm_dealy--;
    gpio_check_dealy++;
    gpio_inv_dealy++;
    gpio_stop_dealy--;
}

void EC20_Dealy_GPIO_Err_On(unsigned short int ms) {
    gpio_err = 1;
    gpio_err_dealy = ms / 10;
    //	LED_ERR_ON();
}

void EC20_Dealy_GPIO_GSM_On(unsigned short int ms) {
    if (ms == 0) {
        gpio_gsm = 0;
    } else {
        gpio_gsm = 1;
        gpio_gsm_dealy = ms / 10;
    }
    //	LED_GSM_ON();
}

void EC20_Dealy_GPIO_Task(void) {
    // **********************************
    static unsigned char run_sig;
    if (gpio_run_dealy > 100 && run_sig == 0) {
        gpio_run_dealy = 0;
        run_sig = 1;
        //		LED_RUN_ON();
    } else if (gpio_run_dealy > 100 && run_sig != 0) {
        gpio_run_dealy = 0;
        run_sig = 0;
        //		LED_RUN_OFF();
    }

    if (gpio_err == 1 && gpio_err_dealy <= 0) {
        gpio_err = 0;
        //		LED_ERR_OFF();
    }
    if (gpio_gsm == 1 && gpio_gsm_dealy <= 0) {
        gpio_gsm = 0;
        //		LED_GSM_OFF();
    }
}
