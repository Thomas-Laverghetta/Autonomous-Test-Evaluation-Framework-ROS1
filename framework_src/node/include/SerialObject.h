#pragma once


class SerialObject
{
private:
	bool flagged;
public:
	virtual void Serialize(char* outBuffer) = 0;
	virtual void Deserialize(const char* inBuffer) = 0;
	virtual int GetObjectSize() = 0;

	template <class T>
	void AddToBuffer(char* dataBuffer, char* dataRef, int& index, T obj)
	{
		for (int i = 0; i < sizeof(T); i++) {
			dataBuffer[index++] = dataRef[i];
		}
	}

	template <class T>
	void TakeFromBuffer(const char* dataBuffer, char* dataRef, int& index, T obj)
	{
		for (int i = 0; i < sizeof(T); i++) {
			dataRef[i] = dataBuffer[index++];
		}
	}

	bool GetFlagged() { return(flagged); }
	bool SetFlagged(bool flag) { flagged = flag; }
};