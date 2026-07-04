/**
  ******************************************************************************
  * @file    ax_ps2.c
  * @author  Musk Han @ XTARK
  * @version V1.0
  * @date    2018-01-01
  * @brief   X-CTR100 PS2游戏手柄驱动文件
  *
  ******************************************************************************
  * @xtark  实验平台：机器人成品 X-CTR100 控制器
  *          官网：http://www.xtark.cn
  *
  ******************************************************************************
  */

#include "ax_ps2.h"

//PS2手柄接口IO宏定义
#define DATA_read()  HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6)
#define CM_H()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET)
#define CM_L()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET)
#define ATT_H()      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET)
#define ATT_L()      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET)
#define CLK_H()      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET)
#define CLK_L()      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET)
#define ACK_read()   HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3)

const uint8_t PS2_cmnd[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //PS2指令数据
static uint8_t PS2_data[9] = {0};//接收数据
static uint8_t ps2_ack_error = 0;  //ACK应答错误计数（本次扫描中丢失应答的字节数）

/**
  * @函  数  PS2初始化
  * @参  数  无
  * @返回值  无
  */
void AX_PS2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* 初始化DWT周期计数器，替代SysTick做高精度延时 */
	AX_DWT_Init();

	__HAL_RCC_GPIOD_CLK_ENABLE();

	//DATA 手柄SPI数据输出  输入
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//COMMMAND 手柄SPI数据输入  输出
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//ATT 手柄SPI选通使能 输出
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//CLOCK 手柄SPI时钟  输出
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//ACK 手柄SPI应答信号  输入
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


/**
  * @函  数  PS2读写一个字节
  * @参  数  data:发送的数据
  * @返回值  接收的数据
  */
static uint8_t PS2_ReadWriteData(uint8_t data)
{
	uint8_t ref, res = 0;

	for(ref = 0x01; ref > 0x00; ref <<= 1)
	{
		CLK_L();
		if(ref&data)
			CM_H();
		else
			CM_L();
		AX_Delayus(5);
		CLK_H();
		if(DATA_read())
			res |= ref;
		AX_Delayus(5);
	}
	CM_H();

	/* 字节传输后检查ACK应答信号（低电平有效） */
	AX_Delayus(5);
	if (ACK_read())  // ACK应为低，读高说明手柄未应答
	{
		ps2_ack_error++;
	}

	//返回接收数据
    return res;
}

/**
  * @函  数  PS2读取数据并解析
  * @参  数  *JoystickStruct 摇杆数据结构体
  * @返回值  无
  */
void AX_PS2_ScanKey(JOYSTICK_TypeDef *JoystickStruct)
{
	uint8_t i;

	ps2_ack_error = 0;  //复位ACK错误计数

	ATT_L();
	AX_Delayus(1);
	for(i=0; i<9; i++)
	{
		PS2_data[i] = PS2_ReadWriteData(PS2_cmnd[i]);
		AX_Delayus(16);
	}

	ATT_H();

	JoystickStruct->mode = PS2_data[1];
	JoystickStruct->btn1 = PS2_data[3];
	JoystickStruct->btn2 = PS2_data[4];
	JoystickStruct->RJoy_LR = PS2_data[5];
	JoystickStruct->RJoy_UD = PS2_data[6];
	JoystickStruct->LJoy_LR = PS2_data[7];
	JoystickStruct->LJoy_UD = PS2_data[8];
}

/**
  * @函  数  获取本次PS2扫描的ACK应答状态
  * @参  数  无
  * @返回值  本次扫描中丢失ACK应答的字节数（0=全部正常）
  */
uint8_t AX_PS2_GetAckError(void)
{
	return ps2_ack_error;
}

/******************* (C) 版权 2018 XTARK **************************************/
