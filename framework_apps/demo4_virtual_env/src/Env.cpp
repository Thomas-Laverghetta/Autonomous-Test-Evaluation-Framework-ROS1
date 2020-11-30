#include "Env.h"
#include "tinyxml2.h"

#include "Clock.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo4_virtual_env/config/parameters.xml";

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
	Subscribe(input1, &vehicle_location);	
	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&Environment::OnReceiveLocation));

	std::string output1 = FindTopicName("output1");
	Publish(output1, &detected_object);

	std::string output2 = FindTopicName("output2");
	Publish(output2, &obstacle);	

	RegisterInitFunction(static_cast<NodeFuncPtr>(&Environment::AppInit));
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&Environment::Process));
}

void Environment::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "Environment";
}

void Environment::AppInit()
{
	sleep(3);

	srand(time(0));		// set random seed

	if(Load(parameterFile) == false)
    {
    	printf("Failed to Load File: %s\n", parameterFile);
    	Node::Get()->Terminate();
    }

	//recv_location = false;
    //ResetClock();
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

    	if(elementName=="object")
		{
		  float x,y;
		  pElem->QueryFloatAttribute("x",&x);
		  pElem->QueryFloatAttribute("y",&y);		  

		  DetectedObject obj;
		  obj.SetX(x);
		  obj.SetY(y);		  
		  objects.push_back(obj);
		}

    	if(elementName=="random")
		{
		  float minX,maxX,minY,maxY;
		  pElem->QueryFloatAttribute("minX",&minX);
		  pElem->QueryFloatAttribute("maxX",&maxX);
		  pElem->QueryFloatAttribute("minY",&minY);
		  pElem->QueryFloatAttribute("maxY",&maxY);	

		  int number;
		  pElem->QueryIntAttribute("number", &number);

			for(int i = 0; i < number; i++)
			{
				float x = ((rand()%100)/100.0f) * (maxX-minX) + minX; 
				float y = ((rand()%100)/100.0f) * (maxY-minY) + minY; 
				DetectedObject obj;
				obj.SetX(x);
				obj.SetY(y);		  
				objects.push_back(obj);
			}
		}		

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

		if(elementName=="environment")
		{
			pElem->QueryFloatAttribute("maxRange",&max_range);
			pElem->QueryFloatAttribute("fieldOfView",&field_of_view);
			pElem->QueryFloatAttribute("update",&update_interval);
		}
    }

  return(true);	
}

void Environment::OnReceiveLocation()
{
	//recv_location = true;

	DetectObject();		// New pose data available.  Compute collision detection.
}

void Environment::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);

    // Iterate and send out information about obstacles within environment
    static int object_ID = 0;
    if(object_ID < objects.size())
    {
    	printf("Publish Obstacle - %i\n", object_ID);
    	obstacle.SetX(objects[object_ID].GetX());
    	obstacle.SetY(objects[object_ID].GetY());
    	obstacle.SetFlagged(true);
    	object_ID++;
    }


 //    float elapsedTime = ElapsedTime();
 //    if(elapsedTime > update_interval && recv_location)
 //    {
 //    	DetectObject();
 //    	//printf("ElapsedTime in IF: %f\n",elapsedTime);
 //    	ResetClock();

 //    	recv_location = false;
 //	   }
}

void Environment::DetectObject()
{
	float x = vehicle_location.GetX();
	float y = vehicle_location.GetY();
	float theta = vehicle_location.GetHeading();
	float thetaRadians = theta * M_PI / 180.0f;
	float thetaFovRadians1 = (theta+field_of_view) * M_PI / 180.0f;
	float thetaFovRadians2 = (theta-field_of_view) * M_PI / 180.0f;

	float minSqDist = FLT_MAX;		// keep track of closest

	//printf("Vehicle X: %f| Y: %f| HEADING: %f|\n", x,y,theta);


	// -------- Obstacle Detection ----------- //
	for(int i = 0; i < objects.size(); i++)
	{
		// Compute distance to object
		float obj_x = objects[i].GetX();
		float obj_y = objects[i].GetY();
		float sqDist = ((obj_x - x) * (obj_x - x) + (obj_y - y) * (obj_y - y));
		objects[i].SetRange(sqrtf(sqDist));

		if(sqDist < max_range*max_range && sqDist < minSqDist)
		{
			minSqDist = sqDist;	// update minimum

			// Compute projection toward object
			float c = cos(thetaRadians);
			float s = sin(thetaRadians);
			float a = (obj_x - x) / sqrtf(sqDist);
			float b = (obj_y - y) / sqrtf(sqDist);
			float dot = c*a + s*b;

			//printf("Dot %f\n", dot);

			if(dot > cos(field_of_view * M_PI / 180.0f))	// within cos(angle/2)
			{
				detected_object = objects[i];	// copy contents to object to publish
				detected_object.SetFlagged(true);				
				//printf("DETECTED OBJECT - X: %f| Y: %f| ID:%i\n", obj_x,obj_y, detected_object.GetObjectID());
			}
		}
	}



	// -------- Line (Boundary) and Corner Detection ----------- //	
	bool hit1;
	bool hit2;
	float bx1,by1;
	float bx2,by2;

	// -------- Line (Boundary) Detection ----------- //	
	for(int i = 0; i < lines.size(); i++)
	{		
		float lx, ly, ldot;

		// ---- First Raycast (Left hand Field of View)
		bool hit = lines[i].Intersect(thetaFovRadians1, x, y, lx, ly);		
		if(hit)
		{
			float sqDist = ((lx-x)*(lx-x))+((ly-y)*(ly-y));

			//printf("Line HIT!!! %i, %f|%f  %f|%f\t", i, sqrtf(sqDist), max_range, lx, ly);

			if(sqDist < max_range*max_range && sqDist < minSqDist)
			{
				minSqDist = sqDist;	// update minimum

				// Compute projection toward detected point
				float c = cos(thetaRadians);
				float s = sin(thetaRadians);
				float a = (lx - x) / sqrtf(sqDist);
				float b = (ly - y) / sqrtf(sqDist);
				float dot = c*a + s*b;

				//printf("Dot %f\n", dot);

				//if(dot > cos(field_of_view * M_PI / 180.0f))	// within cos(angle/2)
				if(dot > 0.0f)
				{
					// Track which hits are good
					hit1 = true;
					bx1 = lx;
					by1 = ly;

					lineDetection.SetX(lx);
					lineDetection.SetY(ly);
					lineDetection.SetRange(sqrtf(sqDist));
					detected_object = lineDetection;
					detected_object.SetFlagged(true);
					//printf("DETECTED LINE - X: %f| Y: %f| ID:%i\n", lx, ly, detected_object.GetObjectID());
				}	
			}
		}


		// ---- Second Raycast (Right hand Field of View)
		hit = lines[i].Intersect(thetaFovRadians2, x, y, lx, ly);
		if(hit)
		{
			float sqDist = ((lx-x)*(lx-x))+((ly-y)*(ly-y));

			//printf("Line HIT!!! %i, %f|%f  %f|%f\t", i, sqrtf(sqDist), max_range, lx, ly);

			if(sqDist < max_range*max_range && sqDist < minSqDist)
			{
				minSqDist = sqDist;	// update minimum

				// Compute projection toward detected point
				float c = cos(thetaRadians);
				float s = sin(thetaRadians);
				float a = (lx - x) / sqrtf(sqDist);
				float b = (ly - y) / sqrtf(sqDist);
				float dot = c*a + s*b;

				//printf("Dot %f\n", dot);

				//if(dot > cos(field_of_view * M_PI / 180.0f))	// within cos(angle/2)
				if(dot > 0.0f)	
				{
					// Track which hits are good
					hit2 = true;
					bx2 = lx;
					by2 = ly;

					lineDetection.SetX(lx);
					lineDetection.SetY(ly);
					lineDetection.SetRange(sqrtf(sqDist));
					detected_object = lineDetection;
					detected_object.SetFlagged(true);
					//printf("DETECTED LINE - X: %f| Y: %f| ID:%i\n", lx, ly, detected_object.GetObjectID());
				}	
			}
		}
	}


	// ----- Corner detection (both rays hit) ------- //
	if(hit1 && hit2)
	{
		float bx = fabs(bx1) > fabs(bx2) ? bx1 : bx2;
		float by = fabs(by1) > fabs(by2) ? by1 : by2;
		float sqDist = ((bx-x)*(bx-x))+((by-y)*(by-y));
		lineDetection.SetX(bx);
		lineDetection.SetY(by);
		lineDetection.SetRange(sqrtf(sqDist));
		detected_object = lineDetection;
		detected_object.SetFlagged(true);		
	}




	// -----------------------------------------------------------------------------------------

}