#pragma once

#include "SerialObject.h"
#include <vector>
#include <cfloat>

struct Vector2 {
	Vector2()
	{
		x = FLT_MAX;
		y = FLT_MAX;
	}

	float x, y;
};

class PointObject : public SerialObject
{
public:
	const static unsigned int MAX_SECTIONS = 10;
	const static unsigned int MAX_POINTS = 50;
private:
	Vector2 points[MAX_POINTS];	
	int currentPoint;
	int SectionIndex;
public:

	PointObject() { currentPoint = 0; }

	void AddPoint(float x, float y)
	{
		points[currentPoint].x = x;
		points[currentPoint].y = y;
		currentPoint = (currentPoint + 1) % MAX_POINTS;		
	}

	void ClearPoints()
	{
		for(int i = 0; i < MAX_POINTS; i++)
		{
			points[i].x = FLT_MAX;
			points[i].y = FLT_MAX;
		}

		currentPoint = 0;
	}
	int SectionIndexSet(int index)
	{	
		SectionIndex = index;
	}
	int SectionIndexGet()
	{	
		return SectionIndex;
	}
	Vector2* Points() { return points; }

	virtual void Serialize(char* outBuffer);
	virtual void Deserialize(const char* inBuffer);
	virtual int GetObjectSize();
};


// class PointObjectArray : public SerialObject
// {
// public:
// 	const static unsigned int MAX_OBJECTS = 20;
// private:
// 	PointObject objects[MAX_OBJECTS];
// public:
// 	PointObjectArray() { }

// 	PointObject& GetObject(int index) { return objects[index]; }

// 	virtual void Serialize(char* outBuffer);
// 	virtual void Deserialize(const char* inBuffer);
// 	virtual int GetObjectSize();
// };



