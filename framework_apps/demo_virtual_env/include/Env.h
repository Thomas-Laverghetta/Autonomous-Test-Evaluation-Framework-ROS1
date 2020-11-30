#pragma once

#include <vector>
#include <float.h>

#include "Node.h"
#include "Line.h"
#include "Quadratic.h"
#include "Ellipse.h"
#include "FloatObject.h"

class Environment : public Node
{
private:
	FloatObject final_heading;
	FloatObject virtual_range;
	std::vector<Line> lines;
	std::vector<Quadratic> quadrs;
	std::vector<Ellipse> ellipses;
	FloatObject virtual_angle;
protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:
	void AppInit();
	bool Load(const char* filename);
	void OnReceiveHeading();
	void Process();
};