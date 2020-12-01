#pragma once

#include "SerialObject.h"

class FloatObject : public SerialObject
{
public:
	float data;

	virtual void Serialize(char* outBuffer);
	virtual void Deserialize(const char* inBuffer);
	virtual int GetObjectSize();
};

