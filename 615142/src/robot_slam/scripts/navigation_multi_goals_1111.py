#!/usr/bin/env python
# -*- coding: utf-8 -*-
#coding: utf-8

import rospy
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

#中转0是曾经用的中间点，中转1是刚开始的中转点（和识别0等值反向），中转2是终点前的中转点（和识别3等值且同向），中转3是尝试直接加进去的终点导航
#start_time = rospy.Time.now()
rotate_180_z = 2.6   #move_rotate_180()中的强制旋转z速度
rotate_time1 = 3   #起点开始的强制旋转时间
rotate_time2 = 3   #中途的强制旋转时间

start_x_v = 0.13  #起点开始的一步到位
start_y_v = -0.24
start_time = 2

end_x_v = 0.13   #终点的一步到位
end_y_v = 0.30
end_x_time = 1.4
end_y_time = 2

#识别点pid参数
recog_pid_flag = 0
recog_pid_k_x = 1.1
recog_pid_k_y = 1.8
recog_nav_and_angle_adjust_time = 5
recog_pid_i_x = 0.0
recog_pid_i_y = 0.0
recog_angle_pid_k = 12
recog_x = [0.0 , 0.382 , 0.382 , 0.382 , 0.382 , 0.382 , 0.382]##识别点x，不需要更改
recog_y = [0.0 , 1.000 , 0.765 , 1.026 , 0.822 , 1.070 , 0.812]##识别点y，根据雷达180来改

#导航pid参数
adjust_time_navigation = 5
target_end_x = 0.186#0.2
target_end_y = 1.636#1.66
position_end=0
navigation_pid_flag=0
x_dis = 0 
y_dis = 0
x_sym = 0
y_sym = 0 
nav_and_angle_adjust_time = 6

#终点导航参数
end_pid_flag = 0
end_nav_pid_k_x = 1.3
end_nav_pid_k_x_a = 1
end_nav_pid_k_x_b = 1.0
end_nav_pid_k_x_max = 1.6
end_nav_pid_k_y = 2
end_nav_pid_i_x = 0.0
end_nav_pid_i_y = 0.0
end_angle_pid_k = 13
end_angle_adjust_time = 3
end_buff_time = 0.5
end_nav_adjust_time = 4

#主函数里面重新定义start_time_for_navigation_pid
move_lidar_k_x = 1.1
move_lidar_k_y = 1.8
move_lidar_i_x = 0.0
move_lidar_i_y = 0.0
integral_x = 0
integral_y = 0
x_dis_miss = [1.026,1.026,1.026,1.046,1.046,1.046,1.010,1.010,1.010]#根据雷达360来改
y_dis_miss = [0.600,0.940,1.280,1.318,0.980,0.638,0.644,0.978,1.318]#根据雷达y_angle_miss来改
end_dis_x = [0.214,0.206,0.228]#根据end_angle_x来改
end_dis_y = [0.235,0.218,0.207]#根据end_angle_y来改
end_angle_x = [360,0,360]
end_angle_y = [540,180,180]
x_sym_miss = [1,1,1,1,1,1,1,1,1]#9个数字
y_sym_miss = [1,1,1,-1,-1,-1,1,1,1]#9个数字
y_angle_miss = [540,540,540,180,180,180,540,540,540]
end_x_sym = [1,-1,1]
end_y_sym = [1,-1,-1]
x_angle = 360 #x_angle是朝前的固定360
y_angle = 0
find_vlm_min = 1
find_vlm_max = 10

#转角度的pid
angle_tolerance = 0.01
angle_1=360
angle_pid_flag=0
angle_adjust_time=3
delta_angle=10
angle_pid_k=13

#主函数新增参数
find_id = 0
cal_symbol = 1 #1表示加法，2表示减法，3表示乘法，4表示除法
end_num = 12 #0表示11号点，1表示12号点，2表示13号点
shibie_flag = 0 #表示要去第几个识别点
shibieing_flag = 0 #表示正在识别
now_num_vlm = 1
find = [0,0,0,0,0,0,0]#代表每个识别点的识别结果
find_vlm = [-1,-1,-1,-1,-1,-1,-1]#大模型
position_end = 0
adjust_time_end = 6
voice_receive_flag = 0#表示是否接收到语音指令
operation = ""
number_str = ""
voice_ok = 0


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

def voice_callback(data):
    global cal_symbol,end_num,voice_receive_flag,operation,number_str
    operation = ""
    number_str = ""
    for char in data.data:
        if char.isdigit():
            number_str += char
        else:
            operation += char
    if operation == "加法":
        cal_symbol = 1
    elif operation == "减法":
        cal_symbol = 2
    elif operation == "乘法":
        cal_symbol = 3
    elif operation == "除法":
        cal_symbol = 4
    else:
        cal_symbol = 1
    end_num = int(number_str)
    voice_receive_flag = 1
    print("cal_symbol=",cal_symbol)
    print("end_num=",end_num)

def result_callback(msg):
    global find_vlm,find,now_num_vlm,find_vlm_min,find_vlm_max
    find_id_vlm = 0
    find_id_vlm = msg.data
    print("find_id_vlm",find_id_vlm)
    rospy.sleep(1)
    for i in range(1,7):
        if(find_vlm[i]==-1):
            find_vlm[i] = find_id_vlm
            now_num_vlm = i
            break
    if(find[now_num_vlm]==0):
        if(find_vlm[now_num_vlm]>=find_vlm_min and find_vlm[now_num_vlm]<=find_vlm_max):
            find[now_num_vlm]=find_vlm[now_num_vlm]
     
def navigation_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global navigation_pid_flag,x_dis,y_dis,x_angle,y_angle,x_sym,y_sym,integral_x,integral_y#第二个开始时间在main中修改
    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global angle_1,delta_angle,angle_pid_flag,angle_adjust_time,angle_pid_k,start_time_for_angle_and_nav,nav_and_angle_adjust_time
    dis_right = msg.ranges[angle_1-delta_angle]
    dis_left = msg.ranges[angle_1+delta_angle]
    dis_now = msg.ranges[angle_1]
    #rospy.loginfo("360_now:%f",np.min(msg.ranges[350:370]))
    #x_now_1 = np.min(msg.ranges[(720-delta_angle):719])
    #x_now_2 = np.min(msg.ranges[0:delta_angle])
    #x_now = np.min([x_now_1,x_now_2])
    #rospy.loginfo("0_now:%f",x_now)
    #rospy.loginfo("540_now:%f",np.min(msg.ranges[530:550]))
    #rospy.loginfo("180_now:%f",np.min(msg.ranges[170:190]))
    #rospy.loginfo("end_11:x=%f,y=%f",msg.ranges[360],msg.ranges[540])
    #rospy.loginfo("end_12:x=%f,y=%f",msg.ranges[0],msg.ranges[180]) 
    #rospy.loginfo("end_13:x=%f,y=%f",msg.ranges[360],msg.ranges[180]) 
    if(angle_pid_flag == 1 and navigation_pid_flag == 1):#flag=1表示开始角度调整
        x_now = np.min(msg.ranges[(x_angle-delta_angle):(x_angle+delta_angle)])
        y_now = np.min(msg.ranges[(y_angle-delta_angle):(y_angle+delta_angle)])
        start_time = start_time_for_angle_and_nav
        duration = Duration.from_sec(nav_and_angle_adjust_time)
        if(x_now<10 and y_now<10 and dis_now<10 and dis_left<10 and dis_right<10 and rospy.Time.now()-start_time<duration):
            delta_dis = dis_right-dis_left
            delta_x = 0
            delta_y = 0
            delta_x = x_now-x_dis
            delta_y = y_now-y_dis
            integral_x += delta_x
            integral_y += delta_y
            angle_vel=(angle_pid_k*(delta_dis))
            nav_x_vel = x_sym*(move_lidar_k_x*delta_x+move_lidar_i_x*integral_x)
            nav_y_vel = y_sym*(move_lidar_k_y*delta_y+move_lidar_i_y*integral_y)
            move_for_lidar_and_angle(nav_x_vel,nav_y_vel,angle_vel)
            
        if(rospy.Time.now()-start_time>=duration 
           or (rospy.Time.now()-start_time>=Duration.from_sec(3) and np.abs(delta_x)<=0.02 and np.abs(delta_y)<=0.02)):#如果任务点时间大于3s且偏差小于0.02m则停止pid
            #mission_pid_voice()
            navigation_pid_flag = 0
            angle_pid_flag = 0 
            integral_x = 0
            integral_y = 0

def recog_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global recog_pid_flag,recog_x,recog_y,x_angle,y_angle,x_sym,y_sym,integral_x,integral_y#第二个开始时间在main中修改
    global recog_pid_k_x,recog_pid_k_y,recog_pid_i_x,recog_pid_i_y,shibie_flag
    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global delta_angle,recog_pid_flag,recog_angle_pid_k,recog_start_time_for_angle_and_nav,recog_nav_and_angle_adjust_time
    #角度三个值
    dis_right = msg.ranges[360-delta_angle]
    dis_left = msg.ranges[360+delta_angle]
    dis_now = msg.ranges[360]
    if(recog_pid_flag == 1):
        x_dis = recog_x[shibie_flag]
        y_dis = recog_y[shibie_flag]
        x_angle = 360
        y_angle = 180
        x_now = np.min(msg.ranges[(x_angle-delta_angle):(x_angle+delta_angle)])
        y_now = np.min(msg.ranges[(y_angle-delta_angle):(y_angle+delta_angle)])
        start_time = recog_start_time_for_angle_and_nav
        duration = Duration.from_sec(recog_nav_and_angle_adjust_time)
        if(x_now<10 and y_now<10 and dis_now<10 and dis_left<10 and dis_right<10 and rospy.Time.now()-start_time<duration):#and np.abs(dis_left-dis_right)>angle_tolerance ):
            delta_dis = dis_right-dis_left
            delta_x = 0
            delta_y = 0
            delta_x = x_now-x_dis
            delta_y = y_now-y_dis
            integral_x += delta_x
            integral_y += delta_y
            angle_vel=(recog_angle_pid_k*(delta_dis))
            nav_x_vel = 1*(recog_pid_k_x*delta_x+recog_pid_i_x*integral_x)
            nav_y_vel = (-1)*(recog_pid_k_y*delta_y+recog_pid_i_y*integral_y)
            move_for_lidar_and_angle(nav_x_vel,nav_y_vel,angle_vel)
        if(rospy.Time.now()-start_time>=duration 
           or (rospy.Time.now()-start_time>=Duration.from_sec(3) and np.abs(delta_x)<=0.02 and np.abs(delta_y)<=0.02)):
            recog_pid_flag = 0
            integral_x = 0
            integral_y = 0

def end_pid_callback(msg):
    global end_pid_flag,x_dis,y_dis,x_angle,y_angle,x_sym,y_sym,integral_x,integral_y
    global angle_1,delta_angle,end_angle_adjust_time,end_nav_adjust_time,end_buff_time,start_time_for_end_pid,end_nav_pid_k_x,end_nav_pid_i_x,end_nav_pid_k_y,end_nav_pid_i_y,end_angle_pid_k

    if(end_pid_flag == 1):#flag=1表示开始角度调整
        dis_right = msg.ranges[angle_1-delta_angle]
        dis_left = msg.ranges[angle_1+delta_angle]
        dis_now = msg.ranges[angle_1]
        if (end_num == 12):
            x_now_1 = np.min(msg.ranges[(720-delta_angle):719])
            x_now_2 = np.min(msg.ranges[0:delta_angle])
            x_now = np.min([x_now_1,x_now_2])
        else:
            x_now = np.min(msg.ranges[(x_angle-delta_angle):(x_angle+delta_angle)])
        y_now = np.min(msg.ranges[(y_angle-delta_angle):(y_angle+delta_angle)])
        start_time = start_time_for_end_pid
        duration_1 = Duration.from_sec(end_angle_adjust_time)
        duration_2 = Duration.from_sec(end_angle_adjust_time + end_buff_time)
        duration_3 = Duration.from_sec(end_angle_adjust_time + end_buff_time + end_nav_adjust_time)
        if(dis_now<10 and dis_left<10 and dis_right<10 and np.abs(dis_left-dis_right)>angle_tolerance and rospy.Time.now()-start_time < duration_1):
            delta_dis = dis_right-dis_left
            move_for_lidar_and_angle(0,0,end_angle_pid_k*(delta_dis))
        if(x_now<10 and y_now<10 and rospy.Time.now()-start_time >= duration_2 and rospy.Time.now()-start_time < duration_3):
            delta_x = 0
            delta_y = 0
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


def angle_lidar_pid(msg):#根据激光雷达某一个角度的距离和相邻两侧角度的距离，控制改角度与墙面垂直,angle_1要在主函数更改
    global angle_1,delta_angle,angle_pid_flag,angle_adjust_time,angle_pid_k,start_time_for_angle_pid
    dis_right = msg.ranges[angle_1-delta_angle]
    dis_left = msg.ranges[angle_1+delta_angle]
    dis_now = msg.ranges[angle_1]
    if(angle_pid_flag == 1):#flag=1表示开始角度调整
        start_time = start_time_for_angle_pid
        duration = Duration.from_sec(angle_adjust_time)
        if(dis_now<10 and dis_left<10 and dis_right<10 and np.abs(dis_left-dis_right)>angle_tolerance ):
            delta_dis = dis_right-dis_left
            move_for_angle(angle_pid_k*(delta_dis))
        if(rospy.Time.now()-start_time>=duration):
            angle_pid_flag = 0

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
        self.ar_sub = rospy.Subscriber('/object_position', Point, self.ar_cb)
        self.ar_cb2 = rospy.Subscriber('/vlm_node/result', Int32, result_callback)
        self.mission_sub = rospy.Subscriber('/scan', LaserScan, navigation_pid_callback)
        self.end_sub = rospy.Subscriber('/scan', LaserScan, end_pid_callback)
        self.recog_sub = rospy.Subscriber('/scan', LaserScan, recog_pid_callback)
        self.voice_sub = rospy.Subscriber('/voice_result', String, voice_callback)
        self.move_base = actionlib.SimpleActionClient("move_base", MoveBaseAction)
        self.move_base.wait_for_server(rospy.Duration(60))
    
    def ar_track_cb(self,data):
        #print ("zhixing AR callback")
        global find,shibieing_flag,shibie_flag
        find_id_ar = 0
        for marker in data.markers:
            find_id_ar = marker.id
        if(find_id_ar <=8 and find_id_ar>=1 and shibieing_flag == 1):
            find[shibie_flag] = find_id_ar
            shibieing_flag = 0

    def ar_cb(self, data):#没问题
        #print ("zhixing hanzi callback")
        global find,shibie_flag,shibieing_flag
        point_msg = data
        find_id_cb = 0
        find_id_cb = id_match(point_msg)
        if(find_id_cb <=8 and find_id_cb>=1 and shibieing_flag == 1):
            find[shibie_flag] = find_id_cb
            shibieing_flag = 0
              

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

def cal_id():#根据不同的运算模式来计算find_id，当作脚本文件执行
    global find,cal_symbol,find_id,shibie_flag
    if(cal_symbol==1):
        find_id = find[shibie_flag]+find[shibie_flag-1]
    elif(cal_symbol==2):
        find_id = find[shibie_flag]-find[shibie_flag-1]
    elif(cal_symbol==3):
        find_id = find[shibie_flag]*find[shibie_flag-1]
    elif(cal_symbol==4):
        find_id = find[shibie_flag]/find[shibie_flag-1]

    if(find_id>9):
        find_id = 9
    print("find_id=",find_id)

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

def talker():
    global voice_receive_flag
    rate = rospy.Rate(1)  # 1hz
    while (voice_receive_flag == 0):
        msg = Int32()
        msg.data = 1
        rospy.loginfo("Publishing wakeup message")
        rate.sleep()

def shibie_voice():
    global find,shibie_flag,pub_voice,voice_ok
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "识别结果为"+str(find[shibie_flag])
    rospy.loginfo("发送文本转语音："+msg.data)
    pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.2)

def mission_voice():
    global find,shibie_flag,operation,find_id,pub_voice,voice_ok
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    operate = operation.replace('法','')
    msg.data = str(find[shibie_flag])+operate+str(find[shibie_flag-1])+"等于"+str(find_id)+"，导航到"+str(find_id)+"点"
    pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.2)

def end_voice():
    global end_num,pub_voice
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "导航到"+str(end_num)+"点，比赛结束"
    pub_voice.publish(msg)

def mission_pid_voice():
    global find_id,voice_ok,pub_voice
    rospy.sleep(2)
    voice_ok = 0
    rate = rospy.Rate(1)  # 1hz
    msg = String()
    msg.data = "已导航到"+str(find_id)+"点"
    pub_voice.publish(msg)
    while(voice_ok == 0):
        rospy.sleep(0.2)

def voice_ok_callback(msg):
    global voice_ok
    voice_ok = 1

if __name__ == "__main__":
    global pub_voice
    rospy.init_node('navigation_demo',anonymous=True)
    pub_voice = rospy.Publisher('/main_voiceWords', String, queue_size=10)
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
    start_time_for_angle_and_nav =rospy.Time.now()
    start_time_for_end = rospy.Time.now()
    recog_start_time_for_angle_and_nav = rospy.Time.now()
    if (True):
        talker()
        move2end(start_x_v,start_y_v,start_time)  #一步到位
        rospy.sleep(0.1)
        move_rotate_180(rotate_time1) #起点开始的强制旋转
        shibie_flag = 1
        navi.goto(goalshibie[1])
        recog_pid_flag = 1
        recog_start_time_for_angle_and_nav = rospy.Time.now()
        while(recog_pid_flag == 1):
            rospy.sleep(0.2)
        rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
        shibieing_flag = 1
        position_end = 0 #是否到达终点
        end_pid_flag = 0
        find_id = 0 
        
        while(position_end == 0):
            #print("shibie_flag=",shibie_flag)
            if(shibie_flag >= 7):
                navi.goto(goalend[1])
                start_time_for_end_pid = rospy.Time.now()#表示当前为开始执行的时间
                x_sym = end_x_sym[end_num-11]
                y_sym = end_y_sym[end_num-11]
                x_dis = end_dis_x[end_num-11]
                y_dis = end_dis_y[end_num-11]
                x_angle = end_angle_x[end_num-11]
                y_angle = end_angle_y[end_num-11]
                end_pid_flag = 1
                rospy.sleep(end_angle_adjust_time + end_buff_time + end_nav_adjust_time)
                end_voice()
                position_end = 1
                
            if(shibie_flag == 1):
                if(find[shibie_flag] >= find_vlm_min and find[shibie_flag] <= find_vlm_max):
                    shibie_voice()
                    print("find",find[shibie_flag])
                    shibie_flag = 2
                    navi.goto(goalshibie[shibie_flag])
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 2):
                if(find[shibie_flag] >= find_vlm_min and find[shibie_flag] <= find_vlm_max):
                    shibie_voice()
                    print("find",find[shibie_flag])
                    cal_id()
                    mission_voice()
                    navi.goto(goals[find_id-1])
                    #####################
                    start_time_for_angle_and_nav = rospy.Time.now()#表示当前为开始执行的时间
                    x_sym = x_sym_miss[find_id-1]
                    y_sym = y_sym_miss[find_id-1]
                    x_dis = x_dis_miss[find_id-1]
                    y_dis = y_dis_miss[find_id-1]
                    y_angle = y_angle_miss[find_id-1]
                    angle_pid_flag = 1
                    navigation_pid_flag = 1
                    mission_pid_voice()
                    while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                        rospy.sleep(0.2)
                    
                    ##上面是导航pid
                    shibie_flag = 3
                    navi.goto(goalshibie[shibie_flag])
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1
            
            if(shibie_flag == 3):
                if(find[shibie_flag] >= find_vlm_min and find[shibie_flag] <= find_vlm_max):
                    shibie_voice()
                    print("find",find[shibie_flag])
                    shibie_flag = 4
                    navi.goto(goalshibie[shibie_flag])
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 4):
                if(find[shibie_flag] >= find_vlm_min and find[shibie_flag] <= find_vlm_max):
                    shibie_voice()
                    print("find",find[shibie_flag])
                    cal_id()
                    mission_voice()
                    #navi.goto(goals[find_id-1])
                    ###################
                    start_time_for_angle_and_nav = rospy.Time.now()#表示当前为开始执行的时间
                    x_sym = x_sym_miss[find_id-1]
                    y_sym = y_sym_miss[find_id-1]
                    x_dis = x_dis_miss[find_id-1]
                    y_dis = y_dis_miss[find_id-1]
                    y_angle = y_angle_miss[find_id-1]
                    angle_pid_flag = 1
                    navigation_pid_flag = 1
                    mission_pid_voice()
                    while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                        rospy.sleep(0.2)
                    ##上面是导航pid
                    shibie_flag = 5
                    navi.goto(goalshibie[shibie_flag])
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 5):
                if(find[shibie_flag] >= find_vlm_min and find[shibie_flag] <= find_vlm_max):
                    shibie_voice()
                    print("find",find[shibie_flag])
                    shibie_flag = 6
                    navi.goto(goalshibie[shibie_flag])
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 6):
                if(find[shibie_flag] >= find_vlm_min and find[shibie_flag] <= find_vlm_max):
                    shibie_voice()
                    print("find",find[shibie_flag])
                    cal_id()
                    mission_voice()
                    #navi.goto(goals[find_id-1])
                    ###################
                    start_time_for_angle_and_nav = rospy.Time.now()#表示当前为开始执行的时间
                    x_sym = x_sym_miss[find_id-1]
                    y_sym = y_sym_miss[find_id-1]
                    x_dis = x_dis_miss[find_id-1]
                    y_dis = y_dis_miss[find_id-1]
                    y_angle = y_angle_miss[find_id-1]
                    angle_pid_flag = 1
                    navigation_pid_flag = 1
                    mission_pid_voice()
                    while(angle_pid_flag == 1 and navigation_pid_flag == 1):
                        rospy.sleep(0.2)
                    ##上面是导航pid
                    shibie_flag = 7

                
            
            else :
                rospy.sleep(0.01)
        if(position_end==1):
            start_time_for_end=rospy.Time.now()
    while not rospy.is_shutdown():
        r.sleep()







