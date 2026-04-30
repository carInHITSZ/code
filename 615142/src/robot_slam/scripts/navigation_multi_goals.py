#!/usr/bin/env python
# -*- coding: utf-8 -*-
#coding: utf-8

import rospy
import math
import os
import actionlib
from actionlib_msgs.msg import *
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal
from nav_msgs.msg import Path
from geometry_msgs.msg import Twist, PoseWithCovarianceStamped, Point, Vector3
from tf_conversions import transformations
from math import pi
from std_msgs.msg import String
from std_msgs.msg import Int32
from ar_track_alvar_msgs.msg import AlvarMarker, AlvarMarkers
from rospy import Time,Duration
from sensor_msgs.msg import LaserScan
import sys
import re
import numpy as np
reload(sys)
sys.setdefaultencoding('utf-8')
passed_recgonition = [0,0,0,0]
counter = 0 #计数器
is_0_or_180 = 0 #指示小车目前是0度还是180度，0表示0度，1表示180度
point_msg = 255

rotate_180_z = 2.8   #move_rotate_180()中的强制旋转z速度

#向右固定旋转90度
rotate_right_z = -3
rotate_right_t = 1.3
start_time_for_rotate_pid=0
rotate_pid_flag = 0
rotate_angle = 540
rotate_angle_pid_k = 80
rotate_delta_angle = 10

rotate_time1 = 1.3   #起点开始的强制旋转时间
rotate_time2 = 3   #中途的强制旋转时间


start_x_v = 0.16  #起点开始的一步到位
start_y_v = -0.16
start_time = 2.3 #4

end_x_v = 0.13   #终点的一步到位
end_y_v = 0.30
end_x_time = 1.4
end_y_time = 2

#识别点pid参数
recog_pid_flag = 0
recog_pid_k_x = 1.2 #1.1
recog_pid_k_y = 1.7   
recog_nav_and_angle_adjust_time = 12

recog_pid_i_x = 0.0
recog_pid_i_y = 0.0
recog_angle_pid_k = 80#32#原来是17
#recog_x = [0.0 , 0.432 , 0.382 , 0.382 , 0.382 , 0.382 , 0.382 ,
#           0.382 , 0.382 , 0.382 , 0.382 , 0.382 , 0.382 ]##识别点x

recog_x = [0.0 , 0.432 , 0.402 , 0.402 , 0.402 , 0.402 , 0.402 ,
           0.402 , 0.402 , 0.402 , 0.402 , 0.402 , 0.402 ]##识别点x



recog_y = [0.0 , 0.612 , 1.796 , 0.596 ,    0.590, 1.808 , 0.584,
                      0.598 , 1.824 , 0.618 ,    0.570, 1.802 , 0.616 ]##识别点y，根据雷达180来改
x_sym_recog = [1, 1,1,1,  1,1,1,  1,1,1,  1,1,1]
y_sym_recog = [1, 1,1,-1, 1,1,-1, 1,1,-1, 1,1,-1]


#导航pid参数
adjust_time_navigation = 5

target_end_x = 0.186#0.2     #与上面一样需要调整
target_end_y = 1.636#1.66
position_end=0
navigation_pid_flag=0
navigation_pid_flag_2=0
x_dis = 0 
y_dis = 0
x_sym = 0
y_sym = 0 
nav_and_angle_adjust_time = 6


#新增终点导航参数
end_lr_k_y = 1.0
end_fb_k_x = 1.0
end_final_k_x = 0.5
end_final_k_y = 0.5
end_pid_lr_flag = 0
end_pid_fb_flag = 0
end_lr_adjust_time = 3  #左右对正且居中用时
angle_l = 540
angle_r = 180   #540为左方，180为右方
angle_rf = 270
angle_rb = 90
speed = 0.2
forward_time = 1    #待测，由中转点前往终点的时间和速度
start_time_for_lr_pid = 0
start_time_for_fb_pid = 0
start_time_for_final_pid = 0
fb_pid_time = 6
lr_pid_time = 6
final_pid_time = 4#6
#待测
lr_tolerance = 0.01
fb_tolerance_angle = 0.01
total_tolerance_angle=0.01
fb_tolerance_dis = 0.05
final_tolerance_x = 0.01
final_tolerance_y = 0.01

#主函数里面重新定义start_time_for_navigation_pid
move_lidar_k_x = 0.8 #1.0
move_lidar_k_y = 1.3 #1.5
move_lidar_i_x = 0
move_lidar_i_y = 0
integral_x = 0
integral_y = 0



#终点的中转点lr坐标

y_end_lr = [1.792,1.824,1.802,1.816]#根据雷达y_angle_end来改180 540 540 180
x_end_fb = [1.844,1.844,1.776,1.776]#1,2任务点一样根据雷达0来改，3,4任务点根据雷达360来改

x_angle_end = [0,0,360,360]
y_angle_end = [180,540,540,180]
x_sym_end = [-1,-1,1,1]
y_sym_end = [-1,1,1,-1]#4个数字

final_x = 1.776#根据雷达360来改
final_y = 1.800#根据雷达540来改
#任务中转点
mission_flag = 0 #表示任务点还是识别点还是终点

x_dis_miss = [0.928,1.010,0.962,0.942]#（以angle_miss为准）1,2根据雷达0来改,3,4根据雷达360来改
y_dis_miss = [0.962,0.932,0.898,0.972]#根据雷达y_angle_miss来改
x_sym_miss = [-1,-1,1,1]#4个数字
y_sym_miss = [-1,1,1,-1]#4个数字
x_angle_miss = [0,0,360,360]        #右后，左后，左前、右前
y_angle_miss = [180,540,540,180]

'''
x_dis_miss_2 = [0.237,0.249,0.271,0.272]#（以angle_miss_2为准）1,2根据雷达360来改,3,4根据雷达0来改
y_dis_miss_2 = [0.227,0.235,0.226,0.222]#根据雷达y_angle_miss来改
'''
x_dis_miss_2 = [0.197,0.185,0.231,0.232]#（以angle_miss_2为准）1,2根据雷达360来改,3,4根据雷达0来改
y_dis_miss_2 = [0.187,0.195,0.186,0.182]#根据雷达y_angle_miss来改
x_sym_miss_2 = [1,1,-1,-1]#4个数字
y_sym_miss_2 = [1,-1,-1,1]#4个数字
x_angle_miss_2 = [360,360,0,0]        
y_angle_miss_2 = [540,180,180,540]


x_angle = 360 #x_angle是朝前的固定360
y_angle = [0,540,540,180,540,540,180,540,540,180,540,540,180]
find_vlm_min = 0
find_vlm_max = 10
find_cb = 0

#转角度的pid
angle_tolerance = 0.01
angle_1=360
angle_pid_flag=0
angle_pid_flag_2=0
angle_adjust_time=3
delta_angle=5
delta_angle_angle=10#10
delta_angle_dis=5
delta_angle_lr=5
delta_angle_fb=3
angle_pid_k=12 #13

#主函数新增参数
find_id = 0
answer_id = 0
cal_symbol = 1 #1表示加法，2表示减法，3表示乘法，4表示除法
end_num = 12 #0表示11号点，1表示12号点，2表示13号点
shibie_flag = 0 #表示要去第几个识别点
shibieing_flag = 0 #表示正在识别
shibieing_real_flag = 0 #表示识别文字内容
end_pid_final_flag = 0
now_clue = 0 #线索编号
now_thing = 0 #目前第几条线索

#clue=['没有','香蕉','苹果','梨子',"葡萄","汉字五","汉字六","二维码七","二维码八"]
clue=['没有','葡萄','数字五','香蕉',"二维码七","苹果","二维码八","梨","数字六"]
question = ""

now_num_vlm = 1
find = [-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1]#代表每个识别点的识别结果


find_vlm = [-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2]#大模型
find_vlm_thing=['','','','','','','','','','','','','']
position_end = 0
adjust_time_end = 6
voice_receive_flag = 0#表示是否接收到语音指令
operation = ""
number_str = ""
voice_ok = 0


#正值为向前 向左走
def move_for_lidar(v_x,v_y):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=v_x
    twist_msg.linear.y=v_y
    twist_msg.angular.z=0
    start_time = 0
    start_time=rospy.Time.now()
    duration = Duration.from_sec(0.1)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.01)

def move_for_lidar_and_angle(v_x,v_y,v_z):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=v_x
    twist_msg.linear.y=v_y
    twist_msg.angular.z=v_z
    start_time = 0
    start_time=rospy.Time.now()
    duration = Duration.from_sec(0.1)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.01)

#这个函数用来启动
def voice_callback(data):
    global voice_receive_flag
    rospy.sleep(1)
    result = ""
    result = data.data
    rospy.loginfo("%s",result)
    voice_receive_flag = 1
    

def result_callback(msg):#获取识别结果为数字
    global find_vlm,find,now_num_vlm,find_vlm_min,find_vlm_max,shibieing_flag,shibie_flag,now_clue,now_thing,find_cb
    find_id_vlm = 0
    find_id_vlm = msg.data
    print("find_id_vlm",find_id_vlm)
    rospy.sleep(1)
    if (now_clue == 0):
        now_clue = find_id_vlm
    elif (now_clue != 0):
        if(find_vlm[shibie_flag]==-2 and find_id_vlm >= find_vlm_min and find_id_vlm <= find_vlm_max):            
            find_vlm[shibie_flag] = find_id_vlm
            now_num_vlm = shibie_flag
            if (now_clue == find_id_vlm):
                find[now_num_vlm]=1
                now_thing += 1
            else:
                find[now_num_vlm]=2
        #elif(find_id_vlm == -1 and find_vlm[shibie_flag]==-2 and find_cb >= 7 and find_cb <= 8):
        elif(find_id_vlm == -1 and find_vlm[shibie_flag]==-2 and (find_cb == 4 or find_cb == 6)):
            find_vlm[shibie_flag] = find_cb
            now_num_vlm = shibie_flag
            if (now_clue == find_cb):
                find[now_num_vlm]=1
                now_thing += 1
            else:
                find[now_num_vlm]=2 
    shibieing_flag = 0
def result_callback_2(msg):#获取识别结果为汉字
    global find_vlm,find_vlm_thing,find,now_num_vlm,find_vlm_min,find_vlm_max,shibieing_flag,shibieing_real_flag,shibie_flag,now_clue
    find_id_vlm = ""
    find_id_vlm = str(msg.data)
    print("find_zhongwen:",find_id_vlm)
    rospy.sleep(1)
    find_vlm_thing[shibie_flag] = find_id_vlm
    shibieing_real_flag = 0

'''
def result_callback(msg):
    global find_vlm,find,now_num_vlm,find_vlm_min,find_vlm_max,shibieing_flag
    find_id_vlm = 0
    find_id_vlm = msg.data
    print("find_id_vlm",find_id_vlm)
    rospy.sleep(1)
    for i in range(1,7):
        if(find_vlm[i]==-2):
            find_vlm[i] = find_id_vlm
            now_num_vlm = i
            break
    if(find[now_num_vlm]==-1):
        if(find_vlm[now_num_vlm]>=find_vlm_min and find_vlm[now_num_vlm]<=find_vlm_max):
            find[now_num_vlm]=find_vlm[now_num_vlm]
            shibieing_flag = 0
'''     

def navigation_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global navigation_pid_flag,x_dis,y_dis,x_angle,y_angle,x_sym,y_sym,integral_x,integral_y#第二个开始时间在main中修改
    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global angle_1,delta_angle,delta_angle_dis,angle_pid_flag,angle_adjust_time,angle_pid_k,start_time_for_angle_and_nav,nav_and_angle_adjust_time
    dis_right = msg.ranges[y_angle_miss[find_id-1]-delta_angle]
    dis_left = msg.ranges[y_angle_miss[find_id-1]+delta_angle]
    dis_now = msg.ranges[y_angle_miss[find_id-1]]
    delta_x = 0
    delta_y = 0
    '''
    rospy.loginfo("360_now:%f",np.min(msg.ranges[350:370]))
    x_now_1 = np.min(msg.ranges[710:719])
    x_now_2 = np.min(msg.ranges[0:10])
    x_now = np.min([x_now_1,x_now_2])
    rospy.loginfo("0_now:%f",x_now)
    rospy.loginfo("540_now:%f",np.min(msg.ranges[530:550]))
    rospy.loginfo("180_now:%f",np.min(msg.ranges[170:190]))
    rospy.loginfo("end_11:x=%f,y=%f",msg.ranges[360],msg.ranges[540])
    rospy.loginfo("end_12:x=%f,y=%f",msg.ranges[0],msg.ranges[180]) 
    rospy.loginfo("end_13:x=%f,y=%f",msg.ranges[360],msg.ranges[180]) 
    
    rospy.loginfo("535_now:%f",msg.ranges[355])
    rospy.loginfo("536_now:%f",msg.ranges[356])
    rospy.loginfo("537_now:%f",msg.ranges[357])
    rospy.loginfo("538_now:%f",msg.ranges[358])
    rospy.loginfo("539_now:%f",msg.ranges[359])
    rospy.loginfo("540_now:%f",msg.ranges[360])
    rospy.loginfo("541_now:%f",msg.ranges[361])
    rospy.loginfo("542_now:%f",msg.ranges[362])
    rospy.loginfo("543_now:%f",msg.ranges[363])
    rospy.loginfo("544_now:%f",msg.ranges[364])
    rospy.loginfo("545_now:%f",msg.ranges[365])
    '''

    if(angle_pid_flag == 1 and navigation_pid_flag == 1):#flag=1表示开始角度调整
        if(find_id <= 2):
            x_now1 = np.min(msg.ranges[715:719])
            x_now2 = np.min(msg.ranges[0:5])
            x_now = np.min([x_now1,x_now2])
        else:
            x_now = np.min(msg.ranges[355:365])
        y_now = np.min(msg.ranges[(y_angle_miss[find_id-1]-delta_angle_dis):(y_angle_miss[find_id-1]+delta_angle_dis)])
        start_time = start_time_for_angle_and_nav
        duration = Duration.from_sec(nav_and_angle_adjust_time)
        if(x_now<10 and y_now<10 and dis_now<10 and dis_left<10 and dis_right<10 and rospy.Time.now()-start_time<duration):
            delta_dis = dis_right-dis_left
            delta_x = x_now-x_dis
            delta_y = y_now-y_dis

            angle_vel=(angle_pid_k*(delta_dis))
            # rospy.loginfo("v_integral_X = %f",move_lidar_i_x*integral_x)
            # rospy.loginfo("v_integral_Y = %f",move_lidar_i_y*integral_y)
            # rospy.loginfo("v_k_X = %f",move_lidar_k_x*delta_x)
            # rospy.loginfo("v_k_Y = %f",move_lidar_k_y*delta_y)
            nav_x_vel = x_sym_miss[find_id-1]*(move_lidar_k_x*delta_x)
            nav_y_vel = y_sym_miss[find_id-1]*(move_lidar_k_y*delta_y)
            move_for_lidar_and_angle(nav_x_vel,nav_y_vel,angle_vel)
            
        if(rospy.Time.now()-start_time>=duration 
           or (rospy.Time.now()-start_time>=Duration.from_sec(3) and np.abs(delta_x)<=0.015 and np.abs(delta_y)<=0.015)):#如果任务点时间大于3s且偏差小于0.02m则停止pid
            #mission_pid_voice()
            navigation_pid_flag = 0
            angle_pid_flag = 0 
def navigation_pid_callback_2(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global navigation_pid_flag_2,x_dis,y_dis,x_sym,y_sym#第二个开始时间在main中修改
    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global delta_angle,delta_angle_dis,angle_pid_flag_2,angle_adjust_time,angle_pid_k,start_time_for_angle_and_nav,nav_and_angle_adjust_time
    dis_right = msg.ranges[y_angle_miss_2[find_id-1]-delta_angle]
    dis_left = msg.ranges[y_angle_miss_2[find_id-1]+delta_angle]
    dis_now = msg.ranges[y_angle_miss_2[find_id-1]]
    delta_x = 0
    delta_y = 0
    if(angle_pid_flag_2 == 1 and navigation_pid_flag_2 == 1):#flag=1表示开始角度调整
        if(find_id >= 3):
            x_now1 = np.min(msg.ranges[715:719])
            x_now2 = np.min(msg.ranges[0:5])
            x_now = np.min([x_now1,x_now2])
        else:
            x_now = np.min(msg.ranges[355:365])
        y_now = np.min(msg.ranges[(y_angle_miss_2[find_id-1]-delta_angle_dis):(y_angle_miss_2[find_id-1]+delta_angle_dis)])
        start_time = start_time_for_angle_and_nav
        duration = Duration.from_sec(nav_and_angle_adjust_time)
        if(x_now<10 and y_now<10 and dis_now<10 and dis_left<10 and dis_right<10 and rospy.Time.now()-start_time<duration):
            delta_dis = dis_right-dis_left

            delta_x = x_now-x_dis
            delta_y = y_now-y_dis

            angle_vel=(angle_pid_k*(delta_dis))
            nav_x_vel = x_sym_miss_2[find_id-1]*(move_lidar_k_x*delta_x)
            nav_y_vel = y_sym_miss_2[find_id-1]*(move_lidar_k_y*delta_y)
            move_for_lidar_and_angle(nav_x_vel,nav_y_vel,angle_vel)
            
        if(rospy.Time.now()-start_time>=duration 
           or (rospy.Time.now()-start_time>=Duration.from_sec(4) and np.abs(delta_x)<=0.015 and np.abs(delta_y)<=0.015)):#如果任务点时间大于3s且偏差小于0.02m则停止pid
            #mission_pid_voice()
            navigation_pid_flag_2 = 0
            angle_pid_flag_2 = 0 

def recog_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global recog_pid_flag,recog_x,recog_y,x_angle,y_angle,x_sym,y_sym,integral_x,integral_y#第二个开始时间在main中修改
    global recog_pid_k_x,recog_pid_k_y,recog_pid_i_x,recog_pid_i_y,shibie_flag

    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global delta_angle,delta_angle_angle,delta_angle_dis,recog_pid_flag,recog_angle_pid_k,recog_start_time_for_angle_and_nav,recog_nav_and_angle_adjust_time
    #角度三个值
    if(shibie_flag % 3 != 1 or shibie_flag == 1):
        dis_right = msg.ranges[360-delta_angle_angle]
        dis_left = msg.ranges[360+delta_angle_angle]
        dis_now = msg.ranges[360]
    else:
        dis_right = msg.ranges[540-delta_angle_angle]
        dis_left = msg.ranges[540+delta_angle_angle]
        dis_now = msg.ranges[540]
    delta_x = 0
    delta_y = 0
    if(recog_pid_flag == 1):
        x_dis = recog_x[shibie_flag]
        y_dis = recog_y[shibie_flag]
        x_angle = 360
        x_now = np.min(msg.ranges[(x_angle-delta_angle_dis):(x_angle+delta_angle_dis)])
        y_now = np.min(msg.ranges[(y_angle[shibie_flag]-delta_angle_dis):(y_angle[shibie_flag]+delta_angle_dis)])
        start_time = recog_start_time_for_angle_and_nav
        duration = Duration.from_sec(recog_nav_and_angle_adjust_time)

        if(x_now<10 and y_now<10 and dis_now<10 and dis_left<10 and dis_right<10 and rospy.Time.now()-start_time<duration):#and np.abs(dis_left-dis_right)>angle_tolerance ):
            delta_dis = dis_right-dis_left

            delta_x = x_now-x_dis
            delta_y = y_now-y_dis
            angle_vel=(recog_angle_pid_k*(delta_dis))
            nav_x_vel = x_sym*(recog_pid_k_x*delta_x)
            nav_y_vel = y_sym*(recog_pid_k_y*delta_y)
            if (nav_y_vel>3.5):
                nav_y_vel=3.5
            if (nav_y_vel<-3.5):
                nav_y_vel=-3.5
            '''
            if ((abs (nav_x_vel)) < 0.2):
                nav_x_vel*=1.5
            if ((abs (angle_vel)) < 0.8):
                angle_vel*=1.5
            '''
            move_for_lidar_and_angle(nav_x_vel,nav_y_vel,angle_vel)

        if(rospy.Time.now()-start_time>=duration 
           or (rospy.Time.now()-start_time>=Duration.from_sec(4) and np.abs(delta_x)<=0.05 and np.abs(delta_y)<=0.05 and np.abs(dis_right - dis_left) < total_tolerance_angle)):
            
            recog_pid_flag = 0
            integral_x = 0
            integral_y = 0

def end_pid_callback(msg):
    global end_pid_flag,x_dis,y_dis,x_angle,y_angle,x_sym,y_sym,integral_x,integral_y
    global angle_1,delta_angle,delta_angle_angle,delta_angle_dis,end_angle_adjust_time,end_nav_adjust_time,end_buff_time,start_time_for_end_pid,end_nav_pid_k_x,end_nav_pid_i_x,end_nav_pid_k_y,end_nav_pid_i_y,end_angle_pid_k

    delta_x = 0
    delta_y = 0
    if(end_pid_flag == 1):#flag=1表示开始角度调整
        dis_right = msg.ranges[angle_1-delta_angle_angle]
        dis_left = msg.ranges[angle_1+delta_angle_angle]
        dis_now = msg.ranges[angle_1]
        if (end_num == 12):
            x_now_1 = np.min(msg.ranges[(720-delta_angle_dis):719])
            x_now_2 = np.min(msg.ranges[0:delta_angle_dis])
            x_now = np.min([x_now_1,x_now_2])
        else:
            x_now = np.min(msg.ranges[(x_angle-delta_angle_dis):(x_angle+delta_angle_dis)])
        y_now = np.min(msg.ranges[(y_angle-delta_angle_dis):(y_angle+delta_angle_dis)])
        start_time = start_time_for_end_pid
        duration_1 = Duration.from_sec(end_angle_adjust_time)
        duration_2 = Duration.from_sec(end_angle_adjust_time + end_buff_time)
        duration_3 = Duration.from_sec(end_angle_adjust_time + end_buff_time + end_nav_adjust_time)
        if(dis_now<10 and dis_left<10 and dis_right<10 and np.abs(dis_left-dis_right)>angle_tolerance and rospy.Time.now()-start_time < duration_1):
            delta_dis = dis_right-dis_left
            move_for_lidar_and_angle(0,0,end_angle_pid_k*(delta_dis))
        if(x_now<10 and y_now<10 and rospy.Time.now()-start_time >= duration_2 and rospy.Time.now()-start_time < duration_3):

            delta_x = x_now-x_dis
            delta_y = y_now-y_dis
            integral_x += delta_x
            integral_y += delta_y
            end_nav_pid_k_x = end_nav_pid_k_x_a*np.abs(delta_x) + end_nav_pid_k_x_b
            end_nav_pid_k_x = np.min([end_nav_pid_k_x,end_nav_pid_k_x_max])
            nav_x_vel = x_sym*(end_nav_pid_k_x*delta_x+end_nav_pid_i_x*integral_x)
            nav_y_vel = y_sym*(end_nav_pid_k_y*delta_y+end_nav_pid_i_y*integral_y)
            move_for_lidar_and_angle(nav_x_vel,nav_y_vel,0)
        if(rospy.Time.now()-start_time >= duration_3):
            end_pid_flag = 0
            integral_x = 0
            integral_y = 0

def end_lr_pid_callback(msg):
    global delta_angle_lr,lr_tolerance,fb_tolerance_angle,end_pid_lr_flag,start_time_for_lr_pid,lr_pid_time,end_lr_k_y
    if(find_id <= 2):
        dis_right = msg.ranges[720-delta_angle_lr]
        dis_left = msg.ranges[delta_angle_lr]
    else:
        dis_right = msg.ranges[x_angle_miss[find_id-1]-delta_angle_lr]
        dis_left = msg.ranges[x_angle_miss[find_id-1]+delta_angle_lr]
    ref_lr = y_end_lr[find_id-1]
    fdb_lr = np.min(msg.ranges[(y_angle_end[find_id-1]-delta_angle_dis):(y_angle_end[find_id-1]+delta_angle_dis)])
    if end_pid_lr_flag == 1:
        delta_lr=fdb_lr-ref_lr
        start_time = start_time_for_lr_pid
        duration = Duration.from_sec(lr_pid_time)
        if( fdb_lr < 10 and dis_right < 10 and dis_left < 10 and rospy.Time.now()-start_time<duration ):
            angle_vel=angle_pid_k*(dis_right - dis_left)
            lr_vel = end_lr_k_y * delta_lr * y_sym_end[find_id-1]
            move_for_lidar_and_angle(0,lr_vel,angle_vel)
        if((abs(delta_lr) < lr_tolerance ) and (abs(dis_right - dis_left) < fb_tolerance_angle)) or (rospy.Time.now()-start_time>duration):
            end_pid_lr_flag = 0

def end_fb_pid_callback(msg):
    global angle_rf,angle_rb,delta_angle_fb,fb_tolerance_dis,end_pid_fb_flag,end_nav_pid_k_x,fb_pid_time,end_fb_k_x
    dis_right = msg.ranges[720-delta_angle_fb]
    dis_left = msg.ranges[delta_angle_fb]
    # 获取右前方和右后方的距离
    ref = x_end_fb[find_id-1]
    if(find_id <= 2):
        x_now_1 = np.min(msg.ranges[(720-delta_angle_fb):719])
        x_now_2 = np.min(msg.ranges[0:delta_angle_fb])
        fdb = np.min([x_now_1,x_now_2])
    else:
        fdb = np.min(msg.ranges[(360-delta_angle_fb):(360+delta_angle_fb)])
    if end_pid_fb_flag == 1:
        delta_fb = fdb - ref
        start_time = start_time_for_fb_pid
        duration = Duration.from_sec(fb_pid_time)
        if(  dis_right < 10 and dis_left < 10 and fdb < 10 and rospy.Time.now()-start_time<duration ):
            fb_vel = end_fb_k_x * delta_fb * x_sym_end[find_id-1]
            angle_vel=angle_pid_k*(dis_right - dis_left)
            move_for_lidar_and_angle(fb_vel, 0, angle_vel)
        if ((abs(delta_fb) < fb_tolerance_dis and abs(dis_right - dis_left) < fb_tolerance_angle) or rospy.Time.now()-start_time>duration):
            end_pid_fb_flag = 0

def final_pid_callback(msg):
    global end_pid_final_flag
    dis_right = msg.ranges[360-delta_angle_fb]
    dis_left = msg.ranges[360+delta_angle_fb]
    delta_x = 0
    delta_y = 0
    # 获取右前方和右后方的距离
    ref_x = final_x
    ref_y = final_y
    fdb_x = np.min(msg.ranges[(360-delta_angle_fb):(360+delta_angle_fb)])
    fdb_y = np.min(msg.ranges[(540-delta_angle_fb):(540+delta_angle_fb)])
    if end_pid_final_flag == 1:
        delta_x = fdb_x - ref_x
        delta_y = fdb_y - ref_y
        start_time = start_time_for_final_pid
        duration = Duration.from_sec(final_pid_time)
        if(fdb_y < 10 and fdb_x < 10 and dis_right < 10 and dis_left < 10 and rospy.Time.now()-start_time<duration ):
            final_vel_x = end_final_k_x * delta_x
            final_vel_y = end_final_k_y * delta_y
            angle_vel=angle_pid_k*(dis_right - dis_left)
            move_for_lidar_and_angle(final_vel_x, final_vel_y, angle_vel)
        if ((abs(delta_x) < final_tolerance_x and abs(delta_y) < final_tolerance_y and abs(dis_right - dis_left) < fb_tolerance_angle) or rospy.Time.now()-start_time>duration):
            end_pid_final_flag = 0

def move_forward():
    global speed,forward_time
    move_for_lidar_and_angle(speed * end_sym,0,0)
    rospy.sleep(forward_time)
    move_for_lidar_and_angle(0,0,0)

#角度计算函数
def calculate_wall_angle_robust(angles_distances):
    # 1. 过滤掉所有非有限(non-finite)的距离值 (inf, nan)
    valid_angles_distances = []
    for angle_deg, distance in angles_distances:
        if np.isfinite(distance):
            valid_angles_distances.append((angle_deg, distance))

    # 2. 检查过滤后是否还有足够的点来拟合一条直线
    if len(valid_angles_distances) < 2:
        # 如果有效点少于2个，无法拟合直线，返回一个安全值
        rospy.logwarn("[calculate_wall_angle_robust] Not enough finite points to calculate angle. Skipping.")
        return 0

    # 3. 使用过滤后的干净数据进行后续计算
    points = []
    for angle_deg, distance in valid_angles_distances:
        # 将角度转换为相对于车头(180°)的角度
        relative_angle = angle_deg - 180

        # 转换为弧度
        angle_rad = math.radians(relative_angle)

        # 直角坐标转换
        x = distance * math.cos(angle_rad)
        y = distance * math.sin(angle_rad)
        points.append((x, y))

    # 拟合直线
    x_coords = [p[0] for p in points]
    y_coords = [p[1] for p in points]

    # 使用线性回归
    if abs(max(x_coords) - min(x_coords)) < 0.001:
        # 如果x坐标几乎相同，说明墙是垂直的
        return 90.0 if y_coords[0] > 0 else -90.0
    
    # 增加一个 try-except 块作为最后的保险
    try:
        A = np.vstack([x_coords, np.ones(len(x_coords))]).T
        k, b = np.linalg.lstsq(A, y_coords, rcond=None)[0]
    except np.linalg.LinAlgError as e:
        rospy.logerr("[calculate_wall_angle_robust] Linear algebra error during lstsq: %s", e)
        return 0 # 在线性代数计算失败时返回安全值

    # 计算夹角
    wall_angle_rad = math.atan(k)
    wall_angle_deg = math.degrees(wall_angle_rad)

    return wall_angle_deg

'''
def calculate_wall_angle_robust(angles_distances):
    if len(angles_distances) < 2:
        return 0

    # 转换为车辆坐标系直角坐标
    points = []
    for angle_deg, distance in angles_distances:
        # 将角度转换为相对于车头(180°)的角度
        relative_angle = angle_deg - 180

        # 转换为弧度
        angle_rad = math.radians(relative_angle)

        # 直角坐标转换
        x = distance * math.cos(angle_rad)
        y = distance * math.sin(angle_rad)
        points.append((x, y))

    # 拟合直线
    x_coords = [p[0] for p in points]
    y_coords = [p[1] for p in points]

    # 使用线性回归
    if abs(max(x_coords) - min(x_coords)) < 0.001:
        # 如果x坐标几乎相同，说明墙是垂直的
        return 90.0 if y_coords[0] > 0 else -90.0

    A = np.vstack([x_coords, np.ones(len(x_coords))]).T
    k, b = np.linalg.lstsq(A, y_coords, rcond=None)[0]

    # 计算夹角
    wall_angle_rad = math.atan(k)
    wall_angle_deg = math.degrees(wall_angle_rad)

    return wall_angle_deg
'''
def angle_lidar_pid(msg):#根据激光雷达某一个角度的距离和相邻两侧角度的距离，控制改角度与墙面垂直,angle_1要在主函数更改
    global rotate_angle,rotate_delta_angle,angle_pid_flag,rotate_angle_pid_k,start_time_for_rotate_pid,rotate_pid_flag
    dir = 0

    if (mission_flag==0):
        dis_right = msg.ranges[rotate_angle-rotate_delta_angle]
        dis_left = msg.ranges[rotate_angle+rotate_delta_angle]
        dis_now = msg.ranges[rotate_angle]
        data_parallel = [
            ((rotate_angle+rotate_delta_angle)/2, dis_left),  # 墙在左侧，与车平行
            (rotate_angle/2, dis_now),
            ((rotate_angle-rotate_delta_angle)/2, dis_right)
        ]
        dir = calculate_wall_angle_robust(data_parallel)
    elif (mission_flag==1):
        dis_right = msg.ranges[360-rotate_delta_angle]
        dis_left = msg.ranges[360+rotate_delta_angle]
        dis_now = msg.ranges[360]
        data_parallel = [
            ((rotate_angle+rotate_delta_angle)/2, dis_left),  # 墙在左侧，与车平行
            (rotate_angle/2, dis_now),
            ((rotate_angle-rotate_delta_angle)/2, dis_right)
        ]
        dir = calculate_wall_angle_robust(data_parallel)  

    
    #rospy.loginfo("dir = %f",dir)
    if(rotate_pid_flag == 1):#flag=1表示开始角度调整
        start_time = start_time_for_rotate_pid
        duration = Duration.from_sec(3)
        if(dis_now<10 and dis_left<10 and dis_right<10  ):
            delta_dis = dis_right-dis_left
            if (mission_flag==0):
                if (dir>10):
                    if (dir-10<=6):
                        move_for_lidar_and_angle(0,0,0.6)
                    elif (dir-10<=10):
                        move_for_lidar_and_angle(0,0,1)
                    else:
                        move_for_lidar_and_angle(0,0,1.8)
                if (dir<-5):
                    if (dir+5>=-6):
                        move_for_lidar_and_angle(0,0,-0.6)
                    elif (dir+5>=-10):  
                        move_for_lidar_and_angle(0,0,-1)
                    else:
                        move_for_lidar_and_angle(0,0,-1.8)
            elif (mission_flag==1):
                if (dir>10):
                    if (dir-10<=6):
                        move_for_lidar_and_angle(0,0,0.6)
                    elif (dir-10<=10):
                        move_for_lidar_and_angle(0,0,1.2)
                    else:
                        move_for_lidar_and_angle(0,0,1.8)
                if (dir<0):
                    if (dir>=-6):
                        move_for_lidar_and_angle(0,0,-0.6)
                    elif (dir>=-10):  
                        move_for_lidar_and_angle(0,0,-1.2)
                    else:
                        move_for_lidar_and_angle(0,0,-1.8)


        if(rospy.Time.now()-start_time>=duration):
            rotate_pid_flag = 0



def id_match(point_msg):#没问题
    if(point_msg.z>=1 and point_msg.z<=20):
        return 1
    elif(point_msg.z>=21 and point_msg.z<=40 ):
        return 2
    #point_msg = None
    elif (point_msg.z>=41 and point_msg.z<=60 ):
        return 3
    #point_msg = None
    elif (point_msg.z>=61 and point_msg.z<=80 ):
        return 4
    #point_msg = None
    elif (point_msg.z>=81 and point_msg.z<=110 ):
        return 5
    #point_msg = None
    elif (point_msg.z>=111 and point_msg.z<=130):
        return 6
    #point_msg = None
    elif (point_msg.z>=131 and point_msg.z<=140 ):
        return 7
    #point_msg = None
    elif (point_msg.z>=141 and point_msg.z<=160 ):
        return 8
    else :
        return 0

class navigation_demo():
    def __init__(self):
        self.set_pose_pub = rospy.Publisher('/initialpose', PoseWithCovarianceStamped, queue_size=5)
        self.arrive_pub = rospy.Publisher('/voiceWords',String,queue_size=10)
        self.ar_sub1 = rospy.Subscriber('/ar_pose_marker',AlvarMarkers,self.ar_track_cb)
        #self.ar_sub = rospy.Subscriber('/object_position', Point, self.ar_cb)
        self.ar_cb2 = rospy.Subscriber('/vlm_node/result', Int32, result_callback)
        self.ar_cb3 = rospy.Subscriber('/vlm_node/result_2', String, result_callback_2)   
        self.mission_sub = rospy.Subscriber('/scan', LaserScan, navigation_pid_callback)
        self.mission_sub_2 = rospy.Subscriber('/scan', LaserScan, navigation_pid_callback_2)
        self.end_lr = rospy.Subscriber('/scan', LaserScan, end_lr_pid_callback)
        self.end_fb = rospy.Subscriber('/scan', LaserScan, end_fb_pid_callback)
        self.final = rospy.Subscriber('/scan', LaserScan, final_pid_callback)
        self.rotate = rospy.Subscriber('/scan', LaserScan, angle_lidar_pid)

        self.recog_sub = rospy.Subscriber('/scan', LaserScan, recog_pid_callback)
        self.voice_sub = rospy.Subscriber('/voice_result', String, voice_callback)
        self.move_base = actionlib.SimpleActionClient("move_base", MoveBaseAction)
        self.move_base.wait_for_server(rospy.Duration(60))

    
    def ar_track_cb(self,data):
        #print ("zhixing AR callback")
        global find,shibieing_flag,shibie_flag,find_vlm,find_cb
        find_id_ar = 0
        for marker in data.markers:
            find_id_ar = marker.id
        #rospy.loginfo("二维码：%d",find_id_ar)
        if(find_id_ar <=8 and find_id_ar>=7):
            if(find_id_ar == 7):
                find_cb = 4
            if(find_id_ar == 8):
                find_cb = 6
            #find_cb = find_id_ar
            #rospy.loginfo("二维码：%d",find_id_ar)

    def ar_cb(self, data):#没问题
        #print ("zhixing hanzi callback")
        global find,shibie_flag,shibieing_flag
        point_msg = data
        find_id_cb = 0
        find_id_cb = id_match(point_msg)
        if(find_id_cb <=8 and find_id_cb>=1 and shibieing_flag == 1):
            find[shibie_flag] = find_id_cb
            shibieing_flag = 0
            rospy.loginfo("汉字：%d",find_id_cb)
              

    def set_pose(self, p):
        if self.move_base is None:
            return False
        x, y, th = p

        pose = PoseWithCovarianceStamped()
        pose.header.stamp = rospy.Time.now()
        pose.header.frame_id = 'map'
        pose.pose.pose.position.x = x
        pose.pose.pose.position.y = y
        q = transformations.quaternion_from_euler(0.0, 0.0, th/180.0*pi)
        pose.pose.pose.orientation.x = q[0]
        pose.pose.pose.orientation.y = q[1]
        pose.pose.pose.orientation.z = q[2]
        pose.pose.pose.orientation.w = q[3]
        self.set_pose_pub.publish(pose)
        return True

    def _done_cb(self, status, result):
        rospy.loginfo("navigation done! status:%d result:%s"%(status, result))
        arrive_str = "arrived to traget point"
        self.arrive_pub.publish(arrive_str)

    def _active_cb(self):
        rospy.loginfo("[Navi] navigation has be actived")

    def _feedback_cb(self, feedback):
        msg = feedback
        #rospy.loginfo("[Navi] navigation feedback\r\n%s"%feedback)

    def goto(self, p):
        rospy.loginfo("[Navi] goto %s"%p)
        #arrive_str = "going to next point"
        #self.arrive_pub.publish(arrive_str)
        goal = MoveBaseGoal()

        goal.target_pose.header.frame_id = 'map'
        goal.target_pose.header.stamp = rospy.Time.now()
        goal.target_pose.pose.position.x = p[0]
        goal.target_pose.pose.position.y = p[1]
        q = transformations.quaternion_from_euler(0.0, 0.0, p[2]/180.0*pi)
        goal.target_pose.pose.orientation.x = q[0]
        goal.target_pose.pose.orientation.y = q[1]
        goal.target_pose.pose.orientation.z = q[2]
        goal.target_pose.pose.orientation.w = q[3]

        self.move_base.send_goal(goal, self._done_cb, self._active_cb, self._feedback_cb)
        result = self.move_base.wait_for_result(rospy.Duration(60))
        #rospy.sleep(2)
        if not result:
            self.move_base.cancel_goal()
            rospy.loginfo("Timed out achieving goal")
        else:
            state = self.move_base.get_state()
            if state == GoalStatus.SUCCEEDED:
                rospy.loginfo("reach goal %s succeeded!"%p)
        return True

    def cancel(self):
        self.move_base.cancel_all_goals()
        return True

def cal_id():#根据AI找到最后结果
    global find_id,question,answer_flag
    msg = String()
    msg.data = question
    answer_flag = 0
    ask_q.publish(question)

    while (answer_flag == 0):
        rospy.sleep(0.2)

def move2end(x_v, y_v, time):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=x_v
    twist_msg.linear.y=y_v
    twist_msg.angular.z=0.0
    start_time=rospy.Time.now()
    duration = Duration.from_sec(time)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.01)

def move_rotate_180(rotate_time):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=0.0
    twist_msg.linear.y=0.0
    twist_msg.angular.z=rotate_180_z
    start_time=rospy.Time.now()
    duration = Duration.from_sec(rotate_time)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.1)

def move_rotate_right(rotate_time):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=0.0
    twist_msg.linear.y=0.0
    twist_msg.angular.z=rotate_right_z
    start_time=rospy.Time.now()
    duration = Duration.from_sec(rotate_time)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.1)

def talker():
    global voice_receive_flag,voice_ok
    rate = rospy.Rate(1)  # 1hz
    while (voice_receive_flag == 0):
        msg = Int32()
        msg.data = 1
        #rospy.loginfo("Publishing wakeup message")
        rate.sleep()

    msg_2 = String()
    msg_2.data = "比赛开始"
 
    file_path = os.path.join("/home/abot/music/比赛开始.wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg_2)
    while(voice_ok == 0):
        rospy.sleep(0.1)

def clue_voice():
    global find,shibie_flag,pub_voice,voice_ok,now_clue
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "线索"+str(clue[now_clue])

    rospy.loginfo("发送文本转语音："+msg.data)
    text = ""
    text = msg.data
    file_path = os.path.join("/home/abot/music", text+".wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.1)

def shibie_voice():
    global find,shibie_flag,pub_voice,voice_ok,now_thing,question
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "已解锁第"+str(now_thing)+"条线索信息"+str(find_vlm_thing[shibie_flag])
    #msg.data = str(find_vlm_thing[shibie_flag])
    question = question + str(find_vlm_thing[shibie_flag])+'。'
    rospy.loginfo("发送文本转语音："+msg.data)
    text = ""
    text = msg.data
    file_path = os.path.join("/home/abot/music", text+".wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.1)
    
        
def mission_voice():
    global fishibie_flag,operation,find_id,pub_voice,voice_ok
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "最终答案是"+str(answer_id)
    text = msg.data
    file_path = os.path.join("/home/abot/music", text+".wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.1)

def mission_voice_2():
    global find_id,pub_voice,voice_ok
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "已导航到任务点"+str(find_id)
    text = ""
    text = msg.data
    file_path = os.path.join("/home/abot/music", text+".wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.1)


def end_voice():
    global end_num,pub_voice,voice_ok
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "导航到终点"
    text = ""
    text = msg.data
    file_path = os.path.join("/home/abot/music", text+".wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.1)

def mission_pid_voice():
    global find_id,voice_ok,pub_voice
    rospy.sleep(2)
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "已导航到"+str(find_id)+"点"
    text = ""
    text = msg.data
    file_path = os.path.join("/home/abot/music", text+".wav")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        voice_ok = 1
    else:
        print("失败，尝试生成语音。")
        pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.1)

def voice_ok_callback(msg):
    global voice_ok
    voice_ok = 1

def answer_callback(msg):
    global answer_flag,find_id,answer_id

    find_id = int(msg.data)
    if(find_id > 8):
        find_id = 8
    
    answer_id = find_id #用来语音

    if(find_id < 1):
        find_id = 1
    if(find_id >= 5 and find_id <= 8):
        find_id = find_id - 4
    print("find_id=",find_id)
    answer_flag = 1

if __name__ == "__main__":
    global pub_voice
    rospy.init_node('navigation_demo',anonymous=True)
    pub_voice = rospy.Publisher('/main_voiceWords', String, queue_size=80)
    #加入问题发布与答案接收
    ask_q = rospy.Publisher('/askquestion', String, queue_size=100)
    rospy.Subscriber("/answer", Int32, answer_callback)

    sub_voice_ok = rospy.Subscriber('/voice_ok', Int32, voice_ok_callback)
    goalListX = rospy.get_param('~goalListX', '2.0, 2.0')
    goalListY = rospy.get_param('~goalListY', '2.0, 4.0')
    goalListYaw = rospy.get_param('~goalListYaw', '0, 90.0')
    goalListXX = rospy.get_param('~goalListXX', '2.0, 2.0')
    goalListYY = rospy.get_param('~goalListYY', '2.0, 4.0')
    goalListYawYaw = rospy.get_param('~goalListYawYaw', '0, 90.0')
    goalListXXX = rospy.get_param('~goalListXXX', '2.0, 2.0')
    goalListYYY = rospy.get_param('~goalListYYY', '2.0, 4.0')
    goalListYawYawYaw = rospy.get_param('~goalListYawYawYaw', '0, 90.0')
    goals = [[float(x), float(y), float(yaw)] for (x, y, yaw) in zip(goalListX.split(","),goalListY.split(","),goalListYaw.split(","))]
    goalshibie = [[float(x), float(y), float(yaw)] for (x, y, yaw) in zip(goalListXX.split(","),goalListYY.split(","),goalListYawYaw.split(","))]
    goalend = [[float(x), float(y), float(yaw)] for (x, y, yaw) in zip(goalListXXX.split(","),goalListYYY.split(","),goalListYawYawYaw.split(","))]
    navi = navigation_demo()
    r = rospy.Rate(1)
    print (goals)
    r.sleep()
    print("zhixing main")

    #初始化以下两个变量
    start_time_for_angle_and_nav =rospy.Time.now()
    recog_start_time_for_angle_and_nav = rospy.Time.now()

    if (True):

        talker()#等待播报比赛开始，用来启动
        '''
        move_for_lidar_and_angle(0,0,0.8)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,1)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,1.2)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,1.4)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,1.6)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,1.8)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,2)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,2.2)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,2.4)
        rospy.sleep(1)
        move_for_lidar_and_angle(0,0,2.6)
        rospy.sleep(1)

        rospy.sleep(1000)
        '''
        #单独处理第一个线索点
        move2end(start_x_v,start_y_v,start_time)  #一步到位
        rospy.sleep(0.1)

        #测试强制旋转
        #move_rotate_right(rotate_right_t) #起点开始的强制旋转
        #rospy.sleep(1000)
        '''
        #测试点位
        navi.goto(goals[0])
        rospy.sleep (2)
        navi.goto(goals[1])
        rospy.sleep (2)        
        navi.goto(goals[2])
        rospy.sleep (2)       
        navi.goto(goals[3])
        rospy.sleep (2)       
        navi.goto(goalshibie[4])
        rospy.sleep (2)       
        navi.goto(goalshibie[7])
        rospy.sleep (2)       
        navi.goto(goalshibie[10])
        rospy.sleep(1000)
        '''
        '''
        for find_id in range(1,5):
            navi.goto(goals[find_id-1])
            start_time_for_angle_and_nav = rospy.Time.now()#表示当前为开始执行的时间
            x_sym = x_sym_miss[find_id-1]
            y_sym = y_sym_miss[find_id-1]
            x_dis = x_dis_miss[find_id-1]                  
            y_dis = y_dis_miss[find_id-1]
            angle_pid_flag = 1
            navigation_pid_flag = 1
            while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                rospy.sleep(0.2)
            start_time_for_angle_and_nav = rospy.Time.now()#表示当前为开始执行的时间
            x_sym = x_sym_miss_2[find_id-1]
            y_sym = y_sym_miss_2[find_id-1]
            x_dis = x_dis_miss_2[find_id-1]                  
            y_dis = y_dis_miss_2[find_id-1]
            angle_pid_flag_2 = 1
            navigation_pid_flag_2 = 1
            while(angle_pid_flag_2 == 1 and navigation_pid_flag_2 == 1):
                rospy.sleep(0.2)
            mission_voice_2()
            rospy.sleep(1)
            start_time_for_angle_and_nav = rospy.Time.now()#表示当前为开始执行的时间
            x_sym = x_sym_miss[find_id-1]
            y_sym = y_sym_miss[find_id-1]
            x_dis = x_dis_miss[find_id-1]-0.6                  
            y_dis = y_dis_miss[find_id-1]
            angle_pid_flag = 1
            navigation_pid_flag = 1
            while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                rospy.sleep(0.2)
            mission_flag = 2
        rospy.sleep(1000)
        '''
        move_rotate_180(rotate_time1) #起点开始的强制旋转
        #rospy.sleep(0.5)
        shibie_flag = 1
        #navi.goto(goalshibie[1])  #被一步到位替代


        x_sym = x_sym_recog[shibie_flag]
        y_sym = y_sym_recog[shibie_flag]
        recog_pid_flag = 1
        recog_start_time_for_angle_and_nav = rospy.Time.now()
        while(recog_pid_flag == 1):
            rospy.sleep(0.2)
        
        rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
        shibieing_flag = 1
        while(shibieing_flag == 1):
            rospy.sleep(0.2)      
        clue_voice()

        shibie_flag=2

        position_end = 0 #是否到达终点
        end_pid_flag = 0
        find_id = 0 
        shibieing_real_flag = 0
        mission_flag = 0 #表示任务点还是识别点还是终点

        while(position_end == 0):
            if (mission_flag == 0):#0任务前 1任务中 2任务后
                if (find[shibie_flag] <= 0):
                    if (shibie_flag % 3 == 1):

                        
                        move_rotate_right(rotate_right_t)
                        rospy.sleep(0.3)
                        rotate_pid_flag = 1
                        start_time_for_rotate_pid = rospy.Time.now()
                        while(rotate_pid_flag == 1):
                            rospy.sleep(0.2)
                        '''
                        shibie_flag=shibie_flag-1m
                        if (find[shibie_flag]<=0):
                            x_sym = x_sym_recog[shibie_flag]
                            y_sym = y_sym_recog[shibie_flag]
                            recog_pid_flag = 1
                            recog_start_time_for_angle_and_nav = rospy.Time.now()
                            while(recog_pid_flag == 1):
                                rospy.sleep(0.2)
                        shibie_flag=shibie_flag+1
                        '''
                        
                        #navi.goto(goalshibie[shibie_flag])

                    x_sym = x_sym_recog[shibie_flag]
                    y_sym = y_sym_recog[shibie_flag]
                    '''
                    rospy.loginfo("360_now:%f",np.min(msg.ranges[350:370]))
                    rospy.loginfo("540_now:%f",np.min(msg.ranges[530:550]))
                    '''
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)

                    find[shibie_flag] =2
                    
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1     
                    shibieing_real_flag = 1
                    while(shibieing_flag == 1):
                        rospy.sleep(0.2) 
                    
                elif (find[shibie_flag] == 1):

                    while(shibieing_real_flag == 1):
                        rospy.sleep(0.2)
                    shibie_voice()
                    print("find",find_vlm_thing[shibie_flag])

                    if(shibie_flag >= 10):
                        mission_flag = 1
                    elif(shibie_flag % 3 == 2):
                        shibie_flag  += 2
                    elif(shibie_flag % 3 == 1):
                        shibie_flag  += 3
                    else:
                        shibie_flag  += 1

                elif (find[shibie_flag] == 2):
                    shibie_flag += 1
                    if (shibie_flag>=13):
                        mission_flag = 1
                else :
                    rospy.sleep(0.01)
            elif(mission_flag == 1):
                cal_id()
                #测试能否进入任务点
                #answer_id = now_clue #用来语音
                #find_id=now_clue%4 #线索编号 #测试点位
                print("find_id=",find_id)
                mission_voice()
                '''
                if((find_id == 1 or find_id == 4 )and (shibie_flag == 10 or shibie_flag == 11)):

                    rotate_pid_flag = 1
                    start_time_for_rotate_pid = rospy.Time.now()
                    while(rotate_pid_flag == 1):
                        rospy.sleep(0.2)

                    shibie_flag = 12
                    x_sym = x_sym_recog[shibie_flag]
                    y_sym = y_sym_recog[shibie_flag]
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)  

                if((find_id == 2 or find_id == 3) and (shibie_flag >= 11)):       

                    rotate_pid_flag = 1
                    start_time_for_rotate_pid = rospy.Time.now()
                    while(rotate_pid_flag == 1):
                        rospy.sleep(0.2)

                    shibie_flag = 10
                    x_sym = x_sym_recog[shibie_flag]
                    y_sym = y_sym_recog[shibie_flag]
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)  
                '''
                if((find_id == 1 or find_id == 4 )and (shibie_flag == 10 or shibie_flag == 11)):

                    
                    if ( shibie_flag == 10 ):
                        shibie_flag = 11
                        x_sym = x_sym_recog[shibie_flag]
                        y_sym = y_sym_recog[shibie_flag]
                        recog_pid_flag = 1
                        recog_start_time_for_angle_and_nav = rospy.Time.now()
                        while(recog_pid_flag == 1):
                            rospy.sleep(0.2)  

                    shibie_flag = 12
                    x_sym = x_sym_recog[shibie_flag]
                    y_sym = y_sym_recog[shibie_flag]
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)  

                if((find_id == 2 or find_id == 3) and (shibie_flag >= 11)):       

                    
                    if ( shibie_flag == 12 ):
                        shibie_flag = 11
                        x_sym = x_sym_recog[shibie_flag]
                        y_sym = y_sym_recog[shibie_flag]
                        recog_pid_flag = 1
                        recog_start_time_for_angle_and_nav = rospy.Time.now()
                        while(recog_pid_flag == 1):
                            rospy.sleep(0.2)  

                    shibie_flag = 10
                    x_sym = x_sym_recog[shibie_flag]
                    y_sym = y_sym_recog[shibie_flag]
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)  



                if (find_id<=2):
                    navi.goto(goals[find_id-1])


                #以下为PID进入任务点-1
                start_time_for_angle_and_nav = rospy.Time.now()
                x_sym = x_sym_miss[find_id-1]
                y_sym = y_sym_miss[find_id-1]
                x_dis = x_dis_miss[find_id-1]                  
                y_dis = y_dis_miss[find_id-1]
                angle_pid_flag = 1
                navigation_pid_flag = 1
                while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                    rospy.sleep(0.2)

                #以下为PID进入任务点-2
                start_time_for_angle_and_nav = rospy.Time.now()
                x_sym = x_sym_miss_2[find_id-1]
                y_sym = y_sym_miss_2[find_id-1]
                x_dis = x_dis_miss_2[find_id-1]                  
                y_dis = y_dis_miss_2[find_id-1]
                angle_pid_flag_2 = 1
                navigation_pid_flag_2 = 1
                while(angle_pid_flag_2 == 1 and navigation_pid_flag_2 == 1):
                    rospy.sleep(0.2)

                mission_voice_2()
                rospy.sleep(1)

                #以下为PID退出任务点
                start_time_for_angle_and_nav = rospy.Time.now()
                x_sym = x_sym_miss[find_id-1]
                y_sym = y_sym_miss[find_id-1]
                x_dis = x_dis_miss[find_id-1]-0.6                  
                y_dis = y_dis_miss[find_id-1]
                angle_pid_flag = 1
                navigation_pid_flag = 1
                while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                    rospy.sleep(0.2)
                mission_flag = 2

            elif (mission_flag == 2):  
                #以下为PID左右居中
                start_time_for_end_pid = rospy.Time.now()
                end_pid_lr_flag = 1
                start_time_for_lr_pid = rospy.Time.now()
                while (end_pid_lr_flag == 1):
                    rospy.sleep(0.2)
                #以下为PID前后居中
                end_pid_fb_flag = 1
                start_time_for_fb_pid = rospy.Time.now()
                while (end_pid_fb_flag == 1):
                    rospy.sleep(0.2)
                end_pid_final_flag = 1
                start_time_for_final_pid = rospy.Time.now()
                while (end_pid_final_flag == 1):
                    rospy.sleep(0.2)
                end_voice()
                position_end = 1
                '''会报错 没有msg这个变量
                rospy.loginfo("360_now:%f",np.min(msg.ranges[350:370]))
                rospy.loginfo("540_now:%f",np.min(msg.ranges[530:550]))
                '''
                rospy.loginfo("到达")

    while not rospy.is_shutdown():
        r.sleep()

