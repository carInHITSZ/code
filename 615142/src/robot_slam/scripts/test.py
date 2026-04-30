#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 移除重复的编码声明
# coding: utf-8

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
from rospy import Time, Duration
from sensor_msgs.msg import LaserScan
import sys
import re
import numpy as np
# 移除Python2特有的重载，Python3中不需要
# reload(sys)
# sys.setdefaultencoding('utf-8')

voice_receive_flag = 0

def voice_callback(data):
    global voice_receive_flag
    rospy.sleep(1)
    voice_receive_flag = 1
    result = ""
    result = data.data
    print(result)

if __name__ == "__main__":
    rospy.init_node('test_sub_node')  # 必须初始化ROS节点
    voice_sub = rospy.Subscriber('/voice_result', String, voice_callback)
    while (True):
        #print("1")
        #print(voice_receive_flag)
        rospy.sleep(10)
    rospy.spin()  # 保持节点运行
