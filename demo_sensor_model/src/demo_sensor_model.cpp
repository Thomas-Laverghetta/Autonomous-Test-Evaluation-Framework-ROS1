#include "demo_sensor_model.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new demo_sensor_model();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void demo_sensor_model::Setup(int argc, char** argv)
{
    std::string input1 = FindTopicName("input1");
    std::string input2 = FindTopicName("input2");
    std::string output1 = FindTopicName("output1");

    // Example of subscribing to certain topic and connecting to object "input".    
	Subscribe(input1, &range_in);
    Subscribe(input2, &angle_in);

    // Also example registing "OnReceiveInput" to be an input function called to handle received data from topic
	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&demo_sensor_model::OnReceiveRangeIN));	
	RegisterInputFunction(input2,static_cast<NodeFuncPtr>(&demo_sensor_model::OnReceiveAngle));    
    
    // Example of publishing to certain topic and connecting to object "output"
	Publish(output1, &range_out);

    // Example of registering "AppInit" to be an initialization function processed once after Setup()
	RegisterInitFunction(static_cast<NodeFuncPtr>(&demo_sensor_model::AppInit));

    // Example of registering "Process" to be a core function processed continuously
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&demo_sensor_model::Process));

    // Example of registering "OnExit" to be an exit function called before application exit
    RegisterExitFunction(static_cast<NodeFuncPtr>(&demo_sensor_model::OnExit));
}

void demo_sensor_model::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "demo_sensor_model";
}

void demo_sensor_model::AppInit()
{	
    // Example application specific initialization 

    recv_angle_in = false;
    recv_range_in = false;

    error_function = new Normal(6.02, 4.25);	// Normal Params: Mean, Std dev
}


void demo_sensor_model::OnReceiveAngle()
{
	// Example handling of receiving data from "INPUT_TOPIC"

    recv_angle_in = true;
}

void demo_sensor_model::OnReceiveRangeIN()
{

    recv_range_in = true;
}

void demo_sensor_model::Process()
{
    // Example handle termination signal CTRL-C --- Call "termination_handler"
    if (signal (SIGINT, termination_handler) == SIG_IGN)
        signal (SIGINT, SIG_IGN);   

    // Example continuous "polling" until all input(s) received
    if(recv_angle_in && recv_range_in)
    {
        //tempRange = AngleError();
        tempRange = DistanceError()+range_in.GetValue();
        range_out = range_in;


        // Example of modifying output value and flagging data for publishing
        range_out.SetValue(tempRange);
        range_out.SetFlagged(true);

        printf("OUTRANGE SenModel: %f\n", tempRange);
        recv_range_in = false; // reset flag for polling
        recv_angle_in = false;
    }

}

// Issues with angle error.  Not being used right now.
float demo_sensor_model::AngleError(){
    float Range_IN = range_in.GetValue();
    float Angle_IN = angle_in.GetValue();

    // CCW Error Function: Erf = { f(x) = -0.575*x+163.18 | x = range }
    float ErrorAngleFunction = (-0.575*Range_IN+163.18);

    // 
    if (Angle_IN  >= 0 && Angle_IN <= 35)
        return (Range_IN);
    // CW
    else if (Angle_IN > 35)
        return (Range_IN+100);
    // CCW
    //else if (Angle_IN < 180 && Angle_IN >= ErrorAngleFunction)
      //  return (Range_IN+100);
    // No Error Found in Angle


}

float demo_sensor_model::DistanceError(){
    // Triangle Distribution
    // float U = (rand() % 100)/100.0f, a = 13, b = 6.1711875, c = -1.25;
    // if (U > 0 && U < ((c-a)/(b-a)))
    //     return (a+sqrtf((b-a)*(c-a)*U));
    // else if (U >= ((c-a)/(b-a)) && U < 1)
    //     return (b-sqrtf(b-a)*(b-c)*(1-U));

    return(error_function->GetRV());
}

void demo_sensor_model::OnExit()
{

}