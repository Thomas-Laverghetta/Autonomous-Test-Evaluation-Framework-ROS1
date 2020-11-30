#include "PointObject.h"

void PointObject::Serialize(char * outBuffer)
{
	int index = 0;
	char * dataRef;

		for(int t = 0; t < PointObject::MAX_POINTS; t++)
		{
			//Vector2 pt = obj.Points()[t];

			dataRef = (char*)&(this->Points()[t].x);
			for (int i = 0; i < sizeof(this->Points()[t].x) / sizeof(char); i++)
				outBuffer[index++] = dataRef[i];

			dataRef = (char*)&(this->Points()[t].y);
			for (int i = 0; i < sizeof(this->Points()[t].y) / sizeof(char); i++)
				outBuffer[index++] = dataRef[i];

		
		}

		dataRef = (char*)&(SectionIndex);
		for (int i = 0; i < sizeof(SectionIndex) / sizeof(char); i++)
			outBuffer[index++] = dataRef[i];
}

void PointObject::Deserialize(const char * inBuffer)
{
	int index = 0;
	char *dataRef;


		for(int t = 0; t < PointObject::MAX_POINTS; t++)
		{
			//Vector2* pt = &obj.Points()[t];

			dataRef = (char*)&(this->Points()[t].x);
			for (int i = 0; i < sizeof(this->Points()[t].x) / sizeof(char); i++)
				dataRef[i] = inBuffer[index++];

			dataRef = (char*)&(this->Points()[t].y);
			for (int i = 0; i < sizeof(this->Points()[t].y) / sizeof(char); i++)
				dataRef[i] = inBuffer[index++];
		}

		dataRef = (char*)&(SectionIndex);
		for (int i = 0; i < sizeof(SectionIndex) / sizeof(char); i++)
			dataRef[i] = inBuffer[index++];
	
}

int PointObject::GetObjectSize()
{
	return(		
		sizeof(Vector2) * PointObject::MAX_POINTS +
		sizeof(SectionIndex)
		);
}












// void PointObjectArray::Serialize(char * outBuffer)
// {
// 	int index = 0;
// 	char * dataRef;

// 	for(int s = 0; s < PointObjectArray::MAX_OBJECTS; s++)
// 	{
// 		PointObject* obj = &objects[s];

// 		for(int t = 0; t < PointObject::MAX_POINTS; t++)
// 		{
// 			//Vector2 pt = obj.Points()[t];

// 			dataRef = (char*)&(obj->Points()[t].x);
// 			for (int i = 0; i < sizeof(obj->Points()[t].x) / sizeof(char); i++)
// 				outBuffer[index++] = dataRef[i];

// 			dataRef = (char*)&(obj->Points()[t].y);
// 			for (int i = 0; i < sizeof(obj->Points()[t].y) / sizeof(char); i++)
// 				outBuffer[index++] = dataRef[i];
// 		}
// 	}
// }

// void PointObjectArray::Deserialize(const char * inBuffer)
// {
// 	int index = 0;
// 	char *dataRef;

// 	for(int s = 0; s < PointObjectArray::MAX_OBJECTS; s++)
// 	{
// 		PointObject* obj = &objects[s];

// 		for(int t = 0; t < PointObject::MAX_POINTS; t++)
// 		{
// 			//Vector2* pt = &obj.Points()[t];

// 			dataRef = (char*)&(obj->Points()[t].x);
// 			for (int i = 0; i < sizeof(obj->Points()[t].x) / sizeof(char); i++)
// 				dataRef[i] = inBuffer[index++];

// 			dataRef = (char*)&(obj->Points()[t].y);
// 			for (int i = 0; i < sizeof(obj->Points()[t].y) / sizeof(char); i++)
// 				dataRef[i] = inBuffer[index++];
// 		}
// 	}	
// }

// int PointObjectArray::GetObjectSize()
// {
// 	return(		
// 		sizeof(Vector2) * PointObject::MAX_POINTS * PointObjectArray::MAX_OBJECTS
// 		);
// }
