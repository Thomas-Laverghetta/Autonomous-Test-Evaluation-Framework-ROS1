#include "WheelAction.h"

#include <cstdio>

void WheelAction::Serialize(char * outBuffer)
{
	int index = 0;
	char * dataRef;

	dataRef = (char*)&_leftWheel;
	for (int i = 0; i < sizeof(_leftWheel) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];

	dataRef = (char*)&_rightWheel;
	for (int i = 0; i < sizeof(_rightWheel) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];
}

void WheelAction::Deserialize(const char * inBuffer)
{
	int index = 0;
	char *dataRef;

	dataRef = (char*)&_leftWheel;
	for (int i = 0; i < sizeof(_leftWheel) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];

	dataRef = (char*)&_rightWheel;
	for (int i = 0; i < sizeof(_rightWheel) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];
}

int WheelAction::GetObjectSize()
{	
	return(
		sizeof(_leftWheel) + 
		sizeof(_rightWheel)
		);
}
