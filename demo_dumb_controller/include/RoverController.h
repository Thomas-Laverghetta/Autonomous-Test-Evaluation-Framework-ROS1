#pragma once


#include "Node.h"
#include "FloatObject.h"
#include "LocationObject.h"
#include "WheelAction.h"

class RoverController : public Node
{
private:
	LocationObject dumblocation;
	WheelAction dumbwheels;
	int leftWheel, rightWheel, N; // Step stuff
	float full_translation_speed, full_rotation_speed; // Speed Stuff
	float update_interval;
	float roverWidth;
	float referenceHead;
	bool Load(const char* filename);
protected:

	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:

	void AppInit();
	void Process();
	void OnExit();
	void OnWheels();

	void Translate(double elapsedTime);
	void Rotate(double elapsedTime);
};