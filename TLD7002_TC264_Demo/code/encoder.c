/*
 * encoder.c
 *
 *  Created on: 2024Äê5ÔÂ9ÈÕ
 *      Author: HUAWEI
 */

#include "zf_common_headfile.h"
int encoder_flag = 0, encoder_printf = 0, cross_flag = 0,str_flag=0;
int64_t encoder_int = 0, cross_int = 0,str_int=0;
float encoder_cm = 0, encoder_k = 18;
float cross_cm = 0, cross_k = 18;
float str_cm = 0, str_k = 18;

int QEKF_INS_flag = 0;
float QEKF_INS_float = 0, QEKF_INS_degree = 0, degree_turn = 360;

void encoder_start(void)
{
    encoder_cm = 0;
    encoder_int = 0;
    encoder_flag = 1;
}

float encoder_getval_cm(void)
{
    if(encoder_flag == 1)
    {
        encoder_cm = encoder_int / encoder_k;
        return encoder_cm;
    }
    else
    {
        encoder_cm = 0;
        return 0;
    }
}

float encoder_end(void)
{
    encoder_cm = encoder_int / encoder_k;
    encoder_int = 0;
    encoder_flag = 0;
    return encoder_cm;
}

void cross_start(void)
{
    cross_cm = 0;
    cross_int = 0;
    cross_flag = 1;
}

float cross_getval_cm(void)
{
    if(cross_flag == 1)
    {
        cross_cm = cross_int / cross_k;
        return cross_cm;
    }
    else
    {
        cross_cm = 0;
        return 0;
    }
}

float cross_end(void)
{
    cross_cm = cross_int / cross_k;
    cross_int = 0;
    cross_flag = 0;
    return cross_cm;
}
void str_start(void)
{
    str_cm = 0;
    str_int = 0;
    str_flag = 1;
}

float str_getval_cm(void)
{
    if(str_flag == 1)
    {
        str_cm = str_int / str_k;
        return str_cm;
    }
    else
    {
        str_cm = 0;
        return 0;
    }
}

float str_end(void)
{
    str_cm = str_int / str_k;
    str_int = 0;
    str_flag = 0;
    return str_cm;
}

void QEKF_INS_start(void)
{
    QEKF_INS_degree = 0;
    if(QEKF_INS.Yaw < 0)
    {
        QEKF_INS_float = QEKF_INS.Yaw + degree_turn;
    }
    if(QEKF_INS.Yaw > 0)
    {
        QEKF_INS_float = QEKF_INS.Yaw;
    }
    QEKF_INS_flag = 1;
}

float QEKF_INS_getval_degree(void)
{
    if(QEKF_INS_flag == 1)
    {
        if(QEKF_INS.Yaw < 0)
        {
            QEKF_INS_degree = QEKF_INS.Yaw + degree_turn - QEKF_INS_float;
        }
        if(QEKF_INS.Yaw > 0)
        {
            QEKF_INS_degree = QEKF_INS.Yaw - QEKF_INS_float;
        }
        return QEKF_INS_degree;
    }
    else
    {
        QEKF_INS_degree = 0;
        return 0;
    }
}

float QEKF_INS_end(void)
{
    if(QEKF_INS.Yaw < 0)
    {
        QEKF_INS_degree = QEKF_INS.Yaw + degree_turn - QEKF_INS_float;
    }
    if(QEKF_INS.Yaw > 0)
    {
        QEKF_INS_degree = QEKF_INS.Yaw - QEKF_INS_float;
    }
    QEKF_INS_flag = 0;
    return QEKF_INS_degree;
}
