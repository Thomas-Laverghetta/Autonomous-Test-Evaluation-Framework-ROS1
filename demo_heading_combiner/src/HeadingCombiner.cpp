#include "HeadingCombiner.h"
#include "FloatObject.h"
#include <cmath>

#include <ros/ros.h> // For access to parameter server

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new HeadingCombiner();
}
// ------------------------------------------

// Wrap Angle
// Helper function to ensure input angle is between 0 and 360 degrees.
float WrapAngle(float angle)
{
  if(angle >= 360.0f)
    angle -= 360.0f;
  else if (angle < 0.0f)
    angle += 360.0f;

  return(angle);
}

// Set Node Name
void HeadingCombiner::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "HeadingCombiner";
}

// Create Objects
void HeadingCombiner::CreateObjects(int argc, char** argv, SerialObject*& physicalObject, SerialObject*& virtualObject, SerialObject*& outputObject)
{
	physicalObject = new FloatObject();
	virtualObject = new FloatObject();
	outputObject = new FloatObject();
}

// Set Topic Names
void HeadingCombiner::SetTopicNames(int argc, char** argv, std::string& physicalName, std::string& virtualName, std::string& outputName)
{
	physicalName = FindTopicName("input1");
	virtualName = FindTopicName("input2");
	outputName = FindTopicName("output1");
}

// Set Mode
// Heading combiner uses ROS parameter server to set mode of operation
// Default is mode 0 (physical)
void HeadingCombiner::SetMode(int argc, char**argv, int& mode)
{
	// Obtain mode from ROS parameter server
	if(ros::param::has("~mode"))
		ros::param::get("~mode", mode);
	else
		mode = 0;
}

// Heading Combine
// This function passes either the physical or virtual heading based on the assigned mode.
// A coordinate system transformation is performed that puts the data in to coordinate system used by other nodes.
// The new coordinate system is flipped and 90 offset from the incoming data.
void HeadingCombiner::Combine(SerialObject* physicalObject, SerialObject* virtualObject, SerialObject* outputObject)
{	
	float physicalHeading = static_cast<FloatObject*>(physicalObject)->GetValue();
	float virtualHeading = static_cast<FloatObject*>(virtualObject)->GetValue();

	float finalHeading = FLT_MAX;

	if (GetMode() == 0)
	{
		finalHeading = WrapAngle(90.0f - physicalHeading);
		// // COPY TIME TESTING!!!!!
		// *static_cast<FloatObject*>(outputObject) = *static_cast<FloatObject*>(physicalObject);
		// // COPY TIME TESTING!!!!!
		static_cast<FloatObject*>(outputObject)->SetTime(static_cast<FloatObject*>(physicalObject)->GetTime());
	}
	else if (GetMode() == 1)
	{
		finalHeading = WrapAngle(virtualHeading);
		// // COPY TIME TESTING!!!!!
		// *static_cast<FloatObject*>(outputObject) = *static_cast<FloatObject*>(virtualObject);
		// // COPY TIME TESTING!!!!!
		static_cast<FloatObject*>(outputObject)->SetTime(static_cast<FloatObject*>(virtualObject)->GetTime());
	}
	// else if (GetMode() == 2)	// --- No Augmentation Mode for this Combiner. Either pick physical or virtual
	// {
	// }


	static_cast<FloatObject*>(outputObject)->SetValue(finalHeading);
	printf("Heading:  %f\n", finalHeading);
}

