/*
 * ips200.c
 *
 *  Created on: 2024Äê3ÔÂ4ÈÕ
 *      Author: xiaoming
 */

#include "C_H.h"
#include "ips200.h"
#include "zf_common_headfile.h"

//extern int sanchachazhi;
//extern int16 speed1;
//extern int16 speed2;
//extern uint16 adcleft;
//extern uint16 adcmiddle;
//extern uint16 adcright;
//extern int16 Speed_Goal_l;
//extern int16 Speed_Goal_r;
//extern int ycircle;
//extern int xcircle;
//extern int axcircle;
//extern int aycircle;
//extern int bxcircle;
//extern int bycircle;
//extern float ang_l;
//extern int outacc_flag;
//extern int Speed_Goal;
int jishu = 0;
//extern int ceshi_flag;
void ips200 ()
{
    ips200_show_string(0, 90, "cha");
    ips200_show_float(30, 90, offset, 3, 3);
    ips200_show_int(0, 105, ImageFlag.image_element_rings_flag, 3);
    ips200_show_int(30, 105, ImageFlag.image_element_barriars_flag, 3);
//    ips200_show_string(0,105,"8l");
//    ips200_show_int(30,105,speed1,3);         ///°ËÁìÓò×ó¶ªÏß
//    ips200_show_string(0,120,"8r");
    ips200_show_int(30, 120, ImageStatus.Road_type, 3);
    ips200_show_string(0, 135, "ptl");
    ips200_show_int(30, 135, ImageStatus.Left_Line, 3);         //±ßÔµ×·Öð×ó¶ªÏß
    ips200_show_string(0, 150, "ptr");
    ips200_show_int(30, 150, ImageStatus.Right_Line, 3);         //±ßÔµ×·ÖðÓÒ¶ªÏß
//    ips200_show_string(0, 180, "ptc");
    ips200_show_float(30, 180, encoder_getval_cm(), 3, 2);
    ips200_show_string(0, 200, "z");
    ips200_show_int(30, 200, Zebra_start_flag, 3);
    ips200_show_int(60, 200, Zebra_flag, 3);
    ips200_show_string(0, 230, "ptc");
    ips200_show_int(30, 230, ImageStatus.OFFLine, 3);         //±ßÔµ×·Öð×ó¶ªÏß
//    ips200_show_float(0, 245, test1, 3, 3);
//    ips200_show_float(0, 260, k_add, 3, 3);
//    ips200_show_float(0, 275, circle_add, 3, 3);
    ips200_show_string(0, 245, "acc");
    ips200_show_int(30, 245, ImageStatus.straight_acc, 3);
    ips200_show_string(0, 260, "roll");
    ips200_show_float(80, 260, QEKF_INS.Roll, 3, 2);
    ips200_show_string(0, 275, "pitch");
    ips200_show_float(80, 275, QEKF_INS.Pitch, 3, 2);
    ips200_show_string(0, 290, "yaw");
    ips200_show_float(80, 290, QEKF_INS.Yaw, 3, 2);
//    ips200_show_string(0,165,"cir");
//    ips200_show_int(30,165,ImageFlag.image_element_rings_flag,1);
//    ips200_show_string(120,100,"fc");
//    ips200_show_int(80,105, ImageStatus.Road_type,3);  ¡¾       ///°ËÁìÓò×ó¶ªÏß
//    ips200_show_string(0,60,"l");
//    ips200_show_uint(30,60,adcleft,4);
////    ips200_show_string(60,75,"adm");
////    ips200_show_uint(90,75,adcmsun,4);
//    ips200_show_string(0,75,"r");
//    ips200_show_uint(30,75,adcright,4);
//    ips200_show_string(0,90,"sum");
//    ips200_show_uint(30,90,adcsum,4);

//    ips200_show_string(60,120,"sz");
//    ips200_show_uint(90,120,ImageStatus.Cross_ture_lenth*OX,3);
//    ips200_show_string(60,105,"p3x");
//    ips200_show_int(90,105,ImageStatus.pansancha_Lenth*OX,3);
    //ips200_show_string(60,120,"cr1");
    //ips200_show_uint(90,120,ImageStatus.Cirque1lenth*OX,4);

}

void drawleftline ()
{
    uint16 x;
    for (uint16 i = 0; i <= 59; i += 1)
    {
//        x = (uint16) ImageDeal[i].LeftBoundary_First;
        x = (uint16) ImageDeal[i].LeftBorder;
        ips200_draw_point(LimitL(x), i, RGB565_RED);
    }
}

void drawrightline ()
{
    uint16 x;
    for (uint16 i = 0; i <= 59; i += 1)
    {
//        x = (uint16) ImageDeal[i].RightBoundary_First;
        x = (uint16) ImageDeal[i].RightBorder;
        ips200_draw_point(x, i, RGB565_BLUE);
    }
}

void drawcenterline ()
{
    uint16 x;
    for (uint16 i = 0; i <= 58; i += 1)
    {

//        x=ImageDeal[i].Center;
        // x = (ImageDeal[i].RightBorder + ImageDeal[i].LeftBorder)/2;
        x = (ImageDeal[i].RightBorder + ImageDeal[i].LeftBorder) / 2;
        ips200_draw_point(x, i, RGB565_GREEN);
        if (jishu <= 58)
        {
//          wireless_uart_send_byte(x);
            jishu++;
        }

    }
}

void draw_math ()
{
    uint16 x;
    uint16 j = 0;
    uint16 y = 0;
    for (uint16 i = 4; i <= 58; i++)
    {
        x = (ImageDeal[i].RightBorder - ImageDeal[i].LeftBorder) / 2;
        ips200_show_int(j, y, x, 2);
        j += 15;

        if (j > 100)
        {
            j = 0;
            y += 15;
        }
    }
}
void drawoffline ()
{
    uint16 x;
    for (uint16 i = 0; i <= 79; i += 1)
    {
        x = ImageStatus.OFFLine;
        ips200_draw_point(i, x, RGB565_RED);
    }
}

void drawtowpointUP ()
{
    uint16 x;
    for (uint16 i = 0; i <= 79; i += 1)
    {
        x = ImageStatus.TowPoint - 5;
        ips200_draw_point(i, x, RGB565_CYAN);
    }
}

void drawtowpointDOWN ()
{
    uint16 x;
    for (uint16 i = 0; i <= 79; i += 1)
    {
        x = ImageStatus.TowPoint + 5;
        ips200_draw_point(i, x, RGB565_CYAN);
    }
}

