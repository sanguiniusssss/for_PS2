/**
  ******************************************************************************
  * @file    ax_basis.h
  * @author  Musk Han @ XTARK
  * @version V1.1
  * @date    2018-01-01
  * @brief   X-CTR100基础外设驱动头文件
  *          LED、BEEP、按键开关、延时、UART1
  *
  ******************************************************************************
  * @xtark  实验平台：机器人成品 X-CTR100 控制器
  *          官网：http://www.xtark.cn
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_BASIS_H
#define __AX_BASIS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/*** X-CTR100 位带操作实现51GPIO方式 **********/
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014
#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010

//IO口操作
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入
#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入
#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入


/*** X-CTR100 基础功能驱动程序 **********/
//X-CTR100 初始化函数
void AX_Init(uint32_t baud);    //X-CTR100初始化

//X-CTR100 LED控制IO宏定义
#define AX_LEDG_Off()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET)      //LEDG绿色熄灭
#define AX_LEDG_On()        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)    //LEDG绿色点亮
#define AX_LEDG_Toggle()    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11)                   //LEDG绿色状态翻转
#define AX_LEDR_Off()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET)      //LEDR红色熄灭
#define AX_LEDR_On()        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET)    //LEDR红色点亮
#define AX_LEDR_Toggle()    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_10)                   //LEDR红色状态翻转

//X-CTR100 蜂鸣器控制IO宏定义
#define AX_BEEP_Off()       HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET)       //蜂鸣器关
#define AX_BEEP_On()        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET)     //蜂鸣器开
#define AX_BEEP_Toggle()    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6)                    //蜂鸣器状态翻转

//X-CTR100 读取按键开关状态函数
uint8_t AX_SW_GetSW12Status(void);   //SW选择开关状态
uint8_t AX_SW_GetSW1Status(void);    //SW1选择开关状态
uint8_t AX_SW_GetSW2Status(void);    //SW2选择开关状态

//X-CTR100 微秒级延时函数
void AX_Delayus(__IO uint16_t us);    //微秒级延时
void AX_Delayms(__IO uint16_t ms);    //毫秒级延时

#endif

/******************* (C) 版权 2018 XTARK **************************************/
