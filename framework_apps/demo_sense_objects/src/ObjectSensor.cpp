#include "ObjectSensor.h"
#include "Clock.h"
#include "Keyboard.h"

#include <cmath>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
// #include <termios.h>


// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new ObjectSensor();
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate();
}


void ObjectSensor::Setup(int argc, char** argv)
{
	std::string input1 = FindTopicName("input1");
	std::string input2 = FindTopicName("input2");
	std::string output1 = FindTopicName("output1");

	Subscribe(input1, &range);
	Subscribe(input2, &heading);

	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&ObjectSensor::OnReceiveRange));
	RegisterInputFunction(input2,static_cast<NodeFuncPtr>(&ObjectSensor::OnReceiveHeading));

	Publish(output1, &points_to_send);	

	RegisterInitFunction(static_cast<NodeFuncPtr>(&ObjectSensor::AppInit));
	
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&ObjectSensor::Process));
}

void ObjectSensor::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "ObjectSensor";
}

void ObjectSensor::AppInit()
{	
	current_object_index = 0;	
	previous_object_index = 0;
	previous_range = FLT_MAX;
	recv_range = false;
	recv_heading = false;

	for(int i = 0; i < PointObject::MAX_SECTIONS; i++)
		points[i].SectionIndexSet(i);
	
	ResetClock();	// Start Clock for timing

	Keyboard_Init(); 
}

void ObjectSensor::OnReceiveRange()
{
	recv_range = true;
}

void ObjectSensor::OnReceiveHeading()
{
	recv_heading = true;
}

// ObjectSensor::Process
// --- Detects objects based on difference in range and heading over time.
// --- Object is detected based on detection of a "break point" (difference in range greater than some threshold or detecting of infinity)
// --- Only finite number of objects are stored in array format. Object index is based on the CCW or CW rotation from previous iteration
// --- If object is detected, X,Y points are added to the currently indexed object in the stored array. Object stored points are finite number, so only a certain number of recent points will be kept.
// --- Object array is published after reaching a break point after detecting and object
void ObjectSensor::Process()
{
	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);

  //   Keyboard_Update(0,1000);
  //   if(Keyboard_GetLastKey() == 'r')	// -- reset all points
  //   {
		// // for(int i = 0; i < PointObjectArray::MAX_OBJECTS; i++)
		// 	// temp_array.GetObject(i).ClearPoints();

		// for(int i = 0; i < PointObject::MAX_SECTIONS; i++)
		// 	points[i].ClearPoints();		

		// object_array = temp_array;
		// object_array.SetFlagged(true);
  //   }

	if(recv_range && recv_heading)
	{		
		// current_object_index = (heading.GetValue() / 360.0f) * PointObjectArray::MAX_OBJECTS;
		current_object_index = (heading.GetValue() / 360.0f) * PointObject::MAX_SECTIONS;

		printf("Elapsed=%f\n", ElapsedTime());

   //  	if(ElapsedTime() > 1.0f)	// copy & publish every # seconds
   //  	{
   //  		printf("CLOCK - Publishing Index %i.... Current Index = %i\n", previous_object_index, current_object_index);
			// // object_array = temp_array;		
			// // object_array.SetFlagged(true);

			// points_to_send = points[current_object_index];
			// points_to_send.SetFlagged(true);

			// ResetClock();
   //  	}


		if(current_object_index != previous_object_index)
		{
			printf("BREAK - Publishing Index %i.... Current Index = %i\n", previous_object_index, current_object_index);
			
			// object_array = temp_array;		// copy & publish moving from one section to another
			// object_array.SetFlagged(true);		
			// temp_array.GetObject(current_object_index).ClearPoints();

			points_to_send = points[current_object_index];
			points_to_send.SetFlagged(true);

			previous_object_index = current_object_index;
		}
		else
		{		
			if(range.GetValue() != FLT_MAX)
			{
				// compute coordinates
				float headingRadians = heading.GetValue() * 3.14159265f / 180.0f;			
				float x = range.GetValue() * cos(headingRadians);
				float y = range.GetValue() * sin(headingRadians);

				// add x,y point to current object in focus
				// temp_array.GetObject(current_object_index).AddPoint(x,y);
				points[current_object_index].AddPoint(x,y);								
			}
			else
			{
				// reset point to empty values
				float x = FLT_MAX;
				float y = FLT_MAX;
				points[current_object_index].AddPoint(x,y);				
			}
		}

		previous_range = range.GetValue();

		// reset flags for receiving data
		recv_range = false;
		recv_heading = false;

	}

}


// ObjectSensor::SubtractHeadings
// --- Helper function to find the smallest angle between two input angles in degrees.
// --- Params:	(in) h1 - first angle in degrees
//				(in) h2 - second angle in degrees
// --- Return:	smallest difference in angles in degrees
float ObjectSensor::SubtractHeadings(float h1, float h2)
{
	float d1 = h1 - h2;
	float d2 = h2 - h1;
	float d = abs(d1) < abs(d2) ? d1 : d2;
	return(d);
}



void ObjectSensor::AppExit()
{

}










