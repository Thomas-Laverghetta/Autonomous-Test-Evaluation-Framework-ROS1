##############################################################################
# CMake
##############################################################################

cmake_minimum_required(VERSION 2.8.0)
project(demo4_viz)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

##############################################################################
# Catkin
##############################################################################

# qt_build provides the qt cmake glue, roscpp the comms for a default talker
find_package(catkin REQUIRED COMPONENTS
	qt_build
	roscpp 
	std_msgs 
	demo_node
	demo_float_object
	demo_location_object
	)

include_directories(
	${catkin_INCLUDE_DIRS} 
	${demo_node_INCLUDE_DIRS}
	${demo_float_object_INCLUDE_DIRS}
	${demo_location_object_INCLUDE_DIRS}
	)

## directories for other necessary libraries
link_directories(
	lib/tinyxml2
)

# Use this to define what the package will export (e.g. libs, headers).
# Since the default here is to produce only a binary, we don't worry about
# exporting anything. 
catkin_package()

##############################################################################
# Qt Environment
##############################################################################

# this comes from qt_build's qt-ros.cmake which is automatically 
# included via the dependency call in package.xml
# rosbuild_prepare_qt4(QtCore QtGui) # Add the appropriate components to the component list here

## Include necessary Qt package and files
find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui)
message(STATUS "QT_LIBRARIES = ${QT_LIBRARIES}")
include(${QT_USE_FILE})
include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

##############################################################################
# Sections
##############################################################################

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

file(GLOB QT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ui/*.ui)
file(GLOB QT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*.qrc)
file(GLOB_RECURSE QT_MOC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS include/*.h)

QT4_ADD_RESOURCES(QT_RESOURCES_CPP ${QT_RESOURCES})
QT4_WRAP_UI(QT_FORMS_HPP ${QT_FORMS})
QT4_WRAP_CPP(QT_MOC_HPP ${QT_MOC})

##############################################################################
# Sources
##############################################################################

file(GLOB_RECURSE QT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)

##############################################################################
# Binaries
##############################################################################

add_executable(${PROJECT_NAME}_node ${QT_SOURCES} ${QT_RESOURCES_CPP} ${QT_FORMS_HPP} ${QT_MOC_HPP})
target_link_libraries(${PROJECT_NAME}_node
 ${QT_LIBRARIES} 
 ${catkin_LIBRARIES}
 ${demo_node_LIBRARIES}
 ${demo_float_object_LIBRARIES}
 ${demo_location_object_LIBRARIES}
 tinyxml2
 )
# install(TARGETS ${PROJECT_NAME}_node RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

