#include "FloatObject.h"

#include <cstdio>

void FloatObject::Serialize(char * outBuffer)
{
	int index = 0;

	AddToBuffer(outBuffer, (char*)&data, index, data);
}

void FloatObject::Deserialize(const char * inBuffer)
{
	int index = 0;

	TakeFromBuffer(inBuffer, (char*)&data, index, data);
}

int FloatObject::GetObjectSize()
{	
	return sizeof(data);
}
