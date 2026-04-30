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
find_id = 0
point_msg = 255

#中转0是曾经用的中间点，中转1是刚开始的中转点（和识别0等值反向），中转2是终点前的中转点（和识别3等值且同向），中转3是尝试直接加进去的终点导航
#start_time = rospy.Time.now()
rotate_180_z = 2.6   #move_rotate_180()中的强制旋转z速度
rotate_time1 = 3   #起点开始的强制旋转时间
rotate_time2 = 3   #中途的强制旋转时间

start_x_v = 0.13  #起点开始的一步到位
start_y_v = -0.24
start_time = 2

end_x_v = 0.13   #终点强制
end_y_v = 0.30
end_x_time = 1.4
end_y_time = 2

adjust_time = 6
adjust_time_navigation = 5
target_end_x = 0.186#0.2
target_end_y = 1.636#1.66

global position_end
global navigation_pid_flag
position_end = 0
move_lidar_k_x = 0.6
move_lidar_k_y = 0.7

move_lidar_i_x = 0.000001
move_lidar_i_y = 0.00001

integral_x = 0
integral_y = 0


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

def result_callback(msg):
    global find_id
    find_id = msg.data
    if(find_id<=8 and find_id>=1):
        print(find_id)
    else:
        find_id = 0

def LidarCallback(msg):
    global position_end,target_end_y,target_end_x,start_time_for_end
    x_dist = msg.ranges[360]
    y_dist = msg.ranges[180]
    #rospy.loginfo("x_dist = %f ",x_dist)
    #rospy.loginfo("y_dist = %f ",y_dist)

    if(position_end == 1):
        start_time=rospy.Time.now()
        duration = Duration.from_sec(adjust_time)
        if(x_dist>=target_end_x and y_dist<=target_end_y and x_dist<10 and y_dist<10):
            move_for_lidar(move_lidar_k_x*abs(x_dist-target_end_x),move_lidar_k_y*abs(y_dist-target_end_y))
        elif(x_dist>=target_end_x and y_dist>=target_end_y and x_dist<10 and y_dist<10):
            move_for_lidar(move_lidar_k_x*abs(x_dist-target_end_x ),0)
        elif(y_dist<=target_end_y and x_dist<target_end_x and x_dist<10 and y_dist<10):
            move_for_lidar(0,move_lidar_k_y*abs(y_dist-target_end_y))
        else:
            os.system('mplayer %s' % "~/music/arrive_end.mp3")
            position_end = 0
        if(rospy.Time.now()-start_time_for_end>=duration):
            os.system('mplayer %s' % "~/music/arrive_end.mp3")
            position_end = 0


    

def navigation_pid_callback(msg):#x_dis是点距离挡板的距离，每一次都要记得积分项清零
    global navigation_pid_flag,start_time_for_navigation_pid,x_dis,y_dis,x_angle,y_angle,x_sym,y_sym#第二个开始时间在main中修改
    #x_angle是x轴角度，y_angle是y轴角度
    #x_sym是x轴方向，y_sym是y轴方向
    x_now = msg.ranges[x_angle]
    y_now = msg.ranges[y_angle]
    if (navigation_pid_flag == 1):#flag=1表示开始导航  
        start_time = rospy.Time.now()
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

class navigation_demo():
    def __init__(self):
        self.set_pose_pub = rospy.Publisher('/initialpose', PoseWithCovarianceStamped, queue_size=5)
        self.arrive_pub = rospy.Publisher('/voiceWords',String,queue_size=10)
        self.ar_sub1 = rospy.Subscriber('/ar_pose_marker',AlvarMarkers,self.ar_track_cb)
        self.ar_sub = rospy.Subscriber('/object_position', Point, self.ar_cb)
        self.ar_cb2 = rospy.Subscriber('/vlm_node/result', Int32, result_callback)
        self.lidar_sub = rospy.Subscriber('/scan', LaserScan, LidarCallback)
        self.move_base = actionlib.SimpleActionClient("move_base", MoveBaseAction)
        self.move_base.wait_for_server(rospy.Duration(60))
    
    def ar_track_cb(self,data):
        #print ("zhixing AR callback")
        global find_id
        for marker in data.markers:
            find_id = marker.id
        print(find_id)

    def ar_cb(self, data):
        #print ("zhixing hanzi callback")
        global point_msg, find_id
        point_msg = data
        #rospy.loginfo('z = %d', point_msg.z)
        if(point_msg.z>=1 and point_msg.z<=20):
            find_id = 1
            print(find_id)
        elif(point_msg.z>=21 and point_msg.z<=40 ):
            find_id = 2
            print(find_id)
        #point_msg = None
        elif (point_msg.z>=41 and point_msg.z<=60 ):
            find_id = 3
            print(find_id)
        #point_msg = None
        elif (point_msg.z>=61 and point_msg.z<=80 ):
            find_id = 4
            print(find_id)
        #point_msg = None
        elif (point_msg.z>=81 and point_msg.z<=110 ):
            find_id = 5
            print(find_id)
        #point_msg = None
        elif (point_msg.z>=111 and point_msg.z<=130):
            find_id = 6
            #print(find_id)
        #point_msg = None
        elif (point_msg.z>=131 and point_msg.z<=140 ):
            find_id = 7
            print(find_id)
        #point_msg = None
        elif (point_msg.z>=141 and point_msg.z<=160 ):
            find_id = 8
            print(find_id)

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

    if (input == '1'):
        os.system('mplayer %s' % "~/music/arrive_begin.mp3")
        move2end(start_x_v,start_y_v,start_time)  #一步到位
        rospy.sleep(0.1)
        move_rotate_180(rotate_time1) #起点开始的强制旋转
        is_0_or_180 = 1 - is_0_or_180 #姿态翻转
        navi.goto(goalshibie[0])
        passed_recgonition[0]=1
        rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
        position_end = 0 #是否到达终点
        while(position_end == 0):

            if(find_id == 1):
                os.system('mplayer %s' % "~/music/shibie_1.mp3")
                #input = raw_input()  #KKK第一个识别点调位置
                if (is_0_or_180 == 0 and goals[0][2] == 180) or (is_0_or_180 == 1 and goals[0][2] == 0):
                    move_rotate_180(rotate_time2) #中途的强制旋转
                    is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[0])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_1.mp3")
                #input = raw_input()  #KKK第一个目标点调位置
                if(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                elif(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1
                #navi.goto(goalshibie[])
                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                find_id = 0
                

            if(find_id == 2):
                os.system('mplayer %s' % "~/music/shibie_2.mp3")
                #input = raw_input()  #KKK第一个识别点调位置
                if (is_0_or_180 == 0 and goals[1][2] == 180) or (is_0_or_180 == 1 and goals[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[1])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_2.mp3")
                #input = raw_input()  #KKK第二个目标点调位置
                #move_rotate_180(rotate_time1)	
                if(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                elif(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1

                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                find_id = 0
                
            if(find_id == 3):
                os.system('mplayer %s' % "~/music/shibie_3.mp3")
                #input = raw_input()  #KKK第二个识别点调位置
                if (is_0_or_180 == 0 and goals[2][2] == 180) or (is_0_or_180 == 1 and goals[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[2])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_3.mp3")
                #input = raw_input()  #KKK第三个目标点调位置
                #navi.goto(goalzhongzhuan[0])
                if(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                elif(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1

                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                find_id = 0
                
            if(find_id == 4):
                os.system('mplayer %s' % "~/music/shibie_4.mp3")
                #input = raw_input()  #KKK第二个识别点调位置
                if (is_0_or_180 == 0 and goals[3][2] == 180) or (is_0_or_180 == 1 and goals[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[3])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_4.mp3")
                #input = raw_input()  #KKK第四个目标点调位置
                if(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                elif(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1
                    
                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                find_id = 0
                
            if(find_id == 5 ):
                os.system('mplayer %s' % "~/music/shibie_5.mp3")
                #input = raw_input()  #KKK第三个识别点调位置
                if (is_0_or_180 == 0 and goals[4][2] == 180) or (is_0_or_180 == 1 and goals[4][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[4])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_5.mp3")
                #input = raw_input()  #KKK第五个目标点调位置
                #navi.goto(goalzhongzhuan[0])
                if(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                elif(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1
                    
                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                find_id = 0
                
            if(find_id == 6):
                os.system('mplayer %s' % "~/music/shibie_6.mp3")
                #input = raw_input()  #KKK第三个识别点调位置
                #nowtimeflag = 0
                if (is_0_or_180 == 0 and goals[5][2] == 180) or (is_0_or_180 == 1 and goals[5][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[5])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_6.mp3")
                #input = raw_input()  #KKK第六个目标点调位置
                if(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                elif(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1
                    
                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                find_id = 0
                
            if(find_id == 7):
                os.system('mplayer %s' % "~/music/shibie_7.mp3")
                #input = raw_input()  #KKK第四个识别点调位置
                if (is_0_or_180 == 0 and goals[6][2] == 180) or (is_0_or_180 == 1 and goals[6][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[6])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_7.mp3")
                #input = raw_input()  #KKK第七个目标点调位置
                if(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1
                elif(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                    
                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                #move2end(0,end_y7_v,end_y7_time)  #move2endfory7
                #rospy.sleep(0.5)
                #move2end(end_x7_v,0,end_x7_time)  #move2endforx7
                #os.system('mplayer %s' % "~/music/arrive_end.mp3")
                find_id = 0

            if(find_id == 8):
                os.system('mplayer %s' % "~/music/shibie_8.mp3")
                #input = raw_input()  #KKK第四个识别点调位置
                if (is_0_or_180 == 0 and goals[7][2] == 180) or (is_0_or_180 == 1 and goals[7][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                navi.goto(goals[7])
                counter = counter+1
                os.system('mplayer %s' % "~/music/arrive_8.mp3")
                #input = raw_input()  #KKK第八个目标点调位置
                if(passed_recgonition[3]==0):
                    if (is_0_or_180 == 0 and goalshibie[3][2] == 180) or (is_0_or_180 == 1 and goalshibie[3][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[3])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[3]=1
                elif(passed_recgonition[2]==0):
                    if (is_0_or_180 == 0 and goalshibie[2][2] == 180) or (is_0_or_180 == 1 and goalshibie[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[2])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[2]=1
                elif(passed_recgonition[1]==0):
                    if (is_0_or_180 == 0 and goalshibie[1][2] == 180) or (is_0_or_180 == 1 and goalshibie[1][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[1])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[1]=1
                elif(passed_recgonition[0]==0):
                    if (is_0_or_180 == 0 and goalshibie[0][2] == 180) or (is_0_or_180 == 1 and goalshibie[0][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalshibie[0])
                    rospy.set_param('/top_view_shot_node/im_flag',1) #拍照识别算式
                    passed_recgonition[0]=1
                if(counter == 4):
                    if (is_0_or_180 == 0 and goalzhongzhuan[2][2] == 180) or (is_0_or_180 == 1 and goalzhongzhuan[2][2] == 0):
                        move_rotate_180(rotate_time2) #中途的强制旋转
                        is_0_or_180 = 1 - is_0_or_180
                    navi.goto(goalzhongzhuan[2])
                    # move2end(end_x_v,0,end_x_time)  #终点强制
                    # rospy.sleep(0.5)
                    # move2end(0,end_y_v,end_y_time)  #终点强制
                    # rospy.sleep(0.5)
                    # os.system('mplayer %s' % "~/music/arrive_end.mp3")
                    position_end=1
                #os.system('mplayer %s' % "~/music/arrive_end.mp3")
                find_id = 0
            
            
            else :
                rospy.sleep(0.01)
        if(position_end==1):
            start_time_for_end=rospy.Time.now()
    while not rospy.is_shutdown():
        r.sleep()




