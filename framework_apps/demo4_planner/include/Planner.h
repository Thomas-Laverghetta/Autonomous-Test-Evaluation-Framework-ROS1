#pragma once


#include "Node.h"
#include "DetectedObject.h"
#include "FloatObject.h"
#include "LocationObject.h"
#include "WheelAction.h"

#include <map>

class Planner : public Node
{
private:

	DetectedObject detected_object;
	LocationObject vehicle_location;
	WheelAction wheels;

	std::map<int, DetectedObject> internal_objects;

	bool recv_object;
	bool recv_location;
	float steering_timer;

	int recv_objectID;

	bool halt;

protected:

	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:

	void AppInit();	

	void OnReceiveObject();	

	void OnReceiveLocation();
	
	void Process();

	void OnExit();
};