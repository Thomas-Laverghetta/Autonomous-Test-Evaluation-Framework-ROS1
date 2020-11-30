#include "Sorter.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>
#include <algorithm>

#include <ros/ros.h> // For access to parameter server

// ------------------------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new FloatSorter();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


bool compare(FloatObject& i, FloatObject& j) { return(i.GetTime() < j.GetTime()); }   // Used to compare float object during Sort.

void FloatSorter::Setup(int argc, char** argv)
{
    // // Obtain mode from ROS parameter server
    // if(ros::param::has("~numInputs"))
    //     ros::param::get("~numInputs", numInputs);
    // else
    //     numInputs = 0;


    // for(int i = 0; i < numInputs; i++)
    // {
    //     FloatObject object;

    //     input.push_back(object);
    //     output.push_back(object);

    //     std::string input_topic = FindTopicName(std::string("input") + std::string(itoa(i+1)));
    //     std::string output_topic = FindTopicName(std::string("output") + std::string(itoa(i+1)));

    //     Subscribe(input_topic, &input[i]);
    // }

    std::string input1 = FindTopicName("input1");
    std::string input2 = FindTopicName("input2");
    std::string output1 = FindTopicName("output1");
    std::string output2 = FindTopicName("output2");

	Subscribe(input1, &inputObj1);
    Subscribe(input2, &inputObj2);

	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&FloatSorter::OnReceiveInput1));
    RegisterInputFunction(input2,static_cast<NodeFuncPtr>(&FloatSorter::OnReceiveInput2));	

	Publish(output1, &outputObj1);
    Publish(output2, &outputObj2);

	RegisterInitFunction(static_cast<NodeFuncPtr>(&FloatSorter::AppInit));

	RegisterCoreFunction(static_cast<NodeFuncPtr>(&FloatSorter::Process));

    RegisterExitFunction(static_cast<NodeFuncPtr>(&FloatSorter::OnExit));
}

void FloatSorter::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "FloatSorter";
}

void FloatSorter::AppInit()
{	
    // Example application specific initialization 
}


void FloatSorter::OnReceiveInput1()
{
    queue1.push_back(inputObj1);

    std::sort(queue1.begin(), queue1.end(), compare);
    
    printf("Recv Input1 - %i\n", inputObj1.GetTime());
    printf("Smallest Input1 - %i\n", queue1[0].GetTime());
}

void FloatSorter::OnReceiveInput2()
{
    queue2.push_back(inputObj2);

    std::sort(queue2.begin(), queue2.end(), compare);

    printf("Recv Input2 - %i\n", inputObj2.GetTime());
    printf("Smallest Input2 - %i\n", queue2[0].GetTime());
    
}

void FloatSorter::Process()
{
 	// Example handle termination signal CTRL-C --- Call "termination_handler"
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);	

    if(queue1.size() != 0 && queue2.size() != 0) // something is in the queues (vectors)
    {
        //printf("Something in both queues!!!!!! \n"); // debugging

        if(queue1[0].GetTime() == queue2[0].GetTime())
        {
            printf("Times match - %i, %i\n", queue1[0].GetTime(), queue2[0].GetTime());

            outputObj1 = queue1[0];
            outputObj2 = queue2[0];

            queue1.erase(queue1.begin());
            queue2.erase(queue2.begin());

            outputObj1.SetFlagged(true);
            outputObj2.SetFlagged(true);
        }
        // if it gets stuck from glitch
        else if (queue1.size() > 1 && queue2.size() > 1)
        { // New
            for (int j = 0; j < queue1.size(); j++)
            {
            	for(int i = 0; i < queue2.size(); i++)	
            	{
	                if (queue1[j].GetTime() == queue2[i].GetTime())
	                {
	                    printf("Times match FORLOOP- %i, %i\n", queue1[j].GetTime(), queue2[i].GetTime());
	                    outputObj1 = queue1[j];
	                    outputObj2 = queue2[i];

	                    // erasing the first through 'j' elements
	                    queue1.erase(queue1.begin(), queue1.begin() + j);
	                    queue2.erase(queue2.begin(), queue2.begin() + i);

	                    outputObj1.SetFlagged(true);
	                    outputObj2.SetFlagged(true);
	                    //break;
	                    return;
	                }
            	}
            }
        }
    }

}


void FloatSorter::OnExit()
{
    // Example handling application exit....
    printf("Node Finished.......\n");
}