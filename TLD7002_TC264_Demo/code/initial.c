/*
 * initial.c
 *
 *  Created on: 2023年12月21日
 *      Author: HP
 */

#include "zf_common_headfile.h"
#include "C_H.h"
void data_init ()
{
//    temp_palstance = kp_palstance;
#if MENU_INIT
#else
    {
        kp_servo = 3.0;
        kd_servo = 10.0;
        kp_offset = 0.89;
        kd_offset = 0.5;
        angle_range = 30;
        kp_angle = 4.0;
        kp_angle_max = 6.5;
        kp_angle_min = 4.0;
        kp_angle_left = 4.0;
        kp_angle_right = 5.2;
        kd_angle = 16.0;
        kp_palstance = 46.5;
        ki_palstance = 1.58;
        kd_palstance = 0.0;
//        kp_angle = 4.0;
//        kd_angle = 12.5;
//        kp_palstance = 40.5;
//        ki_palstance = 1.58;
//        kd_palstance = 0.0;
        palstance_servo = 3800;
        palstance_speed = 5000;
        palstance_on = 5500;
        palstance_max = 6000;
        limit_pwm_palstance = 6000;
        pwm_start_run = 0;
        run = 3800;
        run_angle = 30;
        run_servo = 6000.0;
        run_normal = 3800.0;
        run_reduce = 3800.0;
        run_speed = 4800.0;
        run_on = 4300.0;
        servo_range = 30;
        kp_servo_max = 3.0;
        kp_servo_on = 2.0;
        kp_servo_speed = 2.0;
        kp_servo_balance = 0.0;
        kd_servo_balance = 0.0;
        kp_pl = 1600;
        kp_ph = 0.85;
        kp_pr = 0.005;
        kp_offset_limit = 0.95;
        offset_range = 10;
        angle_bias = -0.8;      //往左是负的，往右是正的，-angle_bias为期望

        exposure = 100.12;
        TowPoint_1 = 12.1;
        TowPoint_2 = 15.1;
        TowPoint_3 = 15.1;
        TowPoint_std = 23.1;
        Pitch_Ramp = -8.0;
        Zebra_break_first = 70.0;
        Repair_X_L = 23.1;
        Repair_X_R = 57.1;
        mid_std = 39;      //增大，offset减小，往左靠
        kp_offset_limit_normal = 1.2;
        Zebra_end_distance = 60.0;
        special_ring = 0.0;
        special_cross = 0.0;
        special_type = 0.0;
        kp_run = 30.0;
        ki_run = 5.0;
        kd_run = 4.0;
        Pitch_Ramp = -8;
        Pitch_Ramp_1 = 4;
        Pitch_Ramp_2 = 4;
        Pitch_Ramp_3 = -6;
    }
#endif
}
void initial_all (void)
{
    system_delay_ms(100);
    imu660ra_init();
    Attitude_Init();   //陀螺仪初始化
    gpio_init(P21_2, GPO, 0, GPO_PUSH_PULL);
    gpio_init(P21_4, GPO, 0, GPO_PUSH_PULL);
    gpio_init(Beep, GPO, 0, GPO_PUSH_PULL);
    pwm_init(ATOM0_CH1_P21_3, 500, 0);
    pwm_init(ATOM0_CH3_P21_5, 500, 0);
    mt9v03x_init();
    dot_matrix_screen_init();       // 点阵屏幕初始化
//    gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pwm_init(ATOM1_CH1_P33_9, 300, 822 * 6);    //900 775 650
    encoder_quad_new_init(TIM4_ENCODER, TIM4_ENCODER_CH1_P02_8, TIM4_ENCODER_CH2_P00_9);
    encoder_quad_new_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P33_7, TIM2_ENCODER_CH2_P33_6);

//    uart_init(UART_1, 9600, UART1_TX_P33_12, UART1_RX_P33_13);
    wireless_uart_init();
    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY2 输入 默认高电平 上拉输入
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY3 输入 默认高电平 上拉输入
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY4 输入 默认高电平 上拉输入
#if MENU_INIT
    menu_simp_init();
#else
    ips200_init(IPS200_TYPE_PARALLEL8);
#endif
    data_init();
    pit_ms_init(CCU60_CH1, 25);
    pit_ms_init(CCU61_CH0, 2);
    pit_ms_init(CCU61_CH1, 1);
    pit_ms_init(CCU60_CH0, 5);

//    dot_matrix_screen_show_string("OK");

//    flash_erase_page(0,1);

    //pit_init(CCU60_CH0, 5000);
}

