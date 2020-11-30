cmake_minimum_required(VERSION 2.8.3)

## Declare ROS package name
project(demo4_planner)


## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)


###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package( ) 


## Link to ROS libraries
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  demo_node
  demo_float_object
  demo_wheel_action
  demo_detected_object
  demo_location_object
  demo_clock_library
  demo_keyboard_input
)


## Specify additional locations of header files
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${demo_node_INCLUDE_DIRS}
  ${demo_float_object_INCLUDE_DIRS}
  ${demo_wheel_action_INCLUDE_DIRS}
  ${demo_detected_object_INCLUDE_DIRS}  
  ${demo_location_object_INCLUDE_DIRS}  
  ${demo_clock_library_INCLUDE_DIRS}  
  ${demo_keyboard_input_INCLUDE_DIRS} 
)


## directories for other necessary libraries
link_directories(
)


## Declare a C++ executable
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)
add_executable(${PROJECT_NAME}_node ${SOURCES})


## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
  ${demo_node_LIBRARIES}
  ${demo_float_object_LIBRARIES}
  ${demo_wheel_action_LIBRARIES}
  ${demo_detected_object_LIBRARIES}
  ${demo_location_object_LIBRARIES}
  ${demo_clock_library_LIBRARIES}
  ${demo_keyboard_input_LIBRARIES}  
)

