#include "RoverController.h"
#include "tinyxml2.h"
#include "Clock.h"
#include "Keyboard.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include <termios.h>
#include <fcntl.h>

const char* parameterFile = "../catkin_ws/src/demo_dumb_controller/config/parameters.xml";

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new RoverController();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void RoverController::Setup(int argc, char** argv)
{

	std::string input1 = FindTopicName("input1");
	std::string output1 = FindTopicName("output1");

	Publish(output1, &dumblocation);
	Subscribe(input1, &dumbwheels);

	RegisterInitFunction(static_cast<NodeFuncPtr>(&RoverController::AppInit));

	RegisterInputFunction(input1, static_cast<NodeFuncPtr>(&RoverController::OnWheels));
    
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&RoverController::Process));
    
    RegisterExitFunction(static_cast<NodeFuncPtr>(&RoverController::OnExit));
}

void RoverController::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "RoverController";
}

void RoverController::AppInit()
{
	if(Load(parameterFile) == false)
    {
    	printf("Failed to Load Environment File: %s\n", parameterFile);
    	Node::Get()->Terminate();
    }
	dumblocation.SetX(0);
	dumblocation.SetY(0);
	dumblocation.SetHeading(referenceHead);
	dumblocation.SetFlagged(true);
}


bool RoverController::Load(const char* filename)
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

    	if(elementName=="constants")
		{
			pElem->QueryIntAttribute("N",&N);
			pElem->QueryFloatAttribute("roverWidth",&roverWidth);
			pElem->QueryFloatAttribute("full_rotation_speed",&full_rotation_speed);
			pElem->QueryFloatAttribute("full_translation_speed",&full_translation_speed);
			pElem->QueryFloatAttribute("Heading",&referenceHead);

		}
    }

  return(true);	
}

void RoverController::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);

	printf("%f \t %f \t %f\n", dumblocation.GetX(), dumblocation.GetY(), dumblocation.GetHeading());
	float elapsedTime = ElapsedTime();
    if(elapsedTime > update_interval)
    {
    	//printf("LeftWheel %f| RightWheel %f|\n",dumbwheels.GetLeftWheel(),dumbwheels.GetRightWheel());
    	//printf("X: %f| Y: %f| HEADING: %f|\n", dumblocation.GetX(), dumblocation.GetY(), dumblocation.GetHeading());
    	Rotate(elapsedTime);
    	Translate(elapsedTime);    	
    	ResetClock();    	
	}
}


void RoverController::Rotate(double elapsedTime)
{
	float theta = dumblocation.GetHeading();
	float x = dumblocation.GetX();
	float y = dumblocation.GetY();	
	float Hw = roverWidth/2.0f;

	if(leftWheel != rightWheel)
	{
		int direction = (leftWheel > rightWheel) ? -1 : 1;

		float rotX;
		float rotY;

		// Distance 

		if (leftWheel != 0 && rightWheel ==0){
			rotX = roverWidth/2.0f;
			rotY = 0.0f;
			if (leftWheel > 0)
				direction = -1;
			else 
				direction = 1;
		}
		else if(rightWheel != 0 && leftWheel == 0){
			rotX = -roverWidth/2.0f;
			rotY = 0.0f;
			if (rightWheel > 0)
				direction = 1;
			else 
				direction = -1;
		}
		else {
			rotX = Hw/N*(leftWheel-rightWheel);
			if(leftWheel > rightWheel)
				direction = -1;
			else 
				direction = 1;
		}

		// float speed = (abs(leftWheel - rightWheel) > 1) ? full_rotation_speed : full_rotation_speed / 2.0f;
		float speed = full_rotation_speed;
		float angleToApply = abs(leftWheel-rightWheel)*speed * elapsedTime * direction;


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
		dumblocation.SetHeading(PHI);
		dumblocation.SetFlagged(true);

		// update location
		dumblocation.SetX(x);
		dumblocation.SetY(y);
		dumblocation.SetFlagged(true);
	}


}


void RoverController::Translate(double elapsedTime)
{
	float theta = dumblocation.GetHeading();
	float x = dumblocation.GetX();
	float y = dumblocation.GetY();

	if (leftWheel != 0 && rightWheel != 0 && leftWheel != (-rightWheel))
	{
		float moveVec[2];
		moveVec[0] = cos(theta * M_PI / 180.0f);
		moveVec[1] = sin(theta * M_PI / 180.0f);

		float speed = 0;

		if (abs(rightWheel) > abs(leftWheel))
			speed = rightWheel*full_translation_speed;
		else 
			speed = leftWheel*full_translation_speed;


		x = x + moveVec[0] * speed * elapsedTime;
		y = y + moveVec[1] * speed * elapsedTime;

		// update location
		dumblocation.SetX(x);
		dumblocation.SetY(y);
		dumblocation.SetFlagged(true);

	}
}

// Step of 3 (Might need to change)
void RoverController::OnWheels(){
	// Converting PWM to Range
	for (int k = -N; k <= N; k++){
		int LS = 63+63/N*k, RS = 191 + 63/N*k; // the step PWM calculations
		if (LS == 0) LS++; // PWM cannot equal 0
		if (LS == dumbwheels.GetLeftWheel()) leftWheel = k;
		if (RS == dumbwheels.GetRightWheel()) rightWheel = k;	
	}
	printf("\tLS = %f\t RS = %f\n", leftWheel,rightWheel);
}

void RoverController::OnExit()
{
	Keyboard_Cleanup();
}