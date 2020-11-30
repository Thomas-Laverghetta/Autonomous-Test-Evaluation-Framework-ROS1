#include "qt_app.h"
#include "main_window.h"
#include "tinyxml2.h"

#include <signal.h>
#include <unistd.h>

const char* parameterFile = "../catkin_ws/src/demo_sense_plot_qt/config/parameters.xml";

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
	std::string input2 = FindTopicName("input2");

	Subscribe(input1, &range);
	Subscribe(input2, &heading);

	RegisterInputFunction(input1,static_cast<NodeFuncPtr>(&PlotApp::OnReceiveRange));
	RegisterInputFunction(input2,static_cast<NodeFuncPtr>(&PlotApp::OnReceiveHeading));

	RegisterInitFunction(static_cast<NodeFuncPtr>(&PlotApp::Init));

	RegisterCoreFunction(static_cast<NodeFuncPtr>(&PlotApp::Process));

	recv_range = false;
	recv_heading = false;
}

void PlotApp::SetNodeName(int argc, char** argv, std::string& nodeName)
{
	nodeName = "SENSE_PLOT_QT";
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
    }

  return(true);	
}

void PlotApp::OnReceiveRange()
{
	recv_range = true;
}

void PlotApp::OnReceiveHeading()
{
	recv_heading = true;
}

void PlotApp::Process()
{
 	// ---- Termination Signal ------ //
  	if (signal (SIGINT, termination_handler) == SIG_IGN)
    	signal (SIGINT, SIG_IGN);

	std::chrono::milliseconds span (0);
    if(qt_worker_future.wait_for(span) == std::future_status::ready)
    	Node::Get()->Terminate();

	if(recv_range && recv_heading)
	{
		printf("RANGE TIME: %i| HEADING TIME %i\n", range.GetTime(), heading.GetTime());	// COPY TIME TESTING!!!!!
		printf("%f,%f\n", range.GetValue(), heading.GetValue());
		worker.update(range.GetValue(), heading.GetValue());
		recv_range = false;
		recv_heading = false;
	}

	// worker.update(rand() % 100, rand() % 360);
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

void QtWorkerObj::update(qreal range, qreal heading)
{	
    if(!QMetaObject::invokeMethod(window, "addPoint", Qt::QueuedConnection, Q_ARG(qreal, range), Q_ARG(qreal, heading)))
        //printf(" Failed to invoke method\n");
        ;

}

void QtWorkerObj::setBounds(qreal minX, qreal maxX, qreal minY, qreal maxY)
{
	if(!QMetaObject::invokeMethod(window, "setBounds", Qt::QueuedConnection, Q_ARG(qreal, minX), Q_ARG(qreal, maxX), Q_ARG(qreal, minY), Q_ARG(qreal, maxY)))
		printf(" Failed to invoke method\n");
		;
}
