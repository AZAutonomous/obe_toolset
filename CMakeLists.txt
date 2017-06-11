cmake_minimum_required(VERSION 2.8.4)
project(obe_toolset)
SET(CMAKE_CXX_FLAGS "-std=c++0x") #May not be the best way to do this, but it works for now...

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  std_msgs
  message_generation
)

find_package(Boost REQUIRED COMPONENTS
  filesystem
)

# Generate messages in the 'msg' folder
add_message_files( FILES
  ImageAndPose.msg
)

# Generate added messages and services
generate_messages( DEPENDENCIES
  std_msgs
  geometry_msgs #for Point, which is honestly just a float x/y/z
  sensor_msgs #for ImagePtr
)

# Declare our catkin package
catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS message_runtime
)

include_directories(include ${catkin_INCLUDE_DIRS})
include_directories (${GeographicLib_INCLUDE_DIRS})

add_executable(Dispatcher src/DispatcherNode.cpp)
target_link_libraries(Dispatcher ${catkin_LIBRARIES}) #Also me testing stuff: ${GeographicLib_LIBRARIES})
add_dependencies(Dispatcher ${${PROJECT_NAME}_EXPORTED_TARGETS})


add_executable(Processor src/ProcessorNode.cpp src/ROI_detection.cpp)
target_link_libraries(Processor ${catkin_LIBRARIES})
add_dependencies(Processor ${${PROJECT_NAME}_EXPORTED_TARGETS})

#The SmallProcessor is a processor node that operates on quarter-sized images. It should have the same capabilities as a regular processor node, but seems to pick up false positives more easily (although this could be observed in the test image set only and may be fine for acutal missions)
add_executable(SmallProcessor src/SmallProcessorNode.cpp src/Small_ROI_detection.cpp) 
target_link_libraries(SmallProcessor ${catkin_LIBRARIES})
add_dependencies(SmallProcessor ${${PROJECT_NAME}_EXPORTED_TARGETS})


add_executable(FakeDispatcher src/FakeDispatcherNode.cpp)
target_link_libraries(FakeDispatcher ${catkin_LIBRARIES})
add_dependencies(FakeDispatcher ${${PROJECT_NAME}_EXPORTED_TARGETS})
