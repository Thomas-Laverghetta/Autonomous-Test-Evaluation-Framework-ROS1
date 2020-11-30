#include "VirtualHeading.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new VirtualHeading();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void VirtualHeading::Setup(int argc, char** argv)
{
    std::string output = FindTopicName("output1");
	
    // Example of publishing to certain topic and connecting to object "output"
	Publish(output, &Heading);

    // Example of registering "AppInit" to be an initialization function processed once after Setup()
	RegisterInitFunction(static_cast<NodeFuncPtr>(&VirtualHeading::AppInit));

    // Example of registering "Process" to be a core function processed continuously
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&VirtualHeading::Process));

    // Example of registering "OnExit" to be an exit function called before application exit
    RegisterExitFunction(static_cast<NodeFuncPtr>(&VirtualHeading::OnExit));
}

void VirtualHeading::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "VirtualHeading";
}

void VirtualHeading::AppInit()
{	
    // Example application specific initialization 
    angle = 0.0f;
}

void VirtualHeading::Process()
{
    static long time = 0;

 	// Example handle termination signal CTRL-C --- Call "termination_handler"
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);	

    printf("angle - %f\n", angle);

    angle += 0.03;
    if (angle >= 360)
        angle = 0;

    Heading.SetValue(angle);
    Heading.SetTime(time);
    time += 1;
    Heading.SetFlagged(true);
    usleep(500); // Pause    
}


void VirtualHeading::OnExit()
{
    // Example handling application exit....
    printf("Node Finished.......\n");
}