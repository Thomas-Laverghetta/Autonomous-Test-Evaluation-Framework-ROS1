#include "Env.h"
#include "tinyxml2.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo_virtual_env_PID/config/parameters.xml";

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

    // Example of subscribing to certain topic and connecting to object "input".    
	Subscribe(input1, &rover);
	Publish(output1, &SensorData);

	RegisterInputFunction(input1, static_cast<NodeFuncPtr>(&Environment::OnReceiveLocation));

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

void Environment::LoadImage(std::string textureFile)
{
	// Load texture info
	image = new BMP(textureFile.c_str());

	// Save a copy of the texture's dimensions for later use
	textureWidth = image->GetWidth();
	textureHeight = image->GetHeight();
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

    	if(elementName=="Image")
		{
		  std::string ImageName;
		  ImageName = pElem->Attribute("LineImage");
		  pElem->QueryFloatAttribute("sensorWidth", &width);
		  pElem->QueryFloatAttribute("sensorOffSet",&offset);
		  LoadImage(ImageName);
		}
    }

  return(true);	
}


void Environment::OnReceiveLocation()
{
	CheckForLine();
}


void Environment::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);	

    

}

void Environment::CheckForLine()
{
	//width = textureWidth / 20.0f;
	//offset = textureHeight / 2.0f;
	theta = rover.GetHeading();
	int sum = 0;
	float p[9][2];
	int i[8][2];
	 x = rover.GetX();
	 y = rover.GetY();

	 printf("X = %f and Y = %f and Theta = %f\n", rover.GetX(),rover.GetY(),theta);
	// calculate middle point position (between 3 and 4)
	p[4][0] = x + sin(theta * M_PI / 180.0f) * offset;
	p[4][1] = y - cos(theta * M_PI / 180.0f) * offset;
	i[4][0] = int(p[4][0]);
	i[4][1] = int(p[4][1]);


	// Left Side of Array
	// calculate 3rd point
	p[3][0] = p[4][0] + sin((theta - 90) * M_PI / 180.0f) * width * 0.5;
	p[3][1] = p[4][1] - cos((theta - 90) * M_PI / 180.0f) * width * 0.5;
	i[3][0] = int(p[3][0]);
	i[3][1] = int(p[3][1]);

	// calculate 2nd point
	p[2][0] = p[4][0] + sin((theta - 90) * M_PI / 180.0f) * width * 1.5;
	p[2][1] = p[4][1] - cos((theta - 90) * M_PI / 180.0f) * width * 1.5;
	i[2][0] = int(p[2][0]);
	i[2][1] = int(p[2][1]);

	// Caluate 1 st
	p[1][0] = p[4][0] + sin((theta - 90) * M_PI / 180.0f) * width * 2.5;
	p[1][1] = p[4][1] - cos((theta - 90) * M_PI / 180.0f) * width * 2.5;
	i[1][0] = int(p[1][0]);
	i[1][1] = int(p[1][1]);

	// Caluate 0th
	p[0][0] = p[4][0] + sin((theta - 90) * M_PI / 180.0f) * width * 3.5;
	p[0][1] = p[4][1] - cos((theta - 90) * M_PI / 180.0f) * width * 3.5;
	i[0][0] = int(p[0][0]);
	i[0][1] = int(p[0][1]);

	// Right Side of Array
	// calculate 4th point
	p[5][0] = p[4][0] + sin((theta + 90) * M_PI / 180.0f) * width * 0.5;
	p[5][1] = p[4][1] - cos((theta + 90) * M_PI / 180.0f) * width * 0.5;
	i[5][0] = int(p[5][0]);
	i[5][1] = int(p[5][1]);

	// calculate 5th point
	p[5][0] = p[4][0] + sin((theta + 90) * M_PI / 180.0f) * width * 1.5;
	p[5][1] = p[4][1] - cos((theta + 90) * M_PI / 180.0f) * width * 1.5;
	i[5][0] = int(p[5][0]);
	i[5][1] = int(p[5][1]);
	
	// calculate 6th point
	p[6][0] = p[4][0] + sin((theta + 90) * M_PI / 180.0f) * width * 2.5;
	p[6][1] = p[4][1] - cos((theta + 90) * M_PI / 180.0f) * width * 2.5;
	i[6][0] = int(p[6][0]);
	i[6][1] = int(p[6][1]);

	// calculate 7th point
	p[7][0] = p[4][0] + sin((theta + 90) * M_PI / 180.0f) * width * 3.5;
	p[7][1] = p[4][1] - cos((theta + 90) * M_PI / 180.0f) * width * 3.5;
	i[7][0] = int(p[7][0]);
	i[7][1] = int(p[7][1]);


	// index 7st point in texture and check presence of line
	if (i[0][0] > 0 && i[0][0] < textureWidth && i[0][1] > 0 && i[0][1] < textureHeight)
	{
		int index = (i[0][0] * 3) + (textureHeight - i[0][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
			
	}


	// index 2nd point in texture and check presence of line
	if (i[1][0] > 0 && i[1][0] < textureWidth && i[1][1] > 0 && i[1][1] < textureHeight)
	{
		int index = (i[1][0] * 3) + (textureHeight - i[1][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}


	// index middle point in texture and check presence of line
	if (i[2][0] > 0 && i[2][0] < textureWidth && i[2][1] > 0 && i[2][1] < textureHeight)
	{
		int index = (i[2][0] * 3) + (textureHeight - i[2][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}

	// index 4th point in texture and check presence of line
	if (i[3][0] > 0 && i[3][0] < textureWidth && i[3][1] > 0 && i[3][1] < textureHeight)
	{
		int index = (i[3][0] * 3) + (textureHeight - i[3][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}

	// index 5th point in texture and check presence of line
	if (i[5][0] > 0 && i[5][0] < textureWidth && i[5][1] > 0 && i[5][1] < textureHeight)
	{
		int index = (i[5][0] * 3) + (textureHeight - i[5][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}

	// index 5th point in texture and check presence of line
	if (i[6][0] > 0 && i[6][0] < textureWidth && i[6][1] > 0 && i[6][1] < textureHeight)
	{
		int index = (i[6][0] * 3) + (textureHeight - i[6][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}

	// index 5th point in texture and check presence of line
	if (i[7][0] > 0 && i[7][0] < textureWidth && i[7][1] > 0 && i[7][1] < textureHeight)
	{
		int index = (i[7][0] * 3) + (textureHeight - i[7][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}

	// index 5th point in texture and check presence of line
	if (i[8][0] > 0 && i[8][0] < textureWidth && i[8][1] > 0 && i[8][1] < textureHeight)
	{
		int index = (i[8][0] * 3) + (textureHeight - i[8][1]) * (textureWidth * 3);
		uint8_t b = image->GetPixels()[index];
		if (b != 255){
			sum += 1;
			printf("1");
		}
		else 
			printf("0");
	}
	printf("\t sum = %i\n", sum);
	SensorData.SetValue(sum);
	SensorData.SetFlagged(true);

}