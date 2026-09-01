#ifndef __PUMP_H
#define __PUMP_H

#include "stm32f10x.h"

void Pump_Init(void);

void Pump_ON(void);

void Pump_OFF(void);

uint8_t Pump_GetState(void);

#endif
