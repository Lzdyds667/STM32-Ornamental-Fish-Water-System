#include "stm32f10x.h"                  // Device header
#include "Temp.h"
#include "Delay.h"


/*
    设置GPIO输出
*/
void DS18B20_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


    GPIO_Init(DS18B20_GPIO, &GPIO_InitStructure);
}


/*
    设置GPIO输入
*/
void DS18B20_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(DS18B20_GPIO, &GPIO_InitStructure);
}



/*
    初始化
*/
void DS18B20_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(DS18B20_GPIO, &GPIO_InitStructure);


    GPIO_SetBits(DS18B20_GPIO, DS18B20_PIN
    );
}



/*
    复位
*/
uint8_t DS18B20_Reset(void)
{
    uint8_t response;

    DS18B20_Output();

    GPIO_ResetBits(DS18B20_GPIO, DS18B20_PIN);

    Delay_us(500);

    GPIO_SetBits(DS18B20_GPIO, DS18B20_PIN );

    DS18B20_Input();

    Delay_us(70);

    response = GPIO_ReadInputDataBit(DS18B20_GPIO, DS18B20_PIN);

    Delay_us(500);

    return response;
}


/*
    写1bit
*/
void DS18B20_WriteBit(uint8_t bit)
{
    DS18B20_Output();

    GPIO_ResetBits(DS18B20_GPIO, DS18B20_PIN);

    Delay_us(2);

    if(bit)
    {
        GPIO_SetBits(DS18B20_GPIO, DS18B20_PIN);
    }

    Delay_us(60);

    GPIO_SetBits(DS18B20_GPIO, DS18B20_PIN);
}



/*
    写一个字节
*/
void DS18B20_WriteByte(uint8_t data)
{
    uint8_t i;

    for(i=0;i<8;i++)
    {
        DS18B20_WriteBit(data & 0x01);

        data >>= 1;
    }
}



/*
    读1bit
*/
uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit;

    DS18B20_Output();

    GPIO_ResetBits(DS18B20_GPIO, DS18B20_PIN);

    Delay_us(2);

    GPIO_SetBits(DS18B20_GPIO, DS18B20_PIN);

    DS18B20_Input();

    Delay_us(12);

    bit =GPIO_ReadInputDataBit(DS18B20_GPIO, DS18B20_PIN);

    Delay_us(50);

    return bit;

}



/*
    读一个字节
*/
uint8_t DS18B20_ReadByte(void)
{

    uint8_t i;

    uint8_t data=0;

    for(i=0;i<8;i++)
    {
        data |=DS18B20_ReadBit()
        << i;
    }

    return data;
}




/*
    获取温度
*/
float DS18B20_GetTemp(void)
{
    uint8_t LSB;
    uint8_t MSB;

    uint16_t temp;

    DS18B20_Reset();

    //跳过ROM
    DS18B20_WriteByte(0xCC);

    //开始转换
    DS18B20_WriteByte(0x44);

    Delay_ms(750);

    DS18B20_Reset();

    DS18B20_WriteByte(0xCC);

    //读取温度
    DS18B20_WriteByte(0xBE);

    LSB =DS18B20_ReadByte();

    MSB =DS18B20_ReadByte();



    temp =
    (MSB<<8)|LSB;

    return temp*0.0625;
}