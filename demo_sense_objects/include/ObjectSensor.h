#pragma once

#include <vector>
#include <cfloat>

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/time.h>
// #include <sys/types.h>
// #include <fcntl.h>
// #include <termios.h>

#include "Node.h"
#include "FloatObject.h"
#include "PointObject.h"

class ObjectSensor : public Node
{
private:
	bool recv_range;
	bool recv_heading;

	FloatObject range;
	FloatObject heading;
	// PointObjectArray object_array;
	// PointObjectArray temp_array;

	PointObject points[PointObject::MAX_SECTIONS];
	PointObject points_to_send;
		
	unsigned int current_object_index;
	unsigned int previous_object_index;	
	float previous_range;		
protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:
	void AppInit();	
	void OnReceiveRange();
	void OnReceiveHeading();
	void Process();
	float SubtractHeadings(float h1, float h2);
	void AppExit();	
};