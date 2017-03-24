#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	try
	{
		if(cv_bridge::toCvShare(msg, "bgr8")->image.empty())
		{
			ROS_ERROR("The image message appears to be empty");
		}
		cv::imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
		cv::waitKey(30);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("Oh no =(");
	}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_listener");
	ros::NodeHandle n;
	cv::namedWindow("view");
	cv::startWindowThread();
	image_transport::ImageTransport it(n);
	image_transport::Subscriber sub = it.subscribe("obe/image", 255, imageCallback);
	ros::spin();
	cv::destroyWindow("view");
	return 0;
}
