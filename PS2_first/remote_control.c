/**
  ******************************************************************************
  * @file    remote_control.c
  * @author  XTARK
  * @version V1.0
  * @date    2026-06-28
  * @brief   PS2遥控器封装实现 - 专业嵌入式架构风格
  ******************************************************************************
  */

#include "remote_control.h"
#include "ax_ps2.h"
#include "ax_basis.h"
#include "string.h"

/* 遥控器数据双缓冲 */
static RC_ctrl_t rc_ctrl[2];     // [0]:当前数据TEMP, [1]:上一次的数据LAST
static uint8_t rc_init_flag = 0; // 遥控器初始化标志位

/* 原始PS2数据缓冲区 */
static JOYSTICK_TypeDef ps2_raw_data;

/**
 * @brief 矫正遥控器摇杆的值,超过范围的值置0
 */
static void RectifyRCjoystick(void)
{
    int16_t *ch_ptr = (int16_t *)&rc_ctrl[TEMP].rc.rocker_r_x;
    
    for (uint8_t i = 0; i < 4; ++i) {  // 4个摇杆通道：r_x, r_y, l_x, l_y
        if (abs(ch_ptr[i]) > RC_CH_VALUE_RANGE) {
            ch_ptr[i] = 0;
        }
    }
}

/**
 * @brief PS2原始数据解析
 * 
 * @param raw PS2原始数据
 */
static void ps2_to_rc(const JOYSTICK_TypeDef *raw)
{
    // 保存上一次数据
    memcpy(&rc_ctrl[LAST], &rc_ctrl[TEMP], sizeof(RC_ctrl_t));
    
    // 模式信息 (使用 RC_ctrl_t 中的 mode 字段)
    rc_ctrl[TEMP].mode = raw->mode;
    
    // 判断连接状态 (正常模式值为0x41或0x73)
    rc_ctrl[TEMP].connected = (raw->mode == 0x41 || raw->mode == 0x73) ? 1 : 0;
    
    if (!rc_ctrl[TEMP].connected) {
        // 未连接时清空数据
        memset(&rc_ctrl[TEMP].rc, 0, sizeof(RC_Channel_t));
        return;
    }
    
    // 摇杆数据解析 (0-255 -> -100~100)
    int16_t lr, ud;
    
    // 右摇杆X
    lr = raw->RJoy_LR - RC_CH_VALUE_OFFSET;
    rc_ctrl[TEMP].rc.rocker_r_x = (int16_t)(lr * RC_CH_VALUE_RANGE / RC_CH_VALUE_OFFSET);
    
    // 右摇杆Y
    ud = raw->RJoy_UD - RC_CH_VALUE_OFFSET;
    rc_ctrl[TEMP].rc.rocker_r_y = -(int16_t)(ud * RC_CH_VALUE_RANGE / RC_CH_VALUE_OFFSET);
    
    // 左摇杆X
    lr = raw->LJoy_LR - RC_CH_VALUE_OFFSET;
    rc_ctrl[TEMP].rc.rocker_l_x = (int16_t)(lr * RC_CH_VALUE_RANGE / RC_CH_VALUE_OFFSET);
    
    // 左摇杆Y
    ud = raw->LJoy_UD - RC_CH_VALUE_OFFSET;
    rc_ctrl[TEMP].rc.rocker_l_y = -(int16_t)(ud * RC_CH_VALUE_RANGE / RC_CH_VALUE_OFFSET);
    
    // 拨轮暂未使用，设为0
    rc_ctrl[TEMP].rc.dial = 0;
    
    // 矫正摇杆值
    RectifyRCjoystick();
    
    // 方向键解析 (btn1)
    // btn1: SLCT(0) L3(1) R3(2) STRT(3) UP(4) DOWN(5) L(6) R(7)
    rc_ctrl[TEMP].rc.select = !(raw->btn1 & 0x01);
    rc_ctrl[TEMP].rc.l3 = !(raw->btn1 & 0x02);
    rc_ctrl[TEMP].rc.r3 = !(raw->btn1 & 0x04);
    rc_ctrl[TEMP].rc.start = !(raw->btn1 & 0x08);
    rc_ctrl[TEMP].rc.up = !(raw->btn1 & 0x10);
    rc_ctrl[TEMP].rc.down = !(raw->btn1 & 0x20);
    rc_ctrl[TEMP].rc.left = !(raw->btn1 & 0x40);
    rc_ctrl[TEMP].rc.right = !(raw->btn1 & 0x80);
    
    // 动作按键解析 (btn2)
    // btn2: L2(0) R2(1) L1(2) R1(3) △(4) ○(5) ×(6) □(7)
    rc_ctrl[TEMP].rc.l2 = !(raw->btn2 & 0x01);
    rc_ctrl[TEMP].rc.r2 = !(raw->btn2 & 0x02);
    rc_ctrl[TEMP].rc.l1 = !(raw->btn2 & 0x04);
    rc_ctrl[TEMP].rc.r1 = !(raw->btn2 & 0x08);
    rc_ctrl[TEMP].rc.triangle = !(raw->btn2 & 0x10);
    rc_ctrl[TEMP].rc.circle = !(raw->btn2 & 0x20);
    rc_ctrl[TEMP].rc.cross = !(raw->btn2 & 0x40);
    rc_ctrl[TEMP].rc.square = !(raw->btn2 & 0x80);
    
    // 开关位置 (使用方向键模拟)
    rc_ctrl[TEMP].rc.switch_left = rc_ctrl[TEMP].rc.left ? 1 : 0;
    rc_ctrl[TEMP].rc.switch_right = rc_ctrl[TEMP].rc.right ? 1 : 0;
}

/**
 * @brief 遥控器数据更新函数
 *        应在主循环或定时器中断中调用
 */
void RemoteControlUpdate(void)
{
    if (!rc_init_flag) return;
    
    // 读取PS2原始数据
    AX_PS2_ScanKey(&ps2_raw_data);
    
    // 解析数据
    ps2_to_rc(&ps2_raw_data);
}

/**
 * @brief 遥控器初始化
 * 
 * @return RC_ctrl_t* 遥控器数据指针
 */
RC_ctrl_t *RemoteControlInit(void)
{
    // 初始化PS2手柄
    AX_PS2_Init();
    
    // 初始化遥控器数据缓冲区
    memset(rc_ctrl, 0, sizeof(rc_ctrl));
    
    // 设置初始化标志
    rc_init_flag = 1;
    
    return rc_ctrl;
}

/**
 * @brief 检查遥控器是否在线
 * 
 * @return uint8_t 1=在线, 0=离线
 */
uint8_t RemoteControlIsOnline(void)
{
    if (!rc_init_flag) return 0;
    
    // 通过模式值判断连接状态
    return rc_ctrl[TEMP].connected;
}

/**
 * @brief 获取遥控器数据指针
 * 
 * @return RC_ctrl_t* 遥控器数据指针
 */
RC_ctrl_t *RemoteControlGetData(void)
{
    return rc_ctrl;
}

/******************* (C) 版权 2026 XTARK **************************************/