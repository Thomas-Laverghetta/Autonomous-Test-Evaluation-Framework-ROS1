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
	std::string input1;
	std::string output1;

	output1 = FindTopicName("output1");

	Publish(output1, &wheels);

	RegisterInitFunction(static_cast<NodeFuncPtr>(&RoverController::AppInit));
    
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&RoverController::Process));
    
    RegisterExitFunction(static_cast<NodeFuncPtr>(&RoverController::OnExit));
}

void RoverController::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "RoverController";
}

void RoverController::AppInit()
{

	ResetClock();

	Keyboard_Init();
}

// // https://www.linuxquestions.org/questions/programming-9/game-programming-non-blocking-key-input-740422/
// int kbhit(void)
// {
//         struct termios oldt, newt;
//         int ch;
//         int oldf;

//         tcgetattr(STDIN_FILENO, &oldt);
//         newt = oldt;
//         newt.c_lflag &= ~(ICANON | ECHO);
//         tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//         oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
//         fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

//         ch = getchar();

//         tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//         fcntl(STDIN_FILENO, F_SETFL, oldf);

//         if(ch != EOF)
//         {
//                 ungetc(ch, stdin);
//                 return 1;
//         }

//         return 0;
// }


void RoverController::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);


    Keyboard_Update(0, 1000);
  //   switch(Keyboard_GetLastKey()){
  //   	case 'w':	// Forward
  //   		wheels.SetLeftWheel(1);
		// 	wheels.SetRightWheel(1);
		// 	wheels.SetFlagged(true);
		// 	break;
		// case 's':	// Reverse
		// 	wheels.SetLeftWheel(-1);
		// 	wheels.SetRightWheel(-1);
		// 	wheels.SetFlagged(true);
  //   		break;
  //   	case 'q':	// Stop (Kill Switch)
	 //    	wheels.SetLeftWheel(0);
		// 	wheels.SetRightWheel(0);
		// 	wheels.SetFlagged(true);
		// 	break;
		// case 'a':	// right
		// 	wheels.SetLeftWheel(1);
		// 	wheels.SetRightWheel(-1);
		// 	wheels.SetFlagged(true);
		// 	break;
		// case 'd':	// Left
		// 	wheels.SetLeftWheel(-1);
		// 	wheels.SetRightWheel(1);
		// 	wheels.SetFlagged(true);
		// 	break;	
  //   }
    if (Keyboard_GetLastKey() == 'w'){
    	wheels.SetLeftWheel(1);
		wheels.SetRightWheel(1);
		wheels.SetFlagged(true);
	}
	else if (Keyboard_GetLastKey() == 's'){
    	wheels.SetLeftWheel(-1);
		wheels.SetRightWheel(-1);
		wheels.SetFlagged(true);
	}
	else if (Keyboard_GetLastKey() == 'q'){
    	wheels.SetLeftWheel(0);
		wheels.SetRightWheel(0);
		wheels.SetFlagged(true);
	}
	else if (Keyboard_GetLastKey() == 'a'){
    	wheels.SetLeftWheel(-1);
		wheels.SetRightWheel(1);
		wheels.SetFlagged(true);
	}
	else if (Keyboard_GetLastKey() == 'd'){
    	wheels.SetLeftWheel(1);
		wheels.SetRightWheel(-1);
		wheels.SetFlagged(true);
	}

}


void RoverController::OnExit()
{
	Keyboard_Cleanup();
}