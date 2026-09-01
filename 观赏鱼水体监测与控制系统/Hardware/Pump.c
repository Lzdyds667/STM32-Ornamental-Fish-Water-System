#include "stm32f10x.h"
#include "Pump.h"


static uint8_t PumpState = 0;


/* =========================
   水泵初始化
   ========================= */

void Pump_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    /* 开启GPIOA时钟 */

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA,
        ENABLE
    );


    /* PA8推挽输出 */

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_8;

    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_50MHz;


    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );


    /* 默认关闭 */

    GPIO_ResetBits(
        GPIOA,
        GPIO_Pin_8
    );

    PumpState = 0;
}


/* =========================
   开启水泵
   ========================= */

void Pump_ON(void)
{
    GPIO_SetBits(
        GPIOA,
        GPIO_Pin_8
    );

    PumpState = 1;
}


/* =========================
   关闭水泵
   ========================= */

void Pump_OFF(void)
{
    GPIO_ResetBits(
        GPIOA,
        GPIO_Pin_8
    );

    PumpState = 0;
}


/* =========================
   获取水泵状态
   ========================= */

uint8_t Pump_GetState(void)
{
    return PumpState;
}
