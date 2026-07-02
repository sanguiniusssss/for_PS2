/**
  ******************************************************************************
  * @file    ax_basis.c
  * @author  Musk Han @ XTARK
  * @version V1.1
  * @date    2018-01-01
  * @brief   X-CTR100基础外设驱动文件
  *          LED、BEEP、按键开关、延时、UART1
  *
  ******************************************************************************
  * @xtark  实验平台：机器人成品 X-CTR100 控制器
  *          官网：http://www.xtark.cn
  *
  ******************************************************************************
  */

#include "ax_basis.h"
#include <stdio.h>

extern UART_HandleTypeDef huart1;

/**
  * @函  数  X-CTR100初始化
  * @参  数  baud：波特率设置(例如，9600,115200)
  * @返回值  无
  * @说  明  配置LED、BEEP、按键开关、SysTick、UART1
  */
void AX_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* LED BEEP 按键开关 GPIO配置 */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	//配置红色LED指示灯GPIO  推挽输出模式
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);

	//配置绿色LED指示灯GPIO  推挽输出模式
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);

	//配置蜂鸣器GPIO  推挽输出模式
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	//配置模式选择S1 GPIO  输入模式
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	//配置模式选择S2 GPIO  输入模式
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* 定时器 SysTick 配置 */
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk; 	//时钟固定为AHB时钟的1/8

	/* 配置 USART1接口 */
	//使能GPIO和USART时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	//USART1对应引脚复用映射
	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//USART1参数配置
	huart1.Instance = USART1;
	huart1.Init.BaudRate = baud;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);

	//清除第一个字符，防止乱码BUG
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_TC);
}

/**
  * @函  数  SW选择开关状态
  * @说  明	 返回拨码ON为1，否则为0。
  * @参  数  无
  * @返回值  开关状态
  *			SW1(PA8)  SW2(PE10)    返回值
  *			 0         0             0
  *          0         1             1
  *          1         0             2
  *          1         1             3
  */
uint8_t AX_SW_GetSW12Status(void)
{
    uint8_t sw = 0;

	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15))
	{sw <<= 1;}
	else
	{sw=sw+1; sw<<=1;}

	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10))
	{;}
	else
	{sw=sw+1;}

	return sw;
}

/**
  * @函  数  SW1选择开关状态
  * @参  数  无
  * @返回值  按键开关状态
  *			 1 - 拨码开关在ON侧
  *			 0 - 拨码开关在ON对侧
  */
uint8_t AX_SW_GetSW1Status(void)
{
   	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15))
	return 0;
	else
	return 1;
}

/**
  * @函  数  SW2选择开关状态
  * @参  数  无
  * @返回值  按键开关状态
  *			 1 - 拨码开关在ON侧
  *			 0 - 拨码开关在ON对侧
  */
uint8_t AX_SW_GetSW2Status(void)
{
   	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10))
	return 0;
	else
	return 1;
}

/**
  * @函  数  微秒级延时
  * @参  数  us：延时长度，单位us
  * @返回值  无
  */
void AX_Delayus(uint16_t us)
{
	uint32_t temp;

	SysTick->LOAD=21*us;
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0X00;
}

/**
  * @函  数  毫秒级延时函数
  * @参  数  ms：延时长度，单位ms
  * @返回值  无
  * @说  明  注意ms的范围：SysTick->LOAD为24位寄存器,所以,最大延时为:nTime<=0xffffff*8*1000/SYSCLK
  *          对168M而言,ms<=798ms
  */
static void Delay_ms(uint16_t ms)
{
	uint32_t temp;

	SysTick->LOAD=(uint32_t)21000*ms;
	SysTick->VAL =0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0X00;
}

/**
  * @函  数  毫秒级延时
  * @参  数  ms：延时长度，单位ms
  * @返回值  无
  */
void AX_Delayms(uint16_t ms)
{
	uint8_t repeat=ms/500;
	uint16_t remain=ms%500;

	while(repeat)
	{
		Delay_ms(500);
		repeat--;
	}

	if(remain)
	{
		Delay_ms(remain);
	}

}

/*** 串口打印相关重定向函数 ***********/
/**
  * @函  数  重定向putc函数，使用USART1
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART1->DR = (uint8_t) ch;

	/* Loop until the end of transmission */
	while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET)
	{}

	return ch;
}

/**
  * @函  数  重定向getc函数，使用USART1
  */
int fgetc(FILE *f)
{
	/* 等待串口1接收数据 */
	while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET)
	{}

	return (int)(uint8_t)(USART1->DR);
}

/******************* (C) 版权 2018 XTARK **************************************/
