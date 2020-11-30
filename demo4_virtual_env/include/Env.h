#pragma once

#include "Node.h"
#include "DetectedObject.h"
#include "LocationObject.h"
#include "FloatObject.h"
#include "WheelAction.h"
#include "Line.h"

#include <vector>

class Environment : public Node
{
private:		

	LocationObject vehicle_location;
	DetectedObject detected_object;

	DetectedObject lineDetection;
	std::vector<DetectedObject> objects;
	std::vector<Line> lines;

	LocationObject obstacle;

	float max_range;
	float field_of_view;
	float update_interval;

	//bool recv_location;

protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:
	void AppInit();
	void OnReceiveLocation();
	bool Load(const char* filename);	
	void Process();
	void DetectObject();
};