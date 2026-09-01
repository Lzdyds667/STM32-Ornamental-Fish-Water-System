#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"


// DS18B20连接
#define DS18B20_GPIO GPIOA
#define DS18B20_PIN  GPIO_Pin_1


void DS18B20_Init(void);

uint8_t DS18B20_Reset(void);

void DS18B20_WriteBit(uint8_t bit);

void DS18B20_WriteByte(uint8_t data);

uint8_t DS18B20_ReadBit(void);

uint8_t DS18B20_ReadByte(void);

float DS18B20_GetTemp(void);


#endif