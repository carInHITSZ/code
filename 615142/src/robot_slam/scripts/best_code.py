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


#导航pid参数
adjust_time = 6
adjust_time_navigation = 5
target_end_x = 0.186#0.2
target_end_y = 1.636#1.66
position_end=0
navigation_pid_flag=0
#主函数里面重新定义start_time_for_navigation_pid
move_lidar_k_x = 0.6
move_lidar_k_y = 0.7
move_lidar_i_x = 0.000001
move_lidar_i_y = 0.00001
integral_x = 0
integral_y = 0

#转角度的pid
angle_tolerance = 0.05
global delta_angle
angle_1=180
angle_pid_flag=0
angle_adjust_time=3
delta_angle=30
position_end = 0
angle_pid_k=0.005

#主函数新增参数
find_id = 0
cal_symbol = 1 #1表示加法，2表示减法，3表示乘法，4表示除法
x_dis = 0 
y_dis = 0
x_angle = 0
y_angle = 0
x_sym = 0
y_sym = 0 
end_num = 0 #0表示11号点，1表示12号点，2表示13号点
shibie_flag = 0 #表示要去第几个识别点
shibieing_flag = 0 #表示正在识别
now_num_vlm = 1
find = [0,0,0,0,0,0,0]
find_vlm = [-1,-1,-1,-1,-1,-1,-1]#大模型
def move_for_lidar(v_x,v_y):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=v_x
    twist_msg.linear.y=v_y
    twist_msg.angular.z=0
    start_time=rospy.Time.now()
    duration = Duration.from_sec(0.01)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.01)

def move_for_angle(v_z):
    twist_msg=Twist()
    pub = rospy.Publisher('/cmd_vel',Twist,queue_size=10)
    twist_msg.linear.x=0
    twist_msg.linear.y=0
    twist_msg.angular.z=v_z
    start_time=rospy.Time.now()
    duration = Duration.from_sec(0.01)
    while rospy.Time.now() - start_time<duration:
        pub.publish(twist_msg)
        rospy.sleep(0.01)

def result_callback(msg):
    global find_vlm,find,now_num_vlm
    find_id_vlm = 0
    find_id_vlm = msg.data
    for i in range(1,6):
        if(find_vlm[i]==-1):
            find_vlm[i] = find_id_vlm
            now_num_vlm = i
            break
    if(find[now_num_vlm]==0):
        find[now_num_vlm]=find_vlm[now_num_vlm]
     
    

def navigation_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global navigation_pid_flag,start_time_for_navigation_pid,x_dis,y_dis,x_angle,y_angle,x_sym,y_sym#第二个开始时间在main中修改
    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    x_now = msg.ranges[x_angle]
    y_now = msg.ranges[y_angle]
    if (navigation_pid_flag == 1):#flag=1表示开始导航  
        start_time = start_time_for_navigation_pid
        duration = Duration.from_sec(adjust_time_navigation)
        if(x_now<10 and y_now<10):
            delta_x = x_dis-x_now
            delta_y = y_dis-y_now
            integral_x = integral_x + delta_x
            integral_y = integral_y + delta_y
            move_for_lidar(x_sym*(move_lidar_k_x*delta_x+move_lidar_i_x*integral_x),y_sym*(move_lidar_k_y*delta_y+move_lidar_i_y*integral_y))
        if(rospy.Time.now()-start_time>=duration):
            navigation_pid_flag = 0
            integral_x = 0
            integral_y = 0

def angle_lidar_pid(msg):#根据激光雷达某一个角度的距离和相邻两侧角度的距离，控制改角度与墙面垂直,angle_1要在主函数更改
    global angle_1,delta_angle,angle_pid_flag,angle_adjust_time,angle_pid_k,start_time_for_angle_pid
    dis_left = msg.ranges[angle_1-delta_angle]
    dis_right = msg.ranges[angle_1+delta_angle]
    dis_now = msg.ranges[angle_1]
    if(angle_pid_flag == 1):#flag=1表示开始导航
        start_time = start_time_for_angle_pid
        duration = Duration.from_sec(angle_adjust_time)
        if(dis_now<10 and dis_left<10 and dis_right<10 and (dis_left-dis_right>angle_tolerance or dis_left-dis_right<-1*angle_tolerance)):
            delta_dis = dis_left-dis_right
            move_for_angle(angle_pid_k*(delta_dis))
        if(rospy.Time.now()-start_time>=duration):
            angel_pid_flag = 0

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
        self.lidar_sub = rospy.Subscriber('/scan', LaserScan, navigation_pid_callback)
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


if __name__ == "__main__":
    rospy.init_node('navigation_demo',anonymous=True)
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
    goalzhongzhuan = [[float(x), float(y), float(yaw)] for (x, y, yaw) in zip(goalListXXX.split(","),goalListYYY.split(","),goalListYawYawYaw.split(","))]
    navi = navigation_demo()
    print ('Please 1 to continue: ')
    input = raw_input()
    print (goals)
    r = rospy.Rate(1)
    r.sleep()
    print("zhixing main")
    start_time_for_angle_pid=rospy.Time.now()
    start_time_for_navigation_pid=rospy.Time.now()
    if (input == '1'):
        os.system('mplayer %s' % "~/music/arrive_begin.mp3")
        move2end(start_x_v,start_y_v,start_time)  #一步到位
        rospy.sleep(0.1)
        move_rotate_180(rotate_time1) #起点开始的强制旋转
        shibie_flag = 1
        navi.goto(goalshibie[1])
        rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
        shibieing_flag = 1
        position_end = 0 #是否到达终点
        find_id = 0 
        while(position_end == 0):
            if(shibie_flag >= 7):
                navi.goto(goalzhongzhuan[end_num])

            if(shibie_flag == 1):
                if(find[shibie_flag] >= 1 and find[shibie_flag] <= 8):
                    shibie_flag = 2
                    navi.goto(goalshibie[shibie_flag])
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 2):
                if(find[shibie_flag] >= 1 and find[shibie_flag] <= 8):
                    cal_id()
                    navi.goto(goals[find_id-1])
                    shibie_flag = 3
                    navi.goto(goalshibie[shibie_flag])
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1
            
            if(shibie_flag == 3):
                if(find[shibie_flag] >= 1 and find[shibie_flag] <= 8):
                    shibie_flag = 4
                    navi.goto(goalshibie[shibie_flag])
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 4):
                if(find[shibie_flag] >= 1 and find[shibie_flag] <= 8):
                    cal_id()
                    navi.goto(goals[find_id-1])
                    shibie_flag = 5
                    navi.goto(goalshibie[shibie_flag])
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 5):
                if(find[shibie_flag] >= 1 and find[shibie_flag] <= 8):
                    shibie_flag = 6
                    navi.goto(goalshibie[shibie_flag])
                    rospy.set_param('/top_view_shot_node/im_flag',1)
                    shibieing_flag = 1

            if(shibie_flag == 6):
                if(find[shibie_flag] >= 1 and find[shibie_flag] <= 8):
                    cal_id()
                    navi.goto(goals[find_id-1])
                    shibie_flag = 7

                
            
            else :
                rospy.sleep(0.01)
        if(position_end==1):
            start_time_for_end=rospy.Time.now()
    while not rospy.is_shutdown():
        r.sleep()





