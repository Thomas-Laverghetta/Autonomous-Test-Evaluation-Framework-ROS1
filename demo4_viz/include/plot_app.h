#pragma once

#include "Node.h"
#include "FloatObject.h"
#include "LocationObject.h"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QTimer>

#include <future>
#include <chrono>

#include "main_window.h"

class QtWorkerObj : public QObject
{
	Q_OBJECT
private:
	MainWindow* window;
public:	
	void drawLine(qreal x0, qreal y0, qreal x1, qreal y1);
	void drawArrow(qreal x, qreal y, qreal theta);
	void drawObstacle(qreal x, qreal y);
	void setMaxRange(qreal size);
	void setFieldOfView(qreal fov);
	void setBounds(qreal minX, qreal maxX, qreal minY, qreal maxY);
	void Run(int argc, char** argv);	
};


class PlotApp : public Node
{
private:
	LocationObject vehicle_location;	

	LocationObject obstacle; 

	QtWorkerObj worker;
	std::future<void> qt_worker_future;	

	float max_range;
	float field_of_view;
	
protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);
	void Init();
	bool Load(const char* filename);
	bool LoadEnv(const char* filename);
	void OnReceiveLocation();
	void OnReceiveObstacle();
	void Process();
};

