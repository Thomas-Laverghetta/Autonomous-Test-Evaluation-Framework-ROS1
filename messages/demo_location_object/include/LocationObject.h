#pragma once

#include "SerialObject.h"
#include <cfloat>

class LocationObject : public SerialObject
{
	float _x, _y, _heading;	
public:
	LocationObject() { _x = FLT_MAX; _y = FLT_MAX; }

	float GetX() { return(_x); }
	void SetX(float x){ _x=x; }
	float GetY() { return(_y); }	
	void SetY(float y){ _y=y; }

	float GetHeading() { return(_heading); }	
	void SetHeading(float heading){ _heading=heading; }

	virtual void Serialize(char* outBuffer);
	virtual void Deserialize(const char* inBuffer);
	virtual int GetObjectSize();
};

