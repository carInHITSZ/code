/*
 * PID.h
 *
 *  Created on: 2024Äê7ÔÂ23ÈÕ
 *      Author: HUAWEI
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_
#include "zf_driver_pwm.h"
#include "zf_driver_encoder.h"

void PID_all(float pwm, float fdb, float speed, float offset_forward, float servo);
extern uint8 stop_flag;
extern int count_offset, count_speed, count_angle, flag_turn, roll_flag, count_palstance;
extern float kp_pl, kp_pr, kp_ph, kp_offset_limit, kp_dl, kp_dh, kp_offset_angle_limit, turn, run_angle, run_offset,
run_offset_normal, run_offset_rings, offset_angle, run_speed, run_on;
extern float angle_bias;
extern float kp_offset_limit_normal;
extern float test1, test2, temp_palstance;
extern float Zebra_end_distance;

void PID_offset(float speed);
extern float kp_offset, ki_offset, kd_offset, kp_FFC, kd_FFC, offset_range, kp_offset_max, kp_offset_min;
extern float error_offset, error_offset_last, integral_offset, forward_offset, test_offset,
pwm_offset, pwm_offset_last, pwm_offset_last_last, error_FFC, error_FFC_FFC, limit_offset;

void PID_offset_angle(void);
extern float kp_offset_angle, ki_offset_angle, kd_offset_angle;
extern float error_offset_angle, error__offset_angle_last, integral_offset_angle, pwm_offset_angle,
pwm_offset_angle_last, limit_offset_angle;

void PID_speed(float offset_forward);
extern float kp_speed, ki_speed, kd_speed;
extern float error_speed, error_speed_last, integral_speed, pwm_speed, pwm_speed_last, limit_speed;

void PID_angle(float fdb);
extern float kp_angle, kp_angle_left, kp_angle_right, kp_angle_min, kp_angle_max, ki_angle, kd_angle, angle_range, test_angle;
extern float error_angle, error_angle_last, integral_angle, pwm_angle, pwm_angle_last;

void PID_palstance(float pwm, float fdb, float servo);
extern float kp_palstance, ki_palstance, kd_palstance;
extern float error_palstance, error_palstance_last, error_palstance_last_last, detla_pwm_palstance,
pwm_palstance, pwm_palstance_last, limit_pwm_palstance, palstance_on, palstance_max, palstance_speed, palstance_servo;
extern float ki_palstance_normal,ki_palstance_ring;

void PID_servo(void);
extern float kp_servo, ki_servo, kd_servo, kp_servo_balance, ki_servo_balance, kd_servo_balance, servo_range, kp_servo_max, kp_servo_on, kp_servo_speed;
extern float error_servo, error_servo_last, integral_servo, pwm_servo, pwm_servo_last, limit_pwm_servo, servo_limit, test_servo;
extern float error_servo_balance, error_servo_balance_last, integral_servo_balance, pwm_servo_balance;

void PID_run(void);
void fdb(int encoder_a, int encoder_b);
extern float kp_run, ki_run, kd_run;
extern float run, run_servo, run_normal, run_reduce, fdb_run, error_run, error_run_last, error_run_last_last, detla_pwm_run,
pwm_run, pwm_run_last, pwm_start_run, limit_pwm_run, fdb_speed, encoder_c, encoder_c_last, encoder_d, encoder_d_last;




#endif /* CODE_PID_H_ */
