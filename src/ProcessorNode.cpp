#include <ros/ros.h> //This is for the basic ROS stuff
#include <image_transport/image_transport.h> //image transport ROS pkg. Requires cv_bridge.
#include <opencv2/highgui/highgui.hpp>//FIXME Probably can be deleted; I think it's for viewing the images.
#include <cv_bridge/cv_bridge.h>//CV_Bridge required include for image_transport
#include <ros/console.h> //This is maybe for ROS_ERROR and ROS_INFO? Not sure, maybe try compiling with this removed.
#include <obe_toolset/ImageAndPose.h> //For the custom ImageAndPose message.
#include "obe_toolset/ROI_detection.hpp"
#include <list> //like vector, but it's a Double Linked List. It's used as the return type of ROI_Detection.


class SubPub //subscribe and publish all in one. The idea for this is from here: http://answers.ros.org/question/59725/publishing-to-a-topic-via-subscriber-callback-function/
{
public:
	SubPub()
	{
		this->pub = this->n.advertise<obe_toolset::ImageAndPose>("/ROIs", 767); //For sending ROIs to the base station (on the /ROIs topic, type obe_toolset::ImageAndPose)
		this->sub = this->n.subscribe("/obe/imagePipeN", 255, &SubPub::processImage, this); //This probably needs to be able to change subscriber topic dynamically.
	}

	void processImage(obe_toolset::ImageAndPose msg) //needs to be static so we can take its address
	{
		//ROS_INFO("I recieved some data!");
		CV_ImAndPose ROI_input; //this is a struct with cv::Mat image and doubles x,y,z for coordinates.
		ROI_input.image = cv_bridge::toCvCopy(msg.image, "bgr8")->image; //pulls the image out of msg as a cv::Mat type
		ROI_input.x = msg.x;//set the x,y,z coords (UTM)
		ROI_input.y = msg.y;
		ROI_input.z = msg.z;
		ROI_input.roll = msg.roll;
		ROI_input.pitch = msg.pitch;
		//Yaw comes in as North=0, East=90, clockwise. The algorithm expects 0 to be along the +x axis, increasing counterclockwise. We need to convert.
		auto CompassAngle = 90.0 - msg.yaw; if(CompassAngle < 0.0) CompassAngle += 360.0;
		ROI_input.yaw = CompassAngle;

		//Process to extract the ROIs.
		std::list<CV_ImAndPose> ROI_list = ROI_detection(ROI_input); //runs ROI detection on the image in cv_frame. ROIs are returned in a list of cv::Mat's.

		//Publish everything deemed to be an ROI
		auto end = ROI_list.end();
		//ROS_INFO("I found %d ROIs in that image.", int(ROI_list.size()));
		for(auto it = ROI_list.begin(); it != end; it++)
		{
			obe_toolset::ImageAndPose msg;
			msg.x = it->x;
			msg.y = it->y;
			msg.z = it->z;
			msg.roll = it->roll;
			msg.pitch = it->pitch;
			msg.yaw = it->yaw;
			msg.image = *(cv_bridge::CvImage(std_msgs::Header(), "bgr8", it->image).toImageMsg()); //cv_bridge converts cv::Mat to a message pointer. this pointer is dereferenced and put in the msg's image slot.
			this->pub.publish(msg); //Finally we can send the thing off!
		}
	}


private:
	ros::NodeHandle n;
	ros::Publisher pub;
	ros::Subscriber sub;
}; //end class SubPub definition

/*
void processImage(obe_toolset::ImageAndPose msg)
{
	CV_ImAndPose ROI_input; //this is a struct with cv::Mat image and doubles x,y,z for coordinates.
	ROI_input.image = cv_bridge::toCvCopy(msg.image, "bgr8")->image; //pulls the image out of msg as a cv::Mat type
	ROI_input.x = msg.position.x;//set the x,y,z coords (UTM)
	ROI_input.y = msg.position.y;
	ROI_input.z = msg.position.z;

	//Process to extract the ROIs.
	std::list<CV_ImAndPose> ROI_list = ROI_detection(ROI_input, 200); //runs ROI detection on the image in cv_frame. ROIs are returned in a list of cv::Mat's.

	//Publish everything deemed to be an ROI
	for(auto it = ROI_list.begin(), end = ROI_list.end(); it != end; ++it)
	{
		obe_toolset::ImageAndPose msg;
		msg.position.x = it->x;
		msg.position.y = it->y;
		msg.position.z = it->z;
		msg.image = *(cv_bridge::CvImage(std_msgs::Header(), "bgr8", it->image).toImageMsg()); //cv_bridge converts cv::Mat to a message pointer. this pointer is dereferenced and put in the msg's image slot.
		//this->pub.publish(msg); //Finally we can send the thing off!
	}
}
*/


int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_listener");
	//ros::NodeHandle n;

	//ros::Subscriber sub = n.subscribe("obe/image", 255, processImage); //the topic needs to be changed here, possibly from input arguments

	//PubToBaseStation = new ros::Publisher<> = n.publish();

	SubPub mySubPub; //RAII means the publisher and subscriber are initialized here

	ros::spin();

	return 0;
}
