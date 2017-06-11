#This is the image in question
sensor_msgs/Image image

#DEPRECATED - This is its coordinates in meters (UTM). Follows East North Up scheme. z is meters above ground level.
#This now contains position in LLA, where Lat and Lon are in degrees and z is altitude AGL
float64 lat
float64 lon
float64 z

#This is the rotation of the sensor when the image was captured. Yaw is in degrees and is 0 for North, 90 for East, etc. (like a compass)
float64 roll
float64 pitch
float64 yaw

