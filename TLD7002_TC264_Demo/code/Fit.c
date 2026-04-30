/*
 * Fit.c
 *
 *  Created on: 2024年7月24日
 *      Author: HP
 */
#include "zf_common_headfile.h"
uint8 used_offline = 0;
uint8 Center_first[8] = {0};
float TowPoint_1 = 10.1;
float TowPoint_2 = 15.1;
float TowPoint_3 = 15.1;
float TowPoint_std = 15.1;
float TowPoint_ramp = 10.1;

///extern int BlueTooth_Flag;
void Data_Settings (void)           //参数赋值
{

    ImageStatus.MiddleLine = mid_std;  //中线
    ImageStatus.TowPoint_Gain = 0.2;
    ImageStatus.TowPoint_Offset_Max = 5;
    ImageStatus.TowPoint_Offset_Min = -2;
    ImageStatus.TowPointAdjust_v = 160;
    ImageStatus.Det_all_k = 0.7;  //待定自动补线斜率
    ImageStatus.CirquePass = 'F';
    ImageStatus.IsCinqueOutIn = 'F';
    ImageStatus.CirqueOut = 'F';
    ImageStatus.CirqueOff = 'F';
    ImageStatus.Barn_Flag = 0;
    ImageStatus.straight_acc = 0;
    ImageStatus.TowPoint = (int) (TowPoint_1);           //前瞻  ///
    ImageStatus.Threshold_static = 80;   //静态阈值  40-80
    ImageStatus.Threshold_detach = 140;  //阳光算法  亮斑分离140-220
    ImageScanInterval = 2;               //扫边范围
    ImageScanInterval_Cross = 2;         //十字扫线范围
    ImageStatus.variance_acc = 25;       //直道检测
    ImageStatus.newblue_flag = 0;
}
void Slope_center_first_fit (void)
{
    float sum_x = 0.0;
    float sum_y = 0.0;
    float sum_x2 = 0.0;
    float sum_xy = 0.0;
    float ptr_l = 0, ptr_r = 0;
    if (used_offline == 0)
    {
        k_add = 0;
        return;
    }
    if (ImageStatus.straight_acc)
    {
        k_add = 0;
        return;
    }

    if (ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque && ImageStatus.Road_type != Ramp)
    {
        for (uint8 i = used_offline; i < used_offline + 8; i++)
        {
            Center_first[i - used_offline] = (ImageDeal[i].LeftBoundary_First + ImageDeal[i].RightBoundary_First) / 2;
//        printf("%d,%d\n\r",Center_first[i - used_offline],i);
            sum_x += i;
            sum_y += Center_first[i - used_offline];
            sum_x2 += i * i;
            sum_xy += i * Center_first[i - used_offline];
            if (ImageDeal[i].IsLeftFind == 'W')
            {
                ptr_l++;
            }
            if (ImageDeal[i].IsRightFind == 'W')
            {
                ptr_r++;
            }
        }
    }
    else if (ImageStatus.Road_type != Ramp)
    {
        for (uint8 i = used_offline; i < used_offline + 8; i++)
        {
            Center_first[i - used_offline] = (ImageDeal[i].LeftBorder + ImageDeal[i].RightBorder) / 2;
            //        printf("%d,%d\n\r",Center_first[i - used_offline],i);
            sum_x += i;
            sum_y += Center_first[i - used_offline];
            sum_x2 += i * i;
            sum_xy += i * Center_first[i - used_offline];
            if (ImageDeal[i].IsLeftFind == 'W')
            {
                ptr_l++;
            }
            if (ImageDeal[i].IsRightFind == 'W')
            {
                ptr_r++;
            }
        }
    }
    else
    {
        k_add = 0;
        return;
    }
    sum_x /= 8;
    sum_y /= 8;
    sum_x2 /= 8;
    sum_xy /= 8;
    k_add = -2 * (sum_xy - sum_x * sum_y) / (sum_x2 - sum_x * sum_x) + (ptr_r - ptr_l) / 8;
//    printf("%d,%f\n\r",used_offline,k_add);
//    printf("\n\r%f,%f,%f,%f\n\r",sum_x,sum_y,sum_x2,sum_xy);
}

