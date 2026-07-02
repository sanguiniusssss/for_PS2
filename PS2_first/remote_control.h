/**
  ******************************************************************************
  * @file    remote_control.h
  * @author  XTARK
  * @version V1.0
  * @date    2026-06-28
  * @brief   PS2遥控器封装接口 - 专业嵌入式架构风格
  ******************************************************************************
  */

#ifndef __REMOTE_CONTROL_H
#define __REMOTE_CONTROL_H

#include "stm32f4xx.h"

/* 遥控器通道值定义 */
#define RC_CH_VALUE_OFFSET 128      // 摇杆中位偏移
#define RC_CH_VALUE_MAX    255      // 摇杆最大值
#define RC_CH_VALUE_MIN    0        // 摇杆最小值
#define RC_CH_VALUE_RANGE  100      // 归一化范围



/* 遥控器通道数据结构体 */
typedef struct {
    int16_t rocker_r_x;             // 右摇杆X轴 (-100~100)
    int16_t rocker_r_y;             // 右摇杆Y轴 (-100~100)
    int16_t rocker_l_x;             // 左摇杆X轴 (-100~100)
    int16_t rocker_l_y;             // 左摇杆Y轴 (-100~100)
    int16_t dial;                   // 拨轮 (-100~100)
    
    uint8_t switch_right;           // 右开关 (0左 1右)
    uint8_t switch_left;            // 左开关 (0左 1右)
    
    // PS2特殊按键
    uint8_t select;                 // Select键
    uint8_t start;                  // Start键
    uint8_t l3;                     // 左摇杆按下
    uint8_t r3;                     // 右摇杆按下
    
    // 动作按键
    uint8_t triangle;               // △
    uint8_t circle;                 // ○
    uint8_t cross;                  // ×
    uint8_t square;                 // □
    
    // 肩部按键
    uint8_t l1;
    uint8_t r1;
    uint8_t l2;
    uint8_t r2;
    
    // 方向键
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
} RC_Channel_t;

/* 遥控器控制数据结构体 */
typedef struct {
    RC_Channel_t rc;                // 通道数据
    uint8_t mode;                   // 遥控器模式
    uint8_t connected;              // 连接状态
} RC_ctrl_t;

/* 数据缓冲区索引 */
#define TEMP 0                      // 当前数据
#define LAST 1                      // 上一次数据

/* 遥控器初始化 */
RC_ctrl_t *RemoteControlInit(void);

/* 检查遥控器是否在线 */
uint8_t RemoteControlIsOnline(void);

/* 获取遥控器数据指针 */
RC_ctrl_t *RemoteControlGetData(void);

#endif

/******************* (C) 版权 2026 XTARK **************************************/