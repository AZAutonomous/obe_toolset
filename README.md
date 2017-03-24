# obe_toolset
This is the ROS obe_toolset package that contains the Processor and Dispatcher Nodes. (It may also contain Rick's camera script in the future, too.)
The Dispatcher node is meant to take in images from a directory (see source for where this is, if implemented) along with Position and orientation data and publishes it to one of many processor nodes.
The Processor node takes a data packet from the Dispatcher node and performs ROI detection on the image. The sub-images are then packaged along with the region's Lat/Lon coordinates and sent to the base station.
