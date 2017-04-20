//Kennon McKeever
//4.4.17
//ROI Detection

#include <obe_toolset/ROI_detection.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <list>

//This function crops the original image and returns a list of images. All of said images are of the CV_ImAndPose type so they can be stamped with UTM location data. 
std::list<CV_ImAndPose> ROI_detection(CV_ImAndPose imAndPose, double AGL_feet) //, double Roll_deg, double Pitch_deg, double Yaw_deg) //the roll pitch yaw stuff is to be figured out later. right now I assume 0,0,0 and commented this out so that nobody thinks the code will work if they decide to set them.
{
	cv::Mat img(imAndPose.image); //extract the image from the data packet
	if(!img.data)
		return -1;
	cv::Mat scratch_image(img); //need a copy as a scratchpad
	//cv::InputArray Image_gray, Image_scratch, Image_edges, Image_filled;
	cv::cvtColor( scratch_image, scratch_image, CV_BGR2GRAY);
	#IFDEF DEBUG_KENNON
	namedWindow("window", cv_WINDOW_AUTOSIZE);
	createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold ); //this is for the user to pick a threshold I guess
	cv::blur(Image_gray, Image_scratch, cv::Size(3,3)) //guassian blur of size 3x3
	
	#ENDIF
}

void CannyThreshold(int, void*)
{
	
}
