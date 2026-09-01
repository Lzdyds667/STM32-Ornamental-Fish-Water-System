#include "stm32f10x.h"
#include "Turbidity.h"


/**
  * @brief  浊度传感器初始化
  *         OUT连接PA0 ADC1通道0
  */
void Turbidity_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    ADC_InitTypeDef ADC_InitStructure;


    /*
    开启时钟

    GPIOA
    ADC1
    */

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_ADC1,
        ENABLE);



    /*
    PA0设置为模拟输入
    */

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_0;


    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_AIN;


    GPIO_Init(GPIOA,
              &GPIO_InitStructure);



    /*
    ADC配置
    */

    ADC_InitStructure.ADC_Mode =
        ADC_Mode_Independent;


    ADC_InitStructure.ADC_ScanConvMode =
        DISABLE;


    ADC_InitStructure.ADC_ContinuousConvMode =
        DISABLE;


    ADC_InitStructure.ADC_ExternalTrigConv =
        ADC_ExternalTrigConv_None;


    ADC_InitStructure.ADC_DataAlign =
        ADC_DataAlign_Right;


    ADC_InitStructure.ADC_NbrOfChannel =
        1;


    ADC_Init(ADC1,
             &ADC_InitStructure);



    /*
    ADC通道选择

    PA0
    ADC1_Channel0
    */

    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_0,
        1,
        ADC_SampleTime_55Cycles5);



    /*
    开启ADC
    */

    ADC_Cmd(ADC1,
            ENABLE);



    /*
    ADC校准
    */

    ADC_ResetCalibration(ADC1);


    while(
    ADC_GetResetCalibrationStatus(ADC1));



    ADC_StartCalibration(ADC1);


    while(
    ADC_GetCalibrationStatus(ADC1));


}



/**
  * @brief  获取浊度ADC值
  * @return ADC数值 0~4095
  */

uint16_t Turbidity_GetValue(void)
{

    uint16_t ADC_Value;


    /*
    软件触发ADC转换
    */

    ADC_SoftwareStartConvCmd(
        ADC1,
        ENABLE);



    /*
    等待转换完成
    */

    while(
    ADC_GetFlagStatus(
        ADC1,
        ADC_FLAG_EOC)==RESET);



    /*
    读取ADC
    */

    ADC_Value =
    ADC_GetConversionValue(ADC1);



    return ADC_Value;

}
