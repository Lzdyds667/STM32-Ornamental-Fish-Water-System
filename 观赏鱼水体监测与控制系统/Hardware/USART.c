#include "stm32f10x.h"
#include "USART.h"


/* =========================
   串口接收变量
   ========================= */

/* 接收缓冲区 */
static char USART1_RxBuffer[20];

/* 当前接收到第几个字节 */
static uint8_t USART1_RxIndex = 0;

/* 一条完整命令是否接收完成 */
static uint8_t USART1_RxFlag = 0;


/* =========================
   USART1初始化
   ========================= */

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    /* =========================
       开启时钟
       ========================= */

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );


    /* =========================
       PA9 -> USART1_TX
       ========================= */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* =========================
       PA10 -> USART1_RX
       ========================= */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* =========================
       USART参数
       ========================= */

    USART_InitStructure.USART_BaudRate = 115200;

    USART_InitStructure.USART_WordLength =
        USART_WordLength_8b;

    USART_InitStructure.USART_StopBits =
        USART_StopBits_1;

    USART_InitStructure.USART_Parity =
        USART_Parity_No;

    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;


    USART_Init(USART1, &USART_InitStructure);


    /* =========================
       NVIC中断配置
       ========================= */

    /* 设置中断优先级分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


    /* USART1中断 */
    NVIC_InitStructure.NVIC_IRQChannel =
        USART1_IRQn;

    /* 抢占优先级 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
        1;

    /* 子优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =
        1;

    /* 开启USART1中断 */
    NVIC_InitStructure.NVIC_IRQChannelCmd =
        ENABLE;

    NVIC_Init(&NVIC_InitStructure);


    /* =========================
       开启USART接收中断
       ========================= */

    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );


    /* =========================
       开启USART1
       ========================= */

    USART_Cmd(USART1, ENABLE);
}


/* =========================
   发送一个字节
   ========================= */

void USART1_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);

    while (
        USART_GetFlagStatus(
            USART1,
            USART_FLAG_TXE
        ) == RESET
    );
}


/* =========================
   发送字符串
   ========================= */

void USART1_SendString(char *String)
{
    while (*String != '\0')
    {
        USART1_SendByte(*String);

        String++;
    }
}


/* =========================
   获取一条完整命令
   ========================= */

uint8_t USART1_GetCommand(char *Buffer)
{
    uint8_t i;

    /* 没有收到完整命令 */
    if (USART1_RxFlag == 0)
    {
        return 0;
    }


    /* 关闭中断，防止复制过程中被修改 */
    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        DISABLE
    );


    /* 复制接收到的数据 */

    for (i = 0; i < 20; i++)
    {
        Buffer[i] = USART1_RxBuffer[i];

        if (USART1_RxBuffer[i] == '\0')
        {
            break;
        }
    }


    /* 清除接收标志 */

    USART1_RxFlag = 0;

    USART1_RxIndex = 0;


    /* 清空接收缓冲区 */

    for (i = 0; i < 20; i++)
    {
        USART1_RxBuffer[i] = '\0';
    }


    /* 重新开启接收中断 */

    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );


    return 1;
}


/* =========================
   USART1中断函数
   ========================= */

void USART1_IRQHandler(void)
{
    uint8_t Data;


    /* 判断是不是接收中断 */
    if (
        USART_GetITStatus(
            USART1,
            USART_IT_RXNE
        ) != RESET
    )
    {
        /* 读取收到的数据 */

        Data = USART_ReceiveData(USART1);


        /* =========================
           如果上一条命令还没有处理
           暂时忽略新的数据
           ========================= */

        if (USART1_RxFlag == 0)
        {
            /* 收到回车或者换行 */
            if (
                Data == '\r' ||
                Data == '\n'
            )
            {
                /* 确保字符串结束 */
                USART1_RxBuffer[USART1_RxIndex] = '\0';

                /* 标记一条命令接收完成 */
                USART1_RxFlag = 1;
            }

            else
            {
                /* 防止数组越界 */

                if (USART1_RxIndex < 19)
                {
                    USART1_RxBuffer[USART1_RxIndex] =
                        Data;

                    USART1_RxIndex++;
                }
            }
        }
    }
}
