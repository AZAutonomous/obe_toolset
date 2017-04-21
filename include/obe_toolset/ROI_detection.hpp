//Kennon McKeever
//4.4.17
//This "library" is meant to use the Canny edge detection method to extract ROIs.
#include <opencv2/highgui/highgui.hpp> //for the OpenCV stuff I think
#include <list> //list from std namespace
//#include <opencv2/opencv.hpp> //Is this what I need for the cv::MAT type?


//this function does the guassian blur, sobel operator, thresholding, bounding boxes, and cropping. The cropped images are (currently) saved in the current folder.


//include guard
#ifndef ROI_DETECTION_H
#define ROI_DETECTION_H


struct CV_image_and_position__struct{
	cv::Mat image;
	float x, y, z;
};

typedef struct CV_image_and_position__struct  CV_ImAndPose;

std::list<CV_ImAndPose> ROI_detection(CV_ImAndPose imAndPose, double AGL_feet);//, double Roll_deg = 0, double Yaw_deg = 0); //the RPY stuf is commented out (even though it defaults to zero) because it doesn't do anything at the moment.


#endif //defined ROI_DETECTION_H
