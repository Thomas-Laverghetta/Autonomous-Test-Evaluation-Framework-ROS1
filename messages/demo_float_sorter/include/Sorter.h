#pragma once

#include <vector>

#include "Node.h"
#include "FloatObject.h"

class FloatSorter : public Node
{
private:
	// std::vector<FloatObject> inputs;
	// std::vector<FloatObject> outputs;
	// std::vector<std::queue<FloatObject>> queues;
	// int numInputs;

	FloatObject inputObj1;
	FloatObject inputObj2;
	FloatObject outputObj1;
	FloatObject outputObj2;

	std::vector<FloatObject> queue1; // Aren't actual queues, just too lazy to change all references of queue
	std::vector<FloatObject> queue2;

protected:

	void Setup(int argc, char** argv);

	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:
	void AppInit();	

	void OnReceiveInput1();	

	void OnReceiveInput2();	

	void Process();

	void OnExit();
};