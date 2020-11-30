#pragma once


#include "Node.h"
#include "IntObject.h"
#include "LocationObject.h"
#include "WheelAction.h"
#include "tinyxml2.h"


class PIDController : public Node
{
private:

	WheelAction wheels;		
	IntObject Sensor_Array;
	int LastError, N, LPWM, RPWM;
	bool Load(const char* filename);
	float Kp, Kd;
	int Desired;
	bool zeros;
	
protected:

	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:

	void AppInit();
	void Process();
	void OnExit();
};