#pragma once

#include "Splitter.h"

class WheelSplitter : public Splitter
{
protected:	
	void SetNodeName(int argc, char** argv, std::string& nodeName);	
	void CreateObjects(int argc, char** argv, SerialObject*& physicalObject, SerialObject*& virtualObject, SerialObject*& inputObject);
	void SetTopicNames(int argc, char** argv, std::string& physicalName, std::string& virtualName, std::string& inputName);
	void SetMode(int argc, char**argv, int& mode);
	void Split(SerialObject* physicalObject, SerialObject* virtualObject, SerialObject* inputObject);
};