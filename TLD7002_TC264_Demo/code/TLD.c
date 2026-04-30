/*
 * TLD.c
 *
 *  Created on: 2024闂佽法鍠愰弸濠氬箯閿燂拷7闂佽法鍠愰弸濠氬箯閿燂拷5闂佽法鍠愰弸濠氬箯閿燂拷
 *      Author: HP
 */
// Normol,       //0:闂佸搫鍟版慨鎶藉箲閵忊剝濯撮柡鍥╁Т椤ユ锟介潧搴滈幏锟�
// Straight,     //1:闂佺儵鏅滈幐铏閿燂拷
// Cross,        //2:闂佸憡銇炵粈渚�鎮洪敓锟�
// Ramp,         //3:闂佺宥囩瘈濞存粣鎷�
// LeftCirque,   //4:閻庡綊娼荤粻鎴濄�掓ィ鍐╁仢闁跨噦鎷�
// RightCirque,  //5:闂佸憡鐟ラ崯鍨�掓ィ鍐╁仢闁跨噦鎷�
// LeftBarriar,  //6:閻庡綊娼荤粻鎾斥枔閹殿喖鏋堥柨鐕傛嫹
// RightBarriar, //7:闂佸憡鐟︽繛濠傗枔閹殿喖鏋堥柨鐕傛嫹
// Barn_out,     //8:闂佸憡鍨甸幖顐よ姳閿燂拷
// Barn_in,      //9:闂佺绻堥崕杈╄姳閿燂拷
// Cross_ture,
#include "zf_common_headfile.h"

uint8 run_flag = 0;
uint8 first_image = 0;
uint8 TLD_element_state[9] = {0};
char *TLD_element_state_strings[9][10] = {{"GO"}, {"ST"}, {"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9"},
        {"P0", "P1", "P2", "P3", "P4", "P5", "P6"}, {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"}, {"B0", "B1", "B2",
                "B3", "B4", "B5", "B6", "B7"}};
uint8 show_ips_mode = 0;
uint8 key_test_mode = 0;
uint8 end_dot_show = 0;
uint8 start_dot_show = 0;
float special_type = 0;
void state_turn_ts (void)
{

    if (ImageStatus.last_Road_type != ImageStatus.Road_type && run_flag == 1)
    {
        if (ImageStatus.Road_type == 2
                || ((ImageStatus.Road_type > 2 && ImageStatus.Road_type <= 7) && special_type < 10))
        {
            TLD_ts(ImageStatus.Road_type);
        }
    }
}
float special_dir_ts = 0;
float dir_ts_limit = 8;
void dir_turn_ts (void)
{
    if (run_flag && special_dir_ts < 10)
    {
        if (offset > dir_ts_limit)
        {
            dot_matrix_screen_show_direction('>');
        }
        else if (offset < -dir_ts_limit)
        {
            dot_matrix_screen_show_direction('<');
        }
        else
        {
            dot_matrix_screen_show_direction('^');
        }
    }
}

uint8 START_TLD_flag = 0;
char *START_TLD_strings[2] = {"OK", "NG"};
void START_TLD (void)
{
    dot_matrix_screen_set_brightness(5000);
    if (QEKF_INS.Roll < 10 && QEKF_INS.Roll > -10)
    {
        if (START_TLD_flag == 0)
        {
            dot_matrix_screen_show_string(START_TLD_strings[START_TLD_flag]);
            START_TLD_flag = 1;
        }
    }
    else if (START_TLD_flag)
    {
        dot_matrix_screen_show_string(START_TLD_strings[START_TLD_flag]);
        START_TLD_flag = 0;
    }

}
void mode_ts (void)
{
    if (mt9v03x_finish_flag)
    {
//        printf("Timer counted %lu us.\r\n", system_getval_us());

        if (!first_image)
        {
            system_delay_ms(100);
            first_image++;
            dot_matrix_screen_set_brightness(5000);
//            dot_matrix_screen_show_string("OK ");
            mt9v03x_finish_flag = 0;
//            printf("1111");
        }
        if (mt9v03x_finish_flag)
        {
            ImageStatus.last_Road_type = ImageStatus.Road_type;
            last_state = (uint8) ImageFlag.image_element_rings_flag;
            ImageProcess();
            mt9v03x_finish_flag = 0;
            state_turn_ts();
            dir_turn_ts();
//            printf("L\n\r");
//            for (int i = 0; i < 5; i++)
//            {
//                printf("%c,", Real_state[i][0]);
//            }
//            printf("\n\r");
//            for (int i = 0; i <60; i++)
//            {
//                printf("%d,",(ImageDeal[i].RightBoundary- ImageDeal[i].LeftBoundary)/2);
//            }
//            printf("");
//            printf("\n\r R\n\r");
//            for (int i = 0; i < 5; i++)
//            {
//                printf("%c,", Real_state[i][1]);
//            }
//            printf("\n\r");
//            for (int i = 59; i > 54; i--)
//            {
//                printf("%d,", ImageDeal[i].RightBoundary);
//            }
//            printf("\n\r");

        }
    }
    if (gpio_get_level(SWITCH1) && gpio_get_level(SWITCH2) && !run_flag)
    {

        ips200_clear();
        run_flag = 1;
        start_dot_show = 0;
        key_test_mode = 0;
    }
    else if (!run_flag)
    {
        START_TLD();
        start_dot_show = 0;
        key_test_mode = 0;
        run_flag = 0;
        if (gpio_get_level(SWITCH1))
        {
            start_dot_show = 1;
            if (!gpio_get_level(KEY1))
            {
                show_ips_mode = 0;
            }
            if (!gpio_get_level(KEY2))
            {
                show_ips_mode = 1;
            }
            if (show_ips_mode)
            {
//                ips200_full(RGB565_WHITE);
                ips200_show_gray_image(0, 0, Pixle[0], LCDW, LCDH, LCDW, LCDH, 1);             //濞存粌鑻敓鐣屽帶鐎垫煡宕堕幆褍鍓�
                drawleftline();     //闁汇垼顕ф稊蹇旀綇閸︻厼娈�
                drawrightline();    //闁汇垼顕цぐ鍛婃綇閸︻厼娈�
                drawcenterline();   //闁汇垼顔婇懙鎴犵棯閿燂拷
                drawoffline();      //闁汇垼顕уù姗�宕撹箛娑栵拷濠冩綇閿燂拷
                drawtowpointUP();   //闁汇垼顕ф晶鐘绘儔閺勫繐鐦遍柛銉х節缁楀倻鐥敓锟�
                drawtowpointDOWN(); //闁汇垼顕ф晶鐘绘儔閺勫繐鐦遍柛銉х節缁楀懐鐥敓锟�
            }
            else
            {
//                ips200_full(RGB565_WHITE);
                ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
//                for (int i = 0; i < 188; i++)
//                {
//                    ips200_draw_point(i, 28, RGB565_RED);
//                    ips200_draw_point(i, 8, RGB565_RED);
//
//                }
            }
            ips200();
        }
        else if (gpio_get_level(SWITCH2))
        {
            key_test_mode = 1;
        }
        else
        {
            key_test_mode = 0;
        }
    }
}
void TLD_ts (uint8 ts_state)
{
    static uint8 TS_LINE[11] = {0, 1, 2, 3, 4, 4, 5, 5, 6, 7, 8};
    ts_state = TS_LINE[ts_state];

    TLD_element_state[ts_state]++;
    dot_matrix_screen_show_element(TLD_element_state_strings[ts_state][TLD_element_state[ts_state]]);
}
