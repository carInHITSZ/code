/*
 * PID.c
 *
 *  Created on: 2024年7月23日
 *      Author: HUAWEI
 */
#include "zf_common_headfile.h"

float kp_offset = 0.80, ki_offset = 0.0, kd_offset = 0.5, offset_range = 5, kp_offset_max = 0.95, kp_offset_min = 0.85;
float kp_offset_angle = 0.65, ki_offset_angle = 0.0, kd_offset_angle = 2.0;
float kp_speed = 0.0, ki_speed = 0, kd_speed = 0.0;
float kp_angle = 4.0, kp_angle_left = 4.0, kp_angle_right = 5.2, kp_angle_min = 4.0, kp_angle_max = 6.5, ki_angle = 0.0, kd_angle = 16.0, angle_range = 30;
float kp_palstance = 52.5, ki_palstance = 1.57, kd_palstance = 0.0;
//float ki_palstance_normal = 1.65, ki_palstance_ring = 1.65;
float kp_servo = 3.0, ki_servo = 0, kd_servo = 10, servo_range = 5, kp_servo_max = 3.0, kp_servo_on = 2.0,
        kp_servo_speed = 2.0;
float kp_servo_balance = 0.0, ki_servo_balance = 0.0, kd_servo_balance = 0.0;
float run = 4000, run_servo = 6000, run_normal = 4000, run_reduce = 4000, kp_run = 30.0, ki_run = 5, kd_run = 4;
float kp_FFC = 0, kd_FFC = 0;

float error_offset = 0, error_offset_last = 0, integral_offset = 0, forward_offset = 0, pwm_offset = 0,
        pwm_offset_last = 0, pwm_offset_last_last = 0, error_FFC = 0, error_FFC_FFC = 0, limit_offset = 1000,
        offset_limit = 8, test_offset = 0;
float error_offset_angle = 0, error_offset_angle_last = 0, integral_offset_angle = 0, pwm_offset_angle = 0,
        pwm_offset_angle_last = 0, limit_offset_angle = 0;
float error_speed = 0, error_speed_last = 0, integral_speed = 0, pwm_speed = 0, pwm_speed_last = 0, limit_speed = 0;
float error_angle = 0, error_angle_last = 0, integral_angle = 0, pwm_angle = 0, pwm_angle_last = 0, limit_angle = 500,
        test_angle = 0;
float error_palstance = 0, error_palstance_last = 0, error_palstance_last_last = 0, detla_pwm_palstance = 0,
        pwm_palstance = 0, pwm_palstance_last = 0, limit_pwm_palstance = 5800, palstance_on = 5300,
        palstance_speed = 4800, palstance_servo = 3800, palstance_max = 5800;
float error_servo = 0, error_servo_last = 0, integral_servo = 0, pwm_servo = 0, pwm_servo_last = 0, limit_pwm_servo =
        80, servo_limit = 8, test_servo = 0;
float error_servo_balance = 0, error_servo_balance_last = 0, integral_servo_balance = 0, pwm_servo_balance = 0;
float fdb_run = 0, error_run = 0, error_run_last = 0, error_run_last_last = 0, detla_pwm_run = 0, pwm_run = 0,
        pwm_run_last = 0, pwm_start_run = 0, limit_pwm_run = 1800, fdb_speed = 0, encoder_c = 0, encoder_c_last = 0,
        encoder_d = 0, encoder_d_last = 0;

int count_offset = 0, count_speed = 0, count_angle = 0, count_palstance = 0, flag_turn = 0, roll_flag = 0;
float kp_dl = 4000, kp_dh = 0.65, kp_offset_angle_limit = 0.80;
float kp_pl = 4000, kp_ph = 1.0, kp_pr = 0.02, kp_offset_limit = 2.0, kp_offset_limit_normal = 2.0, turn = 180
        / 3.1415926, run_angle = 30, run_offset = 0.0, run_offset_normal = 0.0, run_offset_rings = 0.0, offset_angle =
        0.001, run_speed = 5000, run_on = 4500;
float angle_bias = -0.8;
float test1 = 0, test2 = 0, temp_palstance = 0;
float Zebra_end_distance = 100;
uint8 check_zebra_flag = 0;
uint8 stop_flag = 0;

void Zebra_stop_check ()
{
    if (Zebra_flag && !check_zebra_flag)
    {
        check_zebra_flag++;
        encoder_start();
    }
    if (check_zebra_flag)
    {
        if (encoder_getval_cm() >= Zebra_end_distance)
        {
            encoder_end();
            stop_flag = 1;
        }
        dot_matrix_screen_show_string("END");
    }
}
void ANGLE_BIAS ()
{
    if (ImageStatus.OFFLine > 4)
    {
        if (offset > 10 && last_offset > 10)
        {
            angle_bias = 1;
        }
        else if (offset < -10 && last_offset < -10)
        {
            angle_bias = -1;
        }
        else
        {
            angle_bias = 0;
        }
    }
}

void PID_all (float pwm, float fdb, float speed, float offset_forward, float servo)
{
//    ANGLE_BIAS();
//    offset = 30;
//    printf("%.4f,%.4f,%d\n\r", new_offset, test1, ImageStatus.Road_type);
//    count_offset++;
    count_speed++;
    count_angle++;
//    count_palstance++;
//    if (count_offset == 10)
//    {
//        PID_offset(speed);
//        count_offset = 0;
//    }
    if (count_speed == 4)
    {
//        PID_offset();

        PID_offset(speed);
        PID_speed(offset_forward);
//        PID_offset_angle();
//        PID_offset(speed);
        count_speed = 0;
    }
    if (count_angle == 2)
    {
        PID_angle(fdb);
        count_angle = 0;
    }
//    if(count_palstance == 2)
//    {
    PID_palstance(pwm, fdb, servo);
//        count_palstance = 0;
//    }
//    PID_offset(speed);
//    PID_speed(offset_forward);
//    PID_angle(fdb);
    Zebra_stop_check();

}

void PID_offset (float speed)
{
    error_offset = offset;
//    kp_offset = fabs(offset) * kp_pr + kp_ph;         //(error_offset * error_offset) / kp_pl +
//    if (kp_offset > kp_offset_limit)
//        kp_offset = kp_offset_limit;
//    test2 = kp_offset;
    integral_offset += error_offset;
    if (integral_offset > limit_offset)
        integral_offset = limit_offset;
    if (integral_offset < -limit_offset)
        integral_offset = -limit_offset;
//    if((error_offset > offset_range || error_offset < -offset_range) && (k_add > 0.5 || k_add < -0.5))
//    {
//        test_offset = kp_offset_max * error_offset - (kp_offset_max - kp_offset_min) * offset_range;
//    }
//    else
//    {
//        test_offset = kp_offset_min * error_offset;
//    }
    pwm_offset = kp_offset * error_offset + ki_offset * integral_offset
            + kd_offset * (error_offset - error_offset_last);
//    if (flag_turn == 1)
//    {
//        pwm_offset = kp_offset * error_offset + ki_offset * integral_offset + kd_offset * QEKF_INS.Gyro[2] * turn / 100;
//    }
//    else
//    {
//        pwm_offset = kp_offset * error_offset + ki_offset * integral_offset
//                + kd_offset * (error_offset - error_offset_last);
//    }
    forward_offset = kp_FFC * (pwm_offset - pwm_offset_last)
            + kd_FFC * (pwm_offset - 2 * pwm_offset_last + pwm_offset_last_last);
//    pwm_offset = kp_offset * error_offset + ki_offset * integral_offset
//            + kd_offset * QEKF_INS.Roll;
//    printf("%f,%f\n\r",error_offset - error_offset_last,QEKF_INS.Gyro[2] * turn);
//    printf("%f,%f\n\r",error_offset - error_offset_last,QEKF_INS.Roll);
    error_offset_last = error_offset;
//    pwm_offset = 0.7 * pwm_offset + 0.3 * pwm_offset_last;
    error_FFC = pwm_offset - pwm_offset_last;
    error_FFC_FFC = pwm_offset - 2 * pwm_offset_last + pwm_offset_last_last;
    pwm_offset_last_last = pwm_offset_last;
    pwm_offset_last = pwm_offset;
}

void PID_offset_angle (void)
{
    error_offset_angle = offset;
//    kp_offset_angle = (sqrt(error_offset_angle)) / kp_dl + kp_dh;
//    if (kp_offset_angle > kp_offset_angle_limit)
//        kp_offset_angle = kp_offset_angle_limit;
    integral_offset_angle += error_offset_angle;
    if (integral_offset_angle > limit_offset_angle)
        integral_offset_angle = limit_offset_angle;
    if (integral_offset_angle < -limit_offset_angle)
        integral_offset_angle = -limit_offset_angle;
//    pwm_offset_angle = kp_offset_angle * error_offset_angle + ki_offset_angle * integral_offset_angle
//            + kd_offset_angle * (error_offset_angle - error_offset_angle_last);
    pwm_offset_angle = kp_offset_angle * error_offset_angle + ki_offset_angle * integral_offset_angle
            + kd_offset_angle * QEKF_INS.Gyro[2] * turn / 100;
//    pwm_offset_angle = 0.7 * pwm_offset_angle + 0.3 * pwm_offset_angle_last;
    error_offset_angle_last = error_offset_angle;
    pwm_offset_angle_last = pwm_offset_angle;
}

void PID_speed (float offset_forward)
{
//    error_speed = pwm_offset - QEKF_INS.Gyro[2] * turn / 6;
//    error_speed =  pwm_offset - QEKF_INS.Gyro[2] * turn;
    error_speed = QEKF_INS.Gyro[2] * turn / 100;
    integral_speed += error_speed;
    if (integral_speed > limit_speed)
        integral_speed = limit_speed;
//    kd_speed = (offset * offset) / kd_dl + kd_dh;
//    if(kd_speed > 0.50)
//        kd_speed = 0.50;
    pwm_speed = offset_forward + kp_speed * error_speed + ki_speed * integral_speed
            + kd_speed * (error_speed - error_speed_last);
//    pwm_speed = kp_speed * error_speed + ki_speed * integral_speed + kd_speed * QEKF_INS.Roll;
//    printf("%f,%f\n\r",error_speed - error_speed_last,QEKF_INS.Roll);
    error_speed_last = error_speed;
//    pwm_speed = 0.7 * pwm_speed + 0.3 * pwm_speed_last;
    pwm_speed_last = pwm_speed;
}

void PID_angle (float fdb)
{
    error_angle = pwm_offset - QEKF_INS.Roll - angle_bias; // + pwm_offset * 0.02         //fdb * pwm_offset_angle * 0.002 +
    integral_angle += error_angle;
    if (integral_angle > limit_angle)
        integral_angle = -limit_angle;
    if (integral_angle < -limit_angle)
        integral_angle = -limit_angle;
//    if(error_angle > angle_range)
//        error_angle = angle_range;
//    if(error_angle < -angle_range)
//        error_angle = -angle_range;
//    if(error_angle > 10)
//        kp_angle = kp_angle_right;
//    if(error_angle < -10)
//        kp_angle = kp_angle_left;
//    if(error_angle > -10 && error_angle < 10)
//        kp_angle = kp_angle_min;
//    pwm_angle = kp_angle * error_angle + ki_angle * integral_angle
//            + kd_angle * (error_angle - error_angle_last);
    if (error_angle < angle_range && error_angle > -angle_range)
    {
        test_angle = kp_angle_min * error_angle;
//        pwm_angle = kp_angle_min * error_angle + ki_angle * integral_angle + kd_angle * QEKF_INS.Gyro[0] * turn / 100;
    }
    else
    {
        test_angle = test_angle - (kp_angle_max - kp_angle_min) * angle_range;
//        pwm_angle = kp_angle_max * error_angle - (kp_angle_max - kp_angle_min) * angle_range + ki_angle * integral_angle + kd_angle * QEKF_INS.Gyro[0] * turn / 100;
    }
    pwm_angle = test_angle + ki_angle * integral_angle + kd_angle * QEKF_INS.Gyro[0] * turn / 100;
//    pwm_angle = offset_forward + kp_angle * error_angle + ki_angle * integral_angle
//                + kd_angle * QEKF_INS.Gyro[0];
//    printf("%f,%f\n\r",error_angle - error_angle_last,QEKF_INS.Gyro[0] * turn);
    error_angle_last = error_angle;
//    pwm_angle = 0.7 * pwm_angle + 0.3 * pwm_angle_last;
    pwm_angle_last = pwm_angle;
}

void PID_palstance (float pwm, float fdb, float servo)
{
    error_palstance = pwm_angle - QEKF_INS.Gyro[0] * turn;      //  + fdb * fdb * pwm_servo * 0.0005
//    error_palstance = pwm_angle - QEKF_INS.Gyro[0];
    detla_pwm_palstance = kp_palstance * (error_palstance - error_palstance_last) + ki_palstance * error_palstance
            + kd_palstance * (error_palstance - 2 * error_palstance_last + error_palstance_last_last);
    error_palstance_last_last = error_palstance_last;
    error_palstance_last = error_palstance;
    pwm_palstance += detla_pwm_palstance;
//    pwm_palstance = 0.7 * pwm_palstance + 0.3 * pwm_palstance_last;
    pwm_palstance_last = pwm_palstance;
//    pwm_palstance = 0;
    if (pwm_palstance > limit_pwm_palstance)
        pwm_palstance = limit_pwm_palstance;
    if (pwm_palstance < -limit_pwm_palstance)
        pwm_palstance = -limit_pwm_palstance;
    if (pwm_palstance >= 0)
    {
        gpio_set_level(P21_2, 0);
        if (pwm_palstance + pwm > 9800)
        {
            pwm_set_duty(ATOM0_CH1_P21_3, 9800);
        }
        else
        {
            pwm_set_duty(ATOM0_CH1_P21_3, pwm_palstance + pwm);
        }
        if (pwm_palstance - pwm >= 0)
        {
            gpio_set_level(P21_4, 0);
            if (pwm_palstance - pwm > 9800)
            {
                pwm_set_duty(ATOM0_CH3_P21_5, 9800);
            }
            else
            {
                pwm_set_duty(ATOM0_CH3_P21_5, pwm_palstance - pwm);
            }
        }
        if (pwm_palstance - pwm < 0)
        {
            gpio_set_level(P21_4, 1);
            if (-pwm_palstance + pwm > 9800)
            {
                pwm_set_duty(ATOM0_CH3_P21_5, 9800);
            }
            else
            {
                pwm_set_duty(ATOM0_CH3_P21_5, -pwm_palstance + pwm);
            }
        }
    }
    if (pwm_palstance < 0)
    {
        if (-pwm_palstance - pwm >= 0)
        {
            gpio_set_level(P21_2, 1);
            if (-pwm_palstance - pwm > 9800)
            {
                pwm_set_duty(ATOM0_CH1_P21_3, 9800);
            }
            else
            {
                pwm_set_duty(ATOM0_CH1_P21_3, -pwm_palstance - pwm);
            }
        }
        if (-pwm_palstance - pwm < 0)
        {
            gpio_set_level(P21_2, 0);
            if (pwm_palstance + pwm > 9800)
            {
                pwm_set_duty(ATOM0_CH1_P21_3, 9800);
            }
            else
            {
                pwm_set_duty(ATOM0_CH1_P21_3, pwm_palstance + pwm);
            }
        }
        gpio_set_level(P21_4, 1);
        if (-pwm_palstance + pwm > 9800)
        {
            pwm_set_duty(ATOM0_CH3_P21_5, 9800);
        }
        else
        {
            pwm_set_duty(ATOM0_CH3_P21_5, -pwm_palstance + pwm);
        }
    }
    if (roll_flag == 1)
    {
        pwm_set_duty(ATOM1_CH1_P33_9, 822 * 6);         //650 775 900
        gpio_set_level(P21_2, 0);
        gpio_set_level(P21_4, 0);
        pwm_set_duty(ATOM0_CH1_P21_3, 0);
        pwm_set_duty(ATOM0_CH3_P21_5, 0);
    }
}

void PID_servo (void)
{
//    error_servo = QEKF_INS.Roll;
    error_servo = offset;
    integral_servo += error_servo;
//    if((error_servo > servo_range || error_servo < -servo_range) && (k_add > 0.5 || k_add < -0.5))
//    {
//        test_servo = kp_servo_max * error_servo - (kp_servo_max - kp_servo_min) * servo_range;
//    }
//    else
//    {
//        test_servo = kp_servo_min * error_servo;
//    }
    pwm_servo = kp_servo * error_servo + ki_servo * integral_servo + kd_servo * (error_servo - error_servo_last);
//    pwm_servo = kp_servo * error_servo + ki_servo * integral_servo
//            + kd_servo * QEKF_INS.Gyro[2] * turn / 100;
//    printf("%f,%f\n\r",error_servo - error_servo_last,QEKF_INS.Gyro[2] * turn);
    error_servo_last = error_servo;
//    pwm_servo = 0.7 * pwm_servo + 0.3 * pwm_servo_last;
    pwm_servo_last = pwm_servo;
    if (pwm_servo > limit_pwm_servo)
        pwm_servo = limit_pwm_servo;
    if (pwm_servo < -limit_pwm_servo)
        pwm_servo = -limit_pwm_servo;
    if (pwm_servo > -servo_limit && pwm_servo < servo_limit)
        pwm_servo = 0;

    error_servo_balance = QEKF_INS.Roll + angle_bias;
    integral_servo_balance += error_servo_balance;
    pwm_servo_balance = kp_servo_balance * error_servo_balance + ki_servo_balance * integral_servo_balance
            + kd_servo_balance * QEKF_INS.Gyro[0] * turn / 100;
//    pwm_servo_balance = kp_servo_balance * error_servo_balance + ki_servo_balance * integral_servo_balance
//            + kd_servo_balance * (error_servo_balance - error_servo_balance_last);
    error_servo_balance_last = error_servo_balance;
    if (pwm_servo_balance > limit_pwm_servo)
        pwm_servo_balance = limit_pwm_servo;
    if (pwm_servo_balance < -limit_pwm_servo)
        pwm_servo_balance = -limit_pwm_servo;
    if (ImageStatus.Road_type == Ramp)
    {
        pwm_servo_balance = 0;
        if (ramp_flag) //下坡
        {
//            limit_pwm_run = 1800;
            run = run_normal;
            limit_pwm_palstance = palstance_max;
//            pwm_servo = 0;
            SSSTOP;
//            limit_pwm_run = 2800;
//            limit_pwm_palstance = 4000;
        }
        else //上坡
        {
//            limit_pwm_run = 1800;
            run = run_servo;
            limit_pwm_palstance = palstance_servo;
//            pwm_servo = 0;
            SSSTOP;
//            limit_pwm_run = 1300;
//            limit_pwm_palstance = 4000;
        }
    }
//    pwm_set_duty(ATOM1_CH1_P33_9, 815 + 0 - pwm_servo_balance);
//    if((pwm_servo + pwm_servo_balance) < limit_pwm_servo && (pwm_servo + pwm_servo_balance) > -limit_pwm_servo)
    pwm_set_duty(ATOM1_CH1_P33_9, (822 + pwm_servo) * 6);
//    else
//        pwm_set_duty(ATOM1_CH1_P33_9, (775 + limit_pwm_servo) * 6);
//    pwm_set_duty(ATOM1_CH1_P33_9, 815 + 0);
}

void PID_run (void)
{
    pwm_run = run - run_angle * fabs(offset);
//    error_run = run - fdb_run - run_angle * fabs(QEKF_INS.Roll + angle_bias); // - run_angle * fabs(QEKF_INS.Roll + angle_bias)
//    detla_pwm_run = kp_run * (error_run - error_run_last) + ki_run * error_run
//            + kd_run * (error_run - 2 * error_run_last + error_run_last_last);
//    error_run_last_last = error_run_last;
//    error_run_last = error_run;
//    pwm_run += detla_pwm_run;
////    pwm_run = 0.7 * pwm_run + 0.3 * pwm_run_last;
//    pwm_run_last = pwm_run;
//    if (pwm_run > limit_pwm_run)
//        pwm_run = limit_pwm_run;
}

void fdb(int encoder_a, int encoder_b)
{
    fdb_run = (encoder_b - encoder_a) / 2.0;
    fdb_speed = (encoder_b + encoder_a) / 2.0;
}
