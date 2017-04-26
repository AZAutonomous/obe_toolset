#This is the image in question
sensor_msgs/Image image

#This is its coordinates in meters (UTM). Follows East North Up scheme. z is meters above ground level.
float64 x
float64 y 
float64 z

#This is the rotation of the sensor when the image was captured. yaw is 0 when east (along the x axis) and pi/2 when along the y axis (north)
float64 roll
float64 pitch
float64 yaw

