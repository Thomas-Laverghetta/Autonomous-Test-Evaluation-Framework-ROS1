#include "plot_app.h"
#include "main_window.h"
#include "tinyxml2.h"

#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo_plot_objects_qt/config/parameters.xml";

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

	std::string input1 = FindTopicName("input1");

	Subscribe(input1, &points_received);
	
	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&PlotApp::OnReceiveObjects));	

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

		if(elementName=="plot")
		{
			pElem->QueryIntAttribute("mode",&mode);
		}
    }

  return(true);	
}


void PlotApp::OnReceiveObjects()
{
	printf("Received Objects....\n");

	int sectionIndex = points_received.SectionIndexGet();
	worker.clearPlot(sectionIndex);

	if(mode == 0)			// 0 = draw points
	{
		for(int j = 0; j < PointObject::MAX_POINTS; j++)
		{
			worker.drawPoint(sectionIndex, points_received.Points()[j].x, points_received.Points()[j].y);
		}
	}
	else if(mode == 1) 		// 1 = draw line graphs	
	{

		FitPoints(sectionIndex, points_received.Points());
	}
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


void PlotApp::FitPoints(int sectionIndex, Vector2* points)
{
//	if(points[0].x == FLT_MAX)		// easy out for unused object
//		return;

	float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
	float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;


	int numValidPoints = 0;
	for(int i = 0; i < PointObject::MAX_POINTS; i++)
	{
		if(points[i].x != FLT_MAX && points[i].y != FLT_MAX)
		{
			// update sums for fitting
			sumX += points[i].x;
			sumY += points[i].y;
			sumXY += points[i].x * points[i].y;
			sumX2 += points[i].x * points[i].x;

			// count this point for fitting
			numValidPoints += 1;

			// track min and max values
			if(points[i].x < minX)
				minX = points[i].x;

			if(points[i].y < minY)
				minY = points[i].y;

			if(points[i].x > maxX)
				maxX = points[i].x;

			if(points[i].y > maxY)
				maxY = points[i].y;			

			//printf("x=%f,y=%f\n", points[i].x, points[i].y);

		}

	}

	
	

	if(numValidPoints > 0)	// draw line if points were fitted for object
	{
		printf("minX:%f,maxX:%f,minY:%f,maxY:%f\n", minX,maxX,minY,maxY);

		float xMean = sumX / numValidPoints;
		float yMean = sumY / numValidPoints;
		float denominator = sumX2 - sumX * xMean;	// least squares

		if(denominator > 0.001)
		{
			float slope = (sumXY - sumX * yMean) / denominator;
			float yInt = yMean - slope * xMean;
			float x0 = minX;
			float x1 = maxX;
			float y0 = slope * x0 + yInt;
			float y1 = slope * x1 + yInt;

			printf("DrawingLine slope:%f, x0:%f, y0:%f, x1:%f, y1:%f\n",slope,x0,y0,x1,y1);
			worker.drawLine(sectionIndex, x0,y0,x1,y1);	// invoke draw line on worker....

		}
	}
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


void QtWorkerObj::drawLine(int sectionIndex, qreal x0, qreal y0, qreal x1, qreal y1)
{
	if(!QMetaObject::invokeMethod(window, "drawLine", Qt::QueuedConnection, Q_ARG(int, sectionIndex), Q_ARG(qreal, x0), Q_ARG(qreal, y0), Q_ARG(qreal, x1), Q_ARG(qreal, y1)))
		//printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::drawPoint(int sectionIndex, qreal _x, qreal _y)
{
	if(!QMetaObject::invokeMethod(window, "drawPoint", Qt::QueuedConnection, Q_ARG(int, sectionIndex), Q_ARG(qreal, _x), Q_ARG(qreal, _y)))
		//printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::clearPlot(int sectionIndex)
{
	if(!QMetaObject::invokeMethod(window, "clearPlot", Qt::QueuedConnection, Q_ARG(int, sectionIndex)))
		//printf(" Failed to invoke method\n");
		;
}

void QtWorkerObj::setBounds(qreal minX, qreal maxX, qreal minY, qreal maxY)
{
	if(!QMetaObject::invokeMethod(window, "setBounds", Qt::QueuedConnection, Q_ARG(qreal, minX), Q_ARG(qreal, maxX), Q_ARG(qreal, minY), Q_ARG(qreal, maxY)))
		printf(" Failed to invoke method\n");
		;
}
