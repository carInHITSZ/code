#!/usr/bin/env python
# -*- coding: utf-8 -*-
#coding: utf-8

import rospy
import math
import os
import actionlib
import tf2_ros
import tf2_geometry_msgs
from geometry_msgs.msg import TransformStamped
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
from collections import deque
import sys
import re
import numpy as np
from nav_msgs.msg import OccupancyGrid
reload(sys)
sys.setdefaultencoding('utf-8')
passed_recgonition = [0,0,0,0]
counter = 0 #计数器
is_0_or_180 = 0 #指示小车目前是0度还是180度，0表示0度，1表示180度
point_msg = 255
tf_buffer = None
tf_listener = None

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
total_tolerance_angle = 0.02

start_x_v = 0.16  #起点开始的一步到位
start_y_v = -0.11
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
x_angle_nav = 0
y_angle_nav = 0

centre_pid_flag = 0
centre_start_time_for_angle_and_nav = 0
x_dis_c = 0.170
y_dis_c = 0.180
centre_angle_pid_k = 15

recog_pid_i_x = 0.0
recog_pid_i_y = 0.0
recog_angle_pid_k = 25#32#原来是17
#recog_x = [0.0 , 0.432 , 0.382 , 0.382 , 0.382 , 0.382 , 0.382 ,
#           0.382 , 0.382 , 0.382 , 0.382 , 0.382 , 0.382 ]##识别点x

recog_x = [0.0 , 0.432 , 0.402 , 0.402 , 0.402 , 0.402 , 0.402 ,
           0.402 , 0.402 , 0.402 , 0.402 , 0.402 , 0.402 ]##识别点x,标号从0到4，0空置



recog_y = [0.0 , 1.796 , 1.796 , 1.796 ,1.796, 1.808 , 0.584,
                      0.598 , 1.824 , 0.618 , 0.570, 1.802 , 0.616 ]##识别点y，根据雷达180来改
x_sym_recog = [1, 1,1,1,  1,1,1,  1,1,1,  1,1,1]
y_sym_recog = [1, -1, -1, -1, -1, -1, 1, 1, 1, -1, 1, 1,- 1]


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

angle_l = 540
angle_r = 180   #540为左方，180为右方
speed = 0.2

edge_occupied = {}

#终点的中转点lr坐标

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


x_angle = 360 #x_angle是朝前的固定360
y_angle = 180
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
delta_angle_angle=8#10
delta_angle_dis=5
delta_angle_lr=5
delta_angle_fb=3
angle_pid_k=12 #13

#主函数新增参数
find_id = [0, 0, 0, 0]
answer_id = 0
end_num = 12 #0表示11号点，1表示12号点，2表示13号点
shibie_flag = 0 #表示要去第几个识别点
mission_now = 0#要去第几个任务点
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

def centre_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global centre_pid_flag,x_angle_centre,y_angle_centre,x_sym,y_sym#第二个开始时间在main中修改
    global centre_pid_k_x,centre_pid_k_y

    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global delta_angle,delta_angle_angle,delta_angle_dis,recog_angle_pid_k,centre_start_time_for_angle_and_nav,centre_nav_and_angle_adjust_time
    #角度三个值
    '''pose = get_pose_from_tf()
    if pose is not None:
        rospy.loginfo("Current pose: x=%.2f, y=%.2f, theta=%.1f°", pose[0], pose[1], pose[2])
    else:
        rospy.logerr("Failed to get pose from tf")
    '''
    dis_right1 = msg.ranges[530]
    dis_left1 = msg.ranges[550]

    dis_right2 = msg.ranges[170]
    dis_left2 = msg.ranges[190]

    dis_right3 = msg.ranges[350]
    dis_left3 = msg.ranges[370]

    dis_right4 = msg.ranges[710]
    dis_left4 = msg.ranges[10]

    delta_x = 0
    delta_y = 0
    if(centre_pid_flag == 1):
        x_now = np.min(msg.ranges[(350):(370)])
        y_now = np.min(msg.ranges[(530):(550)])
        start_time = centre_start_time_for_angle_and_nav
        duration = Duration.from_sec(12)

        if( rospy.Time.now()-start_time<duration):#and np.abs(dis_left-dis_right)>angle_tolerance ):
            delta_dis1 = (dis_right1-dis_left1)/dis_left1
            delta_dis2 = (dis_right2-dis_left2)/dis_left2
            delta_dis3 = (dis_right3-dis_left3)/dis_left3
            delta_dis4 = (dis_right4-dis_left4)/dis_left4
            delta_dis = min([delta_dis1, delta_dis2, delta_dis3, delta_dis4], key=abs)

            delta_x = x_now % 0.4 - x_dis_c
            delta_y = y_now % 0.4 - y_dis_c
            '''
            angle_vel=(recog_angle_pid_k*(delta_dis))
            if angle_vel > 0:
                angle_vel += 0.6
            else:
                angle_vel -= 0.6
            '''
            angle_vel=(centre_angle_pid_k*(delta_dis))

            nav_x_vel = (recog_pid_k_x*delta_x)
            nav_y_vel = (recog_pid_k_y*delta_y)
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
           or (rospy.Time.now()-start_time>=Duration.from_sec(4) and np.abs(delta_x)<=0.05 and np.abs(delta_y)<=0.05 and np.abs(delta_dis) < total_tolerance_angle)):
            
            centre_pid_flag = 0



def recog_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global recog_pid_flag,recog_x,recog_y,x_angle,y_angle,x_sym,y_sym#第二个开始时间在main中修改
    global recog_pid_k_x,recog_pid_k_y,recog_pid_i_x,recog_pid_i_y,shibie_flag

    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    global delta_angle,delta_angle_angle,delta_angle_dis,recog_pid_flag,recog_angle_pid_k,recog_start_time_for_angle_and_nav,recog_nav_and_angle_adjust_time
    #角度三个值


    x_now_1 = np.min(msg.ranges[710:719])
    x_now_2 = np.min(msg.ranges[0:10])
    x_now = np.min([x_now_1,x_now_2])

    '''
    rospy.loginfo("360_now:%f",np.min(msg.ranges[350:370]))
    rospy.loginfo("0_now:%f",x_now)
    rospy.loginfo("540_now:%f",np.min(msg.ranges[530:550]))
    rospy.loginfo("180_now:%f",np.min(msg.ranges[170:190]))
    rospy.loginfo("360=%f,540=%f",msg.ranges[360],msg.ranges[540])
    '''

    
    dis_right = msg.ranges[360-delta_angle_angle]
    dis_left = msg.ranges[360+delta_angle_angle]
    dis_now = msg.ranges[360]

    delta_x = 0
    delta_y = 0
    if(recog_pid_flag == 1):
        x_angle = 360
        y_angle = 180
        x_now = np.min(msg.ranges[(x_angle-delta_angle_dis):(x_angle+delta_angle_dis)])
        y_now = np.min(msg.ranges[(y_angle-delta_angle_dis):(y_angle+delta_angle_dis)])
        start_time = recog_start_time_for_angle_and_nav
        duration = Duration.from_sec(recog_nav_and_angle_adjust_time)

        if(x_now<10 and y_now<10 and dis_now<10 and dis_left<10 and dis_right<10 and rospy.Time.now()-start_time<duration):#and np.abs(dis_left-dis_right)>angle_tolerance ):
            delta_dis = (dis_right-dis_left)/dis_now

            delta_x = x_now-x_dis
            delta_y = y_now-y_dis
            angle_vel=(recog_angle_pid_k*(delta_dis))
            '''
            if angle_vel > 0:
                angle_vel += 0.6
            else:
                angle_vel -= 0.6
            '''
            nav_x_vel =  1*(recog_pid_k_x*delta_x)
            nav_y_vel = -1*(recog_pid_k_y*delta_y)
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

def scan_callback(msg):
    global latest_scan, scan_received
    latest_scan = msg
    scan_received = True

class navigation_demo():
    def __init__(self):
        self.set_pose_pub = rospy.Publisher('/initialpose', PoseWithCovarianceStamped, queue_size=5)
        self.arrive_pub = rospy.Publisher('/voiceWords',String,queue_size=10)
        self.ar_sub1 = rospy.Subscriber('/ar_pose_marker',AlvarMarkers,self.ar_track_cb)
        #self.ar_sub = rospy.Subscriber('/object_position', Point, self.ar_cb)
        self.ar_cb2 = rospy.Subscriber('/vlm_node/result', Int32, result_callback)
        self.ar_cb3 = rospy.Subscriber('/vlm_node/result_2', String, result_callback_2)   
        self.recog_sub = rospy.Subscriber('/scan', LaserScan, recog_pid_callback)
        self.voice_sub = rospy.Subscriber('/voice_result', String, voice_callback)
        self.move_base = actionlib.SimpleActionClient("move_base", MoveBaseAction)
        self.move_base.wait_for_server(rospy.Duration(60))
        self.scan = rospy.Subscriber('/scan', LaserScan, scan_callback)
        self.centre_cb =rospy.Subscriber('/scan', LaserScan, centre_pid_callback)
    
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
              

    def set_ini_pose(self):
        if self.move_base is None:
            return False

        pose = PoseWithCovarianceStamped()
        pose.header.frame_id = 'map'
        pose.header.stamp = rospy.Time.now()
        pose.pose.pose.position.x = -1.6
        pose.pose.pose.position.y =  1.6
        quat = transformations.quaternion_from_euler(0, 0, 0)  # 偏航角0度
        pose.pose.pose.orientation.x = quat[0]
        pose.pose.pose.orientation.y = quat[1]
        pose.pose.pose.orientation.z = quat[2]
        pose.pose.pose.orientation.w = quat[3]
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

def init_tf_listener():
    """初始化 tf2 监听器，应在节点启动后调用一次"""
    global tf_buffer, tf_listener
    tf_buffer = tf2_ros.Buffer()
    tf_listener = tf2_ros.TransformListener(tf_buffer)

def get_pose_from_tf():
    """
    获取机器人当前在 map 坐标系下的位姿。
    :return: (x, y, theta_deg) 或 None
    """
    if tf_buffer is None:
        rospy.logerr("tf_buffer not initialized. Call init_tf_listener() first.")
        return None

    try:
        # 直接使用 lookup_transform，并指定超时时间（0.5秒）
        # 这就是在 ROS 1 中等待变换的标准方法
        trans = tf_buffer.lookup_transform('map', 'base_footprint', rospy.Time(0), rospy.Duration(0.5))
        
        x = trans.transform.translation.x
        y = trans.transform.translation.y
        
        q = trans.transform.rotation
        # 确保四元数格式正确
        _, _, yaw_rad = transformations.euler_from_quaternion([q.x, q.y, q.z, q.w])
        theta_deg = math.degrees(yaw_rad)
        
        return x, y, theta_deg
    except (tf2_ros.LookupException, tf2_ros.ConnectivityException, 
            tf2_ros.ExtrapolationException) as e:
        rospy.logwarn("Failed to get pose from tf: %s", e)
        return None
    
def grid_centers_to_world(grid_ids):
    """
    将格子编号（1-81）转换为对应格子左上角角点的世界坐标。
    编号规则：左上角为1，从左到右，从上到下递增。
    世界坐标系：原点在左上角，x向右，y向下为正。
    :param grid_ids: list of int，格子编号列表
    :return: list of (x, y)，每个格子左上角角点的世界坐标
    """
    coords = []
    for grid_id in grid_ids:
        # 行列索引 0-based，从上到下，从左到右
        row = (grid_id - 1) // 9   # 行号 0..8
        col = (grid_id - 1) % 9    # 列号 0..8
        # 格子左上角角点坐标
        x = (col + 0.5) * 0.4
        y = -(row + 0.5) * 0.4  
        coords.append((x, y))
    return coords

def update_obstacle(pose, move_dir, laser_dist):
    """
    根据检测到的障碍物，更新本地占用图。
    :param pose: (x, y, theta) 机器人精确位姿
    :param move_dir: 'E','W','S','N' 当前移动方向
    :param laser_dist: 检测到的障碍物距离（米）
    """
    # 计算机器人中心坐标，前进方向
    x, y, _ = pose
    # 根据方向计算障碍物位置（假设障碍物位于正前方 laser_dist 处）
    if move_dir == 'E':
        obs_x = x + laser_dist - 0.178
        obs_y = -y
        # 确定障碍物所在的垂直边（东边），即格子 (i,j) 的东边
        i = int(round(obs_x / 0.4))
        j = int(round(obs_y / 0.4))
        if 0 <= i < 9 and 0 <= j < 9:
            edge_occupied[(i, j, 'E')] = True
            # 同时标记相邻格子的西边
            if i+1 < 9:
                edge_occupied[(i+1, j, 'W')] = True
    elif move_dir == 'W':
        obs_x = x - laser_dist + 0.178
        obs_y = y
        i = int(obs_x // 0.4)
        j = int(obs_y // 0.4)
        if 0 <= i < 9 and 0 <= j < 9:
            edge_occupied[(i, j, 'W')] = True
            if i-1 >= 0:
                edge_occupied[(i-1, j, 'E')] = True
    elif move_dir == 'N':
        obs_x = x
        obs_y = y + laser_dist - 0.178
        i = int(obs_x // 0.4)
        j = int(obs_y // 0.4)
        if 0 <= i < 9 and 0 <= j < 9:
            edge_occupied[(i, j, 'N')] = True
            if j+1 < 9:
                edge_occupied[(i, j+1, 'S')] = True
    elif move_dir == 'S':
        obs_x = x
        obs_y = y - laser_dist + 0.178
        i = int(obs_x // 0.4)
        j = int(obs_y // 0.4)
        if 0 <= i < 9 and 0 <= j < 9:
            edge_occupied[(i, j, 'S')] = True
            if j-1 >= 0:
                edge_occupied[(i, j-1, 'N')] = True

def plan_path(current_grid, target_grid):
    """
    BFS 在格子图上规划路径，返回格子序号列表（包括起点和终点）。
    返回的路径会压缩连续同方向的中间格子，只保留转折点。
    """
    from collections import deque

    # 构建邻接关系：每个格子有四个邻居，但需检查边是否被占用
    def neighbors(gid):
        i = (gid - 1) % 9
        j = (gid - 1) // 9
        dirs = [('E', 1, 0), ('W', -1, 0), ('N', 0, 1), ('S', 0, -1)]
        for d, di, dj in dirs:
            ni = i + di
            nj = j + dj
            if 0 <= ni < 9 and 0 <= nj < 9:
                neighbor_id = nj * 9 + ni + 1
                # 检查边是否被占用
                if not edge_occupied.get((i, j, d), False):
                    yield neighbor_id

    # BFS
    queue = deque([current_grid])
    parent = {current_grid: None}
    while queue:
        cur = queue.popleft()
        if cur == target_grid:
            break
        for nb in neighbors(cur):
            if nb not in parent:
                parent[nb] = cur
                queue.append(nb)

    if target_grid not in parent:
        return None

    # 回溯原始路径
    raw_path = []
    cur = target_grid
    while cur is not None:
        raw_path.append(cur)
        cur = parent[cur]
    raw_path.reverse()   # 从起点到终点

    # 如果路径长度小于等于2，无需简化
    if len(raw_path) <= 2:
        return raw_path

    # 辅助：将格子序号转为坐标 (x, y) 索引 (col, row)
    def idx_to_xy(gid):
        col = (gid - 1) % 9
        row = (gid - 1) // 9
        return col, row

    # 压缩连续同方向段
    simplified = [raw_path[0]]
    for i in range(1, len(raw_path) - 1):
        prev = raw_path[i-1]
        curr = raw_path[i]
        nxt = raw_path[i+1]

        # 计算方向向量 (dx, dy)
        x_prev, y_prev = idx_to_xy(prev)
        x_curr, y_curr = idx_to_xy(curr)
        x_nxt, y_nxt = idx_to_xy(nxt)

        dx1 = x_curr - x_prev
        dy1 = y_curr - y_prev
        dx2 = x_nxt - x_curr
        dy2 = y_nxt - y_curr

        # 如果两段方向相同（非零向量相等），则当前点可省略
        if (dx1, dy1) == (dx2, dy2):
            continue
        else:
            simplified.append(curr)

    simplified.append(raw_path[-1])
    return simplified

def test_rotate(duration_per_step, num_steps, angular_inc):
    """
    测试旋转指令：依次发布角速度从 angular_inc 开始，每次增加 angular_inc，共 num_steps 次。
    每个速度持续 duration_per_step 秒。
    :param duration_per_step: 每个速度持续的时间（秒）
    :param num_steps: 指令次数（即测试多少个不同的角速度）
    :param angular_inc: 角速度增量（rad/s）
    """
    rospy.loginfo("Starting rotation test: %d steps, each lasting %.1f sec, angular increment %.2f rad/s",
                  num_steps, duration_per_step, angular_inc)
    for i in range(1, num_steps+1):
        speed = i * angular_inc
        rospy.loginfo("Publishing angular speed: %.2f rad/s for %.1f seconds", speed, duration_per_step)
        time = rospy.Time.now()
        while(rospy.Time.now()-time<Duration.from_sec(duration_per_step)):
            move_for_lidar_and_angle(0.0, 0.0, speed)
        rospy.sleep(2)
        
    # 停止
    move_for_lidar_and_angle(0.0, 0.0, 0.0)
    rospy.loginfo("Rotation test finished")

def get_precise_pose():
    """
    获取机器人在场地中的精确位姿，返回 map 坐标系下的坐标。
    场地坐标系：左上角为 (0,0)，X 轴向右，Y 轴向下为正，尺寸 3.6×3.6 m。
    激光雷达坐标系：0° 为小车后方，逆时针增大，右方180°，前方360°，左方540°。
    假设小车始终朝西（X 负方向），且已通过 calibrate_grid_origin 校准。
    :return: (x, y, theta_deg) 精确位姿（map 坐标系），若失败返回 None
    """
    global latest_scan, map_origin_x, map_origin_y
    if not scan_received or latest_scan is None:
        rospy.logwarn("No laser scan received yet")
        return None
    if map_origin_x is None or map_origin_y is None:
        rospy.logerr("get_precise_pose called before calibration")
        return None

    # 1. 从 tf 获取粗略位姿（map 坐标系）
    try:
        trans = tf_buffer.lookup_transform('map', 'base_footprint', rospy.Time(0), rospy.Duration(0.5))
        x_tf = trans.transform.translation.x
        y_tf = trans.transform.translation.y
        q = trans.transform.rotation
        _, _, yaw_rad = transformations.euler_from_quaternion([q.x, q.y, q.z, q.w])
        theta_deg = math.degrees(yaw_rad)
    except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
        rospy.logwarn("TF lookup failed: %s", e)
        return None

    # 2. 转换到场地坐标系（左上角原点，X右，Y下）
    field_x = x_tf - map_origin_x
    field_y = y_tf - map_origin_y

    # 3. 根据粗略位姿确定所在格子（行列索引）
    col_tf = int(field_x // 0.4)
    row_tf = int(field_y // 0.4)
    col_tf = max(0, min(8, col_tf))
    row_tf = max(0, min(8, row_tf))

    # 格子中心在场地坐标系中的坐标
    center_field_x = col_tf * 0.4 + 0.2
    center_field_y = -row_tf * 0.4 - 0.2

    # 4. 激光雷达参数（固定：0°后，180°右，360°前，540°左，每0.5°一点，共720点）
    total_points = len(latest_scan.ranges)   # 应为720
    half_steps = int(10.0 / 0.5)             # ±10° 对应 ±20 个点

    def get_min_distance_at_angle(angle_deg):
        idx_center = int(angle_deg / 0.5) % total_points
        min_dist = float('inf')
        for step in range(-half_steps, half_steps+1):
            idx = (idx_center + step) % total_points
            d = latest_scan.ranges[idx]
            if np.isfinite(d) and d > 0.05:
                if d < min_dist:
                    min_dist = d
        return min_dist if min_dist != float('inf') else None

    # 测量四个方向的距离（小车朝西，固定映射）
    dist_back  = get_min_distance_at_angle(0)    # 后方 → 东
    dist_right = get_min_distance_at_angle(180)  # 右方 → 南
    dist_front = get_min_distance_at_angle(360)  # 前方 → 西
    dist_left  = get_min_distance_at_angle(540)  # 左方 → 北

    # 5. 计算偏移（模运算）
    def compute_offset(dist):
        if dist is None:
            return None
        return dist % 0.4 -0.180

    offset_x = None
    offset_y = None
    if dist_front is not None:          # 西方向
        offset_x = compute_offset(dist_front)
    elif dist_back is not None:         # 东方向
        off = compute_offset(dist_back)
        offset_x = -off if off is not None else None
    if dist_right is not None:          # 南方向
        offset_y = compute_offset(dist_right)
    elif dist_left is not None:         # 北方向
        off = compute_offset(dist_left)
        offset_y = -off if off is not None else None

    # 精确场地坐标
    field_x = center_field_x + (offset_x if offset_x is not None else 0)
    field_y = center_field_y + (offset_y if offset_y is not None else 0)

    # 边界裁剪
    field_x = max(0.0, min(3.6, field_x))
    field_y = max(0.0, min(3.6, field_y))

    # 转换回 map 坐标
    map_x = map_origin_x + field_x
    map_y = map_origin_y + field_y

    return map_x, map_y, theta_deg

def calibrate_grid_origin():
    """
    在机器人位于 1 号格子中心时调用，获取当前机器人的 map 位姿，
    建立场地格子坐标系与 map 坐标系的平移关系。
    :return: bool 是否校准成功
    """
    global map_origin_x, map_origin_y
    pose = get_pose_from_tf()   # 返回 (x, y, theta_deg)
    if pose is None:
        rospy.logerr("Calibration failed: cannot get pose from tf")
        return False
    map_origin_x = pose[0]
    map_origin_y = pose[1]
    rospy.loginfo("Calibration successful: 1号格子中心在 map 坐标系中的位置 = (%.3f, %.3f)", 
                  map_origin_x, map_origin_y)
    return True

def grid_center_to_world(grid_id):
    """
    将格子编号（1-81）转换为 map 坐标系下的格子中心坐标。
    前提：已调用 calibrate_grid_origin() 完成校准。
    编号规则：左上角为1，从左到右，从上到下递增。
    场地坐标系：左上角为 (0,0)，X 向右，Y 向下为正，格子边长 0.4 m。
    :param grid_id: int 格子序号
    :return: (x, y) map 坐标系下的坐标，可直接用于 navi.goto
    """
    if map_origin_x is None or map_origin_y is None:
        rospy.logerr("grid_center_to_world called before calibration")
        return None
    # 行列索引 0-based，从上到下，从左到右
    row = (grid_id - 1) // 9   # 行号 0..8
    col = (grid_id - 1) % 9    # 列号 0..8
    # 在场地坐标系中的理论坐标（格子中心）
    field_x = col * 0.4 
    field_y = - row * 0.4 
    # 转换为 map 坐标系（假设方向一致，仅平移）
    map_x = map_origin_x + field_x + 0.02
    map_y = map_origin_y + field_y
    return map_x, map_y

def pose_to_grid_id(pose):
    """
    将机器人当前位姿（map 坐标系）转换为所在的格子序号（1-81）。
    前提：已调用 calibrate_grid_origin() 完成校准。
    编号规则：左上角为1，从左到右，从上到下递增。
    场地坐标系：左上角为 (0,0)，X 向右，Y 向下为正，格子边长 0.4 m。
    :param pose: (x, y, theta_deg) 机器人位姿，在 map 坐标系下
    :return: int 格子序号（1-81），若超出场地范围则返回 None
    """
    global map_origin_x, map_origin_y
    if map_origin_x is None or map_origin_y is None:
        rospy.logerr("pose_to_grid_id called before calibration")
        return None

    x_map, y_map, _ = pose
    # 转换到场地坐标系（左上角原点，X 向右，Y 向下）
    field_x = x_map - map_origin_x
    field_y = y_map - map_origin_y

    # 边界检查（场地范围 0~3.6）
    if field_x < 0 or field_x > 3.6 or field_y > 0 or field_y < -3.6:
        rospy.logwarn("Robot out of field: (%.2f, %.2f)", field_x, field_y)
        return None

    # 计算格子索引

    col = int(round(field_x / 0.4))
    row = int(round(-field_y / 0.4))

    # 边界保护（避免浮点误差导致索引为9）
    col = min(col, 8)
    row = min(row, 8)

    # 格子序号：左上角为 1，行优先
    grid_id = row * 9 + col + 1
    return grid_id

def navigate_to_target(target_grid_id):
    global edge_occupied,centre_pid_flag,centre_start_time_for_angle_and_nav

    SLEEP_TIME = 0.1   # 每步暂停时间（秒）


    centre_pid_flag = 1
    centre_start_time_for_angle_and_nav = rospy.Time.now()
    while centre_pid_flag == 1:
        rospy.sleep(0.2)

    current_pose = get_pose_from_tf()
    if current_pose is None:
        rospy.logerr("Failed to get initial pose")
        return
    current_grid = pose_to_grid_id(current_pose)

    rospy.loginfo("Initial pose: (%.2f, %.2f, %.1f°), grid=%d", 
                  current_pose[0], current_pose[1], current_pose[2], current_grid)
    rospy.sleep(SLEEP_TIME)

    max_replan_count = 10
    replan_count = 0

    while not rospy.is_shutdown():
        rospy.loginfo("=== Main loop: current_grid=%d, target_grid=%d ===", current_grid, target_grid_id)
        if current_grid == target_grid_id:
            rospy.loginfo("Reached target grid!")
            break

        # 规划路径
        path = plan_path(current_grid, target_grid_id)
        if not path:
            rospy.logerr("No path found from %d to %d", current_grid, target_grid_id)
            break
        rospy.loginfo("Planned path: %s", path)
        rospy.sleep(SLEEP_TIME)

        # 转换为世界坐标
        waypoints = [grid_center_to_world(gid) for gid in path]
        rospy.loginfo("Waypoints: %s", waypoints)

        replan = False
        for i in range(len(waypoints)-1):
            start_w = waypoints[i]
            end_w = waypoints[i+1]
            if abs(end_w[0] - start_w[0]) > 0.01:
                direction = 'E' if end_w[0] > start_w[0] else 'W'
            else:
                direction = 'N' if end_w[1] > start_w[1] else 'S'
            rospy.loginfo("Segment %d: from (%.2f,%.2f) to (%.2f,%.2f), direction=%s", 
                          i, start_w[0], start_w[1], end_w[0], end_w[1], direction)
            rospy.sleep(SLEEP_TIME)

            success = move_segment(start_w, end_w, direction)
            rospy.loginfo("move_segment returned %s", success)

            # 段后更新位姿
            current_pose = get_pose_from_tf()
            if current_pose is None:
                rospy.logwarn("Cannot get pose after segment, continue")
                continue
            current_grid = pose_to_grid_id(current_pose)
            rospy.loginfo("After segment, pose: (%.2f,%.2f,%.1f°), grid=%d", 
                          current_pose[0], current_pose[1], current_pose[2], current_grid)
            rospy.sleep(SLEEP_TIME)

            if current_grid == target_grid_id:
                rospy.loginfo("Reached target during segment execution")
                return

            if not success:
                replan = True
                break

        if replan:
            replan_count += 1
            rospy.logwarn("Replanning, count=%d", replan_count)
            if replan_count > max_replan_count:
                rospy.logerr("Too many replans, abort")
                break
            # 继续循环，不需要额外更新 current_grid（已在段后更新）
            continue
        else:
            if current_grid == target_grid_id:
                rospy.loginfo("Reached target after finishing path")
            else:
                rospy.logwarn("Path finished but not at target, current_grid=%d", current_grid)
            break

def move_segment(start_world, end_world, direction):
    """
    执行一段直线移动，从 start_world 到 end_world（均为格子中心世界坐标）。
    返回 True 表示成功到达，False 表示中途遇到障碍需要重规划。
    """
    global centre_pid_flag,centre_start_time_for_angle_and_nav
    # 计算移动速度（世界坐标系）
    if direction in ('E', 'W'):
        speed = 0.3
        speed_x = -speed if direction == 'E' else speed
        speed_y = 0.0
    else:
        speed = 0.2
        speed_y = -speed if direction == 'N' else speed
        speed_x = 0.0

    # 根据移动方向确定前进方向对应的激光雷达角度（小车始终朝西）
    # 东→后方(0°), 西→前方(360°), 北→右方(180°), 南→左方(540°)
    angle_map = {'E': 0, 'W': 360, 'N': 180, 'S': 540}
    forward_angle = angle_map[direction]

    SAFE_DIST = 0.45
    n_points = 720                     # 激光雷达总点数
    half_steps = 10                    # ±5° 对应 ±10 个点
    ARRIVAL_TOLERANCE_X = 0.13           # 到达终点判定阈值（米）
    ARRIVAL_TOLERANCE_Y = 0.1           # 到达终点判定阈值（米）

    rate = rospy.Rate(20)              # 控制循环频率 20 Hz

    while not rospy.is_shutdown():
        # 获取当前位姿
        current_pose = get_pose_from_tf()
        if current_pose is None:
            rospy.logwarn("Cannot get precise pose, retrying...")
            rate.sleep()
            continue

        # 计算当前位置与终点的距离
        dx = current_pose[0] - end_world[0]
        dy = current_pose[1] - end_world[1]
        if (abs(dx) < ARRIVAL_TOLERANCE_X and (direction == 'E' or direction == 'W')) or (abs(dy) < ARRIVAL_TOLERANCE_Y and (direction == 'N' or direction == 'S')):
            centre_pid_flag = 1
            centre_start_time_for_angle_and_nav = rospy.Time.now()
            while centre_pid_flag == 1:
                rospy.sleep(0.2)
            rospy.loginfo("Reached segment end")
            move_for_lidar_and_angle(0.0, 0.0, 0.0)
            return True

        # 获取最新激光数据
        if latest_scan is None:
            rospy.logwarn("No laser scan available")
            break

        # 检测前进方向障碍物距离
        min_dist = float('inf')
        for step in range(-half_steps, half_steps+1):
            idx = (forward_angle + step) % n_points
            d = latest_scan.ranges[idx]
            if np.isfinite(d) and d > 0.05:
                if d < min_dist:
                    min_dist = d
        forward_dist = min_dist if min_dist != float('inf') else SAFE_DIST + 1.0

        if forward_dist < SAFE_DIST:
            rospy.logwarn("Obstacle detected at distance %.2f m, approaching with PID", forward_dist)

            # ==================== 用户添加部分 ====================
            centre_pid_flag = 1
            centre_start_time_for_angle_and_nav = rospy.Time.now()
            while centre_pid_flag == 1:
                rospy.sleep(0.2)

            # ====================================================

            min_dist = float('inf')
            for step in range(-half_steps, half_steps+1):
                idx = (forward_angle + step) % n_points
                d = latest_scan.ranges[idx]
                if np.isfinite(d) and d > 0.05:
                    if d < min_dist:
                        min_dist = d
            forward_dist = min_dist if min_dist != float('inf') else SAFE_DIST + 1.0

            # 停止机器人（如果上面未停止）
            move_for_lidar_and_angle(0.0, 0.0, 0.0)
            # 更新障碍物地图（仍然需要记录挡板位置）
            current_pose = get_pose_from_tf()
            if current_pose is not None:
                update_obstacle(current_pose, direction, forward_dist)
            return False

        # 正常前进
        move_for_lidar_and_angle(speed_x, speed_y, 0.0)
        rate.sleep()   # 等待下一个控制周期

    # 循环意外退出，停止机器人
    move_for_lidar_and_angle(0.0, 0.0, 0.0)
    return False

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
    init_tf_listener()

    #初始化以下两个变量
    start_time_for_angle_and_nav =rospy.Time.now()
    recog_start_time_for_angle_and_nav = rospy.Time.now()

    if (True):
        talker()#等待播报比赛开始，用来启动，需要修改，现在需要识别语音（找老版本？
        calibrate_grid_origin()
        move2end(start_x_v,start_y_v,start_time)  #一步到位

        #单独处理第一个线索点
        rospy.sleep(0.1)
        
        position_end = 0 #是否到达终点
        find_id = [33, 40, 42, 50] 
        shibieing_real_flag = 0
        mission_flag = 0 #表示任务点还是识别点还是终点

        while(position_end == 0):
            if (mission_flag == 0):#0任务前 1任务中 2任务后
                while shibie_flag <= 3:#循环三次半，最后一次不需要靠墙后转向
                    shibie_flag += 1


                    if(shibie_flag != 1):
                        move2end(0.0, -1, 1.5)
                    navi.goto(goalshibie[shibie_flag])
                    x_dis = recog_x[shibie_flag]                  
                    y_dis = recog_y[shibie_flag]
                    recog_pid_flag = 1
                    recog_start_time_for_angle_and_nav = rospy.Time.now()
                    while(recog_pid_flag == 1):
                        rospy.sleep(0.2)
                    #识别
                    '''rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1     
                    shibieing_real_flag = 1
                    while(shibieing_flag == 1):
                        rospy.sleep(0.2) 
                    shibie_voice()'''
                mission_flag = 1
            if mission_flag == 1:
                mission_now = 0
                while mission_now <= 3:
                    navigate_to_target(find_id[mission_now])
                    #mission_voice()
                    mission_now += 1
                mission_flag = 2
            if mission_flag == 2:
                navi.goto([2.8, -0.4, 90])#待测
                x_dis = 0.22                
                y_dis = 0.25
                recog_pid_flag = 1
                recog_start_time_for_angle_and_nav = rospy.Time.now()
                while(recog_pid_flag == 1):
                    rospy.sleep(0.2)
                #end_voice()
                position_end = 1


        
        rospy.loginfo("Task finished")



    while not rospy.is_shutdown():
        r.sleep()

