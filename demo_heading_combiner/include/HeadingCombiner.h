#pragma once

#include "Combiner.h"

class HeadingCombiner : public Combiner
{
protected:
	void SetNodeName(int argc, char** argv, std::string& nodeName);	
	void CreateObjects(int argc, char** argv, SerialObject*& physicalObject, SerialObject*& virtualObject, SerialObject*& outputObject);
	void SetTopicNames(int argc, char** argv, std::string& physicalName, std::string& virtualName, std::string& outputName);
	void SetMode(int argc, char**argv, int& mode);
	void Combine(SerialObject* physicalObject, SerialObject* virtualObject, SerialObject* outputObject);
};