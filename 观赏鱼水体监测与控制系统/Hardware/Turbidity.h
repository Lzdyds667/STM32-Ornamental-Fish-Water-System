#ifndef __TURBIDITY_H
#define __TURBIDITY_H

#include "stm32f10x.h"


//浊度模块初始化
void Turbidity_Init(void);


//读取ADC值
uint16_t Turbidity_GetValue(void);


#endif
