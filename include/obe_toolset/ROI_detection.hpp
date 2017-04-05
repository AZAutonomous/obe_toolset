//Kennon McKeever
//4.4.17
//This "library" is meant to use the Canny edge detection method to extract ROIs.
#include <opencv/highgui/hughgui.hpp> //for the OpenCV stuff I think


//this function does the guassian blur, sobel operator, thresholding, bounding boxes, and cropping. The cropped images are (currently) saved in the current folder.
void ROI_detection(cv::InputArray& img);




