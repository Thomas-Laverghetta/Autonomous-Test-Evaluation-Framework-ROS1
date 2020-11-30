#pragma once

#include "Node.h"
#include "FloatObject.h"

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
	void update(qreal range, qreal heading);
	void Run(int argc, char** argv);
};


class PlotApp : public Node
{
private:
	bool recv_range;
	bool recv_heading;
	FloatObject range;
	FloatObject heading;
	QtWorkerObj worker;
	std::future<void> qt_worker_future;	
protected:
	void Setup(int argc, char** argv);
	void SetNodeName(int argc, char** argv, std::string& nodeName);

	void Init();
	void OnReceiveRange();
	void OnReceiveHeading();
	void Process();
	bool Load(const char* filename);
};

