#pragma once

#include "Node.h"
#include "PointObject.h"

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
	void setBounds(qreal minX, qreal maxX, qreal minY, qreal maxY);
	void drawLine(int sectionIndex, qreal x0, qreal y0, qreal x1, qreal y1);
	void drawPoint(int sectionIndex, qreal _x, qreal _y);
	void clearPlot(int sectionIndex);
	void Run(int argc, char** argv);
};


class PlotApp : public Node
{
private:
	// PointObjectArray object_array;
	PointObject points_received;
	QtWorkerObj worker;
	std::future<void> qt_worker_future;	
	int mode;		// 0  = Points, 1 = Line Graphs
protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);

	void Init();
	void OnReceiveObjects();
	void Process();
	void FitPoints(int sectionIndex, Vector2* points);
	bool Load(const char* filename);
};

