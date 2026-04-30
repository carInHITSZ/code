/*
 * C_H.c
 *
 *  Created on: 2024閿熸枻鎷�3閿熸枻鎷�4閿熸枻鎷�
 *      Author: xiaoming
 */

#include "zf_common_headfile.h"
#include "stdlib.h"
#include "C_H.h"
#include <math.h>
#include <stdio.h>
#include "Fit.h"

uint8 Stop_car_Flag = 0;
int ImageScanInterval;                         //鎵敓绔嚖鎷峰洿    閿熸枻鎷蜂竴閿熷彨鐨勮竟鏂ゆ嫹+-ImageScanInterval
int ImageScanInterval_Cross;                   //270閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍗侀敓琛楃鎷锋壂閿熺鍑ゆ嫹鍥�
uint8 Image_Use[LCDH][LCDW];          //閿熸彮璁规嫹鍥鹃敓鏂ゆ嫹
uint8 Pixle[LCDH][LCDW];              //閿熸枻鎷烽敓鑺傝揪鎷烽敓鏂ゆ嫹閿熶茎璁规嫹鍊奸敓鏂ゆ嫹鍥鹃敓鏂ゆ嫹
static int Ysite = 0, Xsite = 0;                   //Y閿熸枻鎷烽敓鏂ゆ嫹=閿熸枻鎷�
static uint8 *PicTemp;                             //閿熸枻鎷烽敓鑺ュ崟閿熸枻鎷峰浘閿熸枻鎷�
static int IntervalLow = 0, IntervalHigh = 0;      //閿熸枻鎷烽敓鏂ゆ嫹鍙╅敓缂搭煉鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
static int ytemp = 0;                              //閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
static int TFSite = 0, FTSite = 0;                 //閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
static float DetR = 0, DetL = 0;                   //閿熸枻鎷烽敓鍙唻鎷烽敓锟�
static int BottomBorderRight = 79,                 //59閿熸枻鎷烽敓鎻竟鏂ゆ嫹
        BottomBorderLeft = 0,                              //59閿熸枻鎷烽敓鏂ゆ嫹鍛撻敓锟�
        BottomCenter = 0;                                  //59閿熸枻鎷烽敓鍙鎷�
int Real_state[5][2];
DIRECTION DIR_1[400];                              //閿熺殕鐧告嫹閿熸枻鎷烽敓鐭鎷烽敓渚ュ嚖鎷烽敓鏂ゆ嫹
DIRECTION DIR_1_Y[400];
DIRECTION DIR_1_X[400];
DIRECTION DIR_LACK;
uint32 dir_stop;
ImageDealDatatypedef ImageDeal[60];                //閿熸枻鎷峰綍閿熸枻鎷烽敓鍙鎷烽敓鏂ゆ嫹鎭�
ImageStatustypedef ImageStatus;                    //鍥鹃敓鏂ゆ嫹閿熼ズ顐嫹鐩撮敓鏂ゆ嫹閿燂拷
ImageStatustypedef ImageData;             ///////閿熸枻鎷疯閿熺潾鏀圭鎷峰浘閿熸枻鎷烽敓鏂ゆ嫹鍊奸敓鏂ゆ嫹閿熸枻鎷�
SystemDatatypdef SystemData;
ImageFlagtypedef ImageFlag;
int Left_RingsFlag_Point1_Ysite, Left_RingsFlag_Point2_Ysite;   //閿熸枻鎷峰渾閿熸枻鎷烽敓鍙柇纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
int Right_RingsFlag_Point1_Ysite, Right_RingsFlag_Point2_Ysite; //閿熸枻鎷峰渾閿熸枻鎷烽敓鍙柇纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
int Point_Xsite, Point_Ysite;                   //閿熺Ц纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
int Repair_Point_Xsite, Repair_Point_Ysite;     //閿熸枻鎷烽敓绔鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
int forklenth;                                 //纭敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浣嶉敓鏂ゆ嫹
int barnlenth;                                 //纭敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浣嶉敓鏂ゆ嫹
int ramplenth;                                 //纭敓鏂ゆ嫹閿熼摪纰夋嫹浣嶉敓鏂ゆ嫹
int ringslenth;
//float Weighting[10] = {1, 0.96, 0.88, 0.83, 0.77, 0.73, 0.71, 0.65, 0.59, 0.53}; //10閿熸枻鎷锋潈閿熸埅璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋ā閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎺ュ府鎷锋顒婃嫹閿熸枻鎷锋幊閿熸枻鎷烽敓鏂ゆ嫹閿熸暀顒婃嫹妞嶉敓鏂ゆ嫹閿熸枻鎷烽敓锟�
float Weighting[10] = {0.53, 0.59, 0.65, 0.71, 0.73, 0.77, 0.83, 0.88, 0.96, 1}; //10閿熸枻鎷锋潈閿熸埅璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋ā閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎺ュ府鎷锋顒婃嫹閿熸枻鎷锋幊閿熸枻鎷烽敓鏂ゆ嫹閿熸暀顒婃嫹妞嶉敓鏂ゆ嫹閿熸枻鎷烽敓锟�

uint8 ExtenLFlag = 0;  //閿熻鍑ゆ嫹閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹蹇�
uint8 ExtenRFlag = 0;  //閿熻鍑ゆ嫹閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹蹇�
float ang_l;
int ycircle = 0;
int xcircle = 0;
int axcircle = 0;
int aycircle = 0;
int bxcircle = 0;
int bycircle = 0;
float K;
uint8 Half_Road_Wide[60] =                      //鐩撮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
        {7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 20,
                21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 30, 30, 30, 31, 31, 32, 33, 33,
                33, 33, 33, 34, 35};                      //一車

uint8 Half_Bend_Wide[60] =                      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        {33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 30, 30, 29, 29, 28, 27, 28, 27, 27, 26, 26, 25, 25, 24,
                24, 23, 22, 21, 21, 22, 22, 22, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30,
                30, 31, 31, 32, 32, 33, };

uint8 buxianwide[60] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
        59, 60, 61, 62, 63, 64, 65};

int adczongzhi;
int tuoluoyijifen;                      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
uint8 last_state = 0;
float circle_add = 0;
float exposure = 60.1;
float special_ring = 0;

int Limit (int num, int numH, int numL)
{
    if (num > numH)
        num = numH;
    if (num < numL)
        num = numL;
    return num;
}

/*****************鐩撮敓鏂ゆ嫹閿熷彨璁规嫹******************/
float Straight_Judge_1 (uint8 dir, uint8 start, uint8 end)     //閿熸枻鎷烽敓鎴枻鎷烽敓鍙槄鎷烽敓锟�1閿熸枻鎷蜂负鐩撮敓鏂ゆ嫹
{
//    sqrt(circle_add);
    int i;
    float S = 0, Sum = 0, Err = 0, k = 0;
    switch (dir)
    {
        case 1 :
            k = (float) (ImageDeal[start].LeftBorder - ImageDeal[end].LeftBorder) / (start - end);
            for (i = 0; i < end - start; i++)
            {
                Err = (ImageDeal[start].LeftBorder + k * i - ImageDeal[i + start].LeftBorder)
                        * (ImageDeal[start].LeftBorder + k * i - ImageDeal[i + start].LeftBorder);
                Sum += Err;
            }
            S = Sum / (end - start);
            break;
        case 2 :
            k = (float) (ImageDeal[start].RightBorder - ImageDeal[end].RightBorder) / (start - end);
            for (i = 0; i < end - start; i++)
            {
                Err = (ImageDeal[start].RightBorder + k * i - ImageDeal[i + start].RightBorder)
                        * (ImageDeal[start].RightBorder + k * i - ImageDeal[i + start].RightBorder);
                Sum += Err;
            }
            S = Sum / (end - start);
            break;
    }
    return S;
}
float Straight_Judge_2 (R_STATE LR, uint8 R2_S, uint8 R2_E) // 閿熸枻鎷烽敓鎴枻鎷烽敓鍙槄鎷烽敓锟�1閿熸枻鎷蜂负鐩撮敓鏂ゆ嫹
{

    int16_t i = 0;
    float k = 0;
    float b = 0;
    float S = 0, Sum = 0;
    uint8 end = R2_E;
    float lack_num = 0;
    if (LR == LEFT)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].IsLeftFind == 'T')
            {
                break;
            }
        }
        end = (uint8) i;
        if (end < R2_S)
        {
            return 1;
        }
        if (end - R2_S < 8)
        {
            return 1;
        }
//        if ((float) (end - R2_S) / (R2_E - R2_S) < 0.7)
//        {
//            return 1;
//        }
        k = (float) (ImageDeal[end].LeftBorder - ImageDeal[R2_S].LeftBorder) / (end - R2_S);
        b = -k * end + ImageDeal[end].LeftBorder;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].LeftBorder - (k * i + b)) >= 2 || (ImageDeal[i].LeftBorder - (k * i + b)) <= -2)
            {
                Sum++;
            }
            if (ImageDeal[R2_S].IsLeftFind == 'T' && ImageDeal[i].IsLeftFind == 'W' && ImageDeal[i].IsLeftFind == 'W')
            {
                return 1;
            }
            if (ImageDeal[i].LeftBoundary_First <= 3)
            {
                lack_num++;
            }
        }
        S = Sum / (end - R2_S);
//        if (lack_num / (end - R2_S) > 0.3)
//        {
//            return 1;
//        }
        uint16 loop = 0;
        for (loop = 4; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].LEFT == end)
                break;
        }
        for (; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].LEFT == R2_S)
                break;
            if (DIR_1[loop].LEFT == 3 && DIR_1[loop + 1].LEFT == 3)
            {
                return 1;
            }
        }
    }
    if (LR == RIGHT)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].IsRightFind == 'T')
            {
                break;
            }
        }
        end = (uint8) i;
        if (end < R2_S)
        {
            return 1;
        }
        if (end - R2_S < 8)
        {
            return 1;
        }
//        if ((float) (end - R2_S) / (R2_E - R2_S) < 0.7&&!)
//        {
////            printf("sss");
//            return 1;
//        }
        k = (float) (ImageDeal[end].RightBorder - ImageDeal[R2_S].RightBorder) / (end - R2_S);
        b = -k * end + ImageDeal[end].RightBorder;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].RightBorder - (k * i + b)) >= 2 || (ImageDeal[i].RightBorder - (k * i + b)) <= -2)
            {
                Sum++;
            }
            if (ImageDeal[R2_S].IsRightFind == 'T' && ImageDeal[i].IsRightFind == 'W'
                    && ImageDeal[i + 1].IsRightFind == 'W')
            {
                return 1;
            }
        }
        S = Sum / (end - R2_S);
//        if (lack_num / (end - R2_S) > 0.3)
        //        {
        //            return 1;
        //        }
        uint16 loop = 0;
        for (loop = 4; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].RIGHT == end)
                break;
        }
        for (; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].RIGHT == R2_S)
                break;
            if (DIR_1[loop].RIGHT == 1 && DIR_1[loop + 1].RIGHT == 1)
            {
                return 1;
            }
            if (ImageDeal[i].RightBoundary_First >= 78)
            {
                lack_num++;
            }
        }
//

    }
    if (LR == CENTER)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].IsLeftFind == 'T' && ImageDeal[i].IsRightFind == 'T')
            {
                break;
            }
        }
        end = (uint8) i;
        if (end < R2_S)
        {
            return 1;
        }
        if (end - R2_S < 8)
        {
            return 1;
        }
        if ((float) (end - R2_S) / (R2_E - R2_S) < 0.7)
        {
            return 1;
        }
        k = (float) (ImageDeal[end].Center - ImageDeal[R2_S].Center) / (end - R2_S);
        b = -k * end + ImageDeal[end].Center;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].Center - (k * i + b)) >= 2 || (ImageDeal[i].Center - (k * i + b)) <= -2)
            {
                Sum++;
            }
            if ((ImageDeal[R2_S].IsLeftFind == 'T' && ImageDeal[i].IsLeftFind == 'W'
                    && ImageDeal[i + 1].IsLeftFind == 'W')
                    || (ImageDeal[R2_S].IsRightFind == 'T' && ImageDeal[i].IsRightFind == 'W'
                            && ImageDeal[i + 1].IsRightFind == 'W'))
            {
                return 1;
            }
        }
        S = Sum / (end - R2_S);
    }
    return S;
}
float Straight_Judge_3 (R_STATE LR, uint8 R2_S, uint8 R2_E) // 閿熸枻鎷烽敓鎴枻鎷烽敓鍙槄鎷烽敓锟�1閿熸枻鎷蜂负鐩撮敓鏂ゆ嫹
{

    int16_t i = 0;
    float k = 0;
    float b = 0;
    float S = 0, Sum = 0;
    uint8 end = R2_E;
    float lack_num = 0;
    if (LR == LEFT)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].LeftBoundary_First >= 4)
            {
                break;
            }
        }
        end = (uint8) i;
        if (end < R2_S)
        {
            return 1;
        }
        if (end - R2_S < 8)
        {
            return 1;
        }
        if ((float) (end - R2_S) / (R2_E - R2_S) < 0.7)
        {
            return 1;
        }
        k = (float) (ImageDeal[end].LeftBoundary_First - ImageDeal[R2_S].LeftBoundary_First) / (end - R2_S);
        b = -k * end + ImageDeal[end].LeftBoundary_First;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].LeftBoundary_First - (k * i + b)) >= 2
                    || (ImageDeal[i].LeftBoundary_First - (k * i + b)) <= -2)
            {
                Sum++;
            }
            if (ImageDeal[R2_S].LeftBoundary_First >= 4 && ImageDeal[i].LeftBoundary_First <= 4
                    && ImageDeal[i].LeftBoundary_First <= 4)
            {
                return 1;
            }
            if (ImageDeal[i].LeftBoundary_First <= 3)
            {
                lack_num++;
            }
        }
        S = Sum / (end - R2_S);
        //        if (lack_num / (end - R2_S) > 0.3)
        //        {
        //            return 1;
        //        }
        uint16 loop = 0;
        for (loop = 4; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].LEFT == end)
                break;
        }
        for (; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].LEFT == R2_S)
                break;
            if (DIR_1[loop].LEFT == 3 && DIR_1[loop + 1].LEFT == 3)
            {
                return 1;
            }
        }
    }
    if (LR == RIGHT)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].RightBoundary_First <= 77)
            {
                break;
            }
        }
        end = (uint8) i;
        if (end < R2_S)
        {
            return 1;
        }
        if (end - R2_S < 8)
        {
            return 1;
        }
        if ((float) (end - R2_S) / (R2_E - R2_S) < 0.7)
        {
            return 1;
        }
        k = (float) (ImageDeal[end].RightBoundary_First - ImageDeal[R2_S].RightBoundary_First) / (end - R2_S);
        b = -k * end + ImageDeal[end].RightBoundary_First;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].RightBoundary_First - (k * i + b)) >= 2
                    || (ImageDeal[i].RightBoundary_First - (k * i + b)) <= -2)
            {
                Sum++;
            }
            if (ImageDeal[R2_S].RightBoundary_First <= 77 && ImageDeal[i].RightBoundary_First >= 77
                    && ImageDeal[i + 1].RightBoundary_First >= 77)
            {
                return 1;
            }
        }
        S = Sum / (end - R2_S);
        //        if (lack_num / (end - R2_S) > 0.3)
        //        {
        //            return 1;
        //        }
        uint16 loop = 0;
        for (loop = 4; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].RIGHT == end)
                break;
        }
        for (; loop < dir_stop - 5; loop++)
        {
            if (DIR_1_Y[loop].RIGHT == R2_S)
                break;
            if (DIR_1[loop].RIGHT == 1 && DIR_1[loop + 1].RIGHT == 1)
            {
                return 1;
            }
            if (ImageDeal[i].RightBoundary_First >= 78)
            {
                lack_num++;
            }
        }
        //
    }
    return S;
}
Con_STATE Concave_Convex_Judge (R_STATE LR, uint8 R2_S, uint8 R2_E)    //閿熸枻鎷烽敓鎴枻鎷烽敓鍙槄鎷烽敓锟�1閿熸枻鎷蜂负鐩撮敓鏂ゆ嫹
{

    int16_t i = 0;
    float k = 0;
    float b = 0;
    float S = 0, Sum1 = 0, Sum2 = 0, Sum = 0;
    uint8 end = R2_E;
    if (LR == LEFT)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].IsLeftFind == 'T')
            {
                break;
            }
        }
        end = (uint8) i;
        k = (float) (ImageDeal[end].LeftBorder - ImageDeal[R2_S].LeftBorder) / (end - R2_S);
        b = -k * end + ImageDeal[end].LeftBorder;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].LeftBorder - (k * i + b)) >= 2)
            {
                Sum1++;
            }
            else if ((ImageDeal[i].LeftBorder - (k * i + b)) <= -2)
            {
                Sum2++;
            }

        }
        Sum = Sum1 + Sum2;
        S = Sum / (end - R2_S);
        if (S > 0.1)
        {
            if (Sum1 / Sum > 0.6)
            {
                return Convex;
            }
            else if (Sum2 / Sum > 0.6)
            {
                return Concave;

            }
        }
    }
    if (LR == RIGHT)
    {
        for (i = R2_E; i > ImageStatus.OFFLine; i--)
        {
            if (ImageDeal[i].IsRightFind == 'T')
            {
                break;
            }
        }
        end = (uint8) i;
        k = (float) (ImageDeal[end].RightBorder - ImageDeal[R2_S].RightBorder) / (end - R2_S);
        b = -k * end + ImageDeal[end].RightBorder;
        for (i = R2_S; i < end; i++)
        {
            if ((ImageDeal[i].RightBorder - (k * i + b)) >= 2)
            {
                Sum2++;
            }
            else if ((ImageDeal[i].RightBorder - (k * i + b)) <= -2)
            {
                Sum1++;
            }
        }
        Sum = Sum1 + Sum2;
        S = Sum / (end - R2_S);
        if (S > 0.1)
        {
            if (Sum1 / Sum > 0.6)
            {
                return Convex;
            }
            else if (Sum2 / Sum > 0.6)
            {
                return Concave;

            }
        }
    }
    return NO;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      閿熸枻鎷峰皬閿熸枻鎷烽敓鍓垮嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//  @param      start    閿熸枻鎷峰閿熸枻鎷�
//  @param      num      閿熸枻鎷烽敓鎹烽潻鎷烽敓鏂ゆ嫹
//  @return     void
//  @since      v1.0
//  Sample usage:  Fit1(119,20);
//-------------------------------------------------------------------------------------------------------------------
void Fit1 (int start, int num)
{
    float sum_x = 0.0;
    float sum_y = 0.0;
    float sum_x2 = 0.0;
    float sum_xy = 0.0;
    float a, b;
    float y = 0;

    for (int i = 0; i < num; ++i)
    {
        sum_x += y++;
        sum_y += ImageDeal[start - num + i].Center;
        sum_x2 += (y - 1) * (y - 1);
        sum_xy += (y - 1) * ImageDeal[start - num + i].Center;
    }

    sum_x /= num;
    sum_y /= num;
    sum_x2 /= num;
    sum_xy /= num;

    a = (sum_xy - sum_x * sum_y) / (sum_x2 - sum_x * sum_x);
    b = (sum_x2 * sum_y - sum_x * sum_xy) / (sum_x2 - sum_x * sum_x);
    for (int i = 0; i < num; i++)
    {
        ImageDeal[start - num + i].Center = a * i + b;
    }
}

float Get_angle (float Ax, float Ay, float Bx, float By, float Cx, float Cy)
{

    float BA = 0.00;     //閿熸枻鎷烽敓鏂ゆ嫹BA閿熸枻鎷锋ā
    float BC = 0.00;
    float SBA_BC = 0.00;     //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯閿熻锟�
    float angle = 0.00;

    BA = sqrt((Ax - Bx) * (Ax - Bx) + (Ay - By) * (Ay - By));
    BC = sqrt((Cx - Bx) * (Cx - Bx) + (Cy - By) * (Cy - By));

    SBA_BC = (Ax - Bx) * (Cx - Bx) + (Ay - By) * (Cy - By);

    angle = acos(SBA_BC * 1.00 / (BA * BC));

    return angle * 57.3;
}

float Fit1_k (int start, int num)
{
    float sum_x = 0.0;
    float sum_y = 0.0;
    float sum_x2 = 0.0;
    float sum_xy = 0.0;
    float a, b;
    float y = 0;
    uint16 middle_NO[60] = {0};

    for (int i = 0; i < num; ++i)
    {
        sum_x += y++;
        sum_y += ImageDeal[start - num + i].Center;
        sum_x2 += (y - 1) * (y - 1);
        sum_xy += (y - 1) * ImageDeal[start - num + i].Center;
    }

    sum_x /= num;
    sum_y /= num;
    sum_x2 /= num;
    sum_xy /= num;

    a = (sum_xy - sum_x * sum_y) / (sum_x2 - sum_x * sum_x);
    b = (sum_x2 * sum_y - sum_x * sum_xy) / (sum_x2 - sum_x * sum_x);
    for (int i = 0; i < num; i++)
    {
        middle_NO[start - num + i] = a * i + b;
    }

    float Y = (float) (59 - ImageStatus.OFFLine + 10 - 1 - 59 - ImageStatus.OFFLine + 1);               //閿熸枻鎷穀閿熺粨闀块敓鏂ゆ嫹
    float X = middle_NO[59 - ImageStatus.OFFLine + 10 - 1] - middle_NO[59 - ImageStatus.OFFLine + 1]; //閿熸枻鎷稾閿熺粨闀块敓鏂ゆ嫹(閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�)
    float k = 1.00 * (X / Y);                             //閿熸枻鎷锋枩閿熸枻鎷�(閿熸枻鎷穣閿熸枻鎷蜂负閿熸枻鎷�)
    if (k < 0)
        k = k * -1;
    return k;
}

float Mh = MT9V03X_H;
float Lh = LCDH;
float Mw = MT9V03X_W;
float Lw = LCDW;

void compressimage ()
{
    int i, j, row, line;
    const float div_h = Mh / Lh, div_w = Mw / Lw;

    for (i = 0; i < LCDH; i++)
    {
        row = i * div_h + 0.5;
        for (j = 0; j < LCDW; j++)
        {
            line = j * div_w + 0.5;
            Image_Use[i][j] = mt9v03x_image[row][line];
        }
    }
    mt9v03x_finish_flag = 0;  //浣块敓鏂ゆ嫹閿熸枻鎷蜂竴甯MA閿熸枻鎷烽敓鏂ゆ嫹閿熼叺纭锋嫹閿熼叺纭锋嫹閿燂拷  閿熸枻鎷烽敓鐨嗗尅鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴甯�
}

int HD_thre;  //閿熸枻鎷锋椂閿熸璇ф嫹閿熸枻鎷烽敓锟�
//閿熸枻鎷峰�奸敓鏂ゆ嫹
void Get01change ()
{
    uint8 thre;
    uint8 i, j;
    for (i = 0; i < LCDH; i++)
    {
        for (j = 0; j < LCDW; j++)
        {
            if (j <= 15)
                thre = (uint8) (ImageStatus.Threshold_static - 10);
            else if ((j > 70 && j <= 75))
                thre = (uint8) (ImageStatus.Threshold_static - 15);
            else if (j >= 65)
                thre = (uint8) (ImageStatus.Threshold_static - 15);
            else
                thre = (uint8) (ImageStatus.Threshold_static);

            if (Image_Use[i][j] > (thre)) //閿熸枻鎷峰�艰秺閿熸枻鎷烽敓鏂ゆ嫹绀洪敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹瓒婇敓娲侊紝閿熸枻鎷锋祬閿熸枻鎷峰浘閿熸枻鎷蜂篃閿熸枻鎷烽敓鏂ゆ嫹绀洪敓鏂ゆ嫹閿熸枻鎷�
                Pixle[i][j] = 1;  //閿熸枻鎷�
            else
                Pixle[i][j] = 0;  //閿熸枻鎷�
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      閿熻剼浼欐嫹閿熶茎杈炬嫹閿燂拷
//  @param      image  鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
//  @param      clo    閿熸枻鎷�
//  @param      row    閿熸枻鎷�
//  @param      pixel_threshold 閿熸枻鎷峰�奸敓鏂ゆ嫹閿熸枻鎷�
//  @return     uint8
//  @since      2021.6.23
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 Threshold_deal (uint8 *image, uint16 col, uint16 row, uint32 pixel_threshold)
{
#define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height;
    uint8 threshold = 0;
    uint8 *data = image;  //鎸囬敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎹风鎷锋寚閿熸枻鎷�
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum = 0;
//缁熼敓鐙＄伆搴︾》鎷烽敓鏂ゆ嫹姣忛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰浘閿熸枻鎷烽敓鍙殑闈╂嫹閿熸枻鎷�
    for (i = 0; i < height; i += 1)
    {
        for (j = 0; j < width; j += 1)
        {
            // if((sun_mode&&data[i*width+j]<pixel_threshold)||(!sun_mode))
            //{
            pixelCount[(int) data[i * width + j]]++;  //閿熸枻鎷烽敓鏂ゆ嫹鍓嶉敓渚ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楃鎷烽敓杞款亷鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾版唻鎷�
            gray_sum += (int) data[i * width + j];  //閿熸彮璁规嫹鍊奸敓鏉扮尨鎷�
            //}
        }
    }

//閿熸枻鎷烽敓鏂ゆ嫹姣忛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍊奸敓渚ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍥鹃敓鏂ゆ嫹閿熷彨鐨勬唻鎷烽敓鏂ゆ嫹
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float) pixelCount[i] / pixelSum;
    }

//閿熸枻鎷烽敓鏂ゆ嫹閿熸彮搴︾》鎷穂0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < pixel_threshold; j++)
    {
        w0 += pixelPro[j]; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹姣忛敓鏂ゆ嫹閿熸彮璁规嫹鍊奸敓鏂ゆ嫹閿熸枻鎷烽敓鎴鎷烽敓鏂ゆ嫹鍗犻敓鏂ゆ嫹閿熸枻鎷蜂箣閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻鐨勬唻鎷烽敓鏂ゆ嫹
        u0tmp += j * pixelPro[j];  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹 姣忛敓鏂ゆ嫹閿熸彮璁规嫹鍊奸敓渚ョ鎷疯皨閿熸枻鎷烽敓锟� *閿熸彮璁规嫹鍊�

        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;

        u0 = u0tmp / w0;    //閿熸枻鎷烽敓鏂ゆ嫹骞抽敓鏂ゆ嫹閿熸彮璁规嫹
        u1 = u1tmp / w1;    //鍓嶉敓鏂ゆ嫹骞抽敓鏂ゆ嫹閿熸彮璁规嫹
        u = u0tmp + u1tmp;  //鍏ㄩ敓鏂ゆ嫹骞抽敓鏂ゆ嫹閿熸彮璁规嫹
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = (uint8) j;
        }
        if (deltaTmp < deltaMax)
            break;
    }
    return threshold;
}

void Get01change_dajin ()
{
    ImageStatus.Threshold = Threshold_deal(Image_Use[0], LCDW, LCDH, ImageStatus.Threshold_detach);
    if (ImageStatus.Threshold < ImageStatus.Threshold_static)
        ImageStatus.Threshold = (uint8) ImageStatus.Threshold_static;
    uint8 i, j = 0;
    uint8 thre;
    for (i = 0; i < LCDH; i++)
    {
        for (j = 0; j < LCDW; j++)
        {
            if (j <= 15)
                thre = ImageStatus.Threshold - 10;
            else if ((j > 70 && j <= 75))
                thre = ImageStatus.Threshold - 10;
            else if (j >= 65)
                thre = ImageStatus.Threshold - 10;
            else
                thre = ImageStatus.Threshold;

            if (Image_Use[i][j] > (thre)) //閿熸枻鎷峰�艰秺閿熸枻鎷烽敓鏂ゆ嫹绀洪敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹瓒婇敓娲侊紝閿熸枻鎷锋祬閿熸枻鎷峰浘閿熸枻鎷蜂篃閿熸枻鎷烽敓鏂ゆ嫹绀洪敓鏂ゆ嫹閿熸枻鎷�
                Pixle[i][j] = 1;  //閿熸枻鎷�
            else
                Pixle[i][j] = 0;  //閿熸枻鎷�
        }
    }
}

//閿熸枻鎷烽敓鏂ゆ嫹閿熷壙璇ф嫹
void Pixle_Filter ()
{
    int nr;  //閿熸枻鎷�
    int nc;  //閿熸枻鎷�

    for (nr = 10; nr < 40; nr++)
    {
        for (nc = 10; nc < 70; nc = nc + 1)
        {
            if ((Pixle[nr][nc] == 0)
                    && (Pixle[nr - 1][nc] + Pixle[nr + 1][nc] + Pixle[nr][nc + 1] + Pixle[nr][nc - 1] >= 3))
            {
                Pixle[nr][nc] = 1;
            }
            //      else
            //      if((Pixle[nr][nc]==1)&&(Pixle[nr-1][nc]+Pixle[nr+1][nc]+Pixle[nr][nc+1]+Pixle[nr][nc-1]<2))
            //      {
            //        Pixle[nr][nc]=0;
            //      }
        }
    }
}

void GetJumpPointFromDet (uint8 *p, uint8 type, int L, int H, JumpPointtypedef *Q) //閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹瑕侀敓鏂ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓浠嬶紙80閿熸枻鎷烽敓濮愶級
                                                                                   //閿熻妭璁规嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹鍛嬮敓鏂ゆ嫹閿熺即顭掓嫹鍐堕敓鏂ゆ嫹閿燂拷
{                                                                              //閿熸枻鎷烽敓鏂ゆ嫹閿熻鍖℃嫹濮嬮敓閰垫枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
    int i = 0;
    if (type == 'L')                              //鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
    {
        for (i = H; i >= L; i--)
        {
            if (*(p + i) == 1 && *(p + i - 1) != 1)   //閿熺即榛戞唻鎷烽敓锟�
            {
                Q->point = i;                           //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
                Q->type = 'T';                          //閿熸枻鎷风‘閿熸枻鎷烽敓鏂ゆ嫹
                break;
            }
            else if (i == (L + 1))                  //閿熸枻鎷烽敓鏂ゆ嫹鎵敓鏂ゆ嫹閿熸枻鎷烽敓鎻浼欐嫹涓氶敓锟�
            {
                if (*(p + (L + H) / 2) != 0)            //閿熸枻鎷烽敓鏂ゆ嫹灞戦敓鏂ゆ嫹鍓嶆閿燂拷
                {
                    Q->point = (L + H) / 2;               //閿熸枻鎷蜂负閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹姊伴敓锟�
                    Q->type = 'W';                        //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨纭锋嫹涓洪敓闃讹綇鎷烽敓鏂ゆ嫹涓烘病閿熷彨鎲嬫嫹
                    break;
                }
                else                                  //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨纭锋嫹涓洪敓鏂ゆ嫹
                {
                    Q->point = H;                         //閿熸枻鎷烽敓鏂ゆ嫹灞戦敓鏂ゆ嫹鍛涜瘶閿燂拷
                    Q->type = 'H';                        //閿熸枻鎷烽敓鏂ゆ嫹閿熻鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹鍊奸敓鏂ゆ嫹閿熸枻鎷蜂负閿熻杈炬嫹閿熸枻鎷烽敓鏂ゆ嫹
                    break;
                }
            }
        }
    }
    else if (type == 'R')                       //鎵敓鏂ゆ嫹閿熸彮鎲嬫嫹閿熸枻鎷�
    {
        for (i = L; i <= H; i++)                    //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鎵�
        {
            if (*(p + i) == 1 && *(p + i + 1) != 1)   //閿熸枻鎷烽敓缂撮粦纰夋嫹閿熼樁纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹
            {
                Q->point = i;                           //閿熸枻鎷峰綍
                Q->type = 'T';
                break;
            }
            else if (i == (H - 1))                  //閿熸枻鎷烽敓鏂ゆ嫹鎵敓鏂ゆ嫹閿熸枻鎷烽敓鎻浼欐嫹涓氶敓锟�
            {
                if (*(p + (L + H) / 2) != 0)            //閿熸枻鎷烽敓鏂ゆ嫹灞戦敓鏂ゆ嫹鍓嶆閿燂拷
                {
                    Q->point = (L + H) / 2;               //閿熸彮鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨纰夋嫹
                    Q->type = 'W';
                    break;
                }
                else                                  //閿熸枻鎷烽敓鏂ゆ嫹姊伴敓鏂ゆ嫹鍛涜瘶閿燂拷
                {
                    Q->point = L;                         //閿熸枻鎷烽敓鏂ゆ嫹閿熻鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹鍊�
                    Q->type = 'H';
                    break;
                }
            }
        }
    }
}

static uint8 DrawLinesFirst (void)
{
    PicTemp = Pixle[59];
    if (*(PicTemp + ImageSensorMid) == 0)                 //閿熸枻鎷烽敓鏂ゆ嫹濡嗛敓閰电》鎷烽敓鏂ゆ嫹姊伴敓杞款亷鎷峰啟閿熸枻鎷锋枑锝忔嫹閿熸枻鎷�
    {
        for (Xsite = 0; Xsite < ImageSensorMid; Xsite++)    //閿熸枻鎷烽敓鏂ゆ嫹閿熸彮鎲嬫嫹閿熸枻鎷�
        {
            if (*(PicTemp + ImageSensorMid - Xsite) != 0) //涓�閿熸枻鎷烽敓鎻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏈ㄩ敓鏂ゆ嫹鑰勵剨鎷烽敓绲檙eak
                break;                                          //閿熸枻鎷烽敓鎻》鎷峰綍Xsite
            if (*(PicTemp + ImageSensorMid + Xsite) != 0)
                break;
        }
        if (*(PicTemp + ImageSensorMid - Xsite) != 0)       //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绔殑浼欐嫹
        {
//            printf("1:%d\n\r", ImageSensorMid - Xsite + 1);
            BottomBorderRight = ImageSensorMid - Xsite + 1;   // 59閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
            for (Xsite = BottomBorderRight; Xsite > 0; Xsite--)  //閿熸枻鎷峰閿熸枻鎷�59閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
            {
                if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite - 1) == 0)             //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻妭鐐癸紝閿熷壙璇ф嫹
                {
                    BottomBorderLeft = Xsite;                     //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂笟閿燂拷
                    Real_state[0][0] = 'T';
                    break;
                }
                else if (Xsite == 1)
                {
                    BottomBorderLeft = 0; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓杞款亷鎷烽敓锟�0
                    Real_state[0][0] = 'W';
                    break;
                }
            }
        }
        else if (*(PicTemp + ImageSensorMid + Xsite) != 0)  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰喍鍙╁箷閿燂拷
        {
            BottomBorderLeft = ImageSensorMid + Xsite - 1;    // 59閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
            for (Xsite = BottomBorderLeft; Xsite < 79; Xsite++)  //閿熸枻鎷峰閿熸枻鎷�59閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
            {
                if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite + 1) == 0)             //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻妭鐐癸紝閿熷壙璇ф嫹
                {
                    BottomBorderRight = Xsite;                    //閿熸彮鎲嬫嫹閿熸枻鎷烽敓鎻鎷�
                    Real_state[0][1] = 'T';
                    break;
                }
                else if (Xsite == 78)
                {
                    BottomBorderRight = 79; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰喍閿熸枻鎷锋捀閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹79
                    Real_state[0][1] = 'W';
                    break;
                }
            }
        }
    }
    else                                                //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋閿熸枻鎷峰墠妞庢ā閿熸枻鎷峰唹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
    {
        for (Xsite = ImageSensorMid; Xsite < 79; Xsite++)   //涓�閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰喍閿熸枻鎷烽敓锟�
        {
            if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite + 1) == 0) //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻妭鐐癸紝閿熷壙璇ф嫹     //閿熸枻鎷烽敓鏂ゆ嫹閿熼樁纰夋嫹
            {
                BottomBorderRight = Xsite;                      //閿熸彮纰夋嫹閿熼叺纭锋嫹褰�
                Real_state[0][1] = 'T';
                break;
            }
            else if (Xsite == 78)
            {
                BottomBorderRight = 79;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰喍閿熸枻鎷锋捀閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹79
                Real_state[0][1] = 'W';
                break;
            }
        }
        for (Xsite = ImageSensorMid; Xsite > 0; Xsite--)    //涓�閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        {
            if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite - 1) == 0)                //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻妭鐐癸紝閿熷壙璇ф嫹
            {
                BottomBorderLeft = Xsite;                       //閿熸彮纰夋嫹閿熼叺纭锋嫹褰�
                Real_state[0][0] = 'T';
                break;
            }
            else if (Xsite == 1)
            {
                BottomBorderLeft = 0;    //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓杞款亷鎷烽敓锟�0
                Real_state[0][0] = 'W';
                break;
            }
        }

    }
    BottomCenter = (BottomBorderLeft + BottomBorderRight) / 2;   // 59閿熸枻鎷烽敓鍙鎷风洿閿熸枻鎷峰彇骞抽敓鏂ゆ嫹
    ImageDeal[59].LeftBorder = BottomBorderLeft; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板浼欐嫹閿熸枻鎷烽敓杈冾澁鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹閿熸枻鎷烽敓鏂ゆ嫹
    ImageDeal[59].RightBorder = BottomBorderRight;
    ImageDeal[59].Center = BottomCenter;                        //纭敓鏂ゆ嫹閿熸枻鎷峰閿燂拷
    ImageDeal[59].Wide = BottomBorderRight - BottomBorderLeft;  //閿熻姤鍌ㄩ敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鎭�
    ImageDeal[59].IsLeftFind = 'T';
    ImageDeal[59].IsRightFind = 'T';
    for (Ysite = 58; Ysite > 54; Ysite--)                       //閿熸枻鎷烽敓鍙》鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熼樁鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹
    {
        PicTemp = Pixle[Ysite];
        for (Xsite = ImageDeal[Ysite + 1].Center; Xsite < 79; Xsite++)                  //閿熸枻鎷峰墠閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
        {
            if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite + 1) == 0)
            {
                ImageDeal[Ysite].RightBorder = Xsite;
                Real_state[59 - Ysite][1] = 'T';
                break;
            }
            else if (Xsite == 78)
            {
                ImageDeal[Ysite].RightBorder = 79;
                Real_state[59 - Ysite][1] = 'W';
                break;
            }
        }
        for (Xsite = ImageDeal[Ysite + 1].Center; Xsite > 0; Xsite--)                   //閿熸枻鎷峰墠閿熸枻鎷蜂竴閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
        {
            if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite - 1) == 0)
            {
                ImageDeal[Ysite].LeftBorder = Xsite;
                Real_state[59 - Ysite][0] = 'T';
                break;
            }
            else if (Xsite == 1)
            {
                ImageDeal[Ysite].LeftBorder = 0;
                Real_state[59 - Ysite][0] = 'W';

                break;
            }
        }
        ImageDeal[Ysite].IsLeftFind = 'T';                        //閿熸枻鎷蜂簺閿熸枻鎷锋伅閿熻姤鍌ㄩ敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        ImageDeal[Ysite].IsRightFind = 'T';
        ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2; //閿熻姤鍌ㄩ敓鍙鎷�
        ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;      //閿熻姤鍌ㄩ敓鏂ゆ嫹閿熸枻鎷�
    }
    return 'T';
} //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎻亷鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓楗鸿鎷烽敓鏂ゆ嫹閿熸澃纰夋嫹閿熸枻鎷烽敓鑴氾綇鎷烽敓鏂ゆ嫹閿熸枻鎷疯閿熻妭甯嫹瑁呴敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閰靛嚖鎷烽敓鏂ゆ嫹鍜忛敓锟�

/*閿熸枻鎷烽敓鏂ゆ嫹杩介敓鏂ゆ嫹閿熸枻鎷风鐜敓楗侯偓鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�*/
float last_D_L = 0, last_D_R = 0;
static void DrawLinesProcess (void)  //////閿熸枻鎷烽敓鐭潻鎷烽敓鏂ゆ嫹
{
    uint8 L_Found_T = 'F';  //纭敓鏂ゆ嫹閿熺潾鎲嬫嫹鏂滈敓缁炵殑浼欐嫹鍑嗛敓鍙唻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熶茎鎲嬫嫹蹇�
    uint8 Get_L_line = 'F';  //閿熸彮纰夋嫹閿熸枻鎷蜂竴甯у浘閿熸枻鎷峰箷閿熼樁纭锋嫹閿熷彨鎲嬫嫹閿燂拷
    uint8 R_Found_T = 'F';  //纭敓鏂ゆ嫹閿熺潾鎲嬫嫹鏂滈敓缁炵殑浼欐嫹鍑嗛敓鍙唻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熶茎鎲嬫嫹蹇�
    uint8 Get_R_line = 'F';  //閿熸彮纰夋嫹閿熸枻鎷蜂竴甯у浘閿熸枻鎷峰箷閿熼樁纭锋嫹閿熷彨鎲嬫嫹閿燂拷
    float D_L = 0;           //閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙唻鎷烽敓锟�
    float D_R = 0;           //閿熸帴绛规嫹閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
    int ytemp_W_L;           //閿熸枻鎷蜂綇閿熼樁杈炬嫹閿熻涓㈡唻鎷烽敓鏂ゆ嫹
    int ytemp_W_R;           //閿熸枻鎷蜂綇閿熼樁杈炬嫹閿熸彮璁规嫹閿熸枻鎷烽敓鏂ゆ嫹
    ExtenRFlag = 0;          //閿熸枻鎷峰織浣嶉敓鏂ゆ嫹0
    ExtenLFlag = 0;
    ImageStatus.Left_Line = 0;
    ImageStatus.WhiteLine = 0;
    ImageStatus.Right_Line = 0;
    for (Ysite = 54; Ysite > ImageStatus.OFFLine; Ysite--) //鍓�5閿熷彨杈炬嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷壙锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷55閿熷彨纰夋嫹閿熸枻鎷烽敓鍊熷畾閿熶茎璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹OFFLine閿熸枻鎷�
    {                        //澶繙閿熸枻鎷峰浘閿熸枻鎷烽敓楗鸿鎷烽敓鏂ゆ嫹OFFLine閿熺殕鐚存嫹鐗熼敓鏂ゆ嫹閿熸枻鎷烽敓锟�
        PicTemp = Pixle[Ysite];
        JumpPointtypedef JumpPoint[2];                                          // 0閿熸枻鎷�1閿熸枻鎷�
        if (ImageStatus.Road_type != Cross_ture
        /* &&SystemData.SpeedData.Length*OX>500*/)
        {
            IntervalLow = ImageDeal[Ysite + 1].RightBorder - ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹-Interval閿熶茎鐐瑰紑濮嬮敓鏂ゆ嫹纭敓鏂ゆ嫹鎵敓鍊熷紑濮嬮敓濮愶級
            IntervalHigh = ImageDeal[Ysite + 1].RightBorder + ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹+Interval閿熶茎纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼ズ鍑ゆ嫹閿熺即顭掓嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷锛�
        }
        else
        {
            IntervalLow = ImageDeal[Ysite + 1].RightBorder - ImageScanInterval_Cross; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹-Interval_Cross閿熶茎鐐瑰紑濮嬮敓鏂ゆ嫹纭敓鏂ゆ嫹鎵敓鍊熷紑濮嬮敓濮愶級
            IntervalHigh = ImageDeal[Ysite + 1].RightBorder + ImageScanInterval_Cross; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹+Interval_Cross閿熶茎鐐瑰紑濮嬮敓鏂ゆ嫹纭敓鏂ゆ嫹鎵敓鍊熷紑濮嬮敓濮愶級
        }

        LimitL(IntervalLow);   //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        LimitH(IntervalHigh);  //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        GetJumpPointFromDet(PicTemp, 'R', IntervalLow, IntervalHigh, &JumpPoint[1]);     //鎵敓鎻唻鎷烽敓鏂ゆ嫹

        IntervalLow = ImageDeal[Ysite + 1].LeftBorder - ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�-5閿熶茎鐐瑰紑濮嬮敓鏂ゆ嫹纭敓鏂ゆ嫹鎵敓鍊熷紑濮嬮敓濮愶級
        IntervalHigh = ImageDeal[Ysite + 1].LeftBorder + ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�+5閿熶茎纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼ズ鍑ゆ嫹閿熺即顭掓嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷锛�

        LimitL(IntervalLow);   //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        LimitH(IntervalHigh);  //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        GetJumpPointFromDet(PicTemp, 'L', IntervalLow, IntervalHigh, &JumpPoint[0]);

        if (JumpPoint[0].type == 'W')      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绔鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸垝锛岄敓鏂ゆ嫹閿熸枻鎷�10閿熸枻鎷烽敓濮愰兘閿熻鐧界鎷�
        {
            ImageDeal[Ysite].LeftBorder = ImageDeal[Ysite + 1].LeftBorder; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姊伴敓鏂ゆ嫹閿熻锟�
        }
        else                                                                          //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
        {
            ImageDeal[Ysite].LeftBorder = JumpPoint[0].point;                             //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        }

        if (JumpPoint[1].type == 'W')                                  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍐堕敓鏂ゆ嫹鍗熼敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
        {
            ImageDeal[Ysite].RightBorder = ImageDeal[Ysite + 1].RightBorder; //閿熸枻鎷烽敓鏂ゆ嫹閿熸彮鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴閿熷彨纰夋嫹閿熸枻鎷峰��
        }
        else                                                                          //閿熸彮鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        {
            ImageDeal[Ysite].RightBorder = JumpPoint[1].point;                            //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        }

        ImageDeal[Ysite].IsLeftFind = JumpPoint[0].type;  //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓绔綇鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
        ImageDeal[Ysite].IsRightFind = JumpPoint[1].type;

        //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷蜂簺閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯皨閿熺殕锟�
        if ((ImageDeal[Ysite].IsLeftFind == 'H' || ImageDeal[Ysite].IsRightFind == 'H'))
        {
            if (ImageDeal[Ysite].IsLeftFind == 'H')                                   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绔揪鎷烽敓鏂ゆ嫹閿熸枻鎷�
                for (Xsite = (ImageDeal[Ysite].LeftBorder + 1); Xsite <= (ImageDeal[Ysite].RightBorder - 1); Xsite++) //閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹涔嬮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鎵敓鏂ゆ嫹
                {
                    if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite + 1) != 0))
                    {
                        ImageDeal[Ysite].LeftBorder = Xsite; //閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺纰夋嫹閿熸彮鎲嬫嫹閿熷彨榛戝府鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熺殕鎲嬫嫹閿熸枻鎷风洿閿熸枻鎷峰彇閿熸枻鎷�
                        ImageDeal[Ysite].IsLeftFind = 'T';
                        break;
                    }
                    else if (*(PicTemp + Xsite) != 0)                           //涓�閿熸枻鎷烽敓鏂ゆ嫹閿熻鐧界鎷烽敓鏂ゆ嫹鐩撮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
                        break;
                    else if (Xsite == (ImageDeal[Ysite].RightBorder - 1))
                    {
                        ImageDeal[Ysite].IsLeftFind = 'T';
                        break;
                    }
                }
            if ((ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder) <= 7)                //鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹钖呴敓锟�
            {
                ImageStatus.OFFLine = Ysite + 1;  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋枩閿燂拷7灏忛敓鍓跨尨鎷烽敓鏂ゆ嫹鐩撮敓鎺ヨ鎷疯閿熸枻鎷�
                break;
            }
            if (ImageDeal[Ysite].IsRightFind == 'H')
                for (Xsite = (ImageDeal[Ysite].RightBorder - 1); Xsite >= (ImageDeal[Ysite].LeftBorder + 1); Xsite--)
                {
                    if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite - 1) != 0))
                    {
                        ImageDeal[Ysite].RightBorder = Xsite; //閿熸枻鎷烽敓鏂ゆ嫹鍐堕敓鏂ゆ嫹鍙╅敓鏂ゆ嫹閿熺浼欐嫹閿熷彨榛戝府鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熺殕鎲嬫嫹閿熸枻鎷风洿閿熸枻鎷峰彇閿熸枻鎷�
                        ImageDeal[Ysite].IsRightFind = 'T';
                        break;
                    }
                    else if (*(PicTemp + Xsite) != 0)
                        break;
                    else if (Xsite == (ImageDeal[Ysite].LeftBorder + 1))
                    {
                        ImageDeal[Ysite].RightBorder = Xsite;
                        ImageDeal[Ysite].IsRightFind = 'T';
                        break;
                    }
                }
        }

        /***********閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹閿熸枻鎷�************/
        int ysite = 0;
        uint8 L_found_point = 0;
        uint8 R_found_point = 0;

        if (ImageStatus.Road_type != Ramp)
        {
            if (ImageDeal[Ysite].IsRightFind == 'W' && Ysite > 10 && Ysite < 50) //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰�奸敓鏂ゆ嫹钖囬敓鏂ゆ嫹閿燂拷
            {
                if (Get_R_line == 'F')    //閿熸枻鎷蜂竴甯у浘閿熸枻鎷锋病閿熸枻鎷烽敓鏉扮櫢鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴閿熼樁纭锋嫹鍙╂媷閿熸枻鎷烽敓杞胯鎷烽敓鏂ゆ嫹閿熸枻鎷�
                {
                    Get_R_line = 'T';       //閿熸枻鎷烽敓鏂ゆ嫹  涓�甯у浘閿熸枻鎷峰彧閿熸枻鎷蜂竴閿熸枻鎷� 閿熸枻鎷蜂负T
                    ytemp_W_R = Ysite + 2;
                    for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++)
                    {
                        if (ImageDeal[ysite].IsRightFind == 'T') //閿熸枻鎷烽敓鐫唻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  涓�閿熷閮介敓鏂ゆ嫹閿熷彨杈圭鎷�
                            R_found_point++;
                    }
                    if (R_found_point > 8) //閿熸彮纰夋嫹閿熸枻鎷峰噯鏂滈敓缁炴唻鎷�  閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹   閿熸枻鎷烽敓鍙竟鐨勭鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹8
                    {
                        D_R =
                                ((float) (ImageDeal[Ysite + R_found_point].RightBorder
                                        - ImageDeal[Ysite + 3].RightBorder)) / ((float) (R_found_point - 3));
                        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浜涢敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
                        //閿熺煫闈╂嫹閿熺潾鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃讹拷
                        if (D_R > 0)
                        {
                            R_Found_T = 'T'; //閿熸枻鎷烽敓鍙唻鎷疯殌閿熸枻鎷烽敓锟�0  閿熸枻鎷蜂箞閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃剁》鎷烽敓锟�  閿熸枻鎷蜂负閿熸枻鎷烽敓杞夸紮鎷烽敓鏂ゆ嫹
                                             //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鎲嬫嫹铓�閿熸枻鎷烽敓锟�0  灏忛敓鏂ゆ嫹0閿熸枻鎷烽敓鏂ゆ嫹閿熸彮璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹鏄犻敓锟� 娌￠敓鏂ゆ嫹瑕�
                        }
                        else
                        {
                            R_Found_T = 'F';                      //娌￠敓鏂ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼樁纭锋嫹閿燂拷
                            if (D_R < 0)
                                ExtenRFlag = 'F';      //閿熸枻鎷烽敓鏂ゆ嫹閿熻鐤氫紮鎷烽敓鏂ゆ嫹閿熺粸顕嗘嫹绾稿爲鎮寸櫢鎷烽敓锟�  閿熸枻鎷锋鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鐭鎷�
                        }
                        last_D_R = D_R;
                    }
                    else if ((ImageStatus.Road_type == LeftCirque) && (ImageFlag.image_element_rings_flag == 7)
                            && R_found_point > 5)
                    {
                        if (R_found_point > 5)
                        {
                            D_R = ((float) (ImageDeal[Ysite + R_found_point].RightBorder
                                    - ImageDeal[Ysite + 3].RightBorder)) / ((float) (R_found_point - 3));
                        }
                        else
                        {
                            D_R = last_D_R;
                        }
                        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浜涢敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
                        //閿熺煫闈╂嫹閿熺潾鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃讹拷
                        if (D_R > 0)
                        {
                            R_Found_T = 'T'; //閿熸枻鎷烽敓鍙唻鎷疯殌閿熸枻鎷烽敓锟�0  閿熸枻鎷蜂箞閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃剁》鎷烽敓锟�  閿熸枻鎷蜂负閿熸枻鎷烽敓杞夸紮鎷烽敓鏂ゆ嫹
                                             //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鎲嬫嫹铓�閿熸枻鎷烽敓锟�0  灏忛敓鏂ゆ嫹0閿熸枻鎷烽敓鏂ゆ嫹閿熸彮璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹鏄犻敓锟� 娌￠敓鏂ゆ嫹瑕�
                        }
                        else
                        {
                            R_Found_T = 'F';                      //娌￠敓鏂ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼樁纭锋嫹閿燂拷
                            if (D_R < 0)
                                ExtenRFlag = 'F';      //閿熸枻鎷烽敓鏂ゆ嫹閿熻鐤氫紮鎷烽敓鏂ゆ嫹閿熺粸顕嗘嫹绾稿爲鎮寸櫢鎷烽敓锟�  閿熸枻鎷锋鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鐭鎷�
                        }
                    }
                }
                if (R_Found_T == 'T')
                    ImageDeal[Ysite].RightBorder = ImageDeal[ytemp_W_R].RightBorder - D_R * (ytemp_W_R - Ysite); //閿熸枻鎷烽敓鏂ゆ嫹涓氶敓鏂ゆ嫹閿燂拷 閿熸枻鎷蜂箞閿熺殕浼欐嫹鍑嗛敓鏂ゆ嫹閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹

                LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
                LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            }

            if (ImageDeal[Ysite].IsLeftFind == 'W' && Ysite > 10 && Ysite < 50) //閿熸枻鎷烽敓鏂ゆ嫹鍚岄敓鏂ゆ嫹  閿熸枻鎷峰憮閿燂拷
            {
                if (Get_L_line == 'F')
                {
                    Get_L_line = 'T';
                    ytemp_W_L = Ysite + 2;
                    for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++)
                    {
                        if (ImageDeal[ysite].IsLeftFind == 'T')
                            L_found_point++;
                    }
                    if (L_found_point > 8)              //閿熸彮纰夋嫹閿熸枻鎷峰噯鏂滈敓缁炴唻鎷�  閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹
                    {
                        D_L = ((float) (ImageDeal[Ysite + 3].LeftBorder - ImageDeal[Ysite + L_found_point].LeftBorder))
                                / ((float) (L_found_point - 3));
                        if (D_L > 0)
                        {
                            L_Found_T = 'T';

                        }
                        else
                        {
                            L_Found_T = 'F';
                            if (D_L < 0)
                                ExtenLFlag = 'F';
                        }
                        last_D_L = D_L;

                    }
                    else if ((ImageStatus.Road_type == RightCirque) && (ImageFlag.image_element_rings_flag == 7)
                            && L_found_point > 5)
                    {
                        if (L_found_point > 5)
                        {
                            D_L = ((float) (ImageDeal[Ysite + 3].LeftBorder
                                    - ImageDeal[Ysite + L_found_point].LeftBorder)) / ((float) (L_found_point - 3));
                        }
                        else
                        {
                            D_L = last_D_L;

                        }
                        if (D_L > 0)
                        {
                            L_Found_T = 'T';

                        }
                        else
                        {
                            L_Found_T = 'F';
                            if (D_L < 0)
                                ExtenLFlag = 'F';
                        }
                    }
                }

                if (L_Found_T == 'T')
                    ImageDeal[Ysite].LeftBorder = ImageDeal[ytemp_W_L].LeftBorder + D_L * (ytemp_W_L - Ysite);

                LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
                LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            }
        }
        if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite].IsRightFind == 'W')
        {
            ImageStatus.WhiteLine++;  //瑕侀敓鏂ゆ嫹閿熸枻鎷烽敓鎻鎷烽敓鐫竟锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�+1
        }
        if (ImageDeal[Ysite].IsLeftFind == 'W' && Ysite < 55)
        {
            ImageStatus.Left_Line++;
        }
        if (ImageDeal[Ysite].IsRightFind == 'W' && Ysite < 55)
        {
            ImageStatus.Right_Line++;
        }

        LimitL(ImageDeal[Ysite].LeftBorder);   //閿熺潾鍑ゆ嫹
        LimitH(ImageDeal[Ysite].LeftBorder);   //閿熺潾鍑ゆ嫹
        LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
        LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹

        ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
        ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;

        if (ImageDeal[Ysite].Wide <= 7)         //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鎺ユ拝鎷烽敓鏂ゆ嫹
        {
            ImageStatus.OFFLine = Ysite + 1;
            break;
        }

        else if (ImageDeal[Ysite].RightBorder <= 10 || ImageDeal[Ysite].LeftBorder >= 70)
        {
            ImageStatus.OFFLine = Ysite + 1;
            break;
        }                    //閿熸枻鎷峰浘閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鈽呮嫹閿燂拷0閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎻竟杈惧埌涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹姝㈠贰閿熸枻鎷�
    }

    return;
}
static void Second_DrawLinesProcess_L (void)  //////閿熸枻鎷烽敓鐭潻鎷烽敓鏂ゆ嫹
{
    uint8 L_Found_T = 'F';  //纭敓鏂ゆ嫹閿熺潾鎲嬫嫹鏂滈敓缁炵殑浼欐嫹鍑嗛敓鍙唻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熶茎鎲嬫嫹蹇�
    uint8 Get_L_line = 'F';  //閿熸彮纰夋嫹閿熸枻鎷蜂竴甯у浘閿熸枻鎷峰箷閿熼樁纭锋嫹閿熷彨鎲嬫嫹閿燂拷
    float D_L = 0;           //閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙拷
    int ytemp_W_L;           //閿熸枻鎷蜂綇閿熼樁杈炬嫹閿熻涓㈡唻鎷烽敓鏂ゆ嫹
    ExtenLFlag = 0;
    ImageStatus.Left_Line = 0;
    ImageStatus.WhiteLine = 0;
    for (Ysite = 58; Ysite > 54; Ysite--)
    {
        ImageDeal[Ysite].LeftBorder = (ImageDeal[Ysite].LeftBorder + last_mid_point) / 2;
    }
    for (Ysite = 54; Ysite > ImageStatus.OFFLine; Ysite--) //鍓�5閿熷彨杈炬嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷壙锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷55閿熷彨纰夋嫹閿熸枻鎷烽敓鍊熷畾閿熶茎璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹OFFLine閿熸枻鎷�
    {                        //澶繙閿熸枻鎷峰浘閿熸枻鎷烽敓楗鸿鎷烽敓鏂ゆ嫹OFFLine閿熺殕鐚存嫹鐗熼敓鏂ゆ嫹閿熸枻鎷烽敓锟�
        PicTemp = Pixle[Ysite];
        JumpPointtypedef JumpPoint[2];                                          // 0閿熸枻鎷�1閿熸枻鎷�

        IntervalLow = ImageDeal[Ysite + 1].LeftBorder - ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�-5閿熶茎鐐瑰紑濮嬮敓鏂ゆ嫹纭敓鏂ゆ嫹鎵敓鍊熷紑濮嬮敓濮愶級
        IntervalHigh = ImageDeal[Ysite + 1].LeftBorder + ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�+5閿熶茎纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼ズ鍑ゆ嫹閿熺即顭掓嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷锛�

        LimitL(IntervalLow);   //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        LimitH(IntervalHigh);  //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        GetJumpPointFromDet(PicTemp, 'L', IntervalLow, IntervalHigh, &JumpPoint[0]);

        if (JumpPoint[0].type == 'W')      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绔鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸垝锛岄敓鏂ゆ嫹閿熸枻鎷�10閿熸枻鎷烽敓濮愰兘閿熻鐧界鎷�
        {
            ImageDeal[Ysite].LeftBorder = ImageDeal[Ysite + 1].LeftBorder; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姊伴敓鏂ゆ嫹閿熻锟�
        }
        else                                                                          //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
        {
            ImageDeal[Ysite].LeftBorder = JumpPoint[0].point;                             //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        }

        ImageDeal[Ysite].IsLeftFind = JumpPoint[0].type;  //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓绔綇鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
        //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷蜂簺閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯皨閿熺殕锟�
        if (ImageDeal[Ysite].IsLeftFind == 'H')
        {
            if (ImageDeal[Ysite].IsLeftFind == 'H')                                   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绔揪鎷烽敓鏂ゆ嫹閿熸枻鎷�
                for (Xsite = (ImageDeal[Ysite].LeftBorder + 1); Xsite <= (ImageDeal[Ysite].RightBorder - 1); Xsite++) //閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹涔嬮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鎵敓鏂ゆ嫹
                {
                    if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite + 1) != 0))
                    {
                        ImageDeal[Ysite].LeftBorder = Xsite; //閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺纰夋嫹閿熸彮鎲嬫嫹閿熷彨榛戝府鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熺殕鎲嬫嫹閿熸枻鎷风洿閿熸枻鎷峰彇閿熸枻鎷�
                        ImageDeal[Ysite].IsLeftFind = 'T';
                        break;
                    }
                    else if (*(PicTemp + Xsite) != 0)                           //涓�閿熸枻鎷烽敓鏂ゆ嫹閿熻鐧界鎷烽敓鏂ゆ嫹鐩撮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
                        break;
                    else if (Xsite == (ImageDeal[Ysite].RightBorder - 1))
                    {
                        ImageDeal[Ysite].IsLeftFind = 'T';
                        break;
                    }
                }
            if ((ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder) <= 7)                //鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹钖呴敓锟�
            {
                ImageStatus.OFFLine = Ysite + 1;  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋枩閿燂拷7灏忛敓鍓跨尨鎷烽敓鏂ゆ嫹鐩撮敓鎺ヨ鎷疯閿熸枻鎷�
                break;
            }
        }

        /***********閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹閿熸枻鎷�************/
        int ysite = 0;
        uint8 L_found_point = 0;

        if (ImageStatus.Road_type != Ramp)
        {
            if (ImageDeal[Ysite].IsLeftFind == 'W' && Ysite > 10 && Ysite < 50) //閿熸枻鎷烽敓鏂ゆ嫹鍚岄敓鏂ゆ嫹  閿熸枻鎷峰憮閿燂拷
            {
                if (Get_L_line == 'F')
                {
                    Get_L_line = 'T';
                    ytemp_W_L = Ysite + 2;
                    for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++)
                    {
                        if (ImageDeal[ysite].IsLeftFind == 'T')
                            L_found_point++;
                    }
                    if (L_found_point > 8)              //閿熸彮纰夋嫹閿熸枻鎷峰噯鏂滈敓缁炴唻鎷�  閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹
                    {
                        D_L = ((float) (ImageDeal[Ysite + 3].LeftBorder - ImageDeal[Ysite + L_found_point].LeftBorder))
                                / ((float) (L_found_point - 3));
                        if (D_L > 0)
                        {
                            L_Found_T = 'T';

                        }
                        else
                        {
                            L_Found_T = 'F';
                            if (D_L < 0)
                                ExtenLFlag = 'F';
                        }
                        last_D_L = D_L;

                    }
                    else if ((ImageStatus.Road_type == RightCirque) && (ImageFlag.image_element_rings_flag == 7)
                            && L_found_point > 5)
                    {
                        if (L_found_point > 5)
                        {
                            D_L = ((float) (ImageDeal[Ysite + 3].LeftBorder
                                    - ImageDeal[Ysite + L_found_point].LeftBorder)) / ((float) (L_found_point - 3));
                        }
                        else
                        {
                            D_L = last_D_L;

                        }
                        if (D_L > 0)
                        {
                            L_Found_T = 'T';

                        }
                        else
                        {
                            L_Found_T = 'F';
                            if (D_L < 0)
                                ExtenLFlag = 'F';
                        }
                    }
                }

                if (L_Found_T == 'T')
                    ImageDeal[Ysite].LeftBorder = ImageDeal[ytemp_W_L].LeftBorder + D_L * (ytemp_W_L - Ysite);

                LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
                LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            }
        }
        if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite].IsRightFind == 'W')
        {
            ImageStatus.WhiteLine++;  //瑕侀敓鏂ゆ嫹閿熸枻鎷烽敓鎻鎷烽敓鐫竟锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�+1
        }
        if (ImageDeal[Ysite].IsLeftFind == 'W' && Ysite < 55)
        {
            ImageStatus.Left_Line++;
        }

        LimitL(ImageDeal[Ysite].LeftBorder);   //閿熺潾鍑ゆ嫹
        LimitH(ImageDeal[Ysite].LeftBorder);   //閿熺潾鍑ゆ嫹
        LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
        LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹

        ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
        ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;

        if (ImageDeal[Ysite].Wide <= 7)         //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鎺ユ拝鎷烽敓鏂ゆ嫹
        {
            ImageStatus.OFFLine = Ysite + 1;
            break;
        }

        else if (ImageDeal[Ysite].RightBorder <= 10 || ImageDeal[Ysite].LeftBorder >= 70)
        {
            ImageStatus.OFFLine = Ysite + 1;
            break;
        }                    //閿熸枻鎷峰浘閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鈽呮嫹閿燂拷0閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎻竟杈惧埌涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹姝㈠贰閿熸枻鎷�
    }

    return;
}
static void Second_DrawLinesProcess_R (void)  //////閿熸枻鎷烽敓鐭潻鎷烽敓鏂ゆ嫹
{
    uint8 R_Found_T = 'F';  //纭敓鏂ゆ嫹閿熺潾鎲嬫嫹鏂滈敓缁炵殑浼欐嫹鍑嗛敓鍙唻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熶茎鎲嬫嫹蹇�
    uint8 Get_R_line = 'F';  //閿熸彮纰夋嫹閿熸枻鎷蜂竴甯у浘閿熸枻鎷峰箷閿熼樁纭锋嫹閿熷彨鎲嬫嫹閿燂拷
    float D_R = 0;           //閿熸帴绛规嫹閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
    int ytemp_W_R;           //閿熸枻鎷蜂綇閿熼樁杈炬嫹閿熸彮璁规嫹閿熸枻鎷烽敓鏂ゆ嫹
    ExtenRFlag = 0;          //閿熸枻鎷峰織浣嶉敓鏂ゆ嫹0
    ImageStatus.WhiteLine = 0;
    ImageStatus.Right_Line = 0;
    for (Ysite = 58; Ysite > 54; Ysite--)
    {
        ImageDeal[Ysite].RightBorder = (ImageDeal[Ysite].RightBorder + last_mid_point) / 2;
    }
    for (Ysite = 54; Ysite > ImageStatus.OFFLine; Ysite--) //鍓�5閿熷彨杈炬嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷壙锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷55閿熷彨纰夋嫹閿熸枻鎷烽敓鍊熷畾閿熶茎璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹OFFLine閿熸枻鎷�
    {                        //澶繙閿熸枻鎷峰浘閿熸枻鎷烽敓楗鸿鎷烽敓鏂ゆ嫹OFFLine閿熺殕鐚存嫹鐗熼敓鏂ゆ嫹閿熸枻鎷烽敓锟�
        PicTemp = Pixle[Ysite];
        JumpPointtypedef JumpPoint[2];                                          // 0閿熸枻鎷�1閿熸枻鎷�
        IntervalLow = ImageDeal[Ysite + 1].RightBorder - ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹-Interval閿熶茎鐐瑰紑濮嬮敓鏂ゆ嫹纭敓鏂ゆ嫹鎵敓鍊熷紑濮嬮敓濮愶級
        IntervalHigh = ImageDeal[Ysite + 1].RightBorder + ImageScanInterval; //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹+Interval閿熶茎纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼ズ鍑ゆ嫹閿熺即顭掓嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷锛�

        LimitL(IntervalLow);   //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        LimitH(IntervalHigh);  //纭敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸垝骞堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        GetJumpPointFromDet(PicTemp, 'R', IntervalLow, IntervalHigh, &JumpPoint[1]);     //鎵敓鎻唻鎷烽敓鏂ゆ嫹

        if (JumpPoint[1].type == 'W')                                  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍐堕敓鏂ゆ嫹鍗熼敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
        {
            ImageDeal[Ysite].RightBorder = ImageDeal[Ysite + 1].RightBorder; //閿熸枻鎷烽敓鏂ゆ嫹閿熸彮鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴閿熷彨纰夋嫹閿熸枻鎷峰��
        }
        else                                                                          //閿熸彮鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        {
            ImageDeal[Ysite].RightBorder = JumpPoint[1].point;                            //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        }
        //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓绔綇鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
        ImageDeal[Ysite].IsRightFind = JumpPoint[1].type;

        //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷蜂簺閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯皨閿熺殕锟�
        if (ImageDeal[Ysite].IsRightFind == 'H')
        {

            if (ImageDeal[Ysite].IsRightFind == 'H')
                for (Xsite = (ImageDeal[Ysite].RightBorder - 1); Xsite >= (ImageDeal[Ysite].LeftBorder + 1); Xsite--)
                {
                    if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite - 1) != 0))
                    {
                        ImageDeal[Ysite].RightBorder = Xsite; //閿熸枻鎷烽敓鏂ゆ嫹鍐堕敓鏂ゆ嫹鍙╅敓鏂ゆ嫹閿熺浼欐嫹閿熷彨榛戝府鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熺殕鎲嬫嫹閿熸枻鎷风洿閿熸枻鎷峰彇閿熸枻鎷�
                        ImageDeal[Ysite].IsRightFind = 'T';
                        break;
                    }
                    else if (*(PicTemp + Xsite) != 0)
                        break;
                    else if (Xsite == (ImageDeal[Ysite].LeftBorder + 1))
                    {
                        ImageDeal[Ysite].RightBorder = Xsite;
                        ImageDeal[Ysite].IsRightFind = 'T';
                        break;
                    }
                }
            if ((ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder) <= 7)                //鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹钖呴敓锟�
            {
                ImageStatus.OFFLine = Ysite + 1;  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋枩閿燂拷7灏忛敓鍓跨尨鎷烽敓鏂ゆ嫹鐩撮敓鎺ヨ鎷疯閿熸枻鎷�
                break;
            }
        }

        /***********閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹閿熸枻鎷�************/
        int ysite = 0;
        uint8 R_found_point = 0;

        if (ImageStatus.Road_type != Ramp)
        {
            if (ImageDeal[Ysite].IsRightFind == 'W' && Ysite > 10 && Ysite < 50) //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰�奸敓鏂ゆ嫹钖囬敓鏂ゆ嫹閿燂拷
            {
                if (Get_R_line == 'F')    //閿熸枻鎷蜂竴甯у浘閿熸枻鎷锋病閿熸枻鎷烽敓鏉扮櫢鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴閿熼樁纭锋嫹鍙╂媷閿熸枻鎷烽敓杞胯鎷烽敓鏂ゆ嫹閿熸枻鎷�
                {
                    Get_R_line = 'T';       //閿熸枻鎷烽敓鏂ゆ嫹  涓�甯у浘閿熸枻鎷峰彧閿熸枻鎷蜂竴閿熸枻鎷� 閿熸枻鎷蜂负T
                    ytemp_W_R = Ysite + 2;
                    for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++)
                    {
                        if (ImageDeal[ysite].IsRightFind == 'T') //閿熸枻鎷烽敓鐫唻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  涓�閿熷閮介敓鏂ゆ嫹閿熷彨杈圭鎷�
                            R_found_point++;
                    }
                    if (R_found_point > 8) //閿熸彮纰夋嫹閿熸枻鎷峰噯鏂滈敓缁炴唻鎷�  閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹   閿熸枻鎷烽敓鍙竟鐨勭鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹8
                    {
                        D_R =
                                ((float) (ImageDeal[Ysite + R_found_point].RightBorder
                                        - ImageDeal[Ysite + 3].RightBorder)) / ((float) (R_found_point - 3));
                        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浜涢敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
                        //閿熺煫闈╂嫹閿熺潾鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃讹拷
                        if (D_R > 0)
                        {
                            R_Found_T = 'T'; //閿熸枻鎷烽敓鍙唻鎷疯殌閿熸枻鎷烽敓锟�0  閿熸枻鎷蜂箞閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃剁》鎷烽敓锟�  閿熸枻鎷蜂负閿熸枻鎷烽敓杞夸紮鎷烽敓鏂ゆ嫹
                                             //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鎲嬫嫹铓�閿熸枻鎷烽敓锟�0  灏忛敓鏂ゆ嫹0閿熸枻鎷烽敓鏂ゆ嫹閿熸彮璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹鏄犻敓锟� 娌￠敓鏂ゆ嫹瑕�
                        }
                        else
                        {
                            R_Found_T = 'F';                      //娌￠敓鏂ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼樁纭锋嫹閿燂拷
                            if (D_R < 0)
                                ExtenRFlag = 'F';      //閿熸枻鎷烽敓鏂ゆ嫹閿熻鐤氫紮鎷烽敓鏂ゆ嫹閿熺粸顕嗘嫹绾稿爲鎮寸櫢鎷烽敓锟�  閿熸枻鎷锋鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鐭鎷�
                        }
                        last_D_R = D_R;
                    }
                    else if ((ImageStatus.Road_type == LeftCirque) && (ImageFlag.image_element_rings_flag == 7)
                            && R_found_point > 5)
                    {
                        if (R_found_point > 5)
                        {
                            D_R = ((float) (ImageDeal[Ysite + R_found_point].RightBorder
                                    - ImageDeal[Ysite + 3].RightBorder)) / ((float) (R_found_point - 3));
                        }
                        else
                        {
                            D_R = last_D_R;
                        }
                        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浜涢敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
                        //閿熺煫闈╂嫹閿熺潾鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃讹拷
                        if (D_R > 0)
                        {
                            R_Found_T = 'T'; //閿熸枻鎷烽敓鍙唻鎷疯殌閿熸枻鎷烽敓锟�0  閿熸枻鎷蜂箞閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓闃剁》鎷烽敓锟�  閿熸枻鎷蜂负閿熸枻鎷烽敓杞夸紮鎷烽敓鏂ゆ嫹
                                             //閿熸枻鎷烽敓鏂ゆ嫹涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鎲嬫嫹铓�閿熸枻鎷烽敓锟�0  灏忛敓鏂ゆ嫹0閿熸枻鎷烽敓鏂ゆ嫹閿熸彮璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹鏄犻敓锟� 娌￠敓鏂ゆ嫹瑕�
                        }
                        else
                        {
                            R_Found_T = 'F';                      //娌￠敓鏂ゆ嫹閿熸彮纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熼樁纭锋嫹閿燂拷
                            if (D_R < 0)
                                ExtenRFlag = 'F';      //閿熸枻鎷烽敓鏂ゆ嫹閿熻鐤氫紮鎷烽敓鏂ゆ嫹閿熺粸顕嗘嫹绾稿爲鎮寸櫢鎷烽敓锟�  閿熸枻鎷锋鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鐭鎷�
                        }
                    }
                }
                if (R_Found_T == 'T')
                    ImageDeal[Ysite].RightBorder = ImageDeal[ytemp_W_R].RightBorder - D_R * (ytemp_W_R - Ysite); //閿熸枻鎷烽敓鏂ゆ嫹涓氶敓鏂ゆ嫹閿燂拷 閿熸枻鎷蜂箞閿熺殕浼欐嫹鍑嗛敓鏂ゆ嫹閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹

                LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
                LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            }
        }
        if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite].IsRightFind == 'W')
        {
            ImageStatus.WhiteLine++;  //瑕侀敓鏂ゆ嫹閿熸枻鎷烽敓鎻鎷烽敓鐫竟锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�+1
        }

        if (ImageDeal[Ysite].IsRightFind == 'W' && Ysite < 55)
        {
            ImageStatus.Right_Line++;
        }

        LimitL(ImageDeal[Ysite].LeftBorder);   //閿熺潾鍑ゆ嫹
        LimitH(ImageDeal[Ysite].LeftBorder);   //閿熺潾鍑ゆ嫹
        LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
        LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹

        ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
        ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;

        if (ImageDeal[Ysite].Wide <= 7)         //閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鎺ユ拝鎷烽敓鏂ゆ嫹
        {
            ImageStatus.OFFLine = Ysite + 1;
            break;
        }

        else if (ImageDeal[Ysite].RightBorder <= 10 || ImageDeal[Ysite].LeftBorder >= 70)
        {
            ImageStatus.OFFLine = Ysite + 1;
            break;
        }                    //閿熸枻鎷峰浘閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鈽呮嫹閿燂拷0閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎻竟杈惧埌涓�閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹姝㈠贰閿熸枻鎷�
    }

    return;
}
static void Sencond_DrawLinesProcess (void)  //////閿熸枻鎷烽敓鐭潻鎷烽敓鏂ゆ嫹
{
    if (ImageStatus.Road_type == Cross)
    {
        ImageStatus.Right_Line = 0;
        ImageStatus.WhiteLine = 0;
        Second_DrawLinesProcess_R();
        ImageStatus.Left_Line = 0;
        ImageStatus.WhiteLine = 0;
        Second_DrawLinesProcess_L();
    }
    else if ((ImageStatus.Right_Line >= 40 || ImageStatus.Left_Line >= 40) && ImageStatus.Left_Line >= 5
            && ImageStatus.Right_Line >= 5 && ImageStatus.Road_type != LeftCirque
            && ImageStatus.Road_type != RightCirque && ImageStatus.Road_type != LeftBarriar
            && ImageStatus.Road_type != RightBarriar)
    {
        uint8 flag_mid_black_L = 0;
        uint8 flag_mid_black_R = 0;
        for (int i = ImageDeal[ImageStatus.OFFLine + 3].Center; i >= ImageDeal[ImageStatus.OFFLine + 3].LeftBorder + 2;
                i--)
        {
            if (Pixle[ImageStatus.OFFLine + 1][i] == 0)
            {
                flag_mid_black_L = 1;
                break;
            }
        }
        for (int i = ImageDeal[ImageStatus.OFFLine + 3].Center; i <= ImageDeal[ImageStatus.OFFLine + 3].RightBorder - 2;
                i++)
        {
            if (Pixle[ImageStatus.OFFLine + 1][i] == 0)
            {

                flag_mid_black_R = 1;
                break;
            }
        }
        if (flag_mid_black_L || flag_mid_black_R)
        {
            if (ImageStatus.Right_Line > 40
                    && (((Real_state[2][1] == 'W' && Real_state[3][1] == 'W') || Real_state[4][1] == 'W')
                            || ImageStatus.Right_Line >= 50 || flag_mid_black_R
                            || (Pixle[ImageStatus.OFFLine + 2][ImageDeal[ImageStatus.OFFLine + 2].Center] == 0)
                            || (Pixle[ImageStatus.OFFLine + 1][ImageDeal[ImageStatus.OFFLine + 1].Center] == 0)))
            {

                ImageStatus.Right_Line = 0;
                ImageStatus.WhiteLine = 0;
                Second_DrawLinesProcess_R();
            }
            if (ImageStatus.Left_Line > 40
                    && (((Real_state[2][0] == 'W' && Real_state[3][0] == 'W') || Real_state[4][0] == 'W')
                            || ImageStatus.Left_Line >= 50 || flag_mid_black_L
                            || (Pixle[ImageStatus.OFFLine + 2][ImageDeal[ImageStatus.OFFLine + 2].Center] == 0)
                            || (Pixle[ImageStatus.OFFLine + 1][ImageDeal[ImageStatus.OFFLine + 1].Center] == 0)))
            {

                ImageStatus.Left_Line = 0;
                ImageStatus.WhiteLine = 0;
                Second_DrawLinesProcess_L();
            }
        }
    }
}
static void DrawExtensionLine (void)        //閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绛规嫹閿熺璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷烽敓绐栬鎷烽敓绔鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
{
    if ((ImageStatus.Road_type != Ramp) && ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque) // g5.22  6.22閿熸枻鎷烽敓鏂ゆ嫹娉ㄩ敓鏂ゆ嫹  閿熻寰楁敼浼欐嫹閿熸枻鎷�
    {
        if (ImageStatus.WhiteLine >= ImageStatus.TowPoint_True - 15)
            TFSite = 55;
//        if (ExtenLFlag != 'F')
        for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4); Ysite--) //閿熸帴纰夋嫹閿熸枻鎷烽敓鍙尅鎷峰閿熸枻鎷烽敓鏂ゆ嫹鎵壂閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�   閿熸枻鎷峰熬閿熸枻鎷烽敓锟�
                                                                      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰彧閿熸枻鎷蜂竴閿熸枻鎷�
        {
            PicTemp = Pixle[Ysite];           //閿熻姤褰撳墠閿熸枻鎷�
            if (ImageDeal[Ysite].IsLeftFind == 'W') //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺鏂ゆ嫹娌℃壂閿熸枻鎷烽敓鏂ゆ嫹鎵敓鏂ゆ嫹閿熸枻鎷烽敓瑙掑府鎷疯壊閿熸枻鎷疯閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋病閿熸枻鎷烽敓鏂ゆ嫹鍛撻敓鏂ゆ嫹
            {
                //**************************************************//**************************************************
                if (ImageDeal[Ysite + 1].LeftBorder >= 70)                    //閿熸枻鎷烽敓鏂ゆ嫹閿熺鏂ゆ嫹瀹為敓鏂ゆ嫹閿熸枻鎷峰お閿熸彮鎲嬫嫹
                {
                    ImageStatus.OFFLine = Ysite + 1;
                    break;                        //鐩撮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
                }
                //************************************************//*************************************************

                while (Ysite >= (ImageStatus.OFFLine + 4))                    //閿熸枻鎷锋椂閿熸枻鎷锋病鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
                {
                    Ysite--;                      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鎵�
                    if (ImageDeal[Ysite].IsLeftFind == 'T' && ImageDeal[Ysite - 1].IsLeftFind == 'T'
                            && ImageDeal[Ysite - 2].IsLeftFind == 'T' && ImageDeal[Ysite - 2].LeftBorder > 0
                            && ImageDeal[Ysite - 2].LeftBorder < 70) //閿熸枻鎷烽敓缂搭煉鎷烽敓鏂ゆ嫹閿熸枻鎷疯皭閿熸枻鎷烽敓鏂ゆ嫹鐬敓鏂ゆ嫹鍐堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍗搁敓鏂ゆ嫹閿熸枻鎷烽敓绔枻鎷锋倴顭掓嫹閿熺鏂ゆ嫹閿熻妭绌哄府鎷烽敓杈冨嚖鎷烽敓鏂ゆ嫹
                    {
                        FTSite = Ysite - 2;          //閿熺獤鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风墶璇欓敓鏂ゆ嫹鍐欓敓鏂ゆ嫹閿熺唇Tsite
                        break;
                    }
                }

                DetL = ((float) (ImageDeal[FTSite].LeftBorder - ImageDeal[TFSite].LeftBorder))
                        / ((float) (FTSite - TFSite));  //閿熸枻鎷峰憮閿熸枻鎷锋枩閿熺粸锝忔嫹閿熷彨纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷/閿熷彨纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
                if (FTSite > ImageStatus.OFFLine)
                    for (ytemp = TFSite; ytemp >= FTSite; ytemp--) //閿熸帴纰夋嫹涓�閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰憮閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯瘷閿熸枻鎷锋閿熸枻鎷烽敓鏂ゆ嫹鏄曨�╃》鎷烽敓鏂ゆ嫹閿熺即銊栨唻鎷烽敓鏂ゆ嫹鐬婚敓鏂ゆ嫹鎴忛敓鏂ゆ嫹閿熸枻鎷烽敓绔枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻锟�
                    {
                        ImageDeal[ytemp].LeftBorder = (int) (DetL * ((float) (ytemp - TFSite)))
                                + ImageDeal[TFSite].LeftBorder; //閿熸枻鎷烽敓鏂ゆ嫹閿熻妭纭锋嫹鐩灮鐘堕敓鏂ゆ嫹閿熸枻鎷锋捀閿熸枻鎷烽敓鍙唻鎷锋捀閿熸枻鎷烽敓渚ュ尅鎷烽敓鏂ゆ嫹娆犻敓鏂ゆ嫹閿熼叺纭锋嫹閿熸枻鎷烽敓锟�
                    }
            }
            else
                TFSite = Ysite + 2; //閿熸枻鎷烽敓缂搭煉鎷烽敓鏂ゆ嫹鍚敓鏂ゆ嫹姊伴敓鏂ゆ嫹閿熺鐣岋紝閿熸枻鎷烽敓鍙揪鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻姤锛岄敓鏂ゆ嫹閿熸枻鎷锋枩閿熺粸锝忔嫹
        }

        if (ImageStatus.WhiteLine >= ImageStatus.TowPoint_True - 15)
            TFSite = 55;
        // g5.22
        if (ImageStatus.CirqueOff == 'T' && ImageStatus.Road_type == RightCirque)
            TFSite = 55;
//        if (ExtenRFlag != 'F')
        for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4); Ysite--) //閿熸帴纰夋嫹閿熸枻鎷烽敓鍙尅鎷峰閿熸枻鎷烽敓鏂ゆ嫹鎵壂閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
        {
            PicTemp = Pixle[Ysite];  //閿熻姤褰撳墠閿熸枻鎷�

            if (ImageDeal[Ysite].IsRightFind == 'W') //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍐跺憮閿熺煫绨ь煉鎷烽敓鏂ゆ嫹閿熺即顭掓嫹閿熸枻鎷烽敓鏂ゆ嫹鍓嶉敓缂搭偓鎷烽敓鍓跨鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鐭紮鎷烽敓鏂ゆ嫹鍐跺憮閿熷锛岄敓鏂ゆ嫹閿熻杈炬嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺傜鎷�
            {
                if (ImageDeal[Ysite + 1].RightBorder <= 10)                 //閿熸枻鎷烽敓鏂ゆ嫹鍐跺憮閿熺粸纰夋嫹閿熸枻鎷烽敓鏁欘偓鎷烽敓鏂ゆ嫹
                {
                    ImageStatus.OFFLine = Ysite + 1; //鐩撮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
                    break;
                }
                while (Ysite >= (ImageStatus.OFFLine + 4))           //閿熸枻鎷锋椂閿熸枻鎷锋病鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
                {
                    Ysite--;
                    if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite - 1].IsRightFind == 'T'
                            && ImageDeal[Ysite - 2].IsRightFind == 'T' && ImageDeal[Ysite - 2].RightBorder < 70
                            && ImageDeal[Ysite - 2].RightBorder > 10) //閿熸枻鎷烽敓缂搭煉鎷烽敓鏂ゆ嫹閿熸枻鎷疯皭閿熸枻鎷烽敓鏂ゆ嫹鐬敓鏂ゆ嫹鍐堕敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍗搁敓鏂ゆ嫹閿熸枻鎷烽敓绔枻鎷锋倴顭掓嫹閿熺鏂ゆ嫹閿熻妭绌哄府鎷烽敓杈冨嚖鎷烽敓鏂ゆ嫹
                    {
                        FTSite = Ysite - 2;                             // 閿熺獤鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风墶璇欓敓鏂ゆ嫹鍐欓敓鏂ゆ嫹閿熺唇Tsite
                        break;
                    }
                }

                DetR = ((float) (ImageDeal[FTSite].RightBorder - ImageDeal[TFSite].RightBorder))
                        / ((float) (FTSite - TFSite));      //閿熸彮杈规枻鎷烽敓鍙唻鎷峰墿閿熸枻鎷锋閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�/閿熷彨纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
                if (FTSite > ImageStatus.OFFLine)
                    for (ytemp = TFSite; ytemp >= FTSite; ytemp--) //閿熸帴纰夋嫹涓�閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿熸彮杈规枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺傝鎷烽敓鍙鎷烽敓鏂ゆ嫹閿熺枼寮�濮嬮敓鏂ゆ嫹閿熸枻鎷锋壂鐩撮敓鏂ゆ嫹閿熺Ц甯嫹閿熻緝鍑ゆ嫹閿熸枻鎷烽敓鎻竟鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楋拷
                    {
                        ImageDeal[ytemp].RightBorder = (int) (DetR * ((float) (ytemp - TFSite)))
                                + ImageDeal[TFSite].RightBorder; //閿熸枻鎷烽敓鏂ゆ嫹閿熻妭纭锋嫹鐩灮鐘堕敓鏂ゆ嫹閿熸枻鎷锋捀閿熸枻鎷烽敓鍙唻鎷锋捀閿熸枻鎷烽敓渚ュ尅鎷烽敓鏂ゆ嫹娆犻敓鏂ゆ嫹閿熼叺纭锋嫹閿熸枻鎷烽敓锟�
                    }
            }
            else
                TFSite = Ysite + 2; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋閿熸枻鎷峰喍鍛撻敓鏂ゆ嫹涓氶敓鏂ゆ嫹鑰嶉敓鏂ゆ嫹閿熺獤闈╂嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯瘷閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閫忛敓绲婩site
        }
    }
    for (Ysite = 59; Ysite >= ImageStatus.OFFLine; Ysite--)
    {
        ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2; //鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姒叏鎷峰憰閿熻顕嗘嫹閿熸枻鎷烽敓鍙》鎷峰�奸敓鏂ゆ嫹閿熸枻鎷�
        ImageDeal[Ysite].Wide = -ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder; //閿熸枻鎷烽敓鑴氫紮鎷蜂箣閿熸枻鎷风洰閿熸枻鎷峰嵈閿熸枻鎷烽敓锟�
    }
}
/*閿熻緝鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楀嚖鎷烽敓鏂ゆ嫹鎵敓绔綇鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熷彨璁规嫹鍏冮敓鎴殑绗鎷烽敓鏂ゆ嫹閿熸枻鎷�*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Bottom_Line_OTSU
//  @brief          閿熸枻鎷峰彇閿熼樁璇ф嫹閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        閿熸枻鎷烽敓鏂ゆ嫹閿熼叺纭锋嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//  @param          Row                                     鍥鹃敓鏂ゆ嫹閿熺祼site
//  @param          Col                                     鍥鹃敓鏂ゆ嫹閿熺祻site
//  @return         Bottonline                              閿熼樁鎲嬫嫹閿熸枻鎷烽�夐敓鏂ゆ嫹
//  @time           2022閿熸枻鎷�10閿熸枻鎷�9閿熸枻鎷�
//  @Author
//  Sample usage:   Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------
void Search_Bottom_Line_OTSU (uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{

//瀵婚敓鏂ゆ嫹閿熸枻鎷峰紤鍛撻敓锟�
    for (int Xsite = ImageDeal[Bottonline].Center; Xsite > 1; Xsite--)
    {
        if (imageInput[Bottonline][Xsite] == 1 && imageInput[Bottonline][Xsite - 1] == 0)
        {
            ImageDeal[Bottonline].LeftBoundary = Xsite;                                       //閿熸枻鎷峰彇閿熼樁鎲嬫嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
            break;
        }
    }
    for (int Xsite = ImageDeal[Bottonline].Center; Xsite < LCDW - 1; Xsite++)
    {
        if (imageInput[Bottonline][Xsite] == 1 && imageInput[Bottonline][Xsite + 1] == 0)
        {
            ImageDeal[Bottonline].RightBoundary = Xsite;                                       //閿熸枻鎷峰彇閿熼樁鎲嬫嫹閿熸彮鎲嬫嫹閿熸枻鎷�
            break;
        }
    }

}

uint16 points_l[(uint16) 400][2] = {{0}};                                       //閿熸枻鎷烽敓鏂ゆ嫹
uint16 points_r[(uint16) 400][2] = {{0}};                                       //閿熸枻鎷烽敓鏂ゆ嫹
uint16 data_stastics_l = 0;                                       //缁熼敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹涓氶敓鏂ゆ嫹閿熶茎闈╂嫹閿熸枻鎷�
uint16 data_stastics_r = 0;                                       //缁熼敓鏂ゆ嫹閿熸彮鎲嬫嫹閿熸彮纰夋嫹閿熸枻鎷锋瘝閿熸枻鎷烽敓锟�
void Search_LEFT (uint16 break_flag, uint8 l_start_x, uint8 l_start_y)
{

    uint8 i = 0, j = 0;
    for (i = 0; i < LCDH; i++)
    {
        Pixle[i][0] = 0;
        Pixle[i][1] = 0;
    }
//閿熸枻鎷峰紤閿熸枻鎷烽敓锟�
    uint8 search_filds_l[8][2] = {{0}};
    uint8 index_l = 0;
    uint8 temp_l[8][2] = {{0}};
    uint8 center_point_l[2] = {0};
    uint16 l_data_statics;                                       //缁熼敓鏂ゆ嫹閿熸枻鎷烽敓锟�
//閿熸枻鎷烽敓鏂ゆ嫹鐑侀敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    static int8 seeds_l[8][2] = {{0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, };
//{-1,-1},{0,-1},{+1,-1},
//{-1, 0},       {+1, 0},
//{-1,+1},{0,+1},{+1,+1},
//閿熸枻鎷烽敓鏂ゆ嫹閿熷壙鍛堟唻鎷烽敓锟�
//閿熸枻鎷蜂竴閿熻娇闈╂嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�  閿熸枻鎷烽敓鎻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楃鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    center_point_l[0] = l_start_x;    //x
    center_point_l[1] = l_start_y;    //y
//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹寰敓鏂ゆ嫹
    while (break_flag--)
    {

        //閿熸枻鎷烽敓锟�
        for (i = 0; i < 8; i++)    //閿熸枻鎷烽敓鏂ゆ嫹8F閿熸枻鎷烽敓鏂ゆ嫹
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];    //x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];    //y
        }
        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎴掑埌閿熺獤鎾呮嫹閿熸彮纰夋嫹閿熶茎纰夋嫹閿熸枻鎷�
        points_l[l_data_statics][0] = center_point_l[0];    //x
        points_l[l_data_statics][1] = center_point_l[1];    //y
        l_data_statics++;    //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴
        index_l = 0;    //閿熸枻鎷烽敓鏂ゆ嫹閿熷锛岄敓鏂ゆ嫹浣块敓鏂ゆ嫹
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;    //閿熸枻鎷烽敓鏂ゆ嫹閿熷锛岄敓鏂ゆ嫹浣块敓鏂ゆ嫹
            temp_l[i][1] = 0;    //閿熸枻鎷烽敓鏂ゆ嫹閿熷锛岄敓鏂ゆ嫹浣块敓鏂ゆ嫹
        }

        //閿熸枻鎷烽敓鏂ゆ嫹鍗搁敓锟�
        for (i = 0; i < 8; i++)
        {
            if (Pixle[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                    && Pixle[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 1)
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
//                DIR_2[l_data_statics - 1].LEFT = (i);    //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
            }

            if (index_l)
            {
                //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
                center_point_l[0] = temp_l[0][0];    //x
                center_point_l[1] = temp_l[0][1];    //y
                for (j = 0; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];    //x
                        center_point_l[1] = temp_l[j][1];    //y
                    }
                }
            }

        }
        if (l_data_statics >= 3
                && (points_l[l_data_statics - 1][0] == points_l[l_data_statics - 2][0]
                        && points_l[l_data_statics - 1][0] == points_l[l_data_statics - 3][0]
                        && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 2][1]
                        && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 3][1]))
        {
            break;
        }
        if (points_l[l_data_statics - 1][1] < ImageStatus.OFFLine)
        {
            //printf("\n閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷壙绛规嫹\n");
//            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//鍙栭敓鏂ゆ嫹閿熸枻鎷峰彥閿燂拷
            //printf("\n閿熸枻鎷穣=%d閿熸枻鎷烽敓鍓跨鎷穃n",*hightest);
            break;
        }

    }
    data_stastics_l = l_data_statics;
}
void Search_RIGHT (uint16 break_flag, uint8 r_start_x, uint8 r_start_y)
{
    uint8 i = 0, j = 0;
    for (i = 0; i < LCDH; i++)
    {
        Pixle[i][LCDW - 1] = 0;
        Pixle[i][LCDW - 2] = 0;
    }
//閿熸彮杈规唻鎷烽敓鏂ゆ嫹
    uint8 search_filds_r[8][2] = {{0}};
    uint8 center_point_r[2] = {0};    //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
    uint8 index_r = 0;    //閿熸枻鎷烽敓鏂ゆ嫹閿熼摪鎲嬫嫹
    uint8 temp_r[8][2] = {{0}};
    uint16 r_data_statics;    //缁熼敓鏂ゆ嫹閿熸彮鎲嬫嫹
//閿熸枻鎷烽敓鏂ゆ嫹鐑侀敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    static int8 seeds_r[8][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, };
//{-1,-1},{0,-1},{+1,-1},
//{-1, 0},       {+1, 0},
//{-1,+1},{0,+1},{+1,+1},
//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓缁炴唻鎷烽敓锟�
//閿熸枻鎷蜂竴閿熻娇闈╂嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�  閿熸枻鎷烽敓鎻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楃鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    center_point_r[0] = r_start_x;    //x
    center_point_r[1] = r_start_y;    //y
//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹寰敓鏂ゆ嫹
    while (break_flag--)
    {
        //閿熸彮鎲嬫嫹
        for (i = 0; i < 8; i++)    //閿熸枻鎷烽敓鏂ゆ嫹8F閿熸枻鎷烽敓鏂ゆ嫹
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];    //x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];    //y
        }
        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎴掑埌閿熺獤鎾呮嫹閿熸彮纰夋嫹閿熶茎纰夋嫹閿熸枻鎷�
        points_r[r_data_statics][0] = center_point_r[0];    //x
        points_r[r_data_statics][1] = center_point_r[1];    //y
        if (r_data_statics >= 3
                && (points_r[r_data_statics][0] == points_r[r_data_statics - 1][0]
                        && points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
                        && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1]
                        && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1]))
        {
            break;
        }
        r_data_statics++;            //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴

        index_r = 0;            //閿熸枻鎷烽敓鏂ゆ嫹閿熷锛岄敓鏂ゆ嫹浣块敓鏂ゆ嫹
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;            //閿熸枻鎷烽敓鏂ゆ嫹閿熷锛岄敓鏂ゆ嫹浣块敓鏂ゆ嫹
            temp_r[i][1] = 0;            //閿熸枻鎷烽敓鏂ゆ嫹閿熷锛岄敓鏂ゆ嫹浣块敓鏂ゆ嫹
        }
        //閿熸彮鎲嬫嫹閿熷彨璁规嫹
        for (i = 0; i < 8; i++)
        {
            if (Pixle[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                    && Pixle[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;            //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷蜂竴
//                DIR_2[r_data_statics - 1].RIGHT = (i);            //閿熸枻鎷峰綍閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
                center_point_r[0] = temp_r[0][0];                //x
                center_point_r[1] = temp_r[0][1];                //y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];                //x
                        center_point_r[1] = temp_r[j][1];                //y
                    }
                }

            }
        }
        if (points_l[r_data_statics - 1][1] < ImageStatus.OFFLine)
        {
            break;
        }
    }
    data_stastics_r = r_data_statics;
}

void Search_Left_and_Right_Lines (uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
//閿熸枻鎷烽敓鏂ゆ嫹灏忛敓鍓跨殑纰夋嫹鍓嶉敓鏂ゆ嫹閿熸枻鎷风姸鎬佷綅閿熸枻鎷蜂负 閿熸枻鎷� 閿熸枻鎷� 閿熸枻鎷� 閿熸枻鎷� 涓�閿熸枻鎷疯閿熸枻鎷� 閿熻緝锝忔嫹閿熸枻鎷烽敓杞款亷鎷烽敓缂达拷 閿熸枻鎷烽敓杈冩唻鎷蜂负閿熸枻鎷疯壊 閿熼摪锝忔嫹閿熸彮鎲嬫嫹涓鸿壊  閿熸彮锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熷彨鐚存嫹鑹�
    /*  鍓嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻棄锛�
     *   0
     * 3   1
     *   2
     */
    /*瀵婚敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷*/
    uint8 Left_Rule[2][8] = {{0, -1, 1, 0, 0, 1, -1, 0},                            //{0,-1},{1,0},{0,1},{-1,0},  (x,y )
            {-1, -1, 1, -1, 1, 1, -1, 1} //{-1,-1},{1,-1},{1,1},{-1,1}
    };
    /*瀵婚敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷*/
    int Right_Rule[2][8] = {{0, -1, 1, 0, 0, 1, -1, 0}, //{0,-1},{1,0},{0,1},{-1,0},
            {1, -1, 1, 1, -1, 1, -1, -1} //{1,-1},{1,1},{-1,1},{-1,-1}
    };
    uint32 num = 0;
    uint8 Left_Ysite = Bottonline;
    uint8 Left_Xsite = (uint8) ImageDeal[Bottonline].LeftBoundary;
    uint8 Left_Rirection = 0; //閿熸枻鎷峰徎閿熸枻鎷烽敓锟�
    uint8 Pixel_Left_Ysite = Bottonline;
    uint8 Pixel_Left_Xsite = 0;

    uint8 Right_Ysite = Bottonline;
    uint8 Right_Xsite = (uint8) ImageDeal[Bottonline].RightBoundary;
    uint8 Right_Rirection = 0; //閿熸彮杈瑰嚖鎷烽敓鏂ゆ嫹
    uint8 Pixel_Right_Ysite = Bottonline;
    uint8 Pixel_Right_Xsite = 0;
    uint8 Ysite = Bottonline;
    ImageStatus.OFFLineBoundary = 5;
    while (1)
    {
        num++;
        if (num > 400)
        {
            ImageStatus.OFFLineBoundary = Ysite;
            break;
        }
        if (Ysite >= Pixel_Left_Ysite && Ysite >= Pixel_Right_Ysite)
        {
            if (Ysite < ImageStatus.OFFLineBoundary)
            {
                ImageStatus.OFFLineBoundary = Ysite;
                break;
            }
            else
            {
                Ysite--;
            }
        }
        /*********閿熸枻鎷烽敓绐栬鎷烽敓锟�*******/
        if ((Pixel_Left_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary) //閿熸彮鎲嬫嫹鎵敓鏂ゆ嫹
        {
            /*閿熸枻鎷烽敓鏂ゆ嫹鍓嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹*/
            Pixel_Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
            Pixel_Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];

            if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0) //鍓嶉敓鏂ゆ嫹閿熻鐚存嫹鑹�
            {
                //椤烘椂閿熸枻鎷烽敓鏂ゆ嫹杞�90
                if (Left_Rirection == 3)
                {
                    Left_Rirection = 0;
                }
                else
                {
                    Left_Rirection++;
                }
                DIR_1[num].LEFT = Left_Rirection;
                DIR_1_Y[num].LEFT = Left_Ysite;
                DIR_1_X[num].LEFT = Left_Xsite;

            }
            else //鍓嶉敓鏂ゆ嫹閿熻甯嫹鑹�
            {
                /*閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�*/
                Pixel_Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                Pixel_Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];

                if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0) //閿熸枻鎷峰墠閿熸枻鎷蜂负閿熸枻鎷疯壊
                {
                    //閿熸枻鎷烽敓娲ヤ笉鎲嬫嫹  Left_Rirection
                    Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0)
                    {
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                        ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                    }
                    DIR_1[num].LEFT = Left_Rirection;
                    DIR_1_Y[num].LEFT = Left_Ysite;
                    DIR_1_X[num].LEFT = Left_Xsite;

                }
                else //閿熸枻鎷峰墠閿熸枻鎷蜂负閿熸枻鎷疯壊
                {
                    // 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓渚ユ唻鎷� Left_Rirection  閿熸枻鎷锋椂閿熸枻鎷�90閿熸枻鎷�
                    Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0)
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                    ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                    if (Left_Rirection == 0)
                        Left_Rirection = 3;
                    else
                        Left_Rirection--;
                    DIR_1[num].LEFT = Left_Rirection;
                    DIR_1_Y[num].LEFT = Left_Ysite;
                    DIR_1_X[num].LEFT = Left_Xsite;

                }

            }
        }
        /*********閿熸彮鎲嬫嫹宸￠敓鏂ゆ嫹*******/
        if ((Pixel_Right_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary) //閿熸彮鎲嬫嫹鎵敓鏂ゆ嫹
        {
            /*閿熸枻鎷烽敓鏂ゆ嫹鍓嶉敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹*/
            Pixel_Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
            Pixel_Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];

            if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0) //鍓嶉敓鏂ゆ嫹閿熻鐚存嫹鑹�
            {
                //閿熸枻鎷锋椂閿熸枻鎷烽敓鏂ゆ嫹杞�90
                if (Right_Rirection == 0)
                    Right_Rirection = 3;
                else
                    Right_Rirection--;
                DIR_1[num].RIGHT = Right_Rirection;
                DIR_1_Y[num].RIGHT = Right_Ysite;
                DIR_1_X[num].RIGHT = Right_Xsite;

            }
            else //鍓嶉敓鏂ゆ嫹閿熻甯嫹鑹�
            {
                /*閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�*/
                Pixel_Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                Pixel_Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];

                if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0) //閿熸枻鎷峰墠閿熸枻鎷蜂负閿熸枻鎷疯壊
                {
                    //閿熸枻鎷烽敓娲ヤ笉鎲嬫嫹  Right_Rirection
                    Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 79)
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                    DIR_1[num].RIGHT = Right_Rirection;
                    DIR_1_Y[num].RIGHT = Right_Ysite;
                    DIR_1_X[num].RIGHT = Right_Xsite;

                }
                else //閿熸枻鎷峰墠閿熸枻鎷蜂负閿熸枻鎷疯壊
                {
                    // 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓渚ユ唻鎷� Right_Rirection  閿熸枻鎷锋椂閿熸枻鎷�90閿熸枻鎷�
                    Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 79)
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                    if (Right_Rirection == 3)
                        Right_Rirection = 0;
                    else
                        Right_Rirection++;
                    DIR_1[num].RIGHT = Right_Rirection;
                    DIR_1_Y[num].RIGHT = Right_Ysite;
                    DIR_1_X[num].RIGHT = Right_Xsite;

                }

            }
        }

        if (abs(Pixel_Right_Xsite - Pixel_Left_Xsite) < 3) //Ysite<80閿熸枻鎷蜂负閿熷壙鍑ゆ嫹閿熻妭搴曡鎷烽敓瑙掑府鎷烽敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷烽敓鏂ゆ嫹閿燂拷  3 && Ysite < 30
        {

            ImageStatus.OFFLineBoundary = Ysite;
            break;
        }

    }
    dir_stop = num;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Border_OTSU
//  @brief          閫氶敓鏂ゆ嫹OTSU閿熸枻鎷峰彇閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷烽敓鏂ゆ嫹鎭�
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        閿熸枻鎷烽敓鏂ゆ嫹閿熼叺纭锋嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//  @param          Row                                     鍥鹃敓鏂ゆ嫹閿熺祼site
//  @param          Col                                     鍥鹃敓鏂ゆ嫹閿熺祻site
//  @param          Bottonline                              閿熼樁鎲嬫嫹閿熸枻鎷烽�夐敓鏂ゆ嫹
//  @return         閿熸枻鎷�
//  @time           2022閿熸枻鎷�10閿熸枻鎷�7閿熸枻鎷�
//  @Author
//  Sample usage:   Search_Border_OTSU(mt9v03x_image, IMAGE_ROW, IMAGE_COL, IMAGE_ROW-8);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Border_OTSU (uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    ImageStatus.WhiteLine_L = 0;
    ImageStatus.WhiteLine_R = 0;
//ImageStatus.OFFLine = 1;
    /*閿熸枻鎷烽敓鏂ゆ嫹閿熼摪杈圭晫澶勯敓鏂ゆ嫹*/
    for (int Xsite = 0; Xsite < LCDW; Xsite++)
    {
        imageInput[0][Xsite] = 0;
        imageInput[Bottonline + 1][Xsite] = 0;
    }
    /*閿熸枻鎷烽敓鏂ゆ嫹閿熸彮杈圭晫澶勯敓鏂ゆ嫹*/
    for (int Ysite = 0; Ysite < LCDH; Ysite++)
    {
        ImageDeal[Ysite].LeftBoundary_First = 0;
        ImageDeal[Ysite].RightBoundary_First = 79;

        imageInput[Ysite][0] = 0;
        imageInput[Ysite][LCDW - 1] = 0;
    }
    /********閿熸枻鎷峰彇閿熼樁璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹*********/
    Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
    /********閿熸枻鎷峰彇閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹*********/
    Search_Left_and_Right_Lines(imageInput, Row, Col, Bottonline);

    for (int Ysite = Bottonline; Ysite > ImageStatus.OFFLineBoundary + 1; Ysite--)
    {
        if (ImageDeal[Ysite].LeftBoundary < 3)
        {
            ImageStatus.WhiteLine_L++;
        }
        if (ImageDeal[Ysite].RightBoundary > LCDW - 3)
        {
            ImageStatus.WhiteLine_R++;
        }
    }
}

//閿熸枻鎷烽敓琛楄鎷烽敓绔鎷锋椂閿熸枻鎷�  閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹閿熷彨纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹
static void RouteFilter (void)
{
    for (Ysite = 58; Ysite >= (ImageStatus.OFFLine + 5); Ysite--)                                   //閿熸帴鍖℃嫹濮嬩綅閿熸枻鎷峰仠姝綅
    {
        if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite].IsRightFind == 'W' && Ysite <= 45
                && ImageDeal[Ysite - 1].IsLeftFind == 'W' && ImageDeal[Ysite - 1].IsRightFind == 'W') //閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熸彮璁规嫹閿熺潾杈癸綇鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍓�45閿熸枻鎷�   閿熷壙璇ф嫹
        {
            ytemp = Ysite;
            while (ytemp >= (ImageStatus.OFFLine + 5))     // 閿熶茎闈╂嫹閿熸枻鎷烽敓鐨嗭綇鎷�-6鏁堥敓鏂ゆ嫹閿熸枻鎷蜂竴浜�
            {
                ytemp--;
                if (ImageDeal[ytemp].IsLeftFind == 'T' && ImageDeal[ytemp].IsRightFind == 'T') //瀵婚敓鏂ゆ嫹閿熸枻鎷烽敓绔鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓渚ワ綇鎷烽敓鎻鎷烽敓璇湰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋湪绛掗敓鏂ゆ嫹閿熸枻鎷烽敓锟�
                {
                    DetR = (float) (ImageDeal[ytemp - 1].Center - ImageDeal[Ysite + 2].Center)
                            / (float) (ytemp - 1 - Ysite - 2);          //閿熸枻鎷锋枩閿熸枻鎷�
                    int CenterTemp = ImageDeal[Ysite + 2].Center;
                    int LineTemp = Ysite + 2;
                    while (Ysite >= ytemp)
                    {
                        ImageDeal[Ysite].Center = (int) (CenterTemp + DetR * (float) (Ysite - LineTemp)); //閿熸枻鎷锋枩閿熺粸璇ф嫹
                        Ysite--;
                    }
                    break;
                }
            }
        }
        ImageDeal[Ysite].Center = (ImageDeal[Ysite - 1].Center + 2 * ImageDeal[Ysite].Center) / 3; //閿熸枻鎷峰钩閿熸枻鎷烽敓鏂ゆ嫹搴旈敓鐭紮鎷峰唹鍖ｉ敓锟�  閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰钩閿熸枻鎷�
    }
}

int icm_start_test_cross = 0;  //閿熸枻鎷烽敓鏂ゆ嫹icm閿熸枻鎷烽敓琛楁唻鎷峰織浣�

/****鍦嗛敓鏂ゆ嫹閿熸枻鎷烽敓锟�***/
uint8 Pass_flag = 'F';
//uint8 Left_Less_Num = 0;
//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
int ceshi_flag;
//--------------------------------------------------------------
//  @name           Element_Judgment_Left_Rings()
//  @brief          閿熸枻鎷烽敓鏂ゆ嫹鍥鹃敓鏂ゆ嫹閿熷彨鏂鎷烽敓鎺ョ尨鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙鎷烽敓鏂ゆ嫹鍦嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Judgment_Left_Rings();
//--------------------------------------------------------------
void Element_Judgment_Left_Rings ()
{

    if (ImageStatus.Right_Line > 20 || ImageStatus.Left_Line < 10 // 13
    || ImageStatus.OFFLine > 5
            ||
//    Straight_Judge_1(2, 25, 45) > 1 ||
            ImageStatus.WhiteLine > 15 || Straight_Judge_2(RIGHT, ImageStatus.OFFLine + 1, 50) > 0.1
            || ImageDeal[30].IsRightFind == 'W'

//                    || ImageDeal[40].IsRightFind == 'W'
            || ImageDeal[20].IsRightFind == 'W' || ImageDeal[10].IsRightFind == 'W')
    {
//printf("%f\n\r",Straight_Judge_1(2, 25, 45));
        return;
    }
//    printf("ppp");
    int ring_ysite = 25;
    int mid_test_points_Y = 0;
    int mid_test_points_X = 0;
    uint8 point_flag = 0;
    uint8 Ring_Help_Flag = 0;                      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰織
//  uint8 Left_Less_Num = 0;
    Left_RingsFlag_Point1_Ysite = 0;
    Left_RingsFlag_Point2_Ysite = 0;
//   ceshi_flag = 1;
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite - 1].LeftBoundary_First > 4)
        {
            Left_RingsFlag_Point1_Ysite = Ysite;
//            printf("test2\n\r");
            break;
        }
    }
//    printf("\n\r");
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
//        printf("%d,%d\n\r",Ysite,ImageDeal[Ysite].LeftBoundary - ImageDeal[Ysite - 1].LeftBoundary);

        if (ImageDeal[Ysite + 1].LeftBoundary - ImageDeal[Ysite].LeftBoundary > 4)
        {
            Left_RingsFlag_Point2_Ysite = Ysite;
//            printf("test3\n\r");

            break;
        }
    }
    for (int Ysite = Left_RingsFlag_Point1_Ysite; Ysite > 25; Ysite--)
    {
        if (((ImageDeal[Ysite + 4].LeftBorder < ImageDeal[Ysite].LeftBorder
                && ImageDeal[Ysite + 3].LeftBorder < ImageDeal[Ysite].LeftBorder
                && ImageDeal[Ysite + 4].LeftBorder + 4 > ImageDeal[Ysite].LeftBorder
                && ImageDeal[Ysite + 3].LeftBorder + 4 > ImageDeal[Ysite].LeftBorder)
                || (ImageDeal[Ysite + 5].LeftBorder < ImageDeal[Ysite].LeftBorder
                        && ImageDeal[Ysite + 3].LeftBorder < ImageDeal[Ysite].LeftBorder
                        && ImageDeal[Ysite + 5].LeftBorder + 4 > ImageDeal[Ysite].LeftBorder
                        && ImageDeal[Ysite + 3].LeftBorder + 4 > ImageDeal[Ysite].LeftBorder)
                || (ImageDeal[Ysite + 4].LeftBorder < ImageDeal[Ysite].LeftBorder
                        && ImageDeal[Ysite + 2].LeftBorder < ImageDeal[Ysite].LeftBorder
                        && ImageDeal[Ysite + 4].LeftBorder + 4 > ImageDeal[Ysite].LeftBorder
                        && ImageDeal[Ysite + 2].LeftBorder + 4 > ImageDeal[Ysite].LeftBorder))
                && ((ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite - 2].LeftBorder
                        && ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite - 3].LeftBorder
                        && ImageDeal[Ysite].LeftBorder - 4 < ImageDeal[Ysite - 2].LeftBorder
                        && ImageDeal[Ysite].LeftBorder - 4 < ImageDeal[Ysite - 3].LeftBorder)
                        || (ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite - 1].LeftBorder
                                && ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite - 3].LeftBorder
                                && ImageDeal[Ysite].LeftBorder - 4 < ImageDeal[Ysite - 1].LeftBorder
                                && ImageDeal[Ysite].LeftBorder - 4 < ImageDeal[Ysite - 3].LeftBorder)
                        || (ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite - 2].LeftBorder
                                && ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite - 4].LeftBorder
                                && ImageDeal[Ysite].LeftBorder - 4 < ImageDeal[Ysite - 2].LeftBorder
                                && ImageDeal[Ysite].LeftBorder - 4 < ImageDeal[Ysite - 4].LeftBorder)))

        {
            Ring_Help_Flag = 1;
//             printf("test\n\r");

            break;
        }
    }
#define UP_JUDGE_L(x,y) (ImageDeal[x].LeftBoundary_First < ImageDeal[y].LeftBoundary_First&& ImageDeal[x].LeftBoundary_First + 4 > ImageDeal[y].LeftBoundary_First)
#define DOWN_JUDGE_L(x,y) (ImageDeal[x].LeftBoundary_First > ImageDeal[y].LeftBoundary_First&& ImageDeal[x].LeftBoundary_First - 4 < ImageDeal[y].LeftBoundary_First)

    for (int Ysite = 45; Ysite > ImageStatus.OFFLine + 5; Ysite--)
    {
        if (((UP_JUDGE_L(Ysite + 4,Ysite) && UP_JUDGE_L(Ysite + 3, Ysite))
                || (UP_JUDGE_L(Ysite + 5,Ysite) && UP_JUDGE_L(Ysite + 3, Ysite))
                || (UP_JUDGE_L(Ysite + 4,Ysite) && UP_JUDGE_L(Ysite + 2, Ysite)))
                && ((DOWN_JUDGE_L(Ysite,Ysite - 2) && DOWN_JUDGE_L(Ysite, Ysite - 3))
                        || (DOWN_JUDGE_L(Ysite,Ysite - 1) && DOWN_JUDGE_L(Ysite, Ysite - 3))
                        || (DOWN_JUDGE_L(Ysite,Ysite - 2) && DOWN_JUDGE_L(Ysite, Ysite - 4))))
        {
//            printf("test2\n\r");
            mid_test_points_Y = Ysite;
            mid_test_points_X = ImageDeal[Ysite].LeftBoundary_First;
            point_flag = 1;
            break;
        }
    }
    if (Left_RingsFlag_Point2_Ysite > Left_RingsFlag_Point1_Ysite && Ring_Help_Flag == 0)
    {
        if (ImageStatus.Left_Line > 7) //13
            Ring_Help_Flag = 2;
    }
//    if (point_flag)
//    {
//        printf("flag:%d\n\r", point_flag);
//    }
//printf("%d,%d\n\r",Left_RingsFlag_Point2_Ysite,Left_RingsFlag_Point1_Ysite);
    if (Left_RingsFlag_Point2_Ysite > 25 && Left_RingsFlag_Point1_Ysite > 20
            && Left_RingsFlag_Point2_Ysite > Left_RingsFlag_Point1_Ysite && ImageFlag.image_element_rings_flag == 0
            && Straight_Judge_2(RIGHT, ImageStatus.OFFLine + 1, 50) < 0.1
            && !(ImageDeal[52].IsLeftFind == 'W' || ImageDeal[53].IsLeftFind == 'W' || ImageDeal[54].IsLeftFind == 'W'))
    {
//        printf("aaa");
        if (Ring_Help_Flag != 0)
        {
//            printf("bbb");
            for (Ysite = Left_RingsFlag_Point2_Ysite;
                    Ysite
                            >= (mid_test_points_Y > Left_RingsFlag_Point2_Ysite - 10 ?
                                    mid_test_points_Y : Left_RingsFlag_Point2_Ysite - 10); Ysite--)
            {
                if (ImageDeal[Ysite].LeftBoundary_First <= 2 && ImageDeal[Ysite - 1].LeftBoundary_First <= 2)
                {
                    ImageFlag.image_element_rings = 1;
                    ImageFlag.image_element_rings_flag = 1;
                    ImageFlag.ring_big_small = 1;
                    ImageStatus.Road_type = LeftCirque;
                    gpio_set_level(Beep, 1);
//                    printf("000");

                    return;
                }
            }
            uint8 down_num = 0;
            uint8 lack_num = 0;
            for (Ysite = 0; Ysite < dir_stop - 5; Ysite++)
            {
                if (DIR_1[Ysite].LEFT == 3)
                {
//                    printf("a2\n\r");
                    break;
                }
            }
            for (; Ysite < dir_stop - 5; Ysite++)
            {
                if (DIR_1[Ysite].LEFT == 2)
                {
                    down_num++;
//                    printf("a3,%d\n\r", down_num);

                }
                if (DIR_1[Ysite].LEFT == 0 || DIR_1[Ysite].LEFT == 1)
                {
                    break;
                }
                if (down_num)
                {
//                    printf("ccc");
                    for (; Ysite < dir_stop - 5; Ysite++)
                    {
                        if (DIR_1[Ysite].LEFT == 0)
                        {
//                            printf("a4\n\r");

                            break;
                        }
                    }
                    uint16 temp_Ysite = (uint16) Ysite;
                    for (; Ysite < dir_stop - 5 && Ysite < temp_Ysite + 15; Ysite++)
                    {
                        if (DIR_1_X[Ysite].LEFT <= 4 && DIR_1_X[Ysite + 1].LEFT <= 4)
                        {
                            lack_num++;
//                            printf("a5\n\r");

                        }
                        if (lack_num >= 5)
                        {
                            ImageFlag.image_element_rings = 1;
                            ImageFlag.image_element_rings_flag = 1;
                            ImageFlag.ring_big_small = 1;
                            ImageStatus.Road_type = LeftCirque;
                            gpio_set_level(Beep, 1);
//                            printf("111");
                            return;
                        }
                    }
                    break;
                }
            }
        }

    }
    else if (point_flag && Straight_Judge_2(RIGHT, ImageStatus.OFFLine + 1, 54) < 0.1
            && Straight_Judge_2(LEFT, mid_test_points_Y > ImageStatus.OFFLine ? mid_test_points_Y : ImageStatus.OFFLine,
                    54) > 0.1 && mid_test_points_Y < 40)
    {
//        printf("tes3\n\r");

        uint8 miss_flag = 0;
        int miss_Y_A_1 = 0;
        int miss_Y_A_2 = 0;
        int miss_Y_B_1 = 0;
        int miss_Y_B_2 = 0;
        for (Ysite = mid_test_points_Y; Ysite > ImageStatus.OFFLine + 4; Ysite--)
        {
            if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite - 1].IsLeftFind == 'W')
            {
                miss_flag++;
                miss_Y_A_1 = Ysite;
                miss_Y_A_2 = Ysite - 2;
                break;
            }
            if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite - 1].IsLeftFind == 'T'
                    && ImageDeal[Ysite - 2].IsLeftFind == 'T')
            {
                break;
            }
        }
        if (miss_flag)
        {
            uint8 lack_ring_l = 0;
            for (Ysite = miss_Y_A_1; Ysite > ImageStatus.OFFLine + 4 && ImageDeal[Ysite].IsLeftFind == 'W'; Ysite--)
            {
                if (ImageDeal[Ysite].LeftBoundary_First <= 2)
                {
//                    printf("%d\n\r", Ysite);
                    lack_ring_l++;
                    break;
                }
            }
            if (!lack_ring_l)
            {
                miss_flag = 0;
            }
            for (Ysite = miss_Y_A_2; Ysite > ImageStatus.OFFLine + 1; Ysite--)
            {

                if (mid_test_points_Y - miss_Y_A_2 < 5)
                {
                    miss_flag = 0;
                    break;
                }
                if (ImageDeal[Ysite].IsLeftFind == 'T')
                {
                    miss_Y_A_2 = Ysite + 1;
                    break;
                }
                if (Ysite == ImageStatus.OFFLine + 2)
                {
                    miss_Y_A_2 = Ysite;
                    break;
                }
            }
            if (miss_flag)
            {
                for (Ysite = mid_test_points_Y; Ysite < 48; Ysite++)
                {
                    if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite + 1].IsLeftFind == 'W'
                            && ImageDeal[Ysite + 2].IsLeftFind == 'W' && ImageDeal[Ysite + 3].IsLeftFind == 'W')
                    {
                        if (ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite - 5].LeftBoundary_First > -10)
                        {
                            miss_flag++;
                            miss_Y_B_1 = Ysite;
                            miss_Y_B_2 = Ysite + 4;
                        }

                        break;
                    }
                    if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite + 1].IsLeftFind == 'T'
                            && ImageDeal[Ysite + 2].IsLeftFind == 'T')
                    {
                        break;
                    }
                }

                if (Ysite == 48 && miss_flag == 1)
                {
                    uint8 real_W_flag = 0;
                    for (int i = 0; i < 5; i++)
                    {
                        if (Real_state[i][0] == 'W')
                        {
                            real_W_flag++;
                        }
                    }
                    if (real_W_flag >= 2)
                    {
                        miss_flag++;
                        miss_Y_B_1 = Ysite;
                        miss_Y_B_2 = Ysite + 4;
                    }
                }
                if (miss_flag != 2)
                {
                    for (Ysite = ImageStatus.OFFLine + 1; Ysite < miss_Y_A_1 && Ysite < 30 && Ysite < mid_test_points_Y;
                            Ysite++)
                    {
                        if (ImageDeal[Ysite].IsLeftFind == 'T' && ImageDeal[Ysite + 1].IsLeftFind == 'T'
                                && ImageDeal[Ysite + 2].IsLeftFind == 'W'
                                && abs(ImageDeal[Ysite].LeftBoundary_First-ImageDeal[Ysite+2].LeftBoundary_First) > 5)
                        {
                            ImageFlag.image_element_rings = 1;
                            ImageFlag.image_element_rings_flag = 2; //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风姸鎬�1閿熸枻鎷烽敓鏂ゆ嫹2閿熸枻鎷烽敓鐭鎷峰疄閿熺粸璇ф嫹閿熸枻鎷�
                            ImageFlag.ring_big_small = 1;
                            ImageStatus.Road_type = LeftCirque;
                            gpio_set_level(Beep, 1);
                            break;
                        }

                    }
                }

            }
        }
        if (miss_flag == 2)
        {
            for (Ysite = miss_Y_B_2; Ysite < 55; Ysite++)
            {
                if (miss_Y_B_2 - mid_test_points_Y > 30)
                {
                    miss_flag = 1;
                    break;
                }
                if (ImageDeal[Ysite].IsLeftFind == 'T')
                {
                    miss_Y_B_2 = Ysite - 1;
                    break;
                }
                if (miss_Y_B_2 == 54)
                {
                    miss_Y_B_2 = Ysite;
                    break;
                }
            }
//        }
//            //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻緝鍖℃嫹閿熺殕纭锋嫹閿熸枻鎷穖iss涔嬮敓鏂ゆ嫹鏈ㄩ敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹璇熼敓鏂ゆ嫹閿熸枻鎷峰嵏閿燂拷
//            //閿熷彨鎾呮嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺姣旇緝鐨勫嚖鎷峰紡閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍗侀敓琛楃鎷峰奖閿熷眾锛岄敓鏂ゆ嫹閿熸枻鎷峰崄閿熻鐨勪紮鎷烽敓鏂ゆ嫹瀹為敓鑺傜鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熺粸鎲嬫嫹閿熸枻鎷蜂斧閿熸枻鎷烽敓鏂ゆ嫹鏋敓鎻紮鎷烽敓鏂ゆ嫹鑰嶉敓鏂ゆ嫹閿熸枻鎷疯穬閿熸枻鎷烽敓鏂ゆ嫹閿熺煫杈炬嫹閿熸枻鎷锋瘡閿熸枻鎷烽敓鏁欘剨鎷烽敓鏂ゆ嫹缂樺閿熸枻鎷烽敓鍙嶆嫹閿燂拷

        }
        else if (Concave_Convex_Judge(LEFT,
                mid_test_points_Y > ImageStatus.OFFLine ? mid_test_points_Y : ImageStatus.OFFLine, 54) == Concave
                && miss_flag)
        {
            miss_flag++;
        }

        if (miss_flag == 2)
        {
            ImageFlag.image_element_rings = 1;
            ImageFlag.image_element_rings_flag = 1;        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风姸鎬�1閿熸枻鎷烽敓鏂ゆ嫹2閿熸枻鎷烽敓鐭鎷峰疄閿熺粸璇ф嫹閿熸枻鎷�
            ImageFlag.ring_big_small = 1;
            ImageStatus.Road_type = LeftCirque;
            gpio_set_level(Beep, 1);
        }

//
    }
    Ring_Help_Flag = 0;
}

//--------------------------------------------------------------
//  @name           Element_Judgment_Right_Rings()
//  @brief          閿熸枻鎷烽敓鏂ゆ嫹鍥鹃敓鏂ゆ嫹閿熷彨鏂鎷烽敓鎺ョ尨鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙鎷烽敓鏂ゆ嫹鍦嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Judgment_Right_Rings();
//--------------------------------------------------------------

void Element_Judgment_Right_Rings ()
{
    if (ImageStatus.Left_Line > 20 || ImageStatus.Right_Line < 13 || ImageStatus.WhiteLine > 10 //13
    || ImageStatus.OFFLine > 5
//    || Straight_Judge_1(1, 15, 45) > 1
            || Straight_Judge_2(LEFT, ImageStatus.OFFLine + 1, 50) > 0.1 || ImageDeal[30].IsLeftFind == 'W'
            || ImageDeal[20].IsLeftFind == 'W'
//                    || ImageDeal[40].IsLeftFind == 'W'
            || ImageDeal[10].IsLeftFind == 'W')
    {
//        printf("%f\n\r",Straight_Judge_2(LEFT, ImageStatus.OFFLine + 1, 50));
        return;
    }
    int ring_ysite = 25;
    int mid_test_points_Y = 0;
    uint8 Ring_Help_Flag = 0;                      //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰織
    Right_RingsFlag_Point1_Ysite = 0;
    Right_RingsFlag_Point2_Ysite = 0;
    uint8 point_flag = 0;

    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite - 1].RightBoundary_First - ImageDeal[Ysite].RightBoundary_First > 4)
        {
            Right_RingsFlag_Point1_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite].RightBoundary - ImageDeal[Ysite + 1].RightBoundary > 4)
        {
            Right_RingsFlag_Point2_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = Right_RingsFlag_Point1_Ysite; Ysite > 25; Ysite--)
    {
//        if (ImageDeal[Ysite + 3].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
//            && ImageDeal[Ysite + 2].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
//            && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 1].RightBoundary_First
//            && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 2].RightBoundary_First
//           )
        if (((ImageDeal[Ysite + 4].RightBorder > ImageDeal[Ysite].RightBorder
                && ImageDeal[Ysite + 3].RightBorder > ImageDeal[Ysite].RightBorder
                && ImageDeal[Ysite + 4].RightBorder - 4 < ImageDeal[Ysite].RightBorder
                && ImageDeal[Ysite + 3].RightBorder - 4 < ImageDeal[Ysite].RightBorder)
                || (ImageDeal[Ysite + 4].RightBorder > ImageDeal[Ysite].RightBorder
                        && ImageDeal[Ysite + 2].RightBorder > ImageDeal[Ysite].RightBorder
                        && ImageDeal[Ysite + 4].RightBorder - 4 < ImageDeal[Ysite].RightBorder
                        && ImageDeal[Ysite + 2].RightBorder - 4 < ImageDeal[Ysite].RightBorder)
                || (ImageDeal[Ysite + 5].RightBorder > ImageDeal[Ysite].RightBorder
                        && ImageDeal[Ysite + 3].RightBorder > ImageDeal[Ysite].RightBorder
                        && ImageDeal[Ysite + 5].RightBorder - 4 < ImageDeal[Ysite].RightBorder
                        && ImageDeal[Ysite + 3].RightBorder - 4 < ImageDeal[Ysite].RightBorder))
                && ((ImageDeal[Ysite].RightBorder < ImageDeal[Ysite - 2].RightBorder
                        && ImageDeal[Ysite].RightBorder < ImageDeal[Ysite - 3].RightBorder
                        && ImageDeal[Ysite].RightBorder + 4 > ImageDeal[Ysite - 2].RightBorder
                        && ImageDeal[Ysite].RightBorder + 4 > ImageDeal[Ysite - 3].RightBorder)
                        || (ImageDeal[Ysite].RightBorder < ImageDeal[Ysite - 1].RightBorder
                                && ImageDeal[Ysite].RightBorder < ImageDeal[Ysite - 3].RightBorder
                                && ImageDeal[Ysite].RightBorder + 4 > ImageDeal[Ysite - 1].RightBorder
                                && ImageDeal[Ysite].RightBorder + 4 > ImageDeal[Ysite - 3].RightBorder)
                        || (ImageDeal[Ysite].RightBorder < ImageDeal[Ysite - 2].RightBorder
                                && ImageDeal[Ysite].RightBorder < ImageDeal[Ysite - 4].RightBorder
                                && ImageDeal[Ysite].RightBorder + 4 > ImageDeal[Ysite - 2].RightBorder
                                && ImageDeal[Ysite].RightBorder + 4 > ImageDeal[Ysite - 4].RightBorder)))

        {
            Ring_Help_Flag = 1;
            break;
        }
    }
    for (int Ysite = 45; Ysite > ImageStatus.OFFLine + 5; Ysite--)
    {
        //        if (ImageDeal[Ysite + 3].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
        //            && ImageDeal[Ysite + 2].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
        //            && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 1].LeftBoundary_First
        //            && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 1].LeftBoundary_First
        //            )
        if (((ImageDeal[Ysite + 4].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                && ImageDeal[Ysite + 3].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                && ImageDeal[Ysite + 4].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First
                && ImageDeal[Ysite + 3].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First)
                || (ImageDeal[Ysite + 4].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                        && ImageDeal[Ysite + 2].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                        && ImageDeal[Ysite + 4].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First
                        && ImageDeal[Ysite + 2].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First)
                || (ImageDeal[Ysite + 5].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                        && ImageDeal[Ysite + 3].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                        && ImageDeal[Ysite + 5].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First
                        && ImageDeal[Ysite + 3].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First))
                && ((ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 2].RightBoundary_First
                        && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 3].RightBoundary_First
                        && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 2].RightBoundary_First
                        && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 3].RightBoundary_First)
                        || (ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 1].RightBoundary_First
                                && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 3].RightBoundary_First
                                && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 1].RightBoundary_First
                                && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 3].RightBoundary_First)
                        || (ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 2].RightBoundary_First
                                && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 4].RightBoundary_First
                                && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 2].RightBoundary_First
                                && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 4].RightBoundary_First)))
        {
            mid_test_points_Y = Ysite;
            point_flag = 1;
            break;
        }
    }
    if (Right_RingsFlag_Point2_Ysite > Right_RingsFlag_Point1_Ysite && Ring_Help_Flag == 0)
    {
        if (ImageStatus.Right_Line > 7)
            Ring_Help_Flag = 2;
    }
    if (Right_RingsFlag_Point2_Ysite > 25 && Right_RingsFlag_Point1_Ysite > 20
            && Right_RingsFlag_Point2_Ysite > Right_RingsFlag_Point1_Ysite && ImageFlag.image_element_rings_flag == 0
            && Straight_Judge_2(LEFT, ImageStatus.OFFLine + 1, 50) < 0.1
            && !(ImageDeal[52].IsRightFind == 'W' || ImageDeal[53].IsRightFind == 'W'
                    || ImageDeal[54].IsRightFind == 'W'))
    {
//        printf("%f,\n\r",Straight_Judge_3(LEFT, ImageStatus.OFFLine + 1, 50));
//        printf("aaa\n\r");
        if (Ring_Help_Flag != 0)
        {
//            printf("a1\n\r");
            for (Ysite = Right_RingsFlag_Point2_Ysite;
                    Ysite
                            >= (mid_test_points_Y > Right_RingsFlag_Point2_Ysite - 10 ?
                                    mid_test_points_Y : Right_RingsFlag_Point2_Ysite - 10); Ysite--)
            {
                if (ImageDeal[Ysite].RightBoundary_First >= 75 && ImageDeal[Ysite - 1].RightBoundary_First >= 75)
                {

                    ImageFlag.image_element_rings = 2;
                    ImageFlag.image_element_rings_flag = 1;
                    ImageFlag.ring_big_small = 1;     //灏忛敓鏂ゆ嫹
                    ImageStatus.Road_type = RightCirque;
                    gpio_set_level(Beep, 1);
                    return;
//                    printf("test:1");
                }
            }
//            printf("a2\n\r");
            uint8 down_num = 0;
            uint8 lack_num = 0;
            for (Ysite = 0; Ysite < dir_stop - 5; Ysite++)
            {
                if (DIR_1[Ysite].RIGHT == 1)
                {
//                    printf("a3\n\r");
                    break;
                }
            }
            for (; Ysite < dir_stop - 5; Ysite++)
            {
                if (DIR_1[Ysite].RIGHT == 2)
                {
                    down_num++;
//                    printf("a4,%d\n\r", down_num);
                }
                if (down_num >= 3)
                {
//                    printf("a5\n\r");
                    for (; Ysite < dir_stop - 5; Ysite++)
                    {
                        if (DIR_1[Ysite].RIGHT == 0)
                        {
//                            printf("a6\n\r");

                            break;
                        }
                    }
                    uint16 temp_Ysite = (uint16) Ysite;
                    for (; Ysite < dir_stop - 5 && Ysite < temp_Ysite + 15; Ysite++)
                    {
                        if (DIR_1_X[Ysite].RIGHT >= 75 && DIR_1_X[Ysite + 1].RIGHT >= 75)
                        {
                            lack_num++;
//                            printf("a7,%d\n\r",lack_num);
                        }
                        if (lack_num >= 5)
                        {
                            ImageFlag.image_element_rings = 2;
                            ImageFlag.image_element_rings_flag = 1;
                            ImageFlag.ring_big_small = 1; // 灏忛敓鏂ゆ嫹
                            ImageStatus.Road_type = RightCirque;
                            gpio_set_level(Beep, 1);
//                            printf("111");
                            return;
                        }
                    }
                    break;
                }
                if (DIR_1[Ysite].RIGHT == 0 || DIR_1[Ysite].RIGHT == 3)
                {
                    break;
                }
            }
        }
    }
    else if (point_flag && Straight_Judge_2(LEFT, ImageStatus.OFFLine + 1, 54) < 0.1
            && Straight_Judge_2(RIGHT,
                    mid_test_points_Y > ImageStatus.OFFLine ? mid_test_points_Y : ImageStatus.OFFLine, 54) > 0.2
            && mid_test_points_Y < 40 && mid_test_points_Y > 8)
    {
        uint8 miss_flag = 0;
        int miss_Y_A_1 = 0;
        int miss_Y_A_2 = 0;
        int miss_Y_B_1 = 0;
        int miss_Y_B_2 = 0;
        for (Ysite = mid_test_points_Y; Ysite > ImageStatus.OFFLine + 4; Ysite--)
        {
            if (ImageDeal[Ysite].IsRightFind == 'W' && ImageDeal[Ysite - 1].IsRightFind == 'W')
            {
                miss_flag++;
                miss_Y_A_1 = Ysite;
                miss_Y_A_2 = Ysite - 2;
                break;
            }
            if (ImageDeal[Ysite].IsRightFind == 'W' && ImageDeal[Ysite - 1].IsRightFind == 'T'
                    && ImageDeal[Ysite - 2].IsRightFind == 'T')
            {
                break;
            }
        }
        if (miss_flag)
        {
            uint8 lack_ring_r = 0;
            for (Ysite = miss_Y_A_1; Ysite > ImageStatus.OFFLine + 4 && ImageDeal[Ysite].IsRightFind == 'W'; Ysite--)
            {
                if (ImageDeal[Ysite].RightBoundary_First >= 75)
                {
//                    printf("%d\n\r", Ysite);
                    lack_ring_r++;
                    break;
                }
            }
            if (!lack_ring_r)
            {
                miss_flag = 0;
            }
            for (Ysite = miss_Y_A_2; Ysite > ImageStatus.OFFLine + 1; Ysite--)
            {
                if (mid_test_points_Y - miss_Y_A_2 < 4)

                {
                    miss_flag = 0;
                    break;
                }
                if (ImageDeal[Ysite].IsRightFind == 'T')
                {
                    miss_Y_A_2 = Ysite + 1;
                    break;
                }
                if (Ysite == ImageStatus.OFFLine + 2)
                {
                    miss_Y_A_2 = Ysite;
                    break;
                }
            }
            if (miss_flag)
            {
                for (Ysite = mid_test_points_Y; Ysite < 48; Ysite++)
                {
                    if (ImageDeal[Ysite].IsRightFind == 'W' && ImageDeal[Ysite + 1].IsRightFind == 'W'
                            && ImageDeal[Ysite + 2].IsRightFind == 'W' && ImageDeal[Ysite + 3].IsRightFind == 'W')
                    {
                        if (ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite - 5].RightBoundary_First < 10)
                        {
                            miss_flag++;
                            miss_Y_B_1 = Ysite;
                            miss_Y_B_2 = Ysite + 4;
                        }
                        break;
                    }
                    if (ImageDeal[Ysite].IsRightFind == 'W' && ImageDeal[Ysite + 1].IsRightFind == 'T'
                            && ImageDeal[Ysite + 2].IsRightFind == 'T')
                    {
                        break;
                    }
                }
                if (Ysite == 48 && miss_flag == 1)
                {
                    uint8 real_W_flag = 0;
                    for (int i = 0; i < 5; i++)
                    {
                        if (Real_state[i][1] == 'W')
                        {
                            real_W_flag++;
                        }
                    }
                    if (real_W_flag >= 2)
                    {
                        miss_flag++;
                        miss_Y_B_1 = Ysite;
                        miss_Y_B_2 = Ysite + 4;
                    }
                }
                if (miss_flag != 2)
                {
                    for (Ysite = ImageStatus.OFFLine + 1; Ysite < miss_Y_A_1 && Ysite < 30 && Ysite < mid_test_points_Y;
                            Ysite++)
                    {
                        if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite + 1].IsRightFind == 'T'
                                && ImageDeal[Ysite + 2].IsRightFind == 'W'
                                && abs(ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite + 2].RightBoundary_First)
                                        > 5 && mid_test_points_Y < 40 && mid_test_points_Y > 8)
                        {
                            ImageFlag.image_element_rings = 2;
                            ImageFlag.image_element_rings_flag = 2;
                            ImageFlag.ring_big_small = 1;     //灏忛敓鏂ゆ嫹
                            ImageStatus.Road_type = RightCirque;
                            gpio_set_level(Beep, 1);
//                            printf("222");

                            break;
                        }

                    }
                }
            }
        }
        if (miss_flag == 2)
        {
            for (Ysite = miss_Y_B_2; Ysite < 55; Ysite++)
            {
                if (miss_Y_B_2 - mid_test_points_Y > 30)
                {
                    miss_flag = 1;
                    break;
                }
                if (ImageDeal[Ysite].IsRightFind == 'T')
                {
                    miss_Y_B_2 = Ysite - 1;
                    break;
                }
                if (miss_Y_B_2 == 54)
                {
                    miss_Y_B_2 = Ysite;
                    break;
                }
            }

        }
        else if (Concave_Convex_Judge(RIGHT,
                mid_test_points_Y > ImageStatus.OFFLine ? mid_test_points_Y : ImageStatus.OFFLine, 54) == Concave
                && miss_flag)
        {
            miss_flag++;
        }
        if (miss_flag == 2)
        {
            ImageFlag.image_element_rings = 2;
            ImageFlag.image_element_rings_flag = 1;
            ImageFlag.ring_big_small = 1;     //灏忛敓鏂ゆ嫹
            ImageStatus.Road_type = RightCirque;
            gpio_set_level(Beep, 1);
//            printf("333");

        }
        //
    }
    Ring_Help_Flag = 0;
    return;
}
//閿熸枻鎷峰渾閿熸枻鎷烽敓鍙鎷�
float circle_L_1 = 0;

float circle_L_2 = 0;
float circle_L_3 = 0;
float circle_L_4 = 0;
float lenth_ring_state_l_7 = 0;
float k_rlen_l = 100;
float Repair_X_L = 23.1;
void Element_Handle_Left_Rings (void)
{
//    angle_bias = -2;
//    kp_servo = 2.0;
    /***************************************閿熷彨璁规嫹**************************************/
    int num = 0;
    for (int Ysite = 55; Ysite > 30; Ysite--)
    {
        if (ImageDeal[Ysite].IsLeftFind == 'W')
            num++;
        if (ImageDeal[Ysite + 3].IsLeftFind == 'W' && ImageDeal[Ysite + 2].IsLeftFind == 'W'
                && ImageDeal[Ysite + 1].IsLeftFind == 'W' && ImageDeal[Ysite].IsLeftFind == 'T')
            break;
    }
    if (ImageFlag.image_element_rings_flag <= 6)     //閿熸枻鎷烽敓鏂ゆ嫹閿熻鐜�
    {
        circle_add = -(circle_L_1);
    }
    else if (ImageFlag.image_element_rings_flag == 7)     //閿熸枻鎷烽敓鏂ゆ嫹
    {
        circle_add = -(circle_L_2);
    }
    else if (ImageFlag.image_element_rings_flag == 8)   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰疄閿熺粸鐧告嫹閿熸暀寰堢煭锝忔嫹閿熸枻鎷烽敓鐨嗙尨鎷�9閿熸枻鎷蜂竴閿熸枻鎷�
    {
        circle_add = -(circle_L_3);
    }
    else

    {
        circle_add = -(circle_L_4);
    }
//鍑嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
    if (ImageFlag.image_element_rings_flag == 1 && num > 10)
    {
//        ImageFlag.image_element_rings_flag = 2;
        encoder_start();
        uint8 state_flag = 0;
        for (int i = 0; i < 5; i++)
        {
            if (Real_state[i][0] == 'W')
            {
                state_flag++;
            }
        }
        if (state_flag >= 3)
        {
            ImageFlag.image_element_rings_flag = 2;
            encoder_start();
        }
        //wireless_uart_send_byte(2);
    }
    else if (ImageFlag.image_element_rings_flag == 2)
    {
        uint8 mid_test_points_Y = 0;
        uint8 point_flag = 0;
        for (int Ysite = 45; Ysite > ImageStatus.OFFLine + 5; Ysite--)
        {
            if (((ImageDeal[Ysite + 4].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
                    && ImageDeal[Ysite + 3].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
                    && ImageDeal[Ysite + 4].LeftBoundary_First + 4 > ImageDeal[Ysite].LeftBoundary_First
                    && ImageDeal[Ysite + 3].LeftBoundary_First + 4 > ImageDeal[Ysite].LeftBoundary_First)
                    || (ImageDeal[Ysite + 5].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
                            && ImageDeal[Ysite + 3].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
                            && ImageDeal[Ysite + 5].LeftBoundary_First + 4 > ImageDeal[Ysite].LeftBoundary_First
                            && ImageDeal[Ysite + 3].LeftBoundary_First + 4 > ImageDeal[Ysite].LeftBoundary_First)
                    || (ImageDeal[Ysite + 4].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
                            && ImageDeal[Ysite + 2].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
                            && ImageDeal[Ysite + 4].LeftBoundary_First + 4 > ImageDeal[Ysite].LeftBoundary_First
                            && ImageDeal[Ysite + 2].LeftBoundary_First + 4 > ImageDeal[Ysite].LeftBoundary_First))
                    && ((ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 2].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 3].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First - 4 < ImageDeal[Ysite - 2].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First - 4 < ImageDeal[Ysite - 3].LeftBoundary_First)
                            || (ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 1].LeftBoundary_First
                                    && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 3].LeftBoundary_First
                                    && ImageDeal[Ysite].LeftBoundary_First - 4 < ImageDeal[Ysite - 1].LeftBoundary_First
                                    && ImageDeal[Ysite].LeftBoundary_First - 4 < ImageDeal[Ysite - 3].LeftBoundary_First)
                            || (ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 2].LeftBoundary_First
                                    && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 4].LeftBoundary_First
                                    && ImageDeal[Ysite].LeftBoundary_First - 4 < ImageDeal[Ysite - 2].LeftBoundary_First
                                    && ImageDeal[Ysite].LeftBoundary_First - 4 < ImageDeal[Ysite - 4].LeftBoundary_First)))
            {
                //            printf("test2\n\r");
                mid_test_points_Y = Ysite;
                point_flag = 1;
                break;
            }
        }
//        printf("TF:%f\n\r", encoder_end());
        if (num < 8)
        {
            if (point_flag)
            {
                if (mid_test_points_Y > 10)
                {
                    ImageFlag.image_element_rings_flag = 5;
                    encoder_end();
                    encoder_start();
                }
            }
            else
            {
                ImageFlag.image_element_rings_flag = 5;
                encoder_end();
                encoder_start();
            }

        }
        else if (encoder_getval_cm() > 40)
        {
            ImageFlag.image_element_rings_flag = 5;
            encoder_end();
            encoder_start();
        }
//        else if
//        {
//            for (Ysite = ImageStatus.OFFLine + 5; Ysite < 30; Ysite++)
//            {
//                if (ImageDeal[Ysite].IsLeftFind == 'T' && ImageDeal[Ysite + 1].IsLeftFind == 'T'
//                        && ImageDeal[Ysite + 2].IsLeftFind == 'W')
//                {
//                    if (Ysite > 10 && Ysite < 20)
//                    {
//                        ImageFlag.image_element_rings_flag = 5;
//                        break;
//                    }
//                }
//
//            }
//        }

        //wireless_uart_send_byte(5);
    }
//閿熸枻鎷烽敓鏂ゆ嫹
    else if (ImageFlag.image_element_rings_flag == 5 && /*num>15)*/ImageStatus.Right_Line > 15)
    {
        ImageFlag.image_element_rings_flag = 6;
        encoder_end();
        encoder_start();

    }
//閿熸枻鎷烽敓鏂ゆ嫹灏忓渾閿熸枻鎷�
    else if (ImageFlag.image_element_rings_flag == 6 && ImageStatus.Right_Line < 12 && encoder_getval_cm() > 20)
    {
        for (Ysite = 54; Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (ImageDeal[Ysite].RightBoundary_First < 78)
            {
                break;
            }
        }
        uint8 last_Ysite = Ysite;
        uint8 jump_flag = 0;
        for (Ysite = 54; Ysite > last_Ysite - 5 && Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite + 1].RightBoundary_First > 2
                    || ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite + 1].RightBoundary_First < -2)
            {
                jump_flag = 1;
                break;
            }
//        for(Xsite=ImageDeal[Ysite].RightBoundary_First;Xsite>ImageDeal[Ysite].RightBoundary_First;Xsite--)
        }
        if (!jump_flag)
        {
            ImageFlag.image_element_rings_flag = 7;
            encoder_end();
            encoder_start();
        }

    }
//閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷峰渾閿熸枻鎷烽敓鍙鎷�
    else if (ImageFlag.image_element_rings_flag == 7 && encoder_getval_cm() > 100 && ImageStatus.Right_Line > 15)
    {
        Point_Ysite = 0;
        Point_Xsite = 0;
        for (int Ysite = 52; Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if ((ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite + 2].RightBoundary_First > 3)
                    || (ImageDeal[Ysite].RightBoundary_First <= ImageDeal[Ysite + 2].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First <= ImageDeal[Ysite - 2].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First <= ImageDeal[Ysite + 1].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First <= ImageDeal[Ysite - 1].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First <= ImageDeal[Ysite + 4].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First <= ImageDeal[Ysite - 4].RightBoundary_First)
//                            && !(ImageDeal[Ysite].RightBoundary_First == ImageDeal[Ysite + 2].RightBorder
//                                    && ImageDeal[Ysite].RightBorder == ImageDeal[Ysite + 1].RightBorder
//                                    && ImageDeal[Ysite].RightBorder == ImageDeal[Ysite + 4].RightBorder)
//                            && !(ImageDeal[Ysite].RightBorder == ImageDeal[Ysite - 2].RightBorder
//                                    && ImageDeal[Ysite].RightBorder == ImageDeal[Ysite - 1].RightBorder
//                                    && ImageDeal[Ysite].RightBorder == ImageDeal[Ysite - 4].RightBorder
                    )
            {
                Point_Xsite = ImageDeal[Ysite].RightBorder;
                Point_Ysite = Ysite;
                break;
            }
        }
        if (Point_Ysite > 24)
        {
            lenth_ring_state_l_7 = encoder_end();
            encoder_start();
            ImageFlag.image_element_rings_flag = 8;
            // wireless_uart_send_byte(8);
            //Stop = 1;
        }
//        else if(Real_state[0][1]=='W'&&Real_state[1][1]=='W'&&Real_state[2][1]=='W'&&Real_state[3][1]=='W'&&Real_state[4][1]=='W')
//        {
//
//        }
    }
//        //閿熸枻鎷烽敓鏂ゆ嫹 灏忓渾閿熸枻鎷烽敓鍙鎷�
//    if (ImageFlag.image_element_rings_flag == 7 && ImageFlag.ring_big_small == 2)
//    {
//        Point_Ysite = 0;
//        Point_Xsite = 0;
//        for (int Ysite = 50; Ysite > ImageStatus.OFFLineBoundary + 3; Ysite--)
//        {
//            if (    ImageDeal[Ysite].RightBoundary < ImageDeal[Ysite + 2].RightBoundary
//                 && ImageDeal[Ysite].RightBoundary < ImageDeal[Ysite - 2].RightBoundary
//               )
//            {
//                Point_Xsite = ImageDeal[Ysite].RightBoundary;
//                Point_Ysite = Ysite;
//                break;
//            }
//        }
//        if (Point_Ysite > 20)
//          ImageFlag.image_element_rings_flag = 8;
//    }
//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
    else if (ImageFlag.image_element_rings_flag == 8)
    {
        if (
        //Straight_Judge(2, ImageStatus.OFFLine+15, 50) < 1
        ImageStatus.Right_Line < 9 && ImageStatus.OFFLine < 10)    //閿熸彮鎲嬫嫹涓虹洿閿熸枻鎷烽敓鎻枻鎷锋閿熷彨锝忔嫹鍓嶇灮鍊奸敓鏂ゆ嫹閿熸枻鎷峰皬
        {
            ImageFlag.image_element_rings_flag = 9;
            encoder_end();
            encoder_start();            //wireless_uart_send_byte(9);
        }
    }

//閿熸枻鎷烽敓鏂ゆ嫹鍦嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
    else if (ImageFlag.image_element_rings_flag == 9)
    {
//        printf("%f\n\r", encoder_getval_ms());
        if (encoder_getval_cm() * encoder_getval_cm() > k_rlen_l * lenth_ring_state_l_7)
        {
            ImageStatus.Road_type = 0;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
            encoder_end();
//            encoder_start();
            gpio_set_level(Beep, 0);
            circle_add = 0;
//            angle_bias = -1;
//            kp_servo = 1.5;
            return;
//            circle_flag = 0;
        }
//        if (encoder_getval_cm() > 120)
//        {
////            encoder_end();
//            int num = 0;
//            for (int Ysite = 54; Ysite > 35; Ysite--)
//            {
//                if (ImageDeal[Ysite].IsLeftFind == 'W')
//                    num++;
//            }
//            if (num < 5)
//            {
//                ImageStatus.Road_type = 0;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
//                ImageFlag.image_element_rings_flag = 0;
//                ImageFlag.image_element_rings = 0;
//                ImageFlag.ring_big_small = 0;
//                encoder_end();
////                encoder_start();
//                gpio_set_level(Beep, 0);
////                angle_bias = -1;
////                kp_servo = 1.5;
//                return;
//            }
//        }
//        if (encoder_getval_cm() > 100)
//        {
//            if (ImageStatus.OFFLine < 5 && Pixle[ImageStatus.OFFLine][40]
//                    && Pixle[ImageStatus.OFFLine][40] == Pixle[Ysite][39]
//                    && Pixle[ImageStatus.OFFLine][40] == Pixle[Ysite][41])
//            {
//                for (Ysite = 54; Ysite > 0; Ysite--)
//                {
//                    if (!(Pixle[Ysite][40] && Pixle[Ysite][40] == Pixle[Ysite][39]
//                            && Pixle[Ysite][40] == Pixle[Ysite][41]))
//                    {
//                        break;
//                    }
//                    if (Ysite <= ImageStatus.OFFLine)
//                    {
//                        ImageFlag.image_element_rings_flag = 10;
//                        encoder_end();
//                        encoder_start();
////                        kp_servo = 1.5;
////                        angle_bias = -1;
//
//                        break;
//                    }
//                }
//
//            }
//        }
    }
    else if (ImageFlag.image_element_rings_flag == 10)
    {
        if (encoder_getval_cm() > 40)
        {
            ImageStatus.Road_type = 0;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
            encoder_end();
            gpio_set_level(Beep, 0);
        }
    }

    /***************************************閿熸枻鎷烽敓鏂ゆ嫹**************************************/
//鍑嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    if (ImageFlag.image_element_rings_flag == 1 || ImageFlag.image_element_rings_flag == 2
            || ImageFlag.image_element_rings_flag == 3 || ImageFlag.image_element_rings_flag == 4)
    {
        for (Ysite = 57; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].LeftBorder = ImageDeal[Ysite].RightBorder - (int) (2.0 * Half_Road_Wide[Ysite]);
            LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2; //鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姒叏鎷峰憰閿熻顕嗘嫹閿熸枻鎷烽敓鍙》鎷峰�奸敓鏂ゆ嫹閿熸枻鎷�
            ImageDeal[Ysite].Wide = -ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder;
        }
        RouteFilter();

    }
//閿熸枻鎷烽敓鏂ゆ嫹  閿熸枻鎷烽敓鏂ゆ嫹
    if (ImageFlag.image_element_rings_flag == 5 || ImageFlag.image_element_rings_flag == 6)
    {
        int flag_Xsite_1 = 0;
        int flag_Ysite_1 = 0;
        float Slope_Rings = 0;
        uint8 flag_Site = 0;
        static int last_flag_Ysite_1 = 0;
        static float last_Slope_Rings_1 = 0;
        static int8 last_flag_Site_1 = 0;
        for (Ysite = 55; Ysite > ImageStatus.OFFLine; Ysite--)   //閿熸枻鎷烽敓鑺ュ姬閿熸枻鎷�
        {
            for (Xsite = ImageDeal[Ysite].LeftBorder + 5; Xsite < ImageDeal[Ysite].RightBorder - 1; Xsite++)
            {
                if (Pixle[Ysite][Xsite] == 1 && Pixle[Ysite][Xsite + 1] == 0)
                {
                    flag_Ysite_1 = Ysite;
                    flag_Xsite_1 = Xsite;
                    flag_Xsite_1 = flag_Xsite_1;
                    Slope_Rings = (float) (79 - flag_Xsite_1 - (int) (0.1 * flag_Ysite_1))
                            / (float) (59 - flag_Ysite_1);
                    flag_Site = 1;
                    break;
                }
            }
            if (flag_Ysite_1 != 0)
            {
                break;
            }
        }
        if (flag_Ysite_1 == 0)
        {
            for (Ysite = ImageStatus.OFFLine + 1; Ysite < 30; Ysite++)
            {
                if (ImageDeal[Ysite].IsLeftFind == 'T' && ImageDeal[Ysite + 1].IsLeftFind == 'T'
                        && ImageDeal[Ysite + 2].IsLeftFind == 'W'
                        && abs(ImageDeal[Ysite].LeftBorder-ImageDeal[Ysite+2].LeftBorder) > 10)
                {
                    flag_Ysite_1 = Ysite;
                    flag_Xsite_1 = ImageDeal[flag_Ysite_1].LeftBorder;
                    ImageStatus.OFFLine = (uint8) Ysite;
                    Slope_Rings = (float) ((79 - (int) (0.1 * flag_Ysite_1) - flag_Xsite_1)
                            / (float) (59 - flag_Ysite_1));
                    flag_Site = 1;
                    break;
                }

            }
        }
        //閿熸枻鎷烽敓鏂ゆ嫹
        if (flag_Site)
        {
            last_flag_Site_1 = 1;
            last_flag_Ysite_1 = flag_Ysite_1;
            last_Slope_Rings_1 = Slope_Rings;
        }
//        else if (last_flag_Site_1)
//        {
//
//            flag_Ysite_1 = last_flag_Ysite_1 + 1;
//            flag_Xsite_1 = ImageDeal[flag_Ysite_1].LeftBorder;
//            Slope_Rings = last_Slope_Rings_1;
//            last_flag_Site_1++;
//            if (last_flag_Site_1 == 4)
//            {
//                last_flag_Site_1 = 2;
//            }
//
//        }
//        else
//        {
//            last_flag_Site_1--;
//            if (last_flag_Site_1 < 0)
//            {
//                last_flag_Site_1 = 0;
//            }
//        }
        if (flag_Ysite_1 != 0)
        {
//            ImageStatus.OFFLine = flag_Ysite_1;
            for (Ysite = flag_Ysite_1 - 1; Ysite < 60; Ysite++)
            {
                ImageDeal[Ysite].RightBorder = flag_Xsite_1 + Slope_Rings * (Ysite - flag_Ysite_1);
                LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
                LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
                Pixle[Ysite][ImageDeal[Ysite].RightBorder] = 0;
//                Pixle[Ysite][ImageDeal[Ysite].RightBorder - 1] = 0;
                Pixle[Ysite][ImageDeal[Ysite].RightBorder + 1] = 0;
                if (ImageDeal[Ysite].RightBorder + 2 < 78)
                {
                    Pixle[Ysite][ImageDeal[Ysite].RightBorder + 2] = 0;

                }
//                ImageDeal[Ysite].Center = ((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2);
//                ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
                //else//灏忓渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
                //    ImageDeal[Ysite].Center = ImageDeal[Ysite].RightBorder - Half_Bend_Wide[Ysite];

//                if (ImageDeal[Ysite].Center < 4)
//                {
//                    ImageDeal[Ysite].Center = 4;
//                    ImageStatus.OFFLine = Ysite + 2;
//                    break;
//                }
//                if (ImageDeal[Ysite].Wide > 8 && ImageDeal[Ysite].RightBorder < ImageDeal[Ysite + 2].RightBorder)
//                {
//                    continue;
//                }
//                else
//                {
//                    ImageStatus.OFFLine = Ysite + 2;
//                    break;
//                }

            }
            DrawLinesFirst();     //閿熸枻鎷烽敓鐙″簳鎲嬫嫹      30us
            DrawLinesProcess();   //閿熺煫纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  8us
            Search_Border_OTSU(Pixle, LCDH, LCDW, LCDH - 2);   //58閿熸枻鎷蜂綅閿熸枻鎷烽敓鏂ゆ嫹
            DrawExtensionLine();
            RouteFilter();
//            ImageDeal[flag_Ysite_1].RightBorder = flag_Xsite_1;
//            for (Ysite = flag_Ysite_1 - 1; Ysite > 10; Ysite--) //A閿熸枻鎷烽敓杈冨嚖鎷烽敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷�
//            {
//                for (Xsite = ImageDeal[Ysite + 1].RightBorder - 4; Xsite < ImageDeal[Ysite + 1].RightBorder + 2;
//                        Xsite++)
//                {
//                    if (Pixle[Ysite][Xsite] == 1 && Pixle[Ysite][Xsite + 1] == 0)
//                    {
//                        ImageDeal[Ysite].RightBorder = Xsite;
//                        LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
//                        LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
//                        ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
//                        ImageDeal[Ysite].Center = ((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2);
//                        ImageStatus.OFFLine = Ysite + 1;
//                        break;
//
//                        //if(ImageFlag.ring_big_small==1)//閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//                        // else//灏忓渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
//                        //     ImageDeal[Ysite].Center = ImageDeal[Ysite].RightBorder - Half_Bend_Wide[Ysite];
//
//                    }
//                }
//                if (ImageDeal[Ysite].Center < 4)
//                {
//                    ImageDeal[Ysite].Center = 4;
//                    break;
//                }
//                if (ImageDeal[Ysite].Wide > 8 && ImageDeal[Ysite].RightBorder < ImageDeal[Ysite + 2].RightBorder)
//                {
//                    continue;
//                }
//                else
//                {
//
//                    break;
//                }
//
//            }
        }
    }
//閿熸枻鎷烽敓鏂ゆ嫹 灏忛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹 閿熻鐜鎷烽敓鏂ゆ嫹
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
//閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹
    if (ImageFlag.image_element_rings_flag == 8 && ImageFlag.ring_big_small == 1)    //閿熸枻鎷峰渾閿熸枻鎷�
    {
        Repair_Point_Xsite = 32;
        Repair_Point_Ysite = 7;
        uint8 temp_Repair_X_L = (uint8) Repair_X_L;
        for (int Ysite = 40; Ysite > 5; Ysite--)
        {
            if (Pixle[Ysite][temp_Repair_X_L] == 1 && Pixle[Ysite - 1][temp_Repair_X_L] == 0)    //28
            {
                Repair_Point_Xsite = temp_Repair_X_L;
                Repair_Point_Ysite = Ysite - 1;
                ImageStatus.OFFLine = Ysite + 1;  //閿熸枻鎷锋閿熸枻鎷烽敓鏂ゆ嫹閿熼摪瑙勫垝
                break;
            }
        }
        for (int Ysite = 57; Ysite > Repair_Point_Ysite - 3; Ysite--)         //閿熸枻鎷烽敓鏂ゆ嫹
        {
            ImageDeal[Ysite].RightBorder = (ImageDeal[58].RightBorder - Repair_Point_Xsite) * (Ysite - 58)
                    / (58 - Repair_Point_Ysite) + ImageDeal[58].RightBorder;
            LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            ImageDeal[Ysite].Center = ((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2);
        }
    }
//        //灏忓渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹
//    if (ImageFlag.image_element_rings_flag == 8 && ImageFlag.ring_big_small == 2)    //灏忓渾閿熸枻鎷�
//    {
//        Repair_Point_Xsite = 0;
//        Repair_Point_Ysite = 0;
//        for (int Ysite = 55; Ysite > 5; Ysite--)
//        {
//            if (Pixle[Ysite][15] == 1 && Pixle[Ysite-1][15] == 0)
//            {
//                Repair_Point_Xsite = 15;
//                Repair_Point_Ysite = Ysite-1;
//                ImageStatus.OFFLine = Ysite + 1;  //閿熸枻鎷锋閿熸枻鎷烽敓鏂ゆ嫹閿熼摪瑙勫垝
//                break;
//            }
//        }
//        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)         //閿熸枻鎷烽敓鏂ゆ嫹
//        {
//            ImageDeal[Ysite].RightBorder = (ImageDeal[58].RightBorder - Repair_Point_Xsite) * (Ysite - 58) / (58 - Repair_Point_Ysite)  + ImageDeal[58].RightBorder;
//            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
//        }
//    }
//閿熺獤绛规嫹閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    if (ImageFlag.image_element_rings_flag == 9)
    {
        for (Ysite = 57; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].LeftBorder = ImageDeal[Ysite].RightBorder - (int) (1.8 * Half_Road_Wide[Ysite]);
            LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2; //鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姒叏鎷峰憰閿熻顕嗘嫹閿熸枻鎷烽敓鍙》鎷峰�奸敓鏂ゆ嫹閿熸枻鎷�
            ImageDeal[Ysite].Wide = -ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder; //閿熸枻鎷烽敓鑴氫紮鎷蜂箣閿熸枻鎷风洰閿熸枻鎷峰嵈閿熸枻鎷烽敓锟�
        }
        RouteFilter();
    }
}
//--------------------------------------------------------------
//  @name           Element_Handle_Right_Rings()
//  @brief          閿熸枻鎷烽敓鏂ゆ嫹鍥鹃敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴鐚存嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Handle_Right_Rings();
//-------------------------------------------------------------
float Repair_X_R = 57.1;
float circle_R_1 = 0;
float circle_R_2 = 0;
float circle_R_3 = 0;
float circle_R_4 = 0;
float lenth_ring_state_r_7 = 0;
float k_rlen_r = 100;
float k_wide_r_9 = 2.0;
void Element_Handle_Right_Rings (void)
{
    /****************閿熷彨璁规嫹*****************/
    int num = 0;
    for (int Ysite = 55; Ysite > 30; Ysite--)
    {
        if (ImageDeal[Ysite].IsRightFind == 'W')
        {
            num++;
        }
        if (ImageDeal[Ysite + 3].IsRightFind == 'W' && ImageDeal[Ysite + 2].IsRightFind == 'W'
                && ImageDeal[Ysite + 1].IsRightFind == 'W' && ImageDeal[Ysite].IsRightFind == 'T')
            break;
    }
    if (ImageFlag.image_element_rings_flag <= 6)
    {
        circle_add = circle_R_1;

    }
    else if (ImageFlag.image_element_rings_flag == 7 && ImageFlag.image_element_rings_flag != 8
            && ImageFlag.image_element_rings_flag != 9)
    {
        circle_add = circle_R_2;
    }
    else if (ImageFlag.image_element_rings_flag == 8)
    {
        circle_add = circle_R_3;
    }
    if (ImageFlag.image_element_rings_flag == 9)
    {
        circle_add = circle_R_4;
    }

//鍑嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
    if (ImageFlag.image_element_rings_flag == 1 && num > 10)
    {
        uint8 state_flag = 0;
        for (int i = 0; i < 5; i++)
        {
            if (Real_state[i][1] == 'W')
            {
                state_flag++;
            }
        }
        if (state_flag >= 3)
        {
            ImageFlag.image_element_rings_flag = 2;
            encoder_end();
            encoder_start();
        }
//        ImageFlag.image_element_rings_flag = 2;

    }
    else if (ImageFlag.image_element_rings_flag == 2)
    {
        uint8 mid_test_points_Y = 0;
        uint8 point_flag = 0;
        for (int Ysite = 45; Ysite > ImageStatus.OFFLine + 5; Ysite--)
        {
            //        if (ImageDeal[Ysite + 3].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
            //            && ImageDeal[Ysite + 2].LeftBoundary_First < ImageDeal[Ysite].LeftBoundary_First
            //            && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 1].LeftBoundary_First
            //            && ImageDeal[Ysite].LeftBoundary_First > ImageDeal[Ysite - 1].LeftBoundary_First
            //            )
            if (((ImageDeal[Ysite + 4].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                    && ImageDeal[Ysite + 3].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                    && ImageDeal[Ysite + 4].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First
                    && ImageDeal[Ysite + 3].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First)
                    || (ImageDeal[Ysite + 4].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                            && ImageDeal[Ysite + 2].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                            && ImageDeal[Ysite + 4].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First
                            && ImageDeal[Ysite + 2].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First)
                    || (ImageDeal[Ysite + 5].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                            && ImageDeal[Ysite + 3].RightBoundary_First > ImageDeal[Ysite].RightBoundary_First
                            && ImageDeal[Ysite + 5].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First
                            && ImageDeal[Ysite + 3].RightBoundary_First - 4 < ImageDeal[Ysite].RightBoundary_First))
                    && ((ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 2].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 3].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 2].RightBoundary_First
                            && ImageDeal[Ysite].RightBoundary_First + 4 > ImageDeal[Ysite - 3].RightBoundary_First)
                            || (ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 1].RightBoundary_First
                                    && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 3].RightBoundary_First
                                    && ImageDeal[Ysite].RightBoundary_First + 4
                                            > ImageDeal[Ysite - 1].RightBoundary_First
                                    && ImageDeal[Ysite].RightBoundary_First + 4
                                            > ImageDeal[Ysite - 3].RightBoundary_First)
                            || (ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 2].RightBoundary_First
                                    && ImageDeal[Ysite].RightBoundary_First < ImageDeal[Ysite - 4].RightBoundary_First
                                    && ImageDeal[Ysite].RightBoundary_First + 4
                                            > ImageDeal[Ysite - 2].RightBoundary_First
                                    && ImageDeal[Ysite].RightBoundary_First + 4
                                            > ImageDeal[Ysite - 4].RightBoundary_First)))
            {
                mid_test_points_Y = Ysite;
                point_flag = 1;
                break;
            }
        }
        if (num < 8)
        {
            if (point_flag)
            {
                if (mid_test_points_Y > 15)
                {
                    ImageFlag.image_element_rings_flag = 5;
                    encoder_end();
                    encoder_start();
                }
            }
            else
            {
                ImageFlag.image_element_rings_flag = 5;
                encoder_end();
                encoder_start();
            }
        }
        else if (encoder_getval_cm() > 40)
        {
            ImageFlag.image_element_rings_flag = 5;
            encoder_end();
            encoder_start();
        }
//        else if
//        {
//            for (Ysite = ImageStatus.OFFLine + 5; Ysite < 30; Ysite++)
//            {
//                if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite + 1].IsRightFind == 'T'
//                        && ImageDeal[Ysite + 2].IsRightFind == 'W')
//                {
//                    if (Ysite > 10)
//                    {
//                        ImageFlag.image_element_rings_flag = 5;
//                        break;
//                    }
//                }
//
//            }
//        }
    }
//閿熸枻鎷烽敓鏂ゆ嫹
    else if (ImageFlag.image_element_rings_flag == 5 && ImageStatus.Left_Line > 15)
    {
        ImageFlag.image_element_rings_flag = 6;
        encoder_end();
        encoder_start();
        // ImageStatus.Road_type = RightCirque;
    }
//閿熸枻鎷烽敓鏂ゆ嫹灏忓渾閿熸枻鎷�
    else if (ImageFlag.image_element_rings_flag == 6 && ImageStatus.Left_Line < 10 && encoder_getval_cm() > 20)
    {
        for (Ysite = 54; Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (ImageDeal[Ysite].LeftBoundary_First > 1)
            {
                break;
            }
        }
        uint8 last_Ysite = Ysite;
        uint8 jump_flag = 0;
        for (Ysite = 54; Ysite > last_Ysite - 5 && Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite + 1].LeftBoundary_First > 2
                    || ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite + 1].LeftBoundary_First < -2)
            {
                jump_flag = 1;
            }
        }
        if (!jump_flag)
        {
            ImageFlag.image_element_rings_flag = 7;
            encoder_end();
            encoder_start();
        }

        //Stop=1;
    }
    else if (ImageFlag.image_element_rings_flag == 7 && encoder_getval_cm() > 100 && ImageStatus.Left_Line > 10)
    {
//        printf("111");
        Point_Xsite = 0;
        Point_Ysite = 0;
        for (Ysite = 52; Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite + 2].LeftBoundary_First < -3
                    || (ImageDeal[Ysite].LeftBoundary_First >= ImageDeal[Ysite - 2].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First >= ImageDeal[Ysite + 1].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First >= ImageDeal[Ysite - 1].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First >= ImageDeal[Ysite + 4].LeftBoundary_First
                            && ImageDeal[Ysite].LeftBoundary_First >= ImageDeal[Ysite - 4].LeftBoundary_First
//                            && !(ImageDeal[Ysite].LeftBorder == ImageDeal[Ysite + 2].LeftBorder
//                                    && ImageDeal[Ysite].LeftBorder == ImageDeal[Ysite + 1].LeftBorder
//                                    && ImageDeal[Ysite].LeftBorder == ImageDeal[Ysite + 4].LeftBorder)
//                            && !(ImageDeal[Ysite].LeftBorder == ImageDeal[Ysite - 2].LeftBorder
//                                    && ImageDeal[Ysite].LeftBorder == ImageDeal[Ysite - 1].LeftBorder
//                                    && ImageDeal[Ysite].LeftBorder == ImageDeal[Ysite - 4].LeftBorder
                    ))

            {
                Point_Xsite = ImageDeal[Ysite].LeftBorder;
                Point_Ysite = Ysite;
                break;
            }
        }
        if (Point_Ysite > 24)
        {
            lenth_ring_state_r_7 = encoder_end();
            encoder_start();
            ImageFlag.image_element_rings_flag = 8;
        }
    }
    else if (ImageFlag.image_element_rings_flag == 8)
    {
        if (
//                Straight_Judge_1(1, ImageStatus.OFFLine + 10, 45) < 1 &&
        ImageStatus.Left_Line < 9 && ImageStatus.OFFLine < 10) //閿熸彮鎲嬫嫹涓虹洿閿熸枻鎷烽敓鎻枻鎷锋閿熷彨锝忔嫹鍓嶇灮鍊奸敓鏂ゆ嫹閿熸枻鎷峰皬
        {
            encoder_end();
            encoder_start();
            ImageFlag.image_element_rings_flag = 9;
        }

    }
    else if (ImageFlag.image_element_rings_flag == 9)
    {
        if (encoder_getval_cm() * encoder_getval_cm() > k_rlen_r * lenth_ring_state_r_7)
        {
            ImageStatus.Road_type = 0;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
            ImageStatus.Road_type = Normol;
            encoder_end();
            gpio_set_level(Beep, 0);
//            printf("end:1");
//            circle_flag = 0;
            circle_add = 0;
            return;

        }
//        if (encoder_getval_cm() > 120)
//        {
//            //            encoder_end();
//            int num = 0;
//            for (int Ysite = 54; Ysite > 35; Ysite--)
//            {
//                if (ImageDeal[Ysite].IsRightFind == 'W')
//                    num++;
//            }
//            if (num < 5)
//            {
//                ImageStatus.Road_type = 0;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
//                ImageFlag.image_element_rings_flag = 0;
//                ImageFlag.image_element_rings = 0;
//                ImageFlag.ring_big_small = 0;
//                encoder_end();
//                gpio_set_level(Beep, 0);
//                return;
//            }
//        }
//        if (encoder_getval_cm() > 100)
//        {
//            if (ImageStatus.OFFLine < 5 && Pixle[ImageStatus.OFFLine][40]
//                    && Pixle[ImageStatus.OFFLine][40] == Pixle[Ysite][39]
//                    && Pixle[ImageStatus.OFFLine][40] == Pixle[Ysite][41])
//            {
//                for (Ysite = 54; Ysite > 0; Ysite--)
//                {
//                    if (!(Pixle[Ysite][40] && Pixle[Ysite][40] == Pixle[Ysite][39]
//                            && Pixle[Ysite][40] == Pixle[Ysite][41]))
//                    {
//                        break;
//                    }
//                    if (Ysite <= ImageStatus.OFFLine)
//                    {
//                        ImageFlag.image_element_rings_flag = 10;
//                        encoder_end();
//                        encoder_start();
//                        break;
//                    }
//                }
//
//            }
//        }
    }
    else if (ImageFlag.image_element_rings_flag == 10)
    {
        if (encoder_getval_cm() > 40)
        {
            ImageStatus.Road_type = 0;   //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
            encoder_end();
            gpio_set_level(Beep, 0);
        }
    }
    /***************************************閿熸枻鎷烽敓鏂ゆ嫹**************************************/
//鍑嗛敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    if (ImageFlag.image_element_rings_flag == 1 || ImageFlag.image_element_rings_flag == 2
            || ImageFlag.image_element_rings_flag == 3 || ImageFlag.image_element_rings_flag == 4)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].RightBorder = ImageDeal[Ysite].LeftBorder + (int) (2.0 * Half_Road_Wide[Ysite]);
            LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2; //鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姒叏鎷峰憰閿熻顕嗘嫹閿熸枻鎷烽敓鍙》鎷峰�奸敓鏂ゆ嫹閿熸枻鎷�
            ImageDeal[Ysite].Wide = -ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder;
        }
        RouteFilter();

    }

//閿熸枻鎷烽敓鏂ゆ嫹  閿熸枻鎷烽敓鏂ゆ嫹
    if (ImageFlag.image_element_rings_flag == 5 || ImageFlag.image_element_rings_flag == 6)
    {
        int flag_Xsite_1 = 0;
        int flag_Ysite_1 = 0;
        float Slope_Right_Rings = 0;
        uint8 flag_Site = 0;
        static int last_flag_Ysite_2 = 0;
        static float last_Slope_Rings_2 = 0;
        static int8 last_flag_Site_2 = 0;
        for (Ysite = 55; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            for (Xsite =ImageDeal[Ysite].RightBorder - 5; Xsite > ImageDeal[Ysite].LeftBorder + 1; Xsite--)
            {
                if (Pixle[Ysite][Xsite] == 1 && Pixle[Ysite][Xsite + 1] == 0)
                {
                    flag_Ysite_1 = Ysite;
                    flag_Xsite_1 = Xsite;
                    Slope_Right_Rings = (float) (0 + (int) (0.02 * flag_Ysite_1) - flag_Xsite_1)
                            / (float) (59 - flag_Ysite_1);
                    flag_Site = 1;
                    break;
                }
            }
            if (flag_Ysite_1 != 0)
            {
                break;
            }
        }
        if (flag_Ysite_1 == 0)
        {
            for (Ysite = ImageStatus.OFFLine + 5; Ysite < 30; Ysite++)
            {
                if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite + 1].IsRightFind == 'T'
                        && ImageDeal[Ysite + 2].IsRightFind == 'W'
                        && abs(ImageDeal[Ysite].RightBorder-ImageDeal[Ysite+2].RightBorder) < -10)
                {
                    flag_Ysite_1 = Ysite;
                    flag_Xsite_1 = ImageDeal[flag_Ysite_1].RightBorder;
                    ImageStatus.OFFLine = (uint8) Ysite;
                    Slope_Right_Rings = (float) (0 + (int) (0.02 * flag_Ysite_1) - flag_Xsite_1)
                            / (float) (59 - flag_Ysite_1);
                    flag_Site = 1;
                    break;
                }

            }

        }
        //閿熸枻鎷烽敓鏂ゆ嫹
        if (flag_Site)
        {
            last_flag_Site_2 = 1;
            last_flag_Ysite_2 = flag_Ysite_1;
            last_Slope_Rings_2 = Slope_Right_Rings;
        }
//        else if (last_flag_Site_2)
//        {
//
//            flag_Ysite_1 = last_flag_Ysite_2 + 1;
//            flag_Xsite_1 = ImageDeal[flag_Ysite_1].LeftBorder;
//            Slope_Right_Rings = last_Slope_Rings_2;
//            last_flag_Site_2++;
//            if (last_flag_Site_2 == 4)
//            {
//                last_flag_Site_2 = 2;
//            }
//
//        }
//        else
//        {
//            last_flag_Site_2--;
//            if (last_flag_Site_2 < 0)
//            {
//                last_flag_Site_2 = 0;
//            }
//        }
        if (flag_Ysite_1 != 0)
        {
            for (Ysite = flag_Ysite_1 - 2; Ysite < 60; Ysite++)
            {
                ImageDeal[Ysite].LeftBorder = flag_Xsite_1 + Slope_Right_Rings * (Ysite - flag_Ysite_1);
                LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
                LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
//                if(ImageFlag.ring_big_small==2)//灏忓渾閿熸枻鎷烽敓鎺ュ府鎷烽敓锟�
//                    ImageDeal[Ysite].Center=ImageDeal[Ysite].LeftBorder+Half_Bend_Wide[Ysite];//閿熸枻鎷烽敓锟�
//                else//閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熸帴甯嫹閿燂拷
                Pixle[Ysite][ImageDeal[Ysite].LeftBorder] = 0;
                Pixle[Ysite][ImageDeal[Ysite].LeftBorder - 1] = 0;
                if (ImageDeal[Ysite].LeftBorder - 2 > 0)
                {
                    Pixle[Ysite][ImageDeal[Ysite].LeftBorder - 2] = 0;
                }

//                ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2;   //閿熸枻鎷烽敓锟�
//                if (ImageDeal[Ysite].Center > 79)
//                    ImageDeal[Ysite].Center = 79;
            }
            DrawLinesFirst();     //閿熸枻鎷烽敓鐙″簳鎲嬫嫹      30us
            DrawLinesProcess();   //閿熺煫纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  8us
            Search_Border_OTSU(Pixle, LCDH, LCDW, LCDH - 2);   //58閿熸枻鎷蜂綅閿熸枻鎷烽敓鏂ゆ嫹
            DrawExtensionLine();
            RouteFilter();
//            ImageStatus.OFFLine = flag_Ysite_1 + 1;
//            ImageDeal[flag_Ysite_1].LeftBorder = flag_Xsite_1;
//            for (Ysite = flag_Ysite_1 - 1; Ysite > 10; Ysite--) //A閿熸枻鎷烽敓杈冨嚖鎷烽敓鏂ゆ嫹閿熸枻鎷锋壂閿熸枻鎷�
//            {
//                for (Xsite = ImageDeal[Ysite + 1].LeftBorder + 4; Xsite > ImageDeal[Ysite + 1].LeftBorder - 4; Xsite--)
//                {
//                    if (Pixle[Ysite][Xsite] == 1 && Pixle[Ysite][Xsite - 1] == 0)
//                    {
//                        ImageDeal[Ysite].LeftBorder = Xsite;
//                        LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
//                        LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
//                        ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
////                     if(ImageFlag.ring_big_small==2)//灏忓渾閿熸枻鎷烽敓鎺ュ府鎷烽敓锟�
////                         ImageDeal[Ysite].Center=ImageDeal[Ysite].LeftBorder+Half_Bend_Wide[Ysite];//閿熸枻鎷烽敓锟�
////                     else//閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熸帴甯嫹閿燂拷
//                        ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2; //閿熸枻鎷烽敓锟�
//                        ImageStatus.OFFLine = Ysite;
//                        break;
//                    }
//                }
//                if (ImageDeal[Ysite].Center > 78)
//                {
//                    ImageDeal[Ysite].Center = 78;
//                    ImageStatus.OFFLine = Ysite + 2;
//                    break;
//                }
//                if (ImageDeal[Ysite].Wide > 8 && ImageDeal[Ysite].LeftBorder > ImageDeal[Ysite + 2].LeftBorder)
//                {
//                    continue;
//                }
//                else
//                {
//                    ImageStatus.OFFLine = Ysite + 2;
//                    break;
//                }
//            }
        }
    }
//閿熸枻鎷烽敓鑺傝鎷烽敓鏂ゆ嫹閿熸枻鎷�
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
//閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹
    uint8 temp_Repair_X_R = (uint8) Repair_X_R;
    if (ImageFlag.image_element_rings_flag == 8)  //閿熸枻鎷峰渾閿熸枻鎷�
    {
        Repair_Point_Xsite = 50;
        Repair_Point_Ysite = 7;
        for (int Ysite = 40; Ysite > 8; Ysite--)
        {
            if (Pixle[Ysite][temp_Repair_X_R] == 1 && Pixle[Ysite - 1][temp_Repair_X_R] == 0)
            {
                Repair_Point_Xsite = temp_Repair_X_R;
                Repair_Point_Ysite = Ysite - 1;
                ImageStatus.OFFLine = Ysite + 1;  //閿熸枻鎷锋閿熸枻鎷烽敓鏂ゆ嫹閿熼摪瑙勫垝
                break;
            }
        }
        for (int Ysite = 57; Ysite > Repair_Point_Ysite - 3; Ysite--)         //閿熸枻鎷烽敓鏂ゆ嫹
        {
            ImageDeal[Ysite].LeftBorder = (ImageDeal[58].LeftBorder - Repair_Point_Xsite) * (Ysite - 58)
                    / (58 - Repair_Point_Ysite) + ImageDeal[58].LeftBorder;
            LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
            //if(ImageDeal[Ysite].LeftBorder<3){ImageDeal[Ysite].LeftBorder = 3;}
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
        }
    }
//閿熺獤绛规嫹閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
    if (ImageFlag.image_element_rings_flag == 9)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].RightBorder = ImageDeal[Ysite].LeftBorder + (int) (k_wide_r_9 * Half_Road_Wide[Ysite]);
            LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) / 2; //鎵敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹姒叏鎷峰憰閿熻顕嗘嫹閿熸枻鎷烽敓鍙》鎷峰�奸敓鏂ゆ嫹閿熸枻鎷�
            ImageDeal[Ysite].Wide = -ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder; //閿熸枻鎷烽敓鑴氫紮鎷蜂箣閿熸枻鎷风洰閿熸枻鎷峰嵈閿熸枻鎷烽敓锟�
        }
        RouteFilter();
    }
}

void Element_Handle_Cross (void)
{
//    int16 i = 0, j = 0;
    if (!(Real_state[0][0] == 'W' && Real_state[1][0] == 'W' && Real_state[2][0] == 'W' && Real_state[3][0] == 'W'
            && Real_state[4][0] == 'W' && Real_state[0][1] == 'W' && Real_state[1][1] == 'W' && Real_state[2][1] == 'W'
            && Real_state[3][1] == 'W' && Real_state[4][1] == 'W'))
    {
        ImageStatus.Road_type = Normol;
    }
}

uint8 Sum_Left_Barriar = 0;
void Element_Handle_Left_Barriars (void)
{
    int16 i = 0, j = 0;
    uint8 check_flag = 0;
    uint8 L_Found_T = 'F';  //纭敓鏂ゆ嫹閿熺潾鎲嬫嫹鏂滈敓缁炵殑浼欐嫹鍑嗛敓鍙唻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熶茎鎲嬫嫹蹇�
    float D_L = 0;           //閿熸帴绛规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鍙唻鎷烽敓锟�
    int ytemp_W_L;           //閿熸枻鎷蜂綇閿熼樁杈炬嫹閿熻涓㈡唻鎷烽敓鏂ゆ嫹
    int ysite = 0;
    uint8 L_found_point = 0;

//    for (int16 i = 59; i > 0; i--)
//    {
//        printf("%c,", ImageDeal[i].IsLeftFind);
//    }
    if (ImageFlag.image_element_barriars_flag == 0)
    {
        for (i = 58; i > ImageStatus.OFFLineBoundary + 3; i--)
        {
            if (ImageDeal[i].IsLeftFind == 'W' && ImageDeal[i - 1].IsLeftFind == 'W'
                    && ImageDeal[i - 2].IsLeftFind == 'W' && ImageDeal[i].LeftBorder > 1
                    && ImageDeal[i - 1].LeftBorder > 1 && ImageDeal[i - 2].LeftBorder > 1)
            {
//                printf("barriar_handle");
                for (j = 2; j < dir_stop && DIR_1_Y[j].LEFT >= i; j++)
                {
                    if (DIR_1[j].LEFT == 1 && DIR_1[j + 1].LEFT == 1 && DIR_1[j + 2].LEFT == 1
                            && DIR_1[j + 3].LEFT == 1)
                    {
                        ytemp_W_L = i + 2;
                        for (ysite = i + 1; ysite < i + 15 && ysite < DIR_1_Y[j].LEFT; ysite++)
                        {
                            if (ImageDeal[ysite].IsLeftFind == 'T')
                                L_found_point++;
                        }
                        if (L_found_point > 6)              //閿熸彮纰夋嫹閿熸枻鎷峰噯鏂滈敓缁炴唻鎷�  閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹
                        {
                            D_L = ((float) (ImageDeal[Ysite + 3].LeftBorder
                                    - ImageDeal[Ysite + L_found_point].LeftBorder)) / ((float) (L_found_point - 3));
                            if (D_L > 0)
                            {
                                L_Found_T = 'T';

                            }
                            else
                            {
                                L_Found_T = 'F';
                            }
                        }
//                        if (L_Found_T == 'T')
//                        {
                        for (Ysite = (DIR_1_Y[j].LEFT + i) / 2; Ysite < 59; Ysite++)
                        {
                            ImageDeal[Ysite].LeftBorder = ImageDeal[ytemp_W_L].LeftBorder + D_L * (ytemp_W_L - Ysite);
                            LimitL(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
                            LimitH(ImageDeal[Ysite].LeftBorder);  //閿熺潾鍑ゆ嫹
                            ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
                            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
                        }
//                        }
                        //閿熸枻鎷烽敓鏂ゆ嫹
                        check_flag = 1;
                        break;
                    }
                }
//                printf("%d,%d\n\r", i, DIR_1_Y[j].LEFT);
                if (check_flag == 0)
                {
                    Sum_Left_Barriar++;
//                    printf("barriar_handle");
                    if (Sum_Left_Barriar == 3)
                    {
                        ImageFlag.image_element_barriars_flag = 1;
                        Sum_Left_Barriar = 0;
                        return;
                    }
                    break;
                }
                break;
            }
        }
    }
    if (ImageFlag.image_element_barriars_flag == 1)
    {
        for (i = 58; i > ImageStatus.OFFLineBoundary + 3; i--)
        {
            if (ImageDeal[i].IsLeftFind == 'W' && ImageDeal[i - 1].IsLeftFind == 'W'
                    && ImageDeal[i - 2].IsLeftFind == 'W' && ImageDeal[i].LeftBorder > 1
                    && ImageDeal[i - 1].LeftBorder > 1 && ImageDeal[i - 2].LeftBorder > 1)
            {
                if (i >= 35)
                {
                    Sum_Left_Barriar++;
                    if (Sum_Left_Barriar == 3)
                    {
                        ImageFlag.image_element_barriars_flag = 2;
                        Sum_Left_Barriar = 0;
                    }
                    return;
                }
            }
        }
    }
    if (ImageFlag.image_element_barriars_flag == 2)
    {
        uint8 end_check_flag = 0;
        for (i = 58; i > 30; i--)
        {
            if (ImageDeal[i].IsLeftFind == 'W' && ImageDeal[i - 1].IsLeftFind == 'W'
                    && ImageDeal[i - 2].IsLeftFind == 'W' && ImageDeal[i].LeftBorder > 1
                    && ImageDeal[i - 1].LeftBorder > 1 && ImageDeal[i - 2].LeftBorder > 1)
            {
                end_check_flag = 1;
                break;
            }
        }
        if (end_check_flag == 0)
        {
            ImageFlag.image_element_barriars_flag = 0;
            ImageStatus.Road_type = Normol;
            return;
        }
    }

}
uint8 Sum_Right_Barriar = 0;
void Element_Handle_Right_Barriars (void)
{
    int16 i = 0, j = 0;
    uint8 check_flag = 0;
    uint8 R_Found_T = 'F';  //纭敓鏂ゆ嫹閿熺潾鎲嬫嫹鏂滈敓缁炵殑浼欐嫹鍑嗛敓鍙唻鎷烽敓鏂ゆ嫹閿熻鍑ゆ嫹閿熸彮纰夋嫹閿熶茎鎲嬫嫹蹇�
    float D_R = 0;           //閿熸帴绛规嫹閿熸枻鎷烽敓鎻唻鎷烽敓鏂ゆ嫹鏂滈敓鏂ゆ嫹
    int ytemp_W_R;           //閿熸枻鎷蜂綇閿熼樁杈炬嫹閿熸彮璁规嫹閿熸枻鎷烽敓鏂ゆ嫹
    int ysite = 0;
    uint8 R_found_point = 0;
//    for (int16 i = 59; i > 0; i--)
//    {
//        printf("%c,", ImageDeal[i].IsLeftFind);
//    }
    if (ImageFlag.image_element_barriars_flag == 0)
    {
        for (i = 58; i > ImageStatus.OFFLineBoundary + 3; i--)
        {
            if (ImageDeal[i].IsRightFind == 'W' && ImageDeal[i - 1].IsRightFind == 'W'
                    && ImageDeal[i - 2].IsRightFind == 'W' && ImageDeal[i].RightBorder < 78
                    && ImageDeal[i - 1].RightBorder < 78 && ImageDeal[i - 2].RightBorder < 78)
            {
//                printf("barriar_handle");
                for (j = 2; j < dir_stop && DIR_1_Y[j].RIGHT >= i; j++)
                {
                    if (DIR_1[j].RIGHT == 3 && DIR_1[j + 1].RIGHT == 3 && DIR_1[j + 2].RIGHT == 3
                            && DIR_1[j + 3].RIGHT == 3)
                    {
                        ytemp_W_R = i + 2;
                        for (ysite = i + 1; ysite < i + 15 && ysite < DIR_1_Y[j].RIGHT; ysite++)
                        {
                            if (ImageDeal[ysite].IsRightFind == 'T')
                                R_found_point++;
                        }
                        if (R_found_point > 6)              //閿熸彮纰夋嫹閿熸枻鎷峰噯鏂滈敓缁炴唻鎷�  閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹纭敓鏂ゆ嫹閿熺潾鎲嬫嫹
                        {
                            D_R = ((float) (ImageDeal[Ysite + 3].RightBorder
                                    - ImageDeal[Ysite + R_found_point].RightBorder)) / ((float) (R_found_point - 3));
                            if (D_R > 0)
                            {
                                R_Found_T = 'T';

                            }
                            else
                            {
                                R_Found_T = 'F';
                            }
                        }
                        //                        if (L_Found_T == 'T')
                        //                        {
                        for (Ysite = (DIR_1_Y[j].RIGHT + i) / 2; Ysite < 59; Ysite++)
                        {
                            ImageDeal[Ysite].RightBorder = ImageDeal[ytemp_W_R].RightBorder + D_R * (ytemp_W_R - Ysite);
                            LimitL(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
                            LimitH(ImageDeal[Ysite].RightBorder);  //閿熺潾鍑ゆ嫹
                            ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
                            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
                        }
                        //                        }
                        //閿熸枻鎷烽敓鏂ゆ嫹
                        check_flag = 1;
                        break;
                    }
                }
//                printf("%d,%d\n\r", i, DIR_1_Y[j].RIGHT);
                if (check_flag == 0)
                {
                    Sum_Right_Barriar++;
                    //                    printf("barriar_handle");
                    if (Sum_Right_Barriar == 3)
                    {
                        ImageFlag.image_element_barriars_flag = 1;
                        Sum_Right_Barriar = 0;
                        return;
                    }
                    break;
                }
                break;
            }
        }
    }
    if (ImageFlag.image_element_barriars_flag == 1)
    {
        for (i = 58; i > ImageStatus.OFFLineBoundary + 3; i--)
        {
            if (ImageDeal[i].IsRightFind == 'W' && ImageDeal[i - 1].IsRightFind == 'W'
                    && ImageDeal[i - 2].IsRightFind == 'W' && ImageDeal[i].RightBorder < 78
                    && ImageDeal[i - 1].RightBorder < 78 && ImageDeal[i - 2].RightBorder < 78)
            {
                if (i >= 35)
                {
                    Sum_Right_Barriar++;
                    if (Sum_Right_Barriar == 3)
                    {
                        ImageFlag.image_element_barriars_flag = 2;
                        Sum_Right_Barriar = 0;
                    }
                    return;
                }
            }
        }
    }
    if (ImageFlag.image_element_barriars_flag == 2)
    {
        uint8 end_check_flag = 0;
        for (i = 58; i > 30; i--)
        {
            if (ImageDeal[i].IsRightFind == 'W' && ImageDeal[i - 1].IsRightFind == 'W'
                    && ImageDeal[i - 2].IsRightFind == 'W' && ImageDeal[i].RightBorder < 78
                    && ImageDeal[i - 1].RightBorder < 78 && ImageDeal[i - 2].RightBorder < 78)
            {
                end_check_flag = 1;
                break;
            }
        }
        if (end_check_flag == 0)
        {
            ImageFlag.image_element_barriars_flag = 0;
            ImageStatus.Road_type = Normol;
            return;
        }
    }
}

//閿熸枻鎷烽敓鑺傜》鎷烽敓鍔鎷风洿閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
float variance, variance_acc;  //閿熸枻鎷烽敓鏂ゆ嫹
void Straightacc_Test (void)
{
//    printf("%d,%d\n\r",,);

    uint8 first_std_flag = 0;
    if (Pixle[ImageStatus.OFFLine + 1][40] && Pixle[ImageStatus.OFFLine + 1][40] == Pixle[Ysite][39]
            && Pixle[ImageStatus.OFFLine + 1][40] == Pixle[Ysite][41] && ImageStatus.Right_Line < 25
            && ImageStatus.Left_Line < 25 && mid_std - ImageDeal[59].LeftBorder > 15
            && ImageDeal[59].RightBorder - mid_std > 15 && Straight_Judge_2(RIGHT, ImageStatus.OFFLine + 10, 50) < 0.1
            && Straight_Judge_2(LEFT, ImageStatus.OFFLine + 10, 50) < 0.1)
    {
        first_std_flag = 1;
    }

    if (QEKF_INS.Roll < 15 && QEKF_INS.Roll > -15 && first_std_flag && ImageStatus.OFFLine <= 5 && offset < 10
            && offset > -10)
    {
        int sum = 0;
        uint8 start_line = 55;
        for (Ysite = start_line; Ysite > ImageStatus.OFFLine + 1; Ysite--)
        {
            if (ImageDeal[Ysite].IsLeftFind == 'T' && ImageDeal[Ysite].IsRightFind == 'T'
                    && ImageDeal[Ysite - 1].IsLeftFind == 'T' && ImageDeal[Ysite].IsRightFind == 'T')
            {
                start_line = Ysite;
            }
            break;
        }
        for (Ysite = start_line; Ysite > ImageStatus.OFFLine + 1; Ysite--)
        {
            sum += (ImageDeal[Ysite].Center - ImageStatus.MiddleLine)
                    * (ImageDeal[Ysite].Center - ImageStatus.MiddleLine);
        }
        variance_acc = (float) sum / (start_line - 1 - ImageStatus.OFFLine);
//        printf("%f\n\r", variance_acc);
        if (variance_acc < ImageStatus.variance_acc)
        {
            if (k_add < 1 && k_add > -1)
            {
                ImageStatus.straight_acc = 1;
                kp_servo = kp_servo_speed;
                run = run_speed;
                limit_pwm_palstance = palstance_speed;
                SSSTOP;
            }
            else
            {
                ImageStatus.straight_acc = 2;
                kp_servo = kp_servo_on;
                run = run_on;
                limit_pwm_palstance = palstance_on;
                SSSTOP;
            }
        }
        else if (Straight_Judge_2(RIGHT, ImageStatus.OFFLine + 10, 50) < 0.1
                && Straight_Judge_2(LEFT, ImageStatus.OFFLine + 10, 50) < 0.1)
        {
            ImageStatus.straight_acc = 2;
            kp_servo = kp_servo_on;
            run = run_on;
            limit_pwm_palstance = palstance_on;
            SSSTOP;
        }
    }
    else
    {
        ImageStatus.straight_acc = 0;
    }
//    printf("%d,%d,%.4f,%4f\n\r", ImageStatus.straight_acc, first_std_flag, QEKF_INS.Roll, offset);
//    if ((Straight_Judge_2(RIGHT, ImageStatus.OFFLine, 50) < 0.1
//            && Straight_Judge_2(LEFT, 10 > ImageStatus.OFFLine ? 10 : ImageStatus.OFFLine, 50) < 0.1
//            && ImageStatus.Right_Line < 7 && ImageStatus.Left_Line < 7 && ImageStatus.OFFLine <= 5))
//    {
//        ImageStatus.straight_acc = 1;
//        str_start();
//        run = run_speed;
//        limit_pwm_palstance = limit_pwm_palstance_min;
//        SSSTOP;
////        angle_bias = 0;
//    }
//    else if ((Straight_Judge_2(RIGHT, ImageStatus.OFFLine, ImageStatus.OFFLine + 10) < 0.1
//            && Straight_Judge_2(LEFT, ImageStatus.OFFLine, ImageStatus.OFFLine + 10) < 0.1
//            && Straight_Judge_2(CENTER, ImageStatus.OFFLine, ImageStatus.OFFLine + 10) < 0.1)
//            && ImageStatus.Right_Line < 5)
//    {
//        ImageStatus.straight_acc = 2;
//        str_start();
//        run = run_on;
//        limit_pwm_palstance = limit_pwm_palstance_min;
//        SSSTOP;
//    }
//    else if (ImageStatus.straight_acc && Straight_Judge_2(RIGHT, ImageStatus.OFFLine, ImageStatus.OFFLine + 10) < 0.1
//            && Straight_Judge_2(LEFT, ImageStatus.OFFLine, ImageStatus.OFFLine + 10) < 0.1
//            && ImageStatus.Right_Line < 5)
//    {
//        ImageStatus.straight_acc = 2;
//        str_start();
//        run = run_on;
//        limit_pwm_palstance = limit_pwm_palstance_min;
//        SSSTOP;
//    }
//    else if (str_getval_cm() > 20 || ImageStatus.OFFLine > 5)
//    {
//        str_end();
//        run = run_normal;
//        limit_pwm_palstance = limit_pwm_palstance_max;
//        SSSTOP;
//        ImageStatus.straight_acc = 0;
//    }
    if (ImageStatus.straight_acc == 0)
    {
        ImageStatus.straight_acc = 0;
        kp_servo = kp_servo_max;
        run = run_normal;
        limit_pwm_palstance = palstance_max;
        SSSTOP;
    }
}
//閿熸枻鎷风洿閿熸枻鎷烽敓鏂ゆ嫹BUG閿熸枻鎷烽敓鏂ゆ嫹娌￠敓鏂ゆ嫹瑕�
////閿熸枻鎷烽敓鑺傛唻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楁唻鎷烽敓鏂ゆ嫹閿熸枻鎷�
//void Straight_Test_2(void) {
//  float midd_k, sum;
//  midd_k = (ImageDeal[55].Center - ImageDeal[ImageStatus.OFFLine + 1].Center) /(float)(55 - ImageStatus.OFFLine - 1);
//  for (Ysite = 55; Ysite > ImageStatus.OFFLine + 1; Ysite--) {
//    ImageDeal[Ysite].mid_temp =ImageDeal[55].Center - midd_k * (55 - Ysite) + 0.5;
//    sum += pow(ImageDeal[Ysite].Center - ImageDeal[Ysite].mid_temp, 2);
//  }
//
//      variance = sum / (54 - ImageStatus.OFFLine);
//  if (variance < ImageStatus.variance && ImageStatus.OFFLine <= 9 ) {
//      ImageStatus.Road_type = Straight;
//  }
//}

/****閿熼摪纰夋嫹閿熸枻鎷烽敓锟�***/
int ramptestflag = 1;   //閿熼摪纰夋嫹閿熷彨鏂唻鎷峰織浣�
int rampnum = 0;     //閿熼摪纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹
void Ramp_Test ()
{
    int i = 0;
    if (ImageStatus.OFFLine == 2 && ImageStatus.Ramp_lenth == 0 && ramptestflag == 1)
    {
        for (Ysite = ImageStatus.OFFLine; Ysite < 7; Ysite++)
        {
            if (ImageDeal[Ysite].Wide > 15 && ImageDeal[Ysite].IsRightFind == 'T'
                    // &&ImageDeal[Ysite].IsLeftFind == 'T'
                    && ImageDeal[Ysite].LeftBorder < 40 && ImageDeal[Ysite].RightBorder > 40
                    && Pixle[Ysite][ImageDeal[Ysite].Center] == 1 && Pixle[Ysite][ImageDeal[Ysite].Center - 2] == 1
                    && Pixle[Ysite][ImageDeal[Ysite].Center + 2] == 1)
                i++;
            if (i >= 4)
            {
                ImageStatus.Road_type = Ramp;
                rampnum++;
                ramptestflag = 0;
                break;
            }
        }
    }
}

void Cross_Test2 ()
{
    int leftlowlen = 0;
    int leftmiddlen = 0;
    int lefthighlen = 0;
    int rightlowlen = 0;
    int rightmiddlen = 0;
    int righthighlen = 0;
//閿熸枻鎷烽敓鏂ゆ嫹閿熺粸顕嗘嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
    for (Ysite = 54; Ysite > ImageStatus.OFFLine + 2; Ysite--)
    {

        //鍗侀敓琛楀簳杈圭鎷烽敓鍙唻鎷�
        if (ImageDeal[Ysite].IsLeftFind == 'T' && ImageDeal[Ysite - 1].IsLeftFind == 'T' && leftlowlen == 0)
        {
            while (ImageDeal[Ysite].IsLeftFind == 'T')
            {
                leftlowlen++;
                Ysite--;
                if (ImageDeal[Ysite].LeftBorder < ImageDeal[Ysite + 5].LeftBorder - 2)
                {
                    leftlowlen = 1;
                    break;
                }

            }
        }

        //鍗侀敓鏂ゆ嫹閿熷彨纭锋嫹浜╅敓鏂ゆ嫹閿燂拷
        if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite - 1].IsLeftFind == 'W' && leftmiddlen == 0)
        {
            while (ImageDeal[Ysite].IsLeftFind == 'W')
            {
                leftmiddlen++;
                Ysite--;
            }
        }

        //鍗侀敓鏂ゆ嫹閿熻緝杈圭鎷烽敓鍙唻鎷�
        if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite - 1].IsRightFind == 'T' && lefthighlen == 0)
        {
            while (ImageDeal[Ysite].IsRightFind == 'T')
            {
                lefthighlen++;
                Ysite--;
            }
        }
    }
//閿熸枻鎷烽敓鎻唻鎷峰崄閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
    for (Ysite = 54; Ysite > ImageStatus.OFFLine + 2; Ysite--)
    {
        //鍗侀敓琛楀簳杈圭鎷烽敓鍙唻鎷�
        if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite - 1].IsRightFind == 'T' && rightlowlen == 0)
        {
            while (ImageDeal[Ysite].IsRightFind == 'T')
            {
                rightlowlen++;
                Ysite--;
                if (ImageDeal[Ysite].RightBorder > ImageDeal[Ysite + 5].RightBorder + 2)
                {
                    rightlowlen = 1;
                    break;
                }
            }
        }

        //鍗侀敓琛楀簳杈圭殑璁规嫹閿熸枻鎷�
        if (ImageDeal[Ysite].IsRightFind == 'W' && ImageDeal[Ysite - 1].IsRightFind == 'W' && rightmiddlen == 0)
        {
            while (ImageDeal[Ysite].IsRightFind == 'W')
            {
                rightmiddlen++;
                Ysite--;
            }
        }

        //鍗侀敓鏂ゆ嫹閿熻緝杈圭鎷烽敓鍙唻鎷�
        if (ImageDeal[Ysite].IsRightFind == 'T' && ImageDeal[Ysite - 1].IsRightFind == 'T' && righthighlen == 0)
        {
            while (ImageDeal[Ysite].IsRightFind == 'T')
            {
                righthighlen++;
                Ysite--;
            }
        }
    }
    if (leftlowlen > 5 && leftmiddlen > 5 && lefthighlen > 4 && rightlowlen > 5 && rightmiddlen > 5 && righthighlen > 4)
        ImageStatus.Road_type = Cross_ture;

//     if(ImageStatus.Cross_ture_lenth*OX>80)
//        ImageStatus.Road_type=0;
// else
//       Fork_dowm=0;

}

void Pcir_Handle ()
{
    if (ImageStatus.Road_type == Cross_ture)   //閿熻妭璁规嫹鍦堥敓鏂ゆ嫹閿熸枻鎷�
    {
        for (Ysite = 58; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = ImageDeal[Ysite].RightBorder - buxianwide[Ysite] * 0.5;
            ImageDeal[Ysite].LeftBorder = ImageDeal[Ysite].RightBorder - buxianwide[Ysite];
        }
        Fit1(59, 50);
    }

}
uint8 Zebra_flag = 0;
uint8 Zebra_start_flag = 0;
float Zebra_angle_L = -2;
float Zebra_angle_H = 5;
float Zebra_limit = 6;
void Zebra_Test (void)
{
    int16 j = 0;
//    Zebra_start_flag=0;
    if (Zebra_start_flag != 1 && QEKF_INS.Pitch > Zebra_angle_L && QEKF_INS.Pitch < Zebra_angle_H
            && ImageStatus.Road_type != Ramp)
    {

        for (Ysite = 20; Ysite < 55; Ysite++)
        {
            j = 0;
            for (Xsite = ImageDeal[Ysite].LeftBorder; Xsite < ImageDeal[Ysite].RightBorder; Xsite++)
            {
                if (Pixle[Ysite][Xsite] != Pixle[Ysite][Xsite + 1])  //閿熸枻鎷风嵕鏂ゆ嫹娉昏瘞閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
                    j++;
            }
//            if(j>2)
//            printf("%d\n\r",j);
            if (j > Zebra_limit)
            {
                if (Zebra_start_flag == 2)
                {
                    if (j > Zebra_limit)
                    {
                        Zebra_flag = 1;
                        end_dot_show = 1;
                        ImageFlag.ring_big_small = 0;
                        ImageFlag.image_element_rings_flag = 0;
                        ImageFlag.image_element_rings = 0;
                        ImageStatus.Road_type = Normol;
                        gpio_set_level(Beep, 0);
//                        printf("end\n\r");
                    }
                }
                else
                {
                    Zebra_start_flag = 1;
                    ImageStatus.Road_type = Normol;
                    ImageFlag.ring_big_small = 0;
                    ImageFlag.image_element_rings_flag = 0;
                    ImageFlag.image_element_rings = 0;
                    gpio_set_level(Beep, 0);
                    encoder_start();
                    dot_matrix_screen_show_string("GO");
//                    printf("start\n\r");
                }
            }
        }
    }
//    Zebra_start_flag=0;

//    printf("%d,%d,%d\n\r", Zebra_start_flag, Zebra_flag, j);

}
void S_Conner_Test (void)
{
    if (ImageStatus.OFFLine < 5 && Pixle[ImageStatus.OFFLine][40] && Pixle[ImageStatus.OFFLine][40] == Pixle[Ysite][39]
            && Pixle[ImageStatus.OFFLine][40] == Pixle[Ysite][41]
            && Straight_Judge_2(RIGHT, 10 > ImageStatus.OFFLine ? 10 : ImageStatus.OFFLine, 50) > 0.1
            && Straight_Judge_2(LEFT, 10 > ImageStatus.OFFLine ? 10 : ImageStatus.OFFLine, 50) > 0.1)
    {
        for (Ysite = 54; Ysite > 0; Ysite--)
        {
            if (!(Pixle[Ysite][40] && Pixle[Ysite][40] == Pixle[Ysite][39] && Pixle[Ysite][40] == Pixle[Ysite][41]))
            {
                break;
            }
            if (Ysite <= ImageStatus.OFFLine)
            {
                ImageStatus.Road_type = S_Conner;
                break;
            }
        }
        if (Ysite > ImageStatus.OFFLine && ImageStatus.Road_type == S_Conner)
        {
            ImageStatus.Road_type = 0;
        }
    }
    else if (ImageStatus.Road_type == S_Conner)
    {
        ImageStatus.Road_type = 0;
    }
}

void Barriar_Test (void)
{
    Sum_Left_Barriar = 0;
    int16 i = 0, j = 0;
    uint8 Barriar_Help_Flag = 0;
    if (ImageStatus.Left_Line < 15 && ImageStatus.Right_Line < 15 && ImageStatus.OFFLine < 8)
    {
        for (i = 0; i < dir_stop - 5 && DIR_1_Y[i + 3].LEFT >= 20; i++)
        {
            if (DIR_1[i].LEFT == 1 && DIR_1[i + 1].LEFT == 1 && DIR_1[i + 2].LEFT == 1 && DIR_1[i + 3].LEFT == 1)
            {

                for (j = DIR_1_Y[i + 3].LEFT; j > ImageStatus.OFFLine + 2 && j >= DIR_1_Y[i + 3].LEFT - 30; j--)
                {
                    if (ImageDeal[j].IsLeftFind == 'W' && ImageDeal[j].LeftBorder < 3)
                    {
                        break;
                    }
                    if (ImageDeal[j].IsLeftFind == 'W' && ImageDeal[j - 1].IsLeftFind == 'W'
                            && ImageDeal[j - 2].IsLeftFind == 'W' && ImageDeal[j].LeftBorder > 1
                            && ImageDeal[j - 1].LeftBorder > 1 && ImageDeal[j - 2].LeftBorder > 1)
                    {
                        uint8 Check_S = 58, Check_E = ImageStatus.OFFLine + 2;
                        for (; ImageDeal[Check_S].RightBorder >= 78; Check_S--)
                            ;
                        if (Straight_Judge_2(RIGHT, Check_E, Check_S) < 0.1)
                        {
                            Barriar_Help_Flag = 1;
                        }
                        break;
                    }
                }
            }
            if (Barriar_Help_Flag == 1)
            {
                break;
            }
        }
    }
    if (ImageStatus.Left_Line < 15 && ImageStatus.Right_Line < 15 && ImageStatus.OFFLine < 8)
    {
        for (i = 0; i < dir_stop - 5 && DIR_1_Y[i + 3].RIGHT >= 20; i++)
        {

            if (DIR_1[i].RIGHT == 3 && DIR_1[i + 1].RIGHT == 3 && DIR_1[i + 2].RIGHT == 3 && DIR_1[i + 3].RIGHT == 3)
            {
                for (j = DIR_1_Y[i + 3].RIGHT; j > ImageStatus.OFFLine + 2 && j >= DIR_1_Y[i + 3].RIGHT - 30; j--)
                {
                    if (ImageDeal[j].IsRightFind == 'W' && ImageDeal[j].RightBorder > 76)
                    {
                        break;
                    }
                    if (ImageDeal[j].IsRightFind == 'W' && ImageDeal[j - 1].IsRightFind == 'W'
                            && ImageDeal[j - 2].IsRightFind == 'W' && ImageDeal[j].RightBorder < 78
                            && ImageDeal[j - 1].RightBorder < 78 && ImageDeal[j - 2].RightBorder < 78)
                    {
//                    printf("test,%d\n\r", j);
                        uint8 Check_S = 58, Check_E = ImageStatus.OFFLine + 2;
                        for (; ImageDeal[Check_S].LeftBorder <= 1; Check_S--)
                            ;
                        if (Straight_Judge_2(LEFT, Check_E, Check_S) < 0.1)
                        {
                            if (Barriar_Help_Flag == 1)
                            {
//                                printf("barriar_test_error");
                            }
                            else
                            {
                                Barriar_Help_Flag = 2;
                            }
                        }
                        break;
                    }
                }
            }
            if (Barriar_Help_Flag == 2)
            {
                break;
            }
        }
    }
//printf("%d\n\r",Barriar_Help_Flag);
    if (Barriar_Help_Flag == 1
            && !(ImageDeal[30].IsRightFind == 'W' || ImageDeal[40].IsRightFind == 'W'
                    || ImageDeal[20].IsRightFind == 'W' || ImageDeal[10].IsRightFind == 'W'))
    {
        ImageStatus.Road_type = LeftBarriar;
    }
    else if (Barriar_Help_Flag == 2
            && !(ImageDeal[30].IsLeftFind == 'W' || ImageDeal[20].IsLeftFind == 'W' || ImageDeal[40].IsLeftFind == 'W'
                    || ImageDeal[10].IsLeftFind == 'W'))
    {
        ImageStatus.Road_type = RightBarriar;
    }
    ImageFlag.image_element_barriars = Barriar_Help_Flag;
    return;
}
float special_cross = 0;
void Cross_Test (void)
{
    int16 i = 0;
    int16 L_jump_point[2] = {0}, R_jump_point[2] = {0};
    uint8 L_jump_flag = 0, R_jump_flag = 0;
    if (Real_state[0][0] == 'W' && Real_state[1][0] == 'W' && Real_state[2][0] == 'W' && Real_state[3][0] == 'W'
            && Real_state[4][0] == 'W' && Real_state[0][1] == 'W' && Real_state[1][1] == 'W' && Real_state[2][1] == 'W'
            && Real_state[3][1] == 'W' && Real_state[4][1] == 'W' && ImageDeal[54].IsLeftFind == 'W'
            && ImageDeal[54].IsRightFind == 'W' && (special_cross > 10 || ImageStatus.OFFLine < 10))
    {
        for (i = 4; i < dir_stop - 5; i++)
        {
            if (DIR_1[i].LEFT == 1 && DIR_1[i + 1].LEFT == 1 && DIR_1[i + 2].LEFT == 1 && DIR_1[i + 3].LEFT == 1
                    && DIR_1[i + 4].LEFT == 1)
            {
                L_jump_point[1] = DIR_1_Y[i].LEFT;
                L_jump_point[0] = (int16) ImageDeal[L_jump_point[1]].LeftBorder;
                L_jump_flag = 1;
                break;
            }
            if (DIR_1[i].LEFT == 3 && DIR_1[i + 1].LEFT == 3 && DIR_1[i + 2].LEFT == 3)
            {
                break;
            }
            if ((DIR_1[i].LEFT == 3 || DIR_1[i].LEFT == 2) && (DIR_1[i + 1].LEFT == 3 || DIR_1[i + 1].LEFT == 2)
                    && (DIR_1[i + 2].LEFT == 3 || DIR_1[i + 2].LEFT == 2)
                    && (DIR_1[i + 3].LEFT == 3 || DIR_1[i + 3].LEFT == 2))
            {
                break;
            }
        }
        for (i = 4; i < dir_stop - 5; i++)
        {
            if (DIR_1[i].RIGHT == 3 && DIR_1[i + 1].RIGHT == 3 && DIR_1[i + 2].RIGHT == 3 && DIR_1[i + 3].RIGHT == 3
                    && DIR_1[i + 4].RIGHT == 3)
            {
                R_jump_point[1] = DIR_1_Y[i].RIGHT;
                R_jump_point[0] = (int16) ImageDeal[R_jump_point[1]].RightBorder;
                R_jump_flag = 1;
                break;
            }
            if (DIR_1[i].RIGHT == 1 && DIR_1[i + 1].RIGHT == 1 && DIR_1[i + 2].RIGHT == 1)
            {
                break;
            }
            if ((DIR_1[i].RIGHT == 1 || DIR_1[i].RIGHT == 2) && (DIR_1[i + 1].RIGHT == 1 || DIR_1[i + 1].RIGHT == 2)
                    && (DIR_1[i + 2].RIGHT == 1 || DIR_1[i + 2].RIGHT == 2)
                    && (DIR_1[i + 1].RIGHT == 1 || DIR_1[i + 1].RIGHT == 2))
            {
                break;
            }
        }
    }
    if (L_jump_flag && R_jump_flag)
    {
//        printf("\n\r(%d,%d),(%d,%d)\n\r", L_jump_point[0], L_jump_point[1], R_jump_point[0], R_jump_point[1]);
        ImageStatus.Road_type = Cross;
        cross_start();
    }

}
/****鍏冮敓鎴》鎷烽敓锟�*****/ //鍦嗛敓鏂ゆ嫹 鍗侀敓鏂ゆ嫹 閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷疯閫氶敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鐨嗙》鎷烽敓鏂ゆ嫹閿熸枻鎷峰厓閿熸埅鎲嬫嫹蹇�  閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰彧閿熸枻鎷蜂竴鐬敓鏂ゆ嫹
//閿熸枻鎷烽敓鏂ゆ嫹娌￠敓鏂ゆ嫹鐛炬枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楁唻鎷峰摕鑸滈敓鏂ゆ嫹閿熸枻鎷烽敓锟�
float Zebra_break_first = 70;
float special_S = 0;
void Element_Test (void)
{
    Zebra_Test();
    if (Zebra_start_flag == 1 && encoder_getval_cm() > Zebra_break_first)
    {
        Zebra_start_flag = 2;
        encoder_end();
    }
    if (Zebra_start_flag != 1)
    {
        if (ImageStatus.Road_type != Cross  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�0
        && ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque && ImageStatus.Road_type != Ramp
                && ImageStatus.Road_type != Cross_ture && ImageStatus.Road_type != LeftBarriar
                && ImageStatus.Road_type != RightBarriar && ImageStatus.Road_type != S_Conner)
        {
            ImageStatus.Road_type = 0;
        }
//
//    if (ImageStatus.Road_type != Cross  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�0
//    && ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque
//            && ImageStatus.Road_type != LeftBarriar && ImageStatus.Road_type != RightBarriar && Zebra_flag == 0
//
////            && ImageStatus.Road_type != Ramp && ImageStatus.Road_type != Cross_ture && eulerAngle.pitch > -3&& eulerAngle.pitch < 3
//                    )
//    {
//        Zebra_Test();
//    }
        if (ImageStatus.Road_type != Cross  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�0
        && ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque
                && ImageStatus.Road_type != LeftBarriar && ImageStatus.Road_type != RightBarriar && Zebra_flag == 0)
        {
            Barriar_Test();
        }
        if (ImageStatus.Road_type != Cross  //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�0
        && ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque && ImageStatus.Road_type != Ramp
                && ImageStatus.Road_type != Cross_ture && ImageStatus.Road_type != LeftBarriar
                && ImageStatus.Road_type != RightBarriar && special_S > 10)
        {
            S_Conner_Test();
        }
        if (ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque
                && ImageStatus.Road_type != LeftBarriar && ImageStatus.Road_type != RightBarriar
                && ImageStatus.Road_type != Ramp && ImageStatus.Road_type != S_Conner && Zebra_flag == 0)
        {
            Straightacc_Test();
        }
        else if (ImageStatus.Road_type != Ramp)
        {
            ImageStatus.straight_acc = 0;
            kp_servo = kp_servo_max;
            run = run_reduce;
            limit_pwm_palstance = palstance_max;
            SSSTOP;
        }
        if (ImageStatus.Road_type != Cross && ImageStatus.Road_type != LeftCirque
                && ImageStatus.Road_type != RightCirque && ImageStatus.Road_type != LeftBarriar
                && ImageStatus.Road_type != RightBarriar && Zebra_flag == 0

                && (!cross_flag || (cross_flag && cross_getval_cm() > 20))) // 鍦嗛敓鏂ゆ嫹閿熸枻鎷烽敓鐙¤璁规嫹閿熸枻鎷烽敓锟�
        {
            cross_end();
            Cross_Test();
        }

        if (ImageStatus.Road_type != Cross && ImageStatus.Road_type != LeftCirque
                && ImageStatus.Road_type != RightCirque // 鍦嗛敓鏂ゆ嫹閿熸枻鎷烽敓锟�
                && ImageStatus.Road_type != Cross_ture && ImageStatus.Road_type != LeftBarriar
                && ImageStatus.Road_type != RightBarriar && Zebra_flag == 0 && special_ring < 10
                && ImageStatus.Road_type != Ramp)

        {
            Element_Judgment_Left_Rings();  // 閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
            Element_Judgment_Right_Rings(); // 閿熸枻鎷峰渾閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
        }
        if (QEKF_INS.Pitch < Pitch_Ramp && ImageStatus.Road_type != Cross // 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�0
        && ImageStatus.Road_type != LeftCirque && ImageStatus.Road_type != RightCirque && ImageStatus.Road_type != Ramp
                && ImageStatus.Road_type != Cross_ture && ImageStatus.Road_type != LeftBarriar
                && ImageStatus.Road_type != RightBarriar)
        {
            ImageStatus.Road_type = Ramp; // 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓閾板嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�0
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
            //            gpio_set_level(Beep, 0);
            circle_add = 0;
        }
        else if (ImageStatus.Road_type == Ramp)
        {
            if (QEKF_INS.Pitch > Pitch_Ramp_1 && ramp_flag == 0)
            {
                ramp_flag = 1;
            }
            else if (QEKF_INS.Pitch < Pitch_Ramp_2 && QEKF_INS.Pitch > Pitch_Ramp_3 && ramp_flag)
            {
                ImageStatus.Road_type = 0;
                ramp_flag = 0;
            }
        }
    }
}

void Element_Handle ()
{
    SSSTOP;
//    ki_palstance = ki_palstance_normal;
    if (ImageFlag.image_element_rings != 0)
    {
//        ki_palstance = ki_palstance_ring;
        if (ImageStatus.Road_type == LeftCirque)
        {
            Element_Handle_Left_Rings();
        }
        else if (ImageStatus.Road_type == RightCirque)
        {
            Element_Handle_Right_Rings();
        }
    }
    else if (ImageFlag.image_element_barriars != 0)
    {
        if (ImageStatus.Road_type == LeftBarriar)
        {
            Element_Handle_Left_Barriars();
        }
        else if (ImageStatus.Road_type == RightBarriar)
        {
            Element_Handle_Right_Barriars();
        }
    }
    else if (ImageStatus.Road_type == Cross)
    {
        Element_Handle_Cross();
    }
}
uint8 all_stop_car[] = "9";
/****閿熸枻鎷烽敓鐣屽閿熸枻鎷�********/

void Stop_Test3 ()
{
    uint8 whitepoint = 0;
    for (uint8 i = 0; i < 80; i++)
    {

        if (Pixle[59][i])
        {
            whitepoint++;
            if (whitepoint < whilepoint_protect)
            {
                Stop_car_Flag = 1;
            }
        }
    }

}

void DrawLine ()  //閿熸枻鎷烽敓绔枻鎷�  閿熺煫纰夋嫹閿熸枻鎷�
{
    uint8 i;
    for (i = 59; i > ImageStatus.OFFLine; i--)
    {
        Pixle[i][ImageDeal[i].LeftBorder + 2] = 0;  //閿熺嫛璁规嫹閿熸枻鎷蜂綅閿熸枻鎷烽敓鑺傝璇ф嫹
        Pixle[i][ImageDeal[i].RightBorder - 2] = 0;
        Pixle[i][ImageDeal[i].Center] = 0;
    }
}
float k_add = 0;
/*****************閿熸枻鎷风晭鎱堫煉鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�**********************/
void GetDet ()
{
    float DetTemp = 0;
    int TowPoint = 0;
    float UnitAll = 0;
    if (ImageStatus.Road_type == Ramp)
    {
        TowPoint = (int) (TowPoint_ramp);
    }
    else if (ImageStatus.straight_acc)
    {
        TowPoint = (int) (TowPoint_std);
    }
    if ((ImageStatus.Road_type == RightCirque) && ImageStatus.CirqueOff == 'F')
    {
        TowPoint = (int) (TowPoint_2);
    }                                                               //鍦嗛敓鏂ゆ嫹鍓嶇灮
    else if ((ImageStatus.Road_type == LeftCirque) && ImageStatus.CirqueOff == 'F')
    {
        TowPoint = (int) (TowPoint_3);
    }                                                                //鍦嗛敓鏂ゆ嫹鍓嶇灮
    else if (ImageFlag.image_element_rings_flag == 1)
    {
        TowPoint = (int) (TowPoint_3);
    }
    else if (ImageFlag.image_element_rings_flag == 2)
    {
        TowPoint = (int) (TowPoint_2);                                                                      //鍦嗛敓鏂ゆ嫹鍓嶇灮
    }
    else
        TowPoint = ImageStatus.TowPoint;                                          //閿熷姭璁规嫹瓒婇敓鏂ゆ嫹鍓嶇灮瓒婇敓鏂ゆ嫹

    if (TowPoint < ImageStatus.OFFLine)
        TowPoint = ImageStatus.OFFLine + 1;

    if (TowPoint >= 49)
        TowPoint = 49;

    if ((TowPoint - 5) >= ImageStatus.OFFLine)
    {                                          //鍓嶇灮鍙栭敓鍊熷畾鍓嶇灮閿熸枻鎷烽敓瑙掑尅鎷烽敓鎺ユ拝鎷烽敓鏂ゆ嫹  閿熸枻鎷疯閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
        used_offline = TowPoint - 5;
        for (int Ysite = (TowPoint - 5); Ysite < TowPoint; Ysite++)
        {
            DetTemp = DetTemp + Weighting[TowPoint - Ysite - 1] * (ImageDeal[Ysite].Center);
            UnitAll = UnitAll + Weighting[TowPoint - Ysite - 1];
        }
        for (Ysite = (TowPoint + 5); Ysite > TowPoint; Ysite--)
        {
            DetTemp += Weighting[-TowPoint + Ysite - 1] * (ImageDeal[Ysite].Center);
            UnitAll += Weighting[-TowPoint + Ysite - 1];
        }
        DetTemp = (ImageDeal[TowPoint].Center + DetTemp) / (UnitAll + 1);

    }
    else if (TowPoint > ImageStatus.OFFLine)
    {
        used_offline = ImageStatus.OFFLine;
        for (Ysite = ImageStatus.OFFLine; Ysite < TowPoint; Ysite++)
        {
            DetTemp += Weighting[TowPoint - Ysite - 1] * (ImageDeal[Ysite].Center);
            UnitAll += Weighting[TowPoint - Ysite - 1];
        }
        for (Ysite = (TowPoint + TowPoint - ImageStatus.OFFLine); Ysite > TowPoint; Ysite--)
        {
            DetTemp += Weighting[-TowPoint + Ysite - 1] * (ImageDeal[Ysite].Center);
            UnitAll += Weighting[-TowPoint + Ysite - 1];
        }
        DetTemp = (ImageDeal[Ysite].Center + DetTemp) / (UnitAll + 1);
    }
    else if (ImageStatus.OFFLine < 49)
    {
        used_offline = ImageStatus.OFFLine;
        for (Ysite = (ImageStatus.OFFLine + 3); Ysite > ImageStatus.OFFLine; Ysite--)
        {
            DetTemp += Weighting[-TowPoint + Ysite - 1] * (ImageDeal[Ysite].Center);
            UnitAll += Weighting[-TowPoint + Ysite - 1];
        }
        DetTemp = (ImageDeal[ImageStatus.OFFLine].Center + DetTemp) / (UnitAll + 1);

    }
    else
        DetTemp = (float) ImageStatus.Det_True;   //閿熸枻鎷烽敓鏂ゆ嫹娴呴敓鏂ゆ嫹閿熺祮FFLine>50閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎻紮鎷疯敋閿熺嫛顐嫹閿熻锟�

    ImageStatus.Det_True = (int) DetTemp;                              //閿熸枻鎷锋椂閿熶茎鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鐙℃枻鎷烽敓閰电》鎷烽敓鐙☆偓鎷烽敓锟�
    ImageStatus.TowPoint_True = TowPoint;                                                //閿熸枻鎷锋椂閿熸枻鎷峰墠鐬�
    Slope_center_first_fit();
}
int my_sign (double x)
{
    if (x > 0)
        return 1;
    else if (x < 0)
        return -1;
    else
        return 0;
}

double fst (double x1, double x2, double u, double r, double h)
{

    double d = r * h; //h涓洪敓鍓胯鎷烽敓鏂ゆ嫹閿熸枻鎷�  r涓洪敓鏂ゆ嫹閿熸枻鎷风郴閿熸枻鎷烽敓鏂ゆ嫹r瓒婇敓鏂ゆ嫹閿熸枻鎷烽敓鍙嶆嫹閿熺殕鏂ゆ嫹鑼敓鏂ゆ嫹閿熻娇顫嫹閿熸枻鎷疯棔鍛曢敓鏂ゆ嫹閿熸枻鎷风棃閿熺嫛纰夋嫹閿熸枻鎷烽敓锟�
    double d0 = d * h;  //閿熸枻鎷蜂箣閿熸枻鎷峰井閿熸枻鎷烽敓鑴氱尨鎷疯秺骞抽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎻紮鎷烽敓鏂ゆ嫹閿熸枻鎷风閿燂拷
    double y = x1 - u + h * x2;
    double a0 = sqrt(d * d + 8 * r * fabs(y));
    double a, f;
    if (fabs(y) <= d0)
        a = x2 + y / h;
    else
        a = x2 + 0.5 * (a0 - d) * my_sign(y);

    if (fabs(a) <= d)
        f = -r * a / d;
    else
        f = -r * my_sign(a);
    return f;
}

double second_order_TD (double in)
{
    static double T = 0.002;
    static double r = 6000;
    static double h = 0.004;
    static double u, x1, x2, x1k, x2k;
    u = in;
    x1k = x1;
    x2k = x2;
    x1 = x1k + T * x2k;
    x2 = x2k + T * fst(x1k, x2k, u, r, h);
    return x1;
}
float new_offset = 0, offset = 0, last_offset = 0;
float mid_std = 39;
void offset_getting (void)
{
    offset = (ImageStatus.Det_True - mid_std + circle_add);
//    if (last_offset * offset > 0)
//    {
////        if (ImageStatus.Road_type != LeftBarriar && ImageStatus.Road_type != RightBarriar
////                && ImageStatus.Road_type != Ramp && ImageStatus.Road_type != LeftCirque
////                && ImageStatus.Road_type != RightCirque)
////        {
////            offset += k_add / 2;
////        }
//        if (k_add > 0.01)
//        {
//            if (ImageStatus.OFFLine <= 12)
//            {
//                offset += (ImageStatus.OFFLine - 3) / 15.0;
//            }
//            else
//            {
//                offset += (ImageStatus.OFFLine + 3) / 25.0;
//            }
//        }
//        else if (k_add < -0.01)
//        {
//            if (ImageStatus.OFFLine <= 12)
//            {
//                offset -= (ImageStatus.OFFLine - 3) / 15.0;
//            }
//            else
//            {
//
//                offset += (ImageStatus.OFFLine + 3) / 25.0;
//            }
//        }
//        else
//        {
//            if (offset > 1)
//            {
//                offset += (ImageStatus.OFFLine - 3) / 20.0;
//            }
//            else if (offset < -1)
//            {
//                offset -= (ImageStatus.OFFLine - 3) / 20.0;
//
//            }
//        }
//    }
//    if ((offset - last_offset > 5 || offset - last_offset < -5) && (offset > 10 || offset < -10))
//    {
//        offset = 0.6 * offset + 0.4 * last_offset;
//    }
    if (ImageFlag.image_element_rings_flag == 6 && (offset - last_offset > 10) || (offset - last_offset < -10))
    {
        offset = 0.4 * offset + 0.6 * last_offset;
    }

    if (ImageStatus.Road_type == S_Conner)
    {
        offset = offset * 0.5;
//        angle_bias = 0;
    }
    if (ImageStatus.Road_type == Ramp && ramp_flag == 0)
    {
        offset = offset * 0.5;
        if (offset > 25)
        {
            offset = 25;
        }
        else if (offset < -25)
        {
            offset = -25;

        }
    }

    new_offset = (float) second_order_TD((double) offset);
    test1 = offset;
    offset = new_offset;
    last_offset = offset;
    if (Zebra_start_flag == 1)
    {
        offset = 0;
//        angle_bias = 0;
    }
    return;
}
int sanchachazhi;

float Det = 0;
uint16 last_mid_point = 40;
uint8 ramp_flag = 0;
float Pitch_Ramp = -8;
float Pitch_Ramp_1 = 4;
float Pitch_Ramp_2 = 4;
float Pitch_Ramp_3 = -6;

void ImageProcess (void)
{
    compressimage();          //鍥鹃敓鏂ゆ嫹鍘嬮敓鏂ゆ嫹 0.6ms
    ImageStatus.OFFLine = 2;  //閿熸枻鎷烽敓琛楃鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓缁炵鎷烽敓鏂ゆ嫹閿熺煫纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
    ImageStatus.WhiteLine = 0;
    for (Ysite = 59; Ysite >= ImageStatus.OFFLine; Ysite--)
    {
        ImageDeal[Ysite].IsLeftFind = 'F';
        ImageDeal[Ysite].IsRightFind = 'F';
        ImageDeal[Ysite].LeftBorder = 0;
        ImageDeal[Ysite].RightBorder = 79;
        ImageDeal[Ysite].LeftTemp = 0;
        ImageDeal[Ysite].RightTemp = 79;
        ImageDeal[Ysite].close_LeftBorder = 0;
        ImageDeal[Ysite].close_RightBorder = 79;

    }                     //閿熺鏂ゆ嫹閿熸枻鎷烽敓琛楃枤浼欐嫹閿熺粸纭锋嫹閿燂拷
    Get01change_dajin();  //鍥鹃敓鏂ゆ嫹閿熻纰夋嫹閿燂拷    2.7ms
    DrawLinesFirst();     //閿熸枻鎷烽敓鐙″簳鎲嬫嫹      30us
    DrawLinesProcess();   //閿熺煫纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹  8us
    Sencond_DrawLinesProcess();
    Search_Border_OTSU(Pixle, LCDH, LCDW, LCDH - 2);   //58閿熸枻鎷蜂綅閿熸枻鎷烽敓鏂ゆ嫹
    /***鍏冮敓鏂ゆ嫹璇嗛敓鏂ゆ嫹*****/
    Element_Test();                   //5us
    /***鍏冮敓鏂ゆ嫹璇嗛敓鏂ゆ嫹*****/
    DrawExtensionLine();
    RouteFilter();        //閿熸枻鎷烽敓鏂ゆ嫹閿熷壙璇ф嫹骞抽敓鏂ゆ嫹 2us
    /***鍏冮敓鎴揪鎷烽敓鏂ゆ嫹*****/
    Element_Handle();  // 3us   +2
    /***鍏冮敓鎴揪鎷烽敓鏂ゆ嫹*****/
    GetDet();               //閿熸枻鎷峰彇閿熸枻鎷锋�佸墠鐬�  閿熸枻鎷烽敓鎻》鎷烽敓鏂ゆ嫹鍥鹃敓鏂ゆ嫹鍋忛敓鏂ゆ嫹 3us
    last_mid_point = (uint16) (ImageDeal[ImageStatus.OFFLine + 5].Center);
    ImageStatus.Foresight = ((((ImageDeal[ImageStatus.OFFLine + 1].Center) + (ImageDeal[ImageStatus.OFFLine + 2].Center)
            + (ImageDeal[ImageStatus.OFFLine + 3].Center)) / 3) - 40);

    ImageStatus.Det_all = (ImageStatus.Foresight + 40) - ImageDeal[54].Center;
    ImageStatus.Det_all_k = (float) (ImageStatus.Det_all) / (ImageStatus.OFFLine + 2 - 54) * 30;
    ImageStatus.Foresight = abs(ImageStatus.Foresight);
//    Search_LEFT(400, ImageDeal[59].LeftBorder, 59);
//    printf("\n\rstart:%d\n\r", data_stastics_l);
//    for (int16 i = 0; i < data_stastics_l; i++)
//    {
//        printf("%d,", DIR_2[i].LEFT);
//    }
}

