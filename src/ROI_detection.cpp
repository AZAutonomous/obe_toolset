//Kennon McKeever
//4.4.17
//ROI Detection

#include "opencv2/imgproc/imgproc.hpp"
#include <list>
#include "obe_toolset/ROI_detection.hpp"
#include <ros/console.h> //This is so we can send info or error messages when the ROI_detection method fails.

//This code is partly from here: http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html

//modifies the image in place, does what's found in the canny tutorial
cv::Mat CannyThreshold(cv::Mat& image, double lowThreshold, double highThreshold)
{
	cv::Mat retval;
	cv::blur(image, retval, cv::Size(3,3));
	cv::Canny(image, image, lowThreshold, highThreshold);
	return retval;
}
cv::Mat CannyThreshold(cv::Mat& image, double lowThreshold)
{
	return CannyThreshold(image, lowThreshold, 3.0 * lowThreshold);
}



//This function crops the original image and returns a list of images. All of said images are of the CV_ImAndPose type so they can be stamped with UTM location data.
std::list<CV_ImAndPose> ROI_detection(CV_ImAndPose imAndPose, double AGL_feet) //, double Roll_deg, double Pitch_deg, double Yaw_deg) //the roll pitch yaw stuff is to be figured out later. right now I assume 0,0,0 and commented this out so that nobody thinks the code will work if they decide to set them.
{
	std::list<CV_ImAndPose> output;
	cv::Mat img(imAndPose.image); //extract the image from the data packet
	if(!img.data)
	{
		ROS_ERROR("ROI_detection function has failed - imAndPose.image was empty");
		return output;
	}
	cv::Mat scratch_image(img), temp; //need a copy as a scratchpad
	//cv::InputArray Image_gray, Image_scratch, Image_edges, Image_filled;
	cv::cvtColor( scratch_image, scratch_image, CV_BGR2GRAY);

	temp = CannyThreshold(scratch_image, .2);

	cv::namedWindow("me");
	cv::imshow("me", temp);
	cv::waitKey(0);

	return output;
}

