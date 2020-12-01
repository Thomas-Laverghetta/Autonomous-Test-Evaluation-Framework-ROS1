#include "LocationObject.h"

#include <cstdio>

void LocationObject::Serialize(char * outBuffer)
{
	int index = 0;
	char * dataRef;

	dataRef = (char*)&_x;
	for (int i = 0; i < sizeof(_x) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];

	dataRef = (char*)&_y;
	for (int i = 0; i < sizeof(_y) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];

	dataRef = (char*)&_heading;
	for (int i = 0; i < sizeof(_heading) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];
}

void LocationObject::Deserialize(const char * inBuffer)
{
	int index = 0;
	char *dataRef;

	dataRef = (char*)&_x;
	for (int i = 0; i < sizeof(_x) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];

	dataRef = (char*)&_y;
	for (int i = 0; i < sizeof(_y) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];

	dataRef = (char*)&_heading;
	for (int i = 0; i < sizeof(_heading) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];	
}

int LocationObject::GetObjectSize()
{	
	return(
		sizeof(_x) + 
		sizeof(_y) + 
		sizeof(_heading)
		);
}
