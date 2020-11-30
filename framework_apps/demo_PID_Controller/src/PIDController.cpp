#include "PIDController.h"

#include <cmath>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo_PID_Controller/config/parameters.xml";

//--------------------------
// IMPORTANT!! - Make sure there is a definition for CreateApplicationNode()
Node* CreateApplicationNode()
{
	return new PIDController();        // Make sure to change this to correct Node class type
}
// ------------------------------------------


void termination_handler (int signum)
{
  Node::Get()->Terminate(); // Example call to terminate the application with OS control signal
}


void PIDController::Setup(int argc, char** argv)
{    
    std::string input1 = FindTopicName("input1");
    std::string output1 = FindTopicName("output1");

    // Example of subscribing to certain topic and connecting to object "input".    
	Subscribe(input1, &Sensor_Array);
	Publish(output1, &wheels);

	RegisterInitFunction(static_cast<NodeFuncPtr>(&PIDController::AppInit));
    
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&PIDController::Process));
    
    RegisterExitFunction(static_cast<NodeFuncPtr>(&PIDController::OnExit));
}

void PIDController::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "PIDController";
}

void PIDController::AppInit()
{
    if(Load(parameterFile) == false)
    {
      printf("Failed to Load Environment File: %s\n", parameterFile);
      Node::Get()->Terminate();
    }
    zeros = false;

}


bool PIDController::Load(const char* filename)
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

    if(elementName=="pidcontroller")
    {
      pElem->QueryFloatAttribute("Kp",&Kp);
      pElem->QueryFloatAttribute("Kd",&Kd);    
      pElem->QueryIntAttribute("Desired",&Desired); 
    }
    if(elementName=="constants")
    {
      pElem->QueryIntAttribute("N",&N);
    }
  }
  return(true); 
}

void PIDController::Process()
{
    // Example handle termination signal CTRL-C --- Call "termination_handler"
  if (signal (SIGINT, termination_handler) == SIG_IGN)
      signal (SIGINT, SIG_IGN);  

  int sensorData[8], sum = 0, SensorCopy = Sensor_Array.GetValue();
  //printf("Sensor Copy = %i\n", SensorCopy);

  for(int i = 0; i <= 7; i++){ 
        // storing remainder in binary array 
    sensorData[i] = SensorCopy % 2; 
    SensorCopy = SensorCopy / 2;
    sum += sensorData[i]; 
    i++; 
  }
  if (!zeros){
    if (sum == 8) // Forward (Right on path)
    {
      LPWM = 84;
      RPWM = 212;
      printf("thing1\n");
    }
    // else if (sum == 0){ // Reverse (Lost signal)
    //   LPWM = 43;
    //   RPWM = 172;
    //   zeros = true;
    //   printf("Thing2\n");
    // }
    else if (sum >= 1)
    {
      if (sensorData[5] || sensorData[4]) // Mid
        Sensor_Array.SetValue(16); // 5th bit
      else if (sum > 1){
        int leftBits = 8, rightBits = 8;
        for(int j = 5; j <= 7; j++){
          if (sensorData[j] == 1){
            leftBits = j-4; // Distance from Mid (5th-bit) 
            break;
          }
        }
        for(int i = 3; i >= 0; i--){
          if (sensorData[i] == 1){
            rightBits = 4-i; // Distance from Mid
            break;
          }
        }
        if (leftBits < rightBits)
          Sensor_Array.SetValue(pow(2,leftBits+4)); // left
        else if (leftBits == rightBits){ // Choosing a side at random
          if (((rand() % 10) + 1)>5)
            Sensor_Array.SetValue(pow(2,leftBits+4)); // left
          else 
            Sensor_Array.SetValue(pow(2,abs(rightBits-4))); // right
        }
        else 
          Sensor_Array.SetValue(pow(2,abs(rightBits-4))); // right

      }
      // Calculating PID
      int error = Sensor_Array.GetValue() - Desired; // PID Error
      LPWM = (Kp * error + Kd*(error - LastError))+wheels.GetLeftWheel(); 
      RPWM = (Kp * error + Kd*(error - LastError))-wheels.GetRightWheel();
      LastError = error; 
      printf("Thing3\n");
    }
  }
  else
    if (sum != 0){ // Stop reversing 
      zeros = false;
      LPWM = 84;
      RPWM = 212;
      printf("Zero\n");
    }
  int r;
  for (int j = 0; j <= 2*N; j++){
    r = RPWM - j*63/N;
    if (r <= 63/N){
      RPWM = j*63;
      if (j == 0)
          RPWM++;
      break;
    }
  }
  LPWM = LPWM - 128;
  for (int j = 0; j <= 2*N; j++){
    r = LPWM - j*63/N;
    if (r <= 63/N){
      LPWM = j*63+128;
      break;
    }
  }
  LPWM = 42;
  RPWM = 170;
  wheels.SetLeftWheel(LPWM); 
  wheels.SetRightWheel(RPWM); 
  wheels.SetFlagged(true);
  printf("\tleftPID = %f\t right = %f\n", LPWM, RPWM);

}


void PIDController::OnExit()
{
	// Keyboard_Cleanup();
}