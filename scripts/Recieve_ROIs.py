#!/usr/bin/env python
import rospy
from obe_toolset.msg import ImageAndPose
import cv2
# from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError

bridge = CvBridge()

def callback(data):
    rospy.loginfo(rospy.get_caller_id())
    cv_image = bridge.imgmsg_to_cv2(data.image, desired_encoding="passthrough")
    cv2.imshow("ImageWindow", cv_image)
    cv2.waitKey(500)
	#rospy.loginfo("im size: " + str(cv_image.shape))
    #rospy.loginfo("x: " + str(data.x))
    #rospy.loginfo("y: " + str(data.y))
    #rospy.loginfo("z: " + str(data.z))
    #rospy.loginfo("roll: " + str(data.roll))
    #rospy.loginfo("pitch: " + str(data.pitch))
    #rospy.loginfo("yaw: " + str(data.yaw))
    #This is where you can pull the image and stuff from the message
        #data.image = the image, in bgr8 format. Use cv_bridge to get it out
        #data.x = easting in meters (UTM)
        #data.y = northin in meters (UTM)
        #data.z = AGL altitude in meters
        #data.roll = (probably 0, not detected as of 4.27.17)
        #data.pitch = (probably 0, not detected as of 4.27.17)
        #data.yaw = radians CCW from East (follows the ENU convention)
def listener():

    # In ROS, nodes are uniquely named. If two nodes with the same
    # node are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
    rospy.init_node('ROI_Listener', anonymous=True)

    rospy.Subscriber("/ROIs", ImageAndPose, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
