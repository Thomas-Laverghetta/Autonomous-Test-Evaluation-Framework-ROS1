#include "Planner.h"
#include "Clock.h"
#include "Keyboard.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new Planner();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void Planner::Setup(int argc, char** argv)
{    
	std::string input1;
	std::string input2;
	std::string output1;

	input1 = FindTopicName("input1");
	input2 = FindTopicName("input2");
	output1 = FindTopicName("output1");

	Subscribe(input1, &detected_object);    
	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&Planner::OnReceiveObject));	

	Subscribe(input2, &vehicle_location);    
	RegisterInputFunction(input2,static_cast<NodeFuncPtr>(&Planner::OnReceiveLocation));

	Publish(output1, &wheels);
    
	RegisterInitFunction(static_cast<NodeFuncPtr>(&Planner::AppInit));
    
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&Planner::Process));
    
    RegisterExitFunction(static_cast<NodeFuncPtr>(&Planner::OnExit));
}

void Planner::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "Planner";
}

void Planner::AppInit()
{
	// --- Initially set to move forwards
	wheels.SetLeftWheel(1);
	wheels.SetRightWheel(1);
	wheels.SetFlagged(true);
	printf("Going straight......\n");

	recv_object = false;
	//steering_timer = FLT_MAX;
	steering_timer = 3.0f;
	ResetClock();

	recv_objectID = -1;

	srand (time(NULL));
}


void Planner::OnReceiveObject()
{
	float obj_x = detected_object.GetX();
	float obj_y = detected_object.GetY();
	recv_objectID = detected_object.GetObjectID();

	if(internal_objects.find(recv_objectID) == internal_objects.end())
	{
		internal_objects.insert(std::make_pair(recv_objectID, detected_object));
		//printf("Found new object: X: %f| Y:%f| ID:%i|\n", obj_x, obj_y, recv_objectID);		
	}
	else
	{
		//printf("Found existing object: X: %f| Y:%f| ID:%i|\n", obj_x, obj_y, recv_objectID);
	}
	
	recv_object = true;
}

void Planner::OnReceiveLocation()
{
	recv_location = true;
	//printf("Location (X,Y,THETA): (%f,%f,%f)\n", vehicle_location.GetX(), vehicle_location.GetY(), vehicle_location.GetHeading());
}
void Planner::Process()
{
 	// Example handle termination signal CTRL-C --- Call "termination_handler"
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);
    
    if(ElapsedTime() > steering_timer && !halt)
    {
    	steering_timer = FLT_MAX;
    	if (!recv_object){
	    	wheels.SetLeftWheel(1);
			wheels.SetRightWheel(1);
			wheels.SetFlagged(true);
			printf("Going straight......\n");
		}
    }


    if(recv_object && recv_location && !halt)
    {
    	if(steering_timer == FLT_MAX)
    	{
	    	float theta = vehicle_location.GetHeading();
	    	float x = vehicle_location.GetX();
	    	float y = vehicle_location.GetY();
	    	float obj_x = detected_object.GetX();
	    	float obj_y = detected_object.GetY();

			float sqDist = ((obj_x - x) * (obj_x - x) + (obj_y - y) * (obj_y - y));
			float c = cos(theta * M_PI / 180.0f);
			float s = sin(theta * M_PI / 180.0f);
			float a = (obj_x - x) / sqrtf(sqDist);
			float b = (obj_y - y) / sqrtf(sqDist);
			float dot = (c*b) - (s*a);	// actually cross-product			

			// printf("Dot %f\n", dot);
			// printf("Dot %f|%f|%f|%f|%f|%f|\n", sqDist, c, s, a, b, dot);

			if(dot < 0)	// object is to the right
			{
				wheels.SetLeftWheel(-1);
				wheels.SetRightWheel(1);
				wheels.SetFlagged(true);
				printf("Turning Left......\n");
			}
			else if(dot > 0) // object is to the left
			{
				wheels.SetLeftWheel(1);
				wheels.SetRightWheel(-1);
				wheels.SetFlagged(true);
				printf("Turning Right......\n");			
			}
			else
			{
				if(rand() % 100 < 50)		// random turn
				{
					wheels.SetLeftWheel(-1);
					wheels.SetRightWheel(1);
					wheels.SetFlagged(true);
					printf("Turning Left......\n");				
				}
				else
				{
					wheels.SetLeftWheel(1);
					wheels.SetRightWheel(-1);
					wheels.SetFlagged(true);
					printf("Turning Right......\n");									
				}
			}

			// Set timer to reset steering
			steering_timer = 3.0f;
			ResetClock();
		}

    	recv_object = false;
    	recv_location = false;
    }



    Keyboard_Update(0, 1000);
    switch(Keyboard_GetLastKey()){
    	case 'q':	// Stop (Kill Switch)
	    	wheels.SetLeftWheel(0);
			wheels.SetRightWheel(0);
			wheels.SetFlagged(true);
			halt = true;
			printf("HALT!!!!!\n");
			break;
		case 'w': 	// Resume (Take kill switch)
			halt = false;
			printf("Resume.....\n");
			break;
    }

}


void Planner::OnExit()
{
}




