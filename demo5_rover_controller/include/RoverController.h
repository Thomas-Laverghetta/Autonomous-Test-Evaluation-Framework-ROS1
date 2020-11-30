#pragma once


#include "Node.h"
#include "FloatObject.h"
#include "LocationObject.h"
#include "WheelAction.h"

class RoverController : public Node
{
private:

	WheelAction wheels;		

	float full_translation_speed;
	float full_rotation_speed;
	float update_interval;
	char keyboard_Input;

protected:

	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:

	void AppInit();
	void Process();
	void OnExit();
};