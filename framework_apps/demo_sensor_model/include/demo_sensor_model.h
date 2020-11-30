#pragma once


#include "Node.h"
#include "FloatObject.h"

#include "Distribution.h"

class demo_sensor_model : public Node
{
private:
	FloatObject	range_in, angle_in;
	FloatObject range_out;	
	bool recv_range_in, recv_angle_in;
	float tempRange;
	Distribution* error_function;
protected:

	// Setup -- REQUIRED
	// Sets up Subscriptions and Publishing for the node. Registers member functions for execution.
	void Setup(int argc, char** argv);

	// SetNodeName -- REQUIRED
	// Used to specify name that identifies node in the network
	void SetNodeName(int argc, char** argv, std::string& nodeName);
private:
	// AppInit -- 
	// Example Initialization function.  Called after Setup().  Used for application specific initialization
	void AppInit();	

	// OnReceiveInput --
	// Example Input function. Called on notification of received data from subscription topic. Used to handle receiving new data.
	void OnReceiveAngle();
	void OnReceiveRangeIN();

	// Process --
	// Example Core function. Called every iteration of control loop. Used to do any continuous process the node might require.
	void Process();

	float AngleError();
	float DistanceError();
	// OnExit -- 
	// Example Exit function. Called right before control loop exits; before the application closes. Used to handle any clean up
	void OnExit();
};