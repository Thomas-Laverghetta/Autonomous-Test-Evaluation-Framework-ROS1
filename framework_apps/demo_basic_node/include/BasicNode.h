#pragma once


#include "Node.h"
#include "SaveState.h"
#include "FloatObject.h"

class BasicNode : public Node, public SaveState
{
	UNIQUE_SAVE_STATE_ID;
public:
	BasicNode() {}
	BasicNode(unsigned int id) : SaveState(id) {}
private:
	FloatObject	input;
	FloatObject output;	
	bool recv_input;
protected:

	// Setup -- REQUIRED
	// Sets up Subscriptions and Publishing for the node. Registers member functions for execution.
	void Setup(int argc, char** argv);

	// SetNodeName -- REQUIRED
	// Used to specify name that identifies node in the network
	void SetNodeName(int argc, char** argv, std::string& nodeName);

	void Save(std::ofstream& saveFile){
		saveFile.write((char*)&input.data, sizeof(input.data));
		saveFile.write((char*)&output.data, sizeof(output.data));
	}

	/* Deserialize state-save child object from load-file.*/
	void Load(std::ifstream& loadFile){
		loadFile.read((char*)&input.data, sizeof(input.data));
		loadFile.read((char*)&output.data, sizeof(output.data));
	}

	static SaveState * New(unsigned int id) { return new BasicNode(id); }
private:
	// AppInit -- 
	// Example Initialization function.  Called after Setup().  Used for application specific initialization
	void AppInit();	

	// OnReceiveInput --
	// Example Input function. Called on notification of received data from subscription topic. Used to handle receiving new data.
	void OnReceiveInput();	

	// Process --
	// Example Core function. Called every iteration of control loop. Used to do any continuous process the node might require.
	void Process();

	// OnExit -- 
	// Example Exit function. Called right before control loop exits; before the application closes. Used to handle any clean up
	void OnExit();
};