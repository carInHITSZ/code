#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
import tf

def tf_subscriber():
    rospy.init_node('tf_subscriber', anonymous=True)
    listener = tf.TransformListener()
    rate = rospy.Rate(10.0)  # 10 Hz

    while not rospy.is_shutdown():
        try:
            # 获取从地图坐标系（/map）到小车基坐标系（/base_link）的变换
            (trans, rot) = listener.lookupTransform('/map', '/base_link', rospy.Time(0))
            x = trans[0]
            y = trans[1]
            z = trans[2]
            rospy.loginfo("Position: x = %f, y = %f, z = %f", x, y, z)
            rospy.loginfo("Orientation: x = %f, y = %f, z = %f, w = %f", 
                          rot[0], rot[1], rot[2], rot[3])
        except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
            continue

        rate.sleep()

if __name__ == '__main__':
    try:
        tf_subscriber()
    except rospy.ROSInterruptException:
        pass
