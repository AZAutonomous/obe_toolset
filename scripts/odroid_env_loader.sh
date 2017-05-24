#!/bin/sh

. /opt/ros/indigo/setup.sh #This line is from the machine tag page
export ROS_PACKAGE_PATH=$ROS_PACKAGE_PATH:/home/odroid/catkin_ws/src/obe_toolset #recommended in an answer found on rosAnswers
export ROS_MASTER_URI=http://192.168.1.5:11311 #Kennon added this
export ROS_IP=`hostname -I | tr " " "\n" | grep 192` #Kennon added this (chooses the 192.* address (in case there are multiple))
export ROS_HOSTNAME=$ROS_IP #Kennon added this
exec "$@" #from the machin tag page
