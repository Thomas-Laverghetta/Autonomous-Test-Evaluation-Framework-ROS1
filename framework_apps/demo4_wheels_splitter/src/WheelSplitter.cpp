#include "WheelSplitter.h"
#include "WheelAction.h"
#include <cmath>

#include <ros/ros.h> // For access to parameter server

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
    return new WheelSplitter();
}
// ------------------------------------------


// Set Node Name
void WheelSplitter::SetNodeName(int argc, char** argv, std::string& nodeName)
{
    nodeName = "WheelSplitter";
}

// Create Objects
void WheelSplitter::CreateObjects(int argc, char** argv, SerialObject*& physicalObject, SerialObject*& virtualObject, SerialObject*& inputObject)
{
	physicalObject = new WheelAction();
	virtualObject = new WheelAction();
	inputObject = new WheelAction();
}

// Set Topic Names
void WheelSplitter::SetTopicNames(int argc, char** argv, std::string& physicalName, std::string& virtualName, std::string& inputName)
{
    physicalName = FindTopicName("output1");
    virtualName = FindTopicName("output2");
    inputName = FindTopicName("input1");
}

// Set Mode
// Heading combiner uses ROS parameter server to set mode of operation
// Default is mode 0 (physical)
void WheelSplitter::SetMode(int argc, char**argv, int& mode)
{
    // Obtain mode from ROS parameter server
    if(ros::param::has("~mode"))
        ros::param::get("~mode", mode);
    else
        mode = 0;
}


void WheelSplitter::Split(SerialObject* physicalObject, SerialObject* virtualObject, SerialObject* inputObject)
{   
	if(GetMode() == 0)
	{
		*(static_cast<WheelAction*>(physicalObject))=*(static_cast<WheelAction*>(inputObject));	// copy physical
	}
	else if(GetMode() == 1)
	{
		*(static_cast<WheelAction*>(virtualObject))=*(static_cast<WheelAction*>(inputObject));	// copy virtual
	}
	else if(GetMode() == 2)
	{
		*(static_cast<WheelAction*>(physicalObject))=*(static_cast<WheelAction*>(inputObject));	// copy physical
		*(static_cast<WheelAction*>(virtualObject))=*(static_cast<WheelAction*>(inputObject));	// copy virtual
	}
}

