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

voice_receive_flag = 0


def voice_callback(data):
    global voice_receive_flag
    rospy.sleep(1)

    rospy.loginfo("%d",voice_receive_flag)
    result = ""
    result = data.data
    rospy.loginfo("%s",result)
    voice_receive_flag = 1



if __name__ == "__main__":
    voice_sub = rospy.Subscriber('/voice_result', String, voice_callback)
