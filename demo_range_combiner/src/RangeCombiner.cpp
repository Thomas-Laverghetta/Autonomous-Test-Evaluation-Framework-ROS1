#include "RangeCombiner.h"
#include "FloatObject.h"
#include <cmath>

#include <ros/ros.h> // For access to parameter server

// ------------------------------------------
Node* CreateApplicationNode()
{
	return new RangeCombiner();
}
// ------------------------------------------


void RangeCombiner::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "RangeCombiner";
}

void RangeCombiner::CreateObjects(int argc, char** argv, SerialObject*& physicalObject, SerialObject*& virtualObject, SerialObject*& outputObject)
{
	physicalObject = new FloatObject();
	virtualObject = new FloatObject();
	outputObject = new FloatObject();
}

void RangeCombiner::SetTopicNames(int argc, char** argv, std::string& physicalName, std::string& virtualName, std::string& outputName)
{
	physicalName = FindTopicName("input1");
	virtualName = FindTopicName("input2");
	outputName = FindTopicName("output1");
}

void RangeCombiner::SetMode(int argc, char**argv, int& mode)
{
	// Obtain mode from ROS parameter server
	if(ros::param::has("~mode"))
		ros::param::get("~mode", mode);
	else
		mode = 0;

	// Obtain minRange from ROS parameter server
	if(ros::param::has("~minRange"))
		ros::param::get("~minRange", minRange);
	else
		minRange = 0.0f;

	// Obtain maxRange from ROS parameter server
	if(ros::param::has("~maxRange"))
		ros::param::get("~maxRange", maxRange);
	else
		maxRange = 300.0f;

	// Obtain offset from ROS parameter server
	if(ros::param::has("~offset"))
		ros::param::get("~offset", offset);
	else
		offset = 0.0f;	
}

void RangeCombiner::Combine(SerialObject* physicalObject, SerialObject* virtualObject, SerialObject* outputObject)
{	
	float physicalRange = static_cast<FloatObject*>(physicalObject)->GetValue() + offset;
	float virtualRange = static_cast<FloatObject*>(virtualObject)->GetValue();
	long physicalTime = static_cast<FloatObject*>(physicalObject)->GetTime();
	long virtualTime = static_cast<FloatObject*>(virtualObject)->GetTime();
	float finalRange = FLT_MAX;

	if (GetMode() == 0)
	{
		finalRange = physicalRange;

		*static_cast<FloatObject*>(outputObject) = *static_cast<FloatObject*>(physicalObject);	// COPY TIME TESTING!!!!!
	}
	else if (GetMode() == 1)
	{
		finalRange = virtualRange;

		*static_cast<FloatObject*>(outputObject) = *static_cast<FloatObject*>(virtualObject);	// COPY TIME TESTING!!!!!

	}
	else if (GetMode() == 2)
	{
		//finalRange = fmin(physicalRange, virtualRange);

		if(physicalRange < virtualRange)
		{
			*static_cast<FloatObject*>(outputObject) = *static_cast<FloatObject*>(physicalObject);	// COPY TIME TESTING!!!!!
			finalRange = physicalRange;
		}
		else
		{
			*static_cast<FloatObject*>(outputObject) = *static_cast<FloatObject*>(virtualObject);	// COPY TIME TESTING!!!!!
			finalRange = virtualRange;
		}

		// NEW
		// which time to choose between phyical and virtual 
		if (virtualTime > physicalTime)
			static_cast<FloatObject*>(outputObject)->SetTime(physicalTime); 
		else 
			static_cast<FloatObject*>(outputObject)->SetTime(virtualTime);
	}

	if(finalRange < minRange || finalRange > maxRange)	// Min/Max Range filter....
		finalRange = FLT_MAX;

	static_cast<FloatObject*>(outputObject)->SetValue(finalRange);
	printf("FinalRange:  %f\n", finalRange);	
}

