#pragma once

#include "SerialObject.h"
#include <cfloat>

class DetectedObject : public SerialObject
{
	float _x, _y;
	float _range;
	int _objectID;
	static int _nextID;
public:
	DetectedObject();

	float GetX() { return(_x); }
	void SetX(float x){ _x=x; }
	float GetY() { return(_y); }	
	void SetY(float y){ _y=y; }

	float GetRange() {return(_range);}
	void SetRange(float range){ _range=range; }

	int GetObjectID(){return(_objectID);}

	virtual void Serialize(char* outBuffer);
	virtual void Deserialize(const char* inBuffer);
	virtual int GetObjectSize();
};

