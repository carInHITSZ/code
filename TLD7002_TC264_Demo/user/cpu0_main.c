/*********************************************************************************************************************
 * TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 TC264 开源库的一部分
 *
 * TC264 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          cpu0_main
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          ADS v1.9.4
 * 适用平台          TC264D
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 接线定义：
 *                  ------------------------------------
 *                  TLD7002驱动模块      单片机管脚
 *                  SR0                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR0_PIN宏定义
 *                  SR1                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR1_PIN宏定义
 *                  SR2                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR2_PIN宏定义
 *                  SR3                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR3_PIN宏定义
 *                  SR4                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR4_PIN宏定义
 *                  SR5                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR5_PIN宏定义
 *                  SR6                 查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SR6_PIN宏定义
 *                  SYNC                查看zf_device_dot_matrix_screen.h 中 DOT_MATRIX_SCREEN_SYNC_PIN宏定义
 *                  RX                  查看 zf_device_tld7002.h 中 TLD7002_UART_RX 宏定义
 *                  HSLIL               查看 zf_device_tld7002.h 中 TLD7002_UART_HLSIL 宏定义
 *                  HSLIH               悬空
 *                  GPIN0               查看 zf_device_tld7002.h 中 TLD7002_GPIN0_PIN 宏定义
 *                  GPIN1               悬空
 *                  VCC                 8-15V电源
 *                  3V3                 3.3V电源（给模块上的同步电路供电）
 *                  GND                 电源地
 *                  ------------------------------------
 * 修改记录
 * 日期              作者                备注
 * 2024-03-12       seekfree            first version
 ********************************************************************************************************************/
#include "zf_common_headfile.h"
#include "zf_device_tld7002.h"
#include "zf_device_dot_matrix_screen.h"

#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

const char temp_string[] =
        " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}";

int stime;
int flag = 0;
int flag_int = 0;
uint8 pit_10ms_flag;
uint8 pit_500ms_flag;
uint8 duty_dir = 1;       // 占空比改变的方向
uint8 display_mode = 0; // 显示模式 0:轮流将4个模式进行显示  1：依次显示OK 、NG、 A1、 B2  2：显示字符666，并带有呼吸效果 3：显示字符888，并带有闪烁效果  4：依次显示取模中所有的字符
uint16 led_duty = 1500;    // 呼吸灯模式时 控制点阵亮度

// **************************** 代码区域 ****************************

int core0_main (void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    system_delay_ms(100);           // 适当的延时后在进行初始化
    initial_all();
    QEKF_INS_start();
    Data_Settings();   // 参数给定
    mt9v03x_set_exposure_time((int) (exposure));  // 曝光时间300
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {

    }
}
#pragma section all restore
// **************************** 代码区域 ****************************
