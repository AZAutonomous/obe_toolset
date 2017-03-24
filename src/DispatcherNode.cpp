#include <ros/ros.h> //For ROS itself
#include <image_transport/image_transport.h> //For the image transport class. Still needs the vc bridge though.
#include <opencv2/highgui/highgui.hpp> //FIXME Not exactly sure why this one is here; maybe it's for displaying the images that come in?
#include <cv_bridge/cv_bridge.h> //The openCV bridge that image_transport needs
#include <ros/console.h> //This is for ROS_ERROR and stuff.

int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_publisher_node");
	ros::NodeHandle n;
	image_transport::ImageTransport it(n);
	image_transport::Publisher im_pub = it.advertise("obe/image", 255); //Buffers up to 255 pictures in case connection is lost
//	cv::Mat image = cv::imread("/home/kennon/images/BrentRambo.jpg", CV_LOAD_IMAGE_COLOR);
//	cv::waitKey(30); //No idea what this line does...
//	cv::imshow("view", image); //This can be used to see the image as it's published

	ros::Rate loop_rate(5); //rate is the number of loop iterations per second
	while(n.ok())
	{
		//The line below is the one tha actually retrieves the image. It should probably be changed to get the image from CHDK or GPhoto2.
		cv::Mat newImage = cv::imread("/home/kennon/images/BrentRambo.jpg", CV_LOAD_IMAGE_COLOR);

		if(newImage.data == NULL)//The image was read improperly if this fails...
		{
			ROS_ERROR("I was unable to read the file image file.");
		}
		else
		{
			sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", newImage).toImageMsg();
			im_pub.publish(msg);
		}
		ros::spinOnce();
		loop_rate.sleep();
	}
	//It shouldn't get here until you hit ctrl-C, but we still need to specify a return value:
	return 0;
}
