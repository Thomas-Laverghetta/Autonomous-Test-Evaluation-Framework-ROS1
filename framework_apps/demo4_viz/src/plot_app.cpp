#include "plot_app.h"
#include "main_window.h"
#include "tinyxml2.h"

#include <signal.h>
#include <unistd.h>

const char* envParameterFile = "../catkin_ws/src/demo4_virtual_env/config/parameters.xml";

const char* parameterFile = "../catkin_ws/src/demo4_viz/config/parameters.xml";

// ------------------------------------------
Node* CreateApplicationNode()
{
	return new PlotApp();
}
// ------------------------------------------

void termination_handler (int signum)
{
  Node::Get()->Terminate();
}

void PlotApp::Setup(int argc, char** argv)
{	
	qt_worker_future = std::async(std::launch::async, &QtWorkerObj::Run, &worker, argc, argv);

	std::string input1;
	std::string input2;
	std::string input3;

	input1 = FindTopicName("input1");
	Subscribe(input1, &vehicle_location);		
	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&PlotApp::OnReceiveLocation));

	input2 = FindTopicName("input2");
	Subscribe(input2, &obstacle);		
	RegisterInputFunction(input2,static_cast<NodeFuncPtr>(&PlotApp::OnReceiveObstacle));

	RegisterInitFunction(static_cast<NodeFuncPtr>(&PlotApp::Init));
	
	RegisterCoreFunction(static_cast<NodeFuncPtr>(&PlotApp::Process));
	
}

void PlotApp::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "PLOT_OBJECTS_QT";
}

void PlotApp::Init()
{
	sleep(3);		// Wait until Qt thread starts

	if(Load(parameterFile) == false)
    {
    	printf("Failed to Load File: %s\n", parameterFile);
    	Node::Get()->Terminate();
    }

    if(LoadEnv(envParameterFile) == false)
    {
    	printf("Failed to Load Environment File: %s\n", envParameterFile);
    	Node::Get()->Terminate();
    }
}



bool PlotApp::Load(const char* filename)
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

		if(elementName=="window")
		{
			float minX,maxX,minY,maxY;
			pElem->QueryFloatAttribute("minX",&minX);
			pElem->QueryFloatAttribute("maxX",&maxX);
			pElem->QueryFloatAttribute("minY",&minY);
			pElem->QueryFloatAttribute("maxY",&maxY);

			worker.setBounds(minX,maxX,minY,maxY);
		}
    }

  return(true);	
}


bool PlotApp::LoadEnv(const char* filename)
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

    	// if(elementName=="object")
		// {
		//   float x,y;
		//   pElem->QueryFloatAttribute("x",&x);
		//   pElem->QueryFloatAttribute("y",&y);		  

		//   worker.drawBox(x,y);
		// }


		if(elementName=="line")
		{
			float a,b,c;
			pElem->QueryFloatAttribute("a",&a);
			pElem->QueryFloatAttribute("b",&b);
			pElem->QueryFloatAttribute("c",&c);

			float minX,maxX,minY,maxY;
			pElem->QueryFloatAttribute("minX",&minX);
			pElem->QueryFloatAttribute("maxX",&maxX);
			pElem->QueryFloatAttribute("minY",&minY);
			pElem->QueryFloatAttribute("maxY",&maxY);

			float x0,y0,x1,y1;
			if(b!=0)
			{
				x0 = minX;
				x1 = maxX;
				y0 = (-1/b)*(a*x0 + c);
				y1 = (-1/b)*(a*x1 + c);
			}
			else
			{
				x0 = c;
				x1 = c;
				y0 = minY;
				y1 = maxY;
			}

			worker.drawLine(x0,y0,x1,y1);
		}


		if(elementName=="environment")
		{
			pElem->QueryFloatAttribute("maxRange",&max_range);
			pElem->QueryFloatAttribute("fieldOfView",&field_of_view);			

			worker.setMaxRange(max_range);
			worker.setFieldOfView(field_of_view);
		}
    }

  return(true);	
}


void PlotApp::OnReceiveLocation()
{
	//printf("Received Location:  X:%f| Y:%f| Theta:%f \n", vehicle_location.GetX(), vehicle_location.GetY(), vehicle_location.GetHeading());
	
	worker.drawArrow(vehicle_location.GetX(), vehicle_location.GetY(), vehicle_location.GetHeading());
}

void PlotApp::OnReceiveObstacle()
{
	// printf("Received Obstacle:  X:%f| Y:%f\n", obstacle.GetX(), obstacle.GetY());
	
	worker.drawObstacle(obstacle.GetX(), obstacle.GetY());	
}

void PlotApp::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);

	std::chrono::milliseconds span (0);
    if(qt_worker_future.wait_for(span) == std::future_status::ready)
    	Node::Get()->Terminate();	
}



void QtWorkerObj::Run(int argc, char** argv)
{
	QApplication app(argc, argv);

	float minX,maxX,minY,maxY;

	// Default bounds
	minX = -300.0f;
	maxX = 300.0f;
	minY = -300.0f;
	maxY = 300.0f;

	window = new MainWindow();
	window->resize(800,800);
	window->setBounds(minX,maxX,minY,maxY);
	window->show();	

	app.exec();	

	printf("\n// Qt Thread Exit--------------------\n");
}


void QtWorkerObj::drawArrow(qreal x, qreal y, qreal theta)
{
	if(!QMetaObject::invokeMethod(window, "drawArrow", Qt::QueuedConnection, Q_ARG(qreal, x), Q_ARG(qreal, y), Q_ARG(qreal, theta)))
		printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::drawObstacle(qreal x, qreal y)
{
	if(!QMetaObject::invokeMethod(window, "drawObstacle", Qt::QueuedConnection, Q_ARG(qreal, x), Q_ARG(qreal, y)))
		printf(" Failed to invoke method\n");
		;
}


void QtWorkerObj::drawLine(qreal x0, qreal y0, qreal x1, qreal y1)
{
	if(!QMetaObject::invokeMethod(window, "drawLine", Qt::QueuedConnection, Q_ARG(qreal, x0), Q_ARG(qreal, y0), Q_ARG(qreal, x1), Q_ARG(qreal, y1)))
		printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::setMaxRange(qreal size)
{
	if(!QMetaObject::invokeMethod(window, "setMaxRange", Qt::QueuedConnection, Q_ARG(qreal, size)))
		printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::setFieldOfView(qreal fov)
{
	if(!QMetaObject::invokeMethod(window, "setFieldOfView", Qt::QueuedConnection, Q_ARG(qreal, fov)))
		printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::setBounds(qreal minX, qreal maxX, qreal minY, qreal maxY)
{
	if(!QMetaObject::invokeMethod(window, "setBounds", Qt::QueuedConnection, Q_ARG(qreal, minX), Q_ARG(qreal, maxX), Q_ARG(qreal, minY), Q_ARG(qreal, maxY)))
		printf(" Failed to invoke method\n");
		;
}