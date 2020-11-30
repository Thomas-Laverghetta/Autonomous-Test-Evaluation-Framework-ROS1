#include "RoverSim.h"
#include "tinyxml2.h"
#include "Clock.h"
#include "Keyboard.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo4_rover_sim/config/parameters.xml";


// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new RoverSim();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void RoverSim::Setup(int argc, char** argv)
{    
	std::string input1;
	std::string output1;

	input1 = FindTopicName("input1");
	output1 = FindTopicName("output1");

	Subscribe(input1, &wheels);

	Publish(output1, &vehicle_location);

	RegisterInitFunction(static_cast<NodeFuncPtr>(&RoverSim::AppInit));
    
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&RoverSim::Process));
    
    RegisterExitFunction(static_cast<NodeFuncPtr>(&RoverSim::OnExit));
}

void RoverSim::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "RoverSim";
}

void RoverSim::AppInit()
{
	if(Load(parameterFile) == false)
    {
    	printf("Failed to Load Parameters File: %s\n", parameterFile);
    	Node::Get()->Terminate();
    }

    // x = 0;
    // y = 0;
    // theta = 180.0f;

	ResetClock();

	Keyboard_Init();
}

bool RoverSim::Load(const char* filename)
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
	  	
    	if(elementName=="rover")
		{
			float x,y, theta;
			pElem->QueryFloatAttribute("x",&x);
			pElem->QueryFloatAttribute("y",&y);
			pElem->QueryFloatAttribute("theta",&theta);

			printf("Initial Position: X:%f   Y:%f   Heading:%f\n", x,y,theta);

			// Initial location
			vehicle_location.SetX(x);
			vehicle_location.SetY(y);
			vehicle_location.SetHeading(theta);
			vehicle_location.SetFlagged(true);

			// vehicle attributes
			pElem->QueryFloatAttribute("rotationSpeed",&full_rotation_speed);
			pElem->QueryFloatAttribute("translationSpeed",&full_translation_speed);

			// simulation attributes
			pElem->QueryFloatAttribute("update",&update_interval);
		}
	}
    

  return(true);	
}



void RoverSim::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);


    Keyboard_Update(0, 1000);
    if(Keyboard_GetLastKey() == 'r')	// -- reload rover 
    {
		if(Load(parameterFile) == false)
		{
			printf("Failed to Load Parameters File: %s\n", parameterFile);
			Node::Get()->Terminate();
		}
    }

    float elapsedTime = ElapsedTime();
    if(elapsedTime > update_interval)
    {
    	//printf("LeftWheel %f| RightWheel %f|\n", wheels.GetLeftWheel(), wheels.GetRightWheel());
    	//printf("X: %f| Y: %f| HEADING: %f|\n", vehicle_location.GetX(), vehicle_location.GetY(), vehicle_location.GetHeading());

    	Rotate(elapsedTime);
    	Translate(elapsedTime);    	
    	ResetClock();    	
	}
}


void RoverSim::Rotate(double elapsedTime)
{
	float theta = vehicle_location.GetHeading();
	float leftWheel = wheels.GetLeftWheel();
	float rightWheel = wheels.GetRightWheel();
	float x = vehicle_location.GetX();
	float y = vehicle_location.GetY();	

	if(leftWheel != rightWheel)
	{
		int direction = (leftWheel > rightWheel) ? -1 : 1;

		float rotX;
		float rotY;

		// Define local axis of rotation for each wheel
		// if (direction == 1)			// Axis passing through Right wheel 
		// {
		// 	rotX = 5.0f;
		// 	rotY = -1.0f;
		// }
		// else if (direction == -1)	// Axis passing through Left wheel 
		// {
		// 	rotX = -5.0f;
		// 	rotY = -1.0f;
		// }
		if (leftWheel == -1 && rightWheel == 0){ // CCW Rev
			rotX = 5.0f;
			rotY = 0.0f;
			direction = 1;
		}
		else if (leftWheel == 0 && rightWheel == -1){ // CW REV
			rotX = -5.0f;
			rotY = 0.0f;
			direction = -1;
		}
		else if (leftWheel == 1 && rightWheel == 0){ // CW 
			rotX = 5.0f;
			rotY = 0.0f;
			direction = -1;
		}
		else if(leftWheel == 0 && rightWheel == 1){	// CCW
			rotX = -5.0f;
			rotY = 0.0f;
			direction = 1;
		} 
		else if (leftWheel == 1 && rightWheel == -1){ // Center CW REV
			rotX = 0.0f;
			rotY = 0.0f;
			direction = -1;
		}
		else if (leftWheel == -1 && rightWheel == 1){ // Center CCW Rev
			rotX = 0.0f;
			rotY = 0.0f;
			direction = 1;
		}


		// float speed = (abs(leftWheel - rightWheel) > 1) ? full_rotation_speed : full_rotation_speed / 2.0f;
		float speed = full_rotation_speed;
		float angleToApply = speed * elapsedTime * direction;


		// Transform axis into world coordinates....
		float c = cos(theta * M_PI / 180.0f);
		float s = sin(theta * M_PI / 180.0f);
		float t_rotX = c * rotX - s * rotY;
		float t_rotY = s * rotX + c * rotY;
		t_rotX += x;
		t_rotY += y;

		// --- Apply Rotation to Theta and X, Y coordinates --- //

		// angle to apply		
		c = cos(angleToApply * M_PI / 180.0f);
		s = sin(angleToApply * M_PI / 180.0f);

		// transform to origin
		x = x - t_rotX;
		y = y - t_rotY;

		// apply rotation
		float tx = c * x - s * y;
		float ty = s * x + c * y;

		// transform back
		x = tx + t_rotX;
		y = ty + t_rotY;

		float PHI = theta + angleToApply;

		if (PHI < 0)
			PHI += 360;
		else if (PHI > 360)
			PHI -= 360;

		// update orientation
		vehicle_location.SetHeading(PHI);
		vehicle_location.SetFlagged(true);

		// update location
		vehicle_location.SetX(x);
		vehicle_location.SetY(y);
		vehicle_location.SetFlagged(true);
	}


}


void RoverSim::Translate(double elapsedTime)
{
	float theta = vehicle_location.GetHeading();
	float leftWheel = wheels.GetLeftWheel();
	float rightWheel = wheels.GetRightWheel();
	float x = vehicle_location.GetX();
	float y = vehicle_location.GetY();

	if (leftWheel != 0 && rightWheel != 0 && leftWheel != (-rightWheel))
	{
		float moveVec[2];
		moveVec[0] = cos(theta * M_PI / 180.0f);
		moveVec[1] = sin(theta * M_PI / 180.0f);

		float speed = 0;

		// int maxWheel = (leftWheel > rightWheel) ? leftWheel : rightWheel;
		
		// if (maxWheel == 1)
		// 	speed = full_translation_speed / 2;
		// else if (maxWheel == 2)
		// 	speed = full_translation_speed;

		if(leftWheel > 0 && rightWheel > 0)
			speed = full_translation_speed;		// FORWARD
		else
			speed = -full_translation_speed;	// REVERSE		

		x = x + moveVec[0] * speed * elapsedTime;
		y = y + moveVec[1] * speed * elapsedTime;

		// update location
		vehicle_location.SetX(x);
		vehicle_location.SetY(y);
		vehicle_location.SetFlagged(true);
	}
}


void RoverSim::OnExit()
{
	Keyboard_Cleanup();
}
