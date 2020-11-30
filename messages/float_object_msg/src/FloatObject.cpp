#include "FloatObject.h"

#include <cstdio>

void FloatObject::Serialize(char * outBuffer)
{
	int index = 0;
	char * dataRef;

	AddToBuffer(outBuffer, dataRef, index, data);
}

void FloatObject::Deserialize(const char * inBuffer)
{
	int index = 0;
	char *dataRef;

	TakeFromBuffer(inBuffer, dataRef, index, data);
}

int FloatObject::GetObjectSize()
{	
	return sizeof(data);
}
