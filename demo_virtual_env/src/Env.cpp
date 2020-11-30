#include "Env.h"
#include "tinyxml2.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo_virtual_env/config/parameters.xml";

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new Environment();
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate();
}


void Environment::Setup(int argc, char** argv)
{
	std::string input1 = FindTopicName("input1");
	std::string output1 = FindTopicName("output1");
	std::string output2 = FindTopicName("output2");

	Subscribe(input1, &final_heading);

	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&Environment::OnReceiveHeading));
	
	Publish(output1, &virtual_range);
	Publish(output2, &virtual_angle);

	RegisterInitFunction(static_cast<NodeFuncPtr>(&Environment::AppInit));
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&Environment::Process));
}

void Environment::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "Environment";
}

void Environment::AppInit()
{
	if(Load(parameterFile) == false)
    {
    	printf("Failed to Load Environment File: %s\n", parameterFile);
    	Node::Get()->Terminate();
    }

    
}

bool Environment::Load(const char* filename)
{
  char wd[256];
  getcwd(wd, 256);
  printf("Current Working Directory = %s\n", wd);
  
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError result = doc.LoadFile(filename);

  if(result != tinyxml2::XML_SUCCESS)
    return(false);

  tinyxml2::XMLElement* pRoot=doc.RootElement();

  for(tinyxml2::XMLElement* pElem=pRoot->FirstChildElement(); pElem; pElem=pElem->NextSiblingElement())
  {
    	std::string elementName = pElem->Value();

    	if(elementName=="line")
		{
		  float a,b,c;
		  pElem->QueryFloatAttribute("a",&a);
		  pElem->QueryFloatAttribute("b",&b);
		  pElem->QueryFloatAttribute("c",&c);

		  float minX,maxX,minY,maxY;
		  pElem->QueryFloatAttribute("minX",&minX);
		  pElem->QueryFloatAttribute("maxX",&maxX);
		  pElem->QueryFloatAttribute("minY",&minY);
		  pElem->QueryFloatAttribute("maxY",&maxY);

		  Line line(a,b,c);
		  line.SetBounds(minX,maxX,minY,maxY);
		  lines.push_back(line);	  
		}
	    else if(elementName=="quadratic")
		{
		  float s,t,u,v;
		  pElem->QueryFloatAttribute("s",&s);
		  pElem->QueryFloatAttribute("t",&t);
		  pElem->QueryFloatAttribute("u",&u);
		  pElem->QueryFloatAttribute("v",&v);	  

		  float minX,maxX,minY,maxY;
		  pElem->QueryFloatAttribute("minX",&minX);
		  pElem->QueryFloatAttribute("maxX",&maxX);
		  pElem->QueryFloatAttribute("minY",&minY);
		  pElem->QueryFloatAttribute("maxY",&maxY);

		  Quadratic quadratic(s,t,u,v);
		  quadratic.SetBounds(minX,maxX,minY,maxY);
		  quadrs.push_back(quadratic);
		}      
		else if(elementName=="ellipse")
		{
		  float centerX, centerY, axisX, axisY;
		  pElem->QueryFloatAttribute("H",&centerX);
		  pElem->QueryFloatAttribute("K",&centerY);
		  pElem->QueryFloatAttribute("A",&axisX);
		  pElem->QueryFloatAttribute("B",&axisY);

		  Ellipse ellipse(centerX,centerY,axisX,axisY);
		  ellipses.push_back(ellipse);
		}    
    }

  return(true);	
}


void Environment::OnReceiveHeading()
{
	printf("TIME: %i\n", final_heading.GetTime());	// COPY TIME TESTING!!!!!

	float minDistance = FLT_MAX;
	float heading = final_heading.GetValue();
	float headingRadians = heading * 3.14159265f / 180.0f;
	float Delta = 0.0f;

	for(int i = 0; i < lines.size(); i++)		// Check intersection with all lines.....
	{
		float x, y;
		bool hit = lines[i].Intersect(headingRadians, x, y);

		if(hit)
		{
			float distance = sqrtf((x*x)+(y*y));
			if(distance < minDistance){		    
				minDistance = distance;
				Delta = lines[i].RelativeAngle(x,y,heading);
			}
		}
	}

	for(int i = 0; i < quadrs.size(); i++)		// Check intersection with all quadratic curves.....
	{
		float x, y;
		bool hit = quadrs[i].Intersect(headingRadians, x, y);
		if(hit)
		{
			float distance = sqrtf((x*x)+(y*y));
			if(distance < minDistance)		    
				minDistance = distance;
		}
	}

	for(int i = 0; i < ellipses.size(); i++)		// Check intersection with all ellipse curves.....
	{
		float x, y;
		bool hit = ellipses[i].Intersect(headingRadians, x, y);
		if(hit)
		{
			float distance = sqrtf((x*x)+(y*y));
			if(distance < minDistance)		    
				minDistance = distance;
		}
	}

	virtual_range = final_heading;		// COPY TIME TESTING!!!

	virtual_range.SetValue(minDistance);		// Set distance for publishing.....
	virtual_range.SetFlagged(true);
	virtual_angle.SetValue(Delta);				// Angle Found (Delta)
	virtual_angle.SetFlagged(true);

	if(minDistance != FLT_MAX)
		printf("Heading %f - Intersect at range %f - Angle %f\n", heading, minDistance, Delta);
	else
		printf("Heading %f - No Intersection\n", heading);
}


void Environment::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);	


    // Testing Rotation of Lines around axis out of screen
	for(int i = 0; i < lines.size(); i++)		// Check intersection with all lines.....
	{
		// lines[i].SetA(sin(1.0f * M_PI / 180.0f));
		lines[i].Rotate();
	}
	// ---------------------------------------------------
}