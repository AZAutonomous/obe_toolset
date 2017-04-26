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
	double x, y, z;// x and y are utm (EastNorthUp scheme), z is AGL altitude. All are in meters
	double roll, pitch, yaw; //These are in radians. Yaw is 0 at east, pi/2 at north. (Follows EastNorthUp scheme)
};

typedef struct CV_image_and_position__struct  CV_ImAndPose;

//The defaults for this are the original plans for the camera stuff
std::list<CV_ImAndPose> ROI_detection(CV_ImAndPose imAndPose, double camera_vertical_FOV_degrees = 50.0852, double camera_horizontal_FOV_degrees = 63.8418);

#endif //defined ROI_DETECTION_H
