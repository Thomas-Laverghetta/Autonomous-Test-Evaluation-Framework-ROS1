#pragma once


#include "Node.h"
#include "FloatObject.h"
#include "LocationObject.h"
#include "WheelAction.h"

class RoverSim : public Node
{
private:

	WheelAction wheels;	
	// FloatObject vehicle_heading;
	LocationObject vehicle_location;	

	float full_translation_speed;
	float full_rotation_speed;
	float update_interval;

protected:

	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:

	void AppInit();
	bool Load(const char* filename);
	void Process();
	void Rotate(double elapsedTime);
	void Translate(double elapsedTime);
	void OnExit();
};