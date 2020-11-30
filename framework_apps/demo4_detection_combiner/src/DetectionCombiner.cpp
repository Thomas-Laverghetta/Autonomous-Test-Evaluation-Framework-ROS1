#include "DetectionCombiner.h"
#include "DetectedObject.h"
#include <cmath>

#include <ros/ros.h> // For access to parameter server

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
    return new DetectionCombiner();
}
// ------------------------------------------


// Set Node Name
void DetectionCombiner::SetNodeName(int argc, char** argv, std::string& nodeName)
{
    nodeName = "DetectionCombiner";
}

// Create Objects
void DetectionCombiner::CreateObjects(int argc, char** argv, SerialObject*& physicalObject, SerialObject*& virtualObject, SerialObject*& outputObject)
{
	physicalObject = new DetectedObject();
	virtualObject = new DetectedObject();
	outputObject = new DetectedObject();
}

// Set Topic Names
void DetectionCombiner::SetTopicNames(int argc, char** argv, std::string& physicalName, std::string& virtualName, std::string& outputName)
{
	physicalName = FindTopicName("input1");
	virtualName = FindTopicName("input2");
    outputName = FindTopicName("output1");
}

// Set Mode
// Heading combiner uses ROS parameter server to set mode of operation
// Default is mode 0 (physical)
void DetectionCombiner::SetMode(int argc, char**argv, int& mode)
{
    // Obtain mode from ROS parameter server
    if(ros::param::has("~mode"))
        ros::param::get("~mode", mode);
    else
        mode = 0;
}


void DetectionCombiner::Combine(SerialObject* physicalObject, SerialObject* virtualObject, SerialObject* outputObject)
{   
	DetectedObject* physicalDetected = static_cast<DetectedObject*>(physicalObject);
	DetectedObject* virtualDetected = static_cast<DetectedObject*>(virtualObject);
	DetectedObject* outputDetected = static_cast<DetectedObject*>(outputObject);


	if(GetMode() == 0)
	{
		*(outputDetected)=*(physicalDetected);	// copy physical
	}
	else if(GetMode() == 1)
	{
		*(outputDetected)=*(virtualDetected);	// copy virtual
	}
	else if(GetMode() == 2)
	{
		// Compare distance between object positions
		if(physicalDetected->GetRange() < virtualDetected->GetRange())
			*(outputDetected)=*(physicalDetected);	// copy physical
		else
			*(outputDetected)=*(virtualDetected);	// copy virtual
	}
}

