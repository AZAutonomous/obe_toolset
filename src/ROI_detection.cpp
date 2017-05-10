//Kennon McKeever
//4.4.17
//ROI Detection
//This code is partly from here: http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html

#include "opencv2/imgproc/imgproc.hpp"
#include <list>
#include "obe_toolset/ROI_detection.hpp"
#include <ros/console.h> //This is so we can send info or error messages when the ROI_detection method fails.
#include <cmath>


//#define DEBUG_KENNON
//#define DESIGN_DAY
#define EXPORT_IMAGE_LOCAL
//#define KENNON_TEST_THRESH_VALS
#ifdef DESIGN_DAY
#include <stdlib.h>
#include <time.h>
#endif


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

cv::Mat KennonsSobelStuff(cv::Mat image, int PxValThresh)
{
	cv::Mat scratch, scratch_y;
	cv::blur(image, scratch, cv::Size(3,3));
	cv::Sobel(scratch, scratch_y, CV_32F, 0, 1); //y gradients in scratch_y
	cv::Sobel(scratch, scratch, CV_32F, 1, 0); //scratch now has the x gradients
	cv::magnitude(scratch, scratch_y, scratch); //scratch has the magnitude of both x and y gradients
	#ifdef DESIGN_DAY
	cv::imwrite(cv::String("../images/processed/Edges.jpg"), scratch);
	#endif
	scratch = scratch > PxValThresh;
	return scratch;
}

void displayImage(const cv::Mat& image)
{
	cv::namedWindow("me", cv::WINDOW_NORMAL);
	cv::imshow("me", image);
	cv::waitKey(10000000);
	cv::destroyWindow("me");
	//do I need to delete the window???
}

double degrees2radians(double degrees)
{
	return degrees * 3.1415926535898 / 180.;
}

//expands the rectangle in the first arg by the amount of pixels in the second arg.
void expandRectangle(cv::Rect& rectangle, int expansionPx)
{
	rectangle.width += expansionPx;
	rectangle.height += expansionPx;
}

// Expand rectangle to square, respecting image boundaries
cv::Rect padToSquare(cv::Rect rect, int imRows, int imCols)
{
	int maxdim = (rect.height > rect.width) ? rect.height : rect.width;
	// Pad height
	if (rect.height < maxdim) {
		int pad = maxdim - rect.height;
		int padtop = pad / 2;
		int padbot = pad - pad / 2;
		if (padtop > rect.y) {
			padbot += padtop - rect.y;
			padtop = rect.y; 
		}
		if (imRows <= rect.y + rect.height + padbot) {
			int temp = padbot;
			padbot = imRows - (rect.y + rect.height + 1);
			padtop += temp - padbot;
			padtop = (padtop > rect.y) ? rect.y : padtop; // Clamp to top of image)
		}
		rect.y -= padtop;
		rect.height += padbot;
	}
	// Pad width
	if (rect.width < maxdim) {
		int pad = maxdim - rect.width;
		int padleft = pad / 2;
		int padright = pad - pad / 2;
		if (padleft > rect.x) {
			padright += padleft - rect.x;
			padleft = rect.x; 
		}
		if (imCols <= rect.x + rect.width + padright) {
			int temp = padright;
			padright = imCols - (rect.x + rect.width + 1);
			padleft += temp - padright;
			padleft = (padleft > rect.x) ? rect.x : padleft; // Clamp to left of image)
		}
		rect.x -= padleft;
		rect.width += padright;
	}

    // Our result should be a square!
    assert(rect.width == rect.height);

	return rect;
}


//This function crops the original image and returns a list of images. All of said images are of the CV_ImAndPose type so they can be stamped with UTM location data.
std::list<CV_ImAndPose> ROI_detection(CV_ImAndPose imAndPose, double camera_vertical_FOV_degrees, double camera_horizontal_FOV_degrees) //, double Roll_deg, double Pitch_deg, double Yaw_deg) //the roll pitch yaw stuff is to be figured out later. right now I assume 0,0,0 and commented this out so that nobody thinks the code will work if they decide to set them.
{
	ROS_WARN_ONCE("Please note that the ROI detection currently can't handle location estimation with images that aren't downward-facing. Proceeding with roll, pitch = 0.");
	//these are settable values that change how this function behaves
	int threshVal = 99;
	int boundingBoxPxUpperLim = 48;
	int boundingBoxPxLowerLim = 24;

	int dilationSize = 9;

	std::list<CV_ImAndPose> output;
	cv::Mat img(imAndPose.image); //extract the image from the data packet
	if(!img.data)
	{
		ROS_ERROR("ROI_detection function has failed - imAndPose.image was empty");
		return output;
	}
	cv::Mat scratch_image(img), temp; //need a copy as a scratchpad
	//cv::InputArray Image_gray, Image_scratch, Image_edges, Image_filled;
	//cv::cvtColor( scratch_image, scratch_image, CV_BGR2GRAY);

	cv::Mat colors[3];
	cv::split(img, colors);

	#ifdef KENNON_TEST_THRESH_VALS
	int i = 0;
	for(int i = 75; i <= 105; i += 5)
	{
		temp = KennonsSobelStuff(colors[0], i);
		cv::Mat temp_small;
		cv::resize(temp, temp_small, cv::Size(), .23, .23);

		cv::namedWindow("me");
		cv::imshow("me", temp_small);
		cv::waitKey(500);

		ROS_INFO("Thresholding value is %i", i);
	}
	#endif //KENNON_TEST_THRESH_VALS

	//This is the part that should be changed if you want a better detection accuracy...
	cv::Mat accumulator = KennonsSobelStuff(colors[0], threshVal); //this one is for OR'ing the results all together
	for(int i = 1; i < 3; i++)
	{
		cv::Mat temp = KennonsSobelStuff(colors[i], threshVal);
	//	ROS_INFO("%i x %i", temp.rows, temp.cols);
		accumulator |=  temp;
	}

	cv::dilate(accumulator, accumulator, cv::Mat::ones(dilationSize, dilationSize, CV_8U)); //stretches current edges found by a few pixels
//	displayImage(accumulator);
	//This portion is in charge of getting the bounding rectangles.
	std::vector<std::vector<cv::Point>> contours; //probably a vector of vectors of points
	std::vector<cv::Rect> boundingRectangles;
	cv::findContours(accumulator, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); //finds all contours that aren't contained in another contour and gives the corner points of them.
	//there's a fair chance that the boundingRect stuff isn't needed.
	std::vector<cv::Mat> arr;
	cv::Mat bounding_box_mask;
	//arr.push_back(accumulator);
	/*for(size_t x = 0; x < contours.size(); ++x)
	{
		bounding_box_mask
	}*/

	//This part can be optimized not to use boundingRect (maybe) since the findContours function gives the bounding rect points anyway.
	//It filters the bounding rectangles based on the if statement's conditions.
	for(size_t contourIndex = 0; contourIndex < contours.size(); ++contourIndex)
	{
		cv::Rect tempRect = cv::boundingRect(contours[contourIndex]); //generate a rectangle based on the contour
		if(tempRect.width < boundingBoxPxUpperLim && tempRect.width > boundingBoxPxLowerLim && tempRect.height < boundingBoxPxUpperLim && tempRect.height > boundingBoxPxLowerLim) //this is the filter function for rectangles.
		{
			tempRect = padToSquare(tempRect, img.rows, img.cols);
			boundingRectangles.push_back(tempRect);
		}
	}

	#ifdef DESIGN_DAY
	cv::Mat drawing = img.clone();
	for(size_t i = 0; i < boundingRectangles.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255, 0, 255);
		cv::rectangle(drawing, boundingRectangles[i].tl(), boundingRectangles[i].br(), color, 3, 8, 0 );
	}
	//displayImage(drawing);
	cv::imwrite(cv::String("../images/processed/BoundingBoxes.jpg"), drawing);
	#endif

	//in order to find the ROI locations, we need to find the center of the image.
	double x_distance, y_distance; //x distance is the physical distance in the horizontal direction (cols)
	double x_0, y_0; //center of the image
	x_0 = img.cols / 2.0;
	y_0 = img.rows / 2.0;
	x_distance = 2.0 * imAndPose.z * tan(degrees2radians(camera_horizontal_FOV_degrees) / 2.0);
	y_distance = 2.0 * imAndPose.z * tan(degrees2radians(camera_vertical_FOV_degrees) / 2.0);

	//#ifdef DEBUG_KENNON
	//ROS_INFO("There should be %d ROIs found....", int(boundingRectangles.size()));
	//#endif

	//now we can loop over each of the bounding boxes to stamp them with locations.
	for( size_t i = 0; i < boundingRectangles.size(); ++i)
	{
		CV_ImAndPose msgData;
		msgData.image = img(boundingRectangles[i]); //crop image
		//displayImage(msgData.image);
		//find location in pixels
		double x_roi, y_roi; // _roi is center of the ROI
		x_roi = boundingRectangles[i].x + (boundingRectangles[i].width / 2.0);
		y_roi = boundingRectangles[i].y + (boundingRectangles[i].height / 2.0);
		//find the distance offset in the width and height directions.
		double width_offset, height_offset; //these are for the actual distance offsets due to the pixel offsets.
		width_offset = double(x_roi - x_0) / img.cols * x_distance;
		height_offset = double(y_0 - y_roi) / img.rows * y_distance; //Note that y_roi and y_0 are flipped. This is because y=0 is the top of the image and not the bottom.
		//assign the x and y position of the target, taking plane rotation into account.
		//NOTE: This method is invalid if there is anything other than 0 for roll and pitch.
		msgData.x = imAndPose.x + (width_offset * sin(imAndPose.yaw)) + (height_offset * cos(imAndPose.yaw));
		msgData.y = imAndPose.y + (width_offset * -1.0 * cos(imAndPose.yaw)) + (-1.0 * height_offset * sin(imAndPose.yaw));
		msgData.z = 0; //Anyone who has a preference for what this is can change it, but I assumed it should be 0 since the targets are on the ground.

		//Now, set the RPY of the image. Yaw is the only one that's important and used as of 4.26.17, so the others will be set as 0.
		msgData.roll = 0;
		msgData.pitch = 0;
		msgData.yaw = imAndPose.yaw;

		output.push_back(msgData); //save it!
		#ifdef DEBUG_KENNON
		ROS_INFO("ROI at pixel value %g,%g of image centered at %g,%g,%g at yaw %g (rad) is claimed to be at %g,%g", x_roi, y_roi, imAndPose.x, imAndPose.y, imAndPose.z, imAndPose.yaw, msgData.x, msgData.y);
		#endif
		#ifdef EXPORT_IMAGES_LOCAL
        static int counter = 0;
		cv::imwrite(cv::String("../images/rois/Cropped_") + cv::String("_") + std::to_string(counter++) + cv::String(".jpg"), img(boundingRectangles[i]));
		#endif
	}
	#ifdef DESIGN_DAY
	for (size_t i = 0; i < 3; ++i) {
		if (i < boundingRectangles.size())
			cv::imwrite(cv::String("../images/processed/ROI_") + std::to_string(i) + cv::String(".jpg"), img(boundingRectangles[i]));
		else
			cv::imwrite(cv::String("../images/processed/ROI_") + std::to_string(i) + cv::String(".jpg"), img(boundingRectangles[boundingRectangles.size() - 1]));
	}
	#endif	
	/*
	size_t boundingRectIndex = 0;
	for(size_t contourIndex = 0; contourIndex < contours.size(); ++contourIndex)
	{
		cv::Rect tempRect = cv::boundingRect(contours[contourIndex]); //generate a rectangle based on the contour
		if(tempRect.width > 75 || tempRect.width > 75) //this is the filter function for rectangles.

	}
	*/

	return output;
}


