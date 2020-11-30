#include "DetectedObject.h"

#include <cstdio>

int DetectedObject::_nextID = 0;

DetectedObject::DetectedObject() 
{ 
	_x = FLT_MAX;
	_y = FLT_MAX;
	_objectID = _nextID;
	_nextID = _nextID + 1;
}

void DetectedObject::Serialize(char * outBuffer)
{
	int index = 0;
	char * dataRef;

	dataRef = (char*)&_x;
	for (int i = 0; i < sizeof(_x) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];

	dataRef = (char*)&_y;
	for (int i = 0; i < sizeof(_y) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];

	dataRef = (char*)&_range;
	for (int i = 0; i < sizeof(_range) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];

	dataRef = (char*)&_objectID;
	for (int i = 0; i < sizeof(_objectID) / sizeof(char); i++)
		outBuffer[index++] = dataRef[i];
}

void DetectedObject::Deserialize(const char * inBuffer)
{
	int index = 0;
	char *dataRef;

	dataRef = (char*)&_x;
	for (int i = 0; i < sizeof(_x) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];

	dataRef = (char*)&_y;
	for (int i = 0; i < sizeof(_y) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];

	dataRef = (char*)&_range;
	for (int i = 0; i < sizeof(_range) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];	

	dataRef = (char*)&_objectID;
	for (int i = 0; i < sizeof(_objectID) / sizeof(char); i++)
		dataRef[i] = inBuffer[index++];
}

int DetectedObject::GetObjectSize()
{	
	return(
		sizeof(_x) + 
		sizeof(_y) + 
		sizeof(_range) + 
		sizeof(_objectID)
		);
}
