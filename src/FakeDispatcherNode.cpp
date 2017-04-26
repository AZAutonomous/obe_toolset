#include <ros/ros.h> //For ROS itself
//#include <image_transport/image_transport.h> //For the image transport class. Still needs the vc bridge though.
#include <opencv2/highgui/highgui.hpp> //FIXME Not exactly sure why this one is here; maybe it's for displaying the images that come in?
#include <cv_bridge/cv_bridge.h> //The openCV bridge that image_transport needs
#include <ros/console.h> //This is for ROS_ERROR and stuff.
#include <boost/filesystem.hpp> //For creating directtories if needed
#include <obe_toolset/ImageAndPose.h> //Needed for the custom Image and Pose message type
#include <string>
#include <vector>

//#include "std_msgs/String.h"

int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_publisher_node");
	ros::NodeHandle n;
	int numOfPipes;
	int currentPipeMinusOne = 0;
	if(!(n.hasParam("numPipes")))
		ROS_INFO("the parameter 'numPipes' for the dispatcher node hasn't been specified; assuming 1 to ensure that no images are lost. This may cause severe back-up issues in a long mission.");
	n.param<int>("numPipes", numOfPipes, 1); //gets the numPipes param (so that we know where to publish), defaulting to 1 if it can't be read.

	std::vector<ros::Publisher> impose_pub_vector(numOfPipes); //vector of publishers
	for(int i = 1; i <= numOfPipes; ++i)
	{
		std::string topic("obe/imagePipe");
		topic += std::to_string(i);
		impose_pub_vector[i-1] = n.advertise<obe_toolset::ImageAndPose>(topic.c_str(), 512); //publishes to the obe/imagePipe<i> and buffers up to 512 images per pipe in case it can't send them all that quickly.
	}

//	image_transport::ImageTransport it(n);
//	ros::Publisher pub = n.advertise<std_msgs::String>("obe/test", 100);
//	image_transport::Publisher impose_pub = it.advertise("obe/image", 255); //Buffers up to 255 pictures in case connection is lost
//	^^That used to be the old publisher.
//	cv::Mat image = cv::imread("/home/kennon/images/BrentRambo.jpg", CV_LOAD_IMAGE_COLOR);
//	cv::waitKey(30); //No idea what this line does...
//	cv::imshow("view", image); //This can be used to see the image as it's published

	//This block makes sure that the needed directories are set up (I'm pretty sure they should be, since this node might end up running from one of them).
	namespace fs = boost::filesystem;
	fs::path home_directory("/home/kennon"); //THIS CAN'T BE ~ FOR WHATEVER REASON, OR THE CODE WILL BREAK. This might need to be changed for it to work on your system

	fs::path new_path("images/new/"); //Will be used to get images.
	fs::path processed_path("images/processed/");
	fs::path error_path("images/unsuccessful/");

	new_path = home_directory / new_path;
	processed_path = home_directory / processed_path;
	error_path = home_directory / error_path;

	fs::path im_fetch_path(new_path);

	fs::create_directories(im_fetch_path);
	fs::create_directories(processed_path);
	fs::create_directories(error_path);
	//NOTE:: IF THERE IS AN ISSUE WITH PERMISSIONS FOR SOME REASON, IT MIGHT BE THAT THE LINES OF CODE ABOVE ARE REMOVING EXECUTE PERMISSIONS. JUST SOMETHING TO CONSIDER


	if (!(fs::exists(im_fetch_path)))
	{
		//If it doesn't exist, something went horribly wrong =/
		ROS_ERROR("Oh no. '%s' isn't a directory =(", im_fetch_path.string().c_str());
		return(-1);
	}
	else if (fs::is_regular_file(im_fetch_path))
	{
		ROS_ERROR("Oh no. '%s' is a regular file =(", im_fetch_path.string().c_str());
		return(-1);
	}

	ros::Rate loop_rate(.5); //rate is the number of outer loop iterations per second
	//Also, we need the loop rate to be relatively quick becuase we don't want delay in dispatching images. Delay in dispatching images could lead to wasted execution time and potential incorrect location stamps.

	while(n.ok())
	{
		//We need to dispatch each of the images in the new directory
		ROS_INFO("Looking at path %s", im_fetch_path.string().c_str());
		fs::directory_iterator end_itr;
		for(fs::directory_iterator cur_path_itr(im_fetch_path); cur_path_itr != end_itr; cur_path_itr++)
		{
			ROS_INFO("I'm looking at this file: %s", cur_path_itr->path().string().c_str());
			if(fs::is_directory(cur_path_itr->path()))
			{
				ROS_INFO_STREAM_ONCE("There's a directory in the ~/images/new path; this is currently unhandled.");
				fs::rename(cur_path_itr->path(), error_path / cur_path_itr->path().filename());

				//FIXME Don't forget to move this file to the folder for files with errors.

			}
			else
			{
				cv::Mat newImage = cv::imread(cur_path_itr->path().string(), CV_LOAD_IMAGE_COLOR);

				if(newImage.data == NULL)//The image was read improperly if this fails...
				{
					ROS_ERROR("I was unable to read the file image file.");
					fs::rename(cur_path_itr->path(), error_path / cur_path_itr->path().filename());
				}
				else
				{
					obe_toolset::ImageAndPose impose_msg; //This is the custom message type that has a sensor_msgs::Image msg and a std_msgs::Pose message.
					impose_msg.image = *(cv_bridge::CvImage(std_msgs::Header(), "bgr8", newImage).toImageMsg()); //The meat of this line is from the image_transport tutorial; I just de-reference their piece to get a sensor_msgs::Image.  Note: There's probably a better way to do this, but it will work for now.

					//This is the point where the fakeing things comes in.
					impose_msg.x = 256.0; //fakes an x UTM value
					impose_msg.y = 65536.0; //etc...
					impose_msg.z = 60.96; //This is actually a decent guess (~200 ft)
					impose_msg.roll = 0.0;
					impose_msg.pitch = 0.0;
					impose_msg.yaw = 0.0; //this is the only one that's used as of 4.26.17
					//End the faking it stuff.

					//publish to the current pipe that's due for another message. NOTE: In the future, this could have a system that keeps track of busy nodes so that no particular node gets bogged down. I'm kind of assuming that we have enough nodes and a fast enough ROI algorithm and randomness is on our side so that this doesn't get out of hand.
					impose_pub_vector[currentPipeMinusOne].publish(impose_msg); //send the impose message.

					//set up the current pipe for the next time we publish something.
					currentPipeMinusOne++;
					currentPipeMinusOne %= numOfPipes; //we want to wrap around

					//Now that we've published it, we can move the file to the processed folder
					fs::rename(cur_path_itr->path(), processed_path / cur_path_itr->path().filename());
				}
			}
		} //Ends the for loop
		ros::spinOnce();
		loop_rate.sleep();
		ROS_INFO("I'm done sleeping");
	}
	//It shouldn't get here until you hit ctrl-C, but we still need to specify a return value:
	return 0;
}
