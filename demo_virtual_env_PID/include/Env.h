#pragma once

#include <vector>
#include <float.h>

#include "Node.h"
#include "IntObject.h"
#include "BMP.h"
#include "LocationObject.h"

class Environment : public Node
{
private:
	int textureWidth, textureHeight; // size of image 
	BMP* image;
	float x, y, theta, width, offset;
	IntObject SensorData;
	LocationObject rover;
protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
	void LoadImage(std::string);
	void CheckForLine();
private:
	void AppInit();
	bool Load(const char* filename);
	void OnReceiveLocation();
	void Process();
};