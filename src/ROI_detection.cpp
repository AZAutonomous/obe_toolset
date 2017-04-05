//Kennon McKeever
//4.4.17
//ROI Detection

#include <obe_toolset/ROI_detection.hpp>
#include "opencv2/imgproc/imgproc.hpp"

void ROI_detection(cv::InputArray& img)
{
	if(!img.data)
		return -1;
	cv::InputArray Image(img); //need a copy as a scratchpad
	cv::InputArray Image_gray, Image_scratch, Image_edges, Image_filled;
	cv::cvtColor( Image, Image, CV_BGR2GRAY);
	#IFDEF DEBUG_KENNON
	namedWindow("window", cv_WINDOW_AUTOSIZE);
	createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold ); //this is for the user to pick a threshold I guess
	cv::blur(Image_gray, Image_scratch, cv::Size(3,3)) //guassian blur of size 3x3
	
	#ENDIF
}

void CannyThreshold(int, void*)
{
	
}
