#!/usr/bin/env python
import rospy
import cv2
from obe_toolset.msg import ImageAndPose
from cv_bridge import CvBridge, CvBridgeError

	#This is where you can pull the image and stuff from the message
def run():

	# In ROS, nodes are uniquely named. If two nodes with the same
	# node are launched, the previous one is kicked off. The
	# anonymous=True flag means that rospy will choose a unique
	# name for our 'listener' node so that multiple listeners can
	# run simultaneously.
	rospy.init_node('ROI_Talker', anonymous=True)

	pub = rospy.Publisher("/ROIs", ImageAndPose, queue_size=10)
	rate = rospy.Rate(10)

	bridge = CvBridge()
	im = cv2.imread('foo.jpg')

	while not rospy.is_shutdown():
		msg = ImageAndPose()
		# sensor_msgs.Image
		msg.image = bridge.cv2_to_imgmsg(im, encoding="passthrough")
		# float
		msg.x = 0.
		# float
		msg.y = 0.
		# float
		msg.z = 0.
		# float
		msg.roll = 0.
		# float
		msg.pitch = 0.
		# float
		msg.yaw = 0.

		pub.publish(msg)
		rate.sleep()

if __name__ == '__main__':
	run()
