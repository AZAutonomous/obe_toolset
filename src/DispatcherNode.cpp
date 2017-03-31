#include <ros/ros.h> //For ROS itself
#include <image_transport/image_transport.h> //For the image transport class. Still needs the vc bridge though.
#include <opencv2/highgui/highgui.hpp> //FIXME Not exactly sure why this one is here; maybe it's for displaying the images that come in?
#include <cv_bridge/cv_bridge.h> //The openCV bridge that image_transport needs
#include <ros/console.h> //This is for ROS_ERROR and stuff.
#include <boost/filesystem.hpp> //For creating directtories if needed

int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_publisher_node");
	ros::NodeHandle n;
	image_transport::ImageTransport it(n);
	image_transport::Publisher im_pub = it.advertise("obe/image", 255); //Buffers up to 255 pictures in case connection is lost
//	cv::Mat image = cv::imread("/home/kennon/images/BrentRambo.jpg", CV_LOAD_IMAGE_COLOR);
//	cv::waitKey(30); //No idea what this line does...
//	cv::imshow("view", image); //This can be used to see the image as it's published

	//This block makes sure that the needed directories are set up (I'm pretty sure they should be, since this node might end up running from one of them).
	boost::filesystem::create_directories("~/images/new");
	boost::filesystem::create_directories("~/images/processed");
	boost::filesystem::create_directories("~/images/unsuccessful");
	//NOTE:: IF THERE IS AN ISSUE WITH PERMISSIONS FOR SOME REASON, IT MIGHT BE THAT THE LINES OF CODE ABOVE ARE REMOVING EXECUTE PERMISSIONS. JUST SOMETHING TO CONSIDER

	boost::filesystem::path im_fetch_path("~/images/new"); //Will be used to get images
	if (!boost::filesystem::exists(im_fetch_path))
	{
		ROS_ERROR("Oh no. ~/images/new isn't a directory =(");
		return(-1);
	}
	else if (boost::filesystem::is_regular_file(im_fetch_path))
	{
		ROS_ERROR("Oh no. ~/images/new is a regular file =(");
		return(-1);
	}

	ros::Rate loop_rate(25); //rate is the number of outer loop iterations per second
	//Also, we need the loop rate to be relatively quick becuase we don't want delay in dispatching images. Delay in dispatching images could lead to wasted execution time and potential incorrect location stamps.

	while(n.ok())
	{
		//We need to dispatch each of the images in the new directory
		for( boost::filesystem::directory_entry& current_path : boost::filesystem::directory_iterator(im_fetch_path))
		{
			if (boost::filesystem::is_directory(current_path)
			{
				//ROS_INFO("There's a directory in the ~/images/new path; this is currently unhandled.);
			}
			else
			{
				cv::Mat newImage = cv::imread(current_path.filename(), CV_LOAD_IMAGE_COLOR);

				if(newImage.data == NULL)//The image was read improperly if this fails...
				{
					ROS_ERROR("I was unable to read the file image file.");
				}
				else
				{
					sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", newImage).toImageMsg();
					im_pub.publish(msg);
				}
			}
		} //Ends the for loop
		ros::spinOnce();
		loop_rate.sleep();
	}
	//It shouldn't get here until you hit ctrl-C, but we still need to specify a return value:
	return 0;
}
