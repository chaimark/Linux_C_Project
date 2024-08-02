#ifndef __WIFI_INTERACTION_H
#define __WIFI_INTERACTION_H

#include "main.h"
#include <stdbool.h>

extern bool WIFI_TASK(void);
extern bool IsAcceptWIFIData(char * UartBuff, int BuffLen);
#endif

