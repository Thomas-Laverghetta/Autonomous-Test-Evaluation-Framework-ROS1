#include "BasicNode.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new BasicNode();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void BasicNode::Setup(int argc, char** argv)
{
    std::string input_topic = FindTopicName("input1");
    std::string output_topic = FindTopicName("output1");

    // Example of subscribing to certain topic and connecting to object "input".    
	Subscribe(input_topic, &input);

    // Also example registing "OnReceiveInput" to be an input function called to handle received data from topic
	RegisterInputFunction(input_topic,static_cast<NodeFuncPtr>(&BasicNode::OnReceiveInput));	
	
    // Example of publishing to certain topic and connecting to object "output"
	Publish(output_topic, &output);

    // Example of registering "AppInit" to be an initialization function processed once after Setup()
	RegisterInitFunction(static_cast<NodeFuncPtr>(&BasicNode::AppInit));

    // Example of registering "Process" to be a core function processed continuously
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&BasicNode::Process));

    // Example of registering "OnExit" to be an exit function called before application exit
    RegisterExitFunction(static_cast<NodeFuncPtr>(&BasicNode::OnExit));

    // registers state object
    SaveStateClassRegister(GetSaveStateClassID(), BasicNode::New);
}

void BasicNode::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "BasicNode";
}

void BasicNode::AppInit()
{	
    // Example application specific initialization 
    output.data = 1;
    input.data = 1;

    printf("Output=%f : input=%f\n", output.data, input.data); fflush(stdout);

    output.SetFlagged(true);
    recv_input = false;
}


void BasicNode::OnReceiveInput()
{
	// Example handling of receiving data from "INPUT_TOPIC"
    recv_input = true;
}

void BasicNode::Process()
{   
 	// Example handle termination signal CTRL-C --- Call "termination_handler"
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);	

    // Example continuous "polling" until all input(s) received
    if(recv_input)
    {
        // Example of modifying output value and flagging data for publishing
        output.data = input.data * 2.0f;
        output.SetFlagged(true);

        recv_input = false; // reset flag for polling        
    }
    printf("Output=%f : input=%f\n", output.data, input.data); fflush(stdout);
    sleep(1);
}


void BasicNode::OnExit()
{
    // Example handling application exit....
    printf("Node Finished.......\n");
}




