/*
 * encoder.h
 *
 *  Created on: 2024Äê5ÔÂ9ÈÕ
 *      Author: HUAWEI
 */
#include "zf_common_headfile.h"
#ifndef CODE_ENCODER_H_
#define CODE_ENCODER_H_

extern int encoder_flag, encoder_printf, cross_flag,str_flag;
extern int64_t encoder_int, cross_int,str_int;
extern float encoder_cm, cross_cm;
extern float str_cm, str_k;


extern int QEKF_INS_flag;
extern float QEKF_INS_float, QEKF_INS_degree, degree_turn;

void encoder_start(void);
float encoder_getval_cm(void);
float encoder_end(void);

void cross_start(void);
float cross_getval_cm(void);
float cross_end(void);

void QEKF_INS_start(void);
float QEKF_INS_getval_degree(void);
float QEKF_INS_end(void);

void str_start(void);
float str_getval_cm(void);
float str_end(void);


#endif /* CODE_ENCODER_H_ */
