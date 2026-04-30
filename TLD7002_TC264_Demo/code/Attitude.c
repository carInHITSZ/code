/*
 * describe: Attitude calculate
 * 姿态解算部分
 * @description: 只需要把陀螺仪的数据传入即可，特别注意角速度 加速度单位要换算！！
 * Attitude_Calculate是姿态解算函数 可以赛循环也可以塞中断 ，Attitude_Init陀螺仪飘初始化
 * 这个代码是给大家的姿态解算的DEMO 基于imu660ra，如果是使用660ra仅需要把963数字全部换成660就行
 * 例如 gyroscope[0]= imu660ra_gyro_transition(imu660ra_gyro_x)* 0.0174533f; 只要把963数字改成660ra即可
 */
#include "Attitude.h"
#include "zf_common_headfile.h"
#include "QuaternionEKF.h"
//==================================================================================================
#define gyroscope_threshold 5
//--------------------------------------------------------------------------------------
float gyroscopeOffset[3] = {0.0f, 0.0f, 0.0f};    //gyro
float gyroscope[3] ={0.0f,0.0f,0.0f};                   //gyro
float accelerometer[3] = {0.0f, 0.0f, 0.0f};    //acc
//--------------------------------------------------------------------------------------
//===================================================================================================

void Attitude_Init(void)
{
    IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0.001f,0); //ekf初始化
    //===========================================================================================
    for (int i = 0; i < 2000; ++i) { //0漂初始化
//        imu660ra_get_gyro();
        imu660ra_get_gyro();
        //0漂初始化阈值判断
        if(fabsf(gyroscope[0])+ fabsf(gyroscope[1])+ fabsf(gyroscope[2])<gyroscope_threshold) {
            gyroscopeOffset[0] += imu660ra_gyro_transition(imu660ra_gyro_x)* 0.0174533f;
            gyroscopeOffset[1] += imu660ra_gyro_transition(imu660ra_gyro_y)* 0.0174533f;
            gyroscopeOffset[2] += imu660ra_gyro_transition(imu660ra_gyro_z)* 0.0174533f;
        }
        else
        {
            i--;   //超过阈值了再取一次
        }
        system_delay_ms(1);  //按照自己的采样率来取值 demo是1000Hz即1ms中断
    }
    gyroscopeOffset[0]/=2000;
    gyroscopeOffset[1]/=2000;
    gyroscopeOffset[2]/=2000;

    //=============================================================================================

}

#define cheat_define 0.0016
void Attitude_Calculate(void)
{
    // replace this with actual gyroscope data in degrees/s
    imu660ra_get_acc();
    imu660ra_get_gyro();
    //imu660ra_get_mag();
    //要进行角度切换 具体角度单位请看下面的注释
    gyroscope[0]= imu660ra_gyro_transition(imu660ra_gyro_x)* 0.0174533f;
    gyroscope[1]= imu660ra_gyro_transition(imu660ra_gyro_y)* 0.0174533f;
    gyroscope[2]= imu660ra_gyro_transition(imu660ra_gyro_z)* 0.0174533f;
    accelerometer[0]=imu660ra_acc_transition(imu660ra_acc_x);
    accelerometer[1]=imu660ra_acc_transition(imu660ra_acc_y);
    accelerometer[2]=imu660ra_acc_transition(imu660ra_acc_z);
    //=================================================================
    //减去零飘 如果前面零漂去了 这里记得注释
    gyroscope[0]-= gyroscopeOffset[0];
    gyroscope[1]-= gyroscopeOffset[1];
    gyroscope[2]-= gyroscopeOffset[2];

    //作弊 cheat_define 是阈值 小于多少值就置零，你也可以把下面三句话注释了
    //===============================================================================
    if(fabsf(gyroscope[0])<cheat_define) gyroscope[0]=0;
    if(fabsf(gyroscope[1])<cheat_define) gyroscope[1]=0;
    if(fabsf(gyroscope[2])<cheat_define) gyroscope[2]=0;
    //===================================================================================
    //gpio_set_level(P20_0,1); 我拿来测运算时间的
//===================================================================================================
    IMU_QuaternionEKF_Update(gyroscope[0],gyroscope[1],gyroscope[2],accelerometer[0],accelerometer[1],accelerometer[2]);
   // gpio_set_level(P20_0,0);
    //姿态解算 后三位为磁力计，一般不用就行 填0
}
