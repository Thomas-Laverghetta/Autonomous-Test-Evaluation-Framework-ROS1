/*
* Creator: Thomas J Laverghetta (tlave002@odu.edu)
*/

#include "SaveStateManager.h"
#include <algorithm>

// Initializing Static Variables
vector<SaveState*> SaveStateManager::_SaveStateList = vector<SaveState*>();
map<unsigned int, NewFunctor> SaveStateManager::_classMap = map<unsigned int, NewFunctor>();

void SaveStateManager::Register(SaveState* ss)
{
	_SaveStateList.push_back(ss);
}

void SaveStateManager::Unregister(SaveState* ss)
{
	// removing ss from vector
	_SaveStateList.erase(std::remove(_SaveStateList.begin(), _SaveStateList.end(), ss), _SaveStateList.end());
}

void SaveStateManager::SaveAll(string& saveFile)
{
	printf("Saving All to %s\n", saveFile.c_str()); fflush(stdout);
	// Creating binary file
	ofstream file{ saveFile, std::ios::binary | std::ios::out };
	for (auto& ss : _SaveStateList) {
		// saving object id
		unsigned int id = ss->GetSaveStateId();
		file.write((char*)&id, sizeof(id));
		
		// saving class type
		id = ss->GetSaveStateClassId();
		file.write((char*)&id, sizeof(id));

		// saving object
		ss->Save(file);
	}
}

// binary search for ID
unsigned int FindId(vector<SaveState*>& ss, unsigned int id) {
	unsigned int r = ss.size() - 1;
	unsigned int l = 0;
	while (l <= r) {
		int m = l + (r - l) / 2;

		// Check if x is present at mid 
		if (ss[m]->GetSaveStateId() == id)
			return m;

		// If x greater, ignore left half 
		if (ss[m]->GetSaveStateId() < id)
			l = m + 1;

		// If x is smaller, ignore right half 
		else
			r = m - 1;
	}

	// if we reach here, then element was 
	// not present 
	return -1;
}
void SaveStateManager::LoadAll(string& loadFile)
{
	printf("LOADING %s\n", loadFile.c_str()); fflush(stdout);
	ifstream file{ loadFile, std::ios::binary | std::ios::in };
	if (!file.is_open()) {
		printf("ERROR: No Load File %s", loadFile.c_str()); fflush(stdout);
		exit(0);
	}

	while (true) {
		unsigned int objId;
		file.read((char*)&objId, sizeof(objId));

		if (file.eof()) {
			break;
		}

		unsigned int classId;
		file.read((char*)&classId, sizeof(classId));

		unsigned int index = FindId(_SaveStateList, objId);
		
		// if init object exist then load to it
		if (index != -1) {
			_SaveStateList[index]->Load(file);
		}

		// create dynamic state object
		else {
			if (_classMap.find(classId) != _classMap.end()){
				// calling static New(id) using function pointer
				_SaveStateList.push_back(_classMap.find(classId)->second(objId));
				_SaveStateList.back()->Load(file);
			}
			else{
				printf("ERROR: Unregistered State Found While Loading from %s\n\a", loadFile.c_str()); fflush(stdout);
				exit(0);
			}
		}
	}

	// Repoint Any pointers
	for (auto& ss: _SaveStateList){
		ss->Repoint(_SaveStateList);
	}
}

std::vector<SaveState*> SaveStateManager::GetSaveStateList()
{
	return _SaveStateList;
}

void SaveStateManager::RegisterClass(unsigned int classId, NewFunctor newFunctor)
{
	_classMap[classId] = newFunctor;
}


