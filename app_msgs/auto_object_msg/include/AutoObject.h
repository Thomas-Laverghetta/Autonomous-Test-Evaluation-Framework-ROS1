#ifndef AUTO_OBJ_H
#define AUTO_OBJ_H

#include "SerialObject.h"

template <class T>
class AutoObject : public SerialObject
{
	virtual void Serialize(char * outBuffer)
	{
		int index = 0;
		char * dataRef;

		AddToBuffer(outBuffer, dataRef, index, data);
	}

	virtual void Deserialize(const char * inBuffer)
	{
		int index = 0;
		char *dataRef;

		TakeFromBuffer(inBuffer, dataRef, index, data);
	}

	virtual int GetObjectSize()
	{	
		return sizeof(data);
	}
public:
	T data;
	AutoObject() {}
};

#endif