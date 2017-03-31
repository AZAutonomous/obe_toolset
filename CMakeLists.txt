cmake_minimum_required(VERSION 2.8.3)
project(obe_toolset)

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

## Generate added messages and services
#generate_messages( DEPENDENCIES
#  std_msgs  # Or other packages containing msgs
#)

# Declare our catkin package
catkin_package(
  CATKIN_DEPENDS message_runtime
)

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(Dispatcher src/DispatcherNode.cpp)
target_link_libraries(Dispatcher ${catkin_LIBRARIES})

add_executable(Processor src/ProcessorNode.cpp)
target_link_libraries(Processor ${catkin_LIBRARIES})

#add_executable(image_subscriber src/my_subscriber.cpp)
#target_link_libraries(image_subscriber ${catkin_LIBRARIES})


