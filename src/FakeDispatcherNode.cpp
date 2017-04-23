#include <ros/ros.h> //For ROS itself
#include <image_transport/image_transport.h> //For the image transport class. Still needs the vc bridge though.
#include <opencv2/highgui/highgui.hpp> //FIXME Not exactly sure why this one is here; maybe it's for displaying the images that come in?
#include <cv_bridge/cv_bridge.h> //The openCV bridge that image_transport needs
#include <ros/console.h> //This is for ROS_ERROR and stuff.
#include <boost/filesystem.hpp> //For creating directtories if needed
#include <obe_toolset/ImageAndPose.h> //Needed for the custom Image and Pose message type

#include "std_msgs/String.h"

int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_publisher_node");
	ros::NodeHandle n;
	image_transport::ImageTransport it(n);
	ros::Publisher impose_pub = n.advertise<obe_toolset::ImageAndPose>("obe/imagePipe1", 100); //In the future, we'll need more of these that publish to many image pipes round-robin style
	ros::Publisher pub = n.advertise<std_msgs::String>("obe/test", 100);
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
					impose_msg.position.x = 256; //fakes an x UTM value
					impose_msg.position.y = 65536; //etc...
					impose_msg.position.z = 16777216;
					//End the faking it stuff.
					impose_pub.publish(impose_msg); //send the impose message.

					//for testing:
					std_msgs::String m;
					m.data = cur_path_itr->path().string();
					pub.publish(m);

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
