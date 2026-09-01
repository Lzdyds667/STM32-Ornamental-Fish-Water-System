#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

/* USART初始化 */
void USART1_Init(void);

/* USART发送 */
void USART1_SendByte(uint8_t Byte);
void USART1_SendString(char *String);

/* 获取接收到的一条完整命令 */
uint8_t USART1_GetCommand(char *Buffer);

#endif
