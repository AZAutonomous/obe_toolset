#!/bin/sh

. /opt/ros/indigo/setup.sh
export ROS_MASTER_URI=http://192.168.1.5:11311
export ROS_IP=`hostname -I | tr " " "\n" | grep 192`
export ROS_HOSTNAME=$ROS_IP
exec "$@"
