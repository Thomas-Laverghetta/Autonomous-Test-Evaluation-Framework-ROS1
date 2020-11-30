
#include <QtGui>
#include <QMessageBox>

#ifndef Q_MOC_RUN
#include <iostream>
#include <cmath>
#include "main_window.h"
#endif


MainWindow::MainWindow(QWidget* parent):  QMainWindow(parent)
{
	customPlot.addGraph();
	customPlot.graph()->setLineStyle((QCPGraph::LineStyle)0);
	customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

	arrow = new QCPItemLine(&customPlot);

	verticalLayout.addWidget(&customPlot);
	centralWidget.setLayout(&verticalLayout);
	this->setCentralWidget(&centralWidget);
}


MainWindow::~MainWindow()
{
}



void MainWindow::setBounds(qreal _minX, qreal _maxX, qreal _minY, qreal _maxY)
{
  minX = _minX;
  maxX = _maxX;
  minY = _minY;
  maxY = _maxY;
  qreal centerX = (maxX + minX) / 2.0f; // midpoint
  qreal widthX = (maxX - minX); 				// distance
  qreal centerY = (maxY + minY) / 2.0f;
  qreal widthY = (maxY - minY);

  customPlot.xAxis->setRange(centerX, widthX, Qt::AlignCenter);
  customPlot.yAxis->setRange(centerY, widthY, Qt::AlignCenter);

	// QVector<double> cirX(180), cirY(180);
	// double r = 200;
	//
	// // Top half of the circle
	// for (int i = 0; i<180;i++){
	// 	cirX[i] = r*cos(i*3.1415/180.0f);
	// 	cirY[i] = r*sin(i*3.1415/180.0f);
	// }
	// customPlot.addGraph();
	// customPlot.graph(1)->setData(cirX,cirY);
	//
	// // Bottom Half of the circle
	// for (int i = 0; i<180;i++){
	// 	cirX[i] = r*cos((i+180.0)*3.1415/180.0f);
	// 	cirY[i] = r*sin((i+180.0)*3.1415/180.0f);
	// }
	// customPlot.addGraph();
	// customPlot.graph(2)->setData(cirX,cirY);



  // customPlot.replot();
}

void MainWindow::setResolution(int resol)
{
	resolution = resol;

	customPlot.clearGraphs();
	for (int i = 0; i < resolution; i++){
		customPlot.addGraph();
		customPlot.graph(i);
		customPlot.graph(i)->setLineStyle((QCPGraph::LineStyle)0);
		customPlot.graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
	}
}

void MainWindow::addPoint(qreal range, qreal heading)
{
	static QTime time(QTime::currentTime());
	headKeeper = (int)( (heading / 360.0f) * resolution);
	double key = time.elapsed() / 1000.0;
	static double lastPointKey = 0;
	if(key - lastPointKey > 0.1)
    {
		float headingRadians = heading * 3.14159265f / 180.0f;
		// compute coordinates
		float _x = range * cos(headingRadians);
		float _y = range * sin(headingRadians);

		// ADDED THOMAS LAVERGHETTA: Don't Know if it works Yet
		//if(customPlot.graph()->data()->at(_x)->value != _y){ // compares Values at key (_x)
			float mag = sqrt(_x*_x+_y*_y);

			arrow->start->setCoords(0,0);
			//arrow->end->setCoords(_x/mag * (maxX-minX) * 0.1f, _y/mag * (maxY-minY) * 0.1f);
			arrow->end->setCoords(cos(headingRadians) * (maxX-minX) * 1.1f, sin(headingRadians) * (maxY-minY) * 1.1f);
			//arrow->end->setCoords(_x + (maxX-minX) * 0.1f, _y + (maxY-minY) * 0.1f);
			arrow->setHead(QCPLineEnding::esSpikeArrow);

			customPlot.graph(headKeeper)->setData(QVector<double>(),QVector<double>());
			customPlot.graph(headKeeper)->addData(_x,_y);
			customPlot.replot();

			range_values.push_back(range);
			heading_values.push_back(heading);

			x_values.push_back(_x);
			y_values.push_back(_y);
		lastPointKey = key;
	}
}


void MainWindow::keyPressEvent(QKeyEvent* k)
{
	switch(tolower(k->key()))
	{
		case('r'):		// reset plot
		{
			customPlot.graph(0)->setData(QVector<double>(),QVector<double>());
			customPlot.replot();
			range_values.clear();
			heading_values.clear();
			x_values.clear();
			y_values.clear();
			break;
		}
		case('q'):		// quit
		{
			QApplication::exit();
			break;
		}
		case('p'):
		{
			printf("Enter filename to write output: \n");
			char filename [256];
			scanf("%255s", filename);
			std::string path = std::string("..//catkin_ws//src//demo_sense_plot_qt//") + std::string(filename);
			saveFile(path);
		}
	}
}

void MainWindow::saveFile(std::string path)
{

	FILE * pfile = fopen(path.c_str(), "w");

	if(pfile != NULL)
	{
		for(unsigned int i = 0; i < range_values.size(); i++)
		{
			QCPGraphData data = *(customPlot.graph(0)->data()->at(i));
			fprintf(pfile, "%f\t%f\t%f\t%f\n", x_values[i], y_values[i], range_values[i], heading_values[i]);
		}

		fclose(pfile);
		printf("Saving Done....\n");
	}
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	printf("Resizing\n");
	customPlot.yAxis->setScaleRatio(customPlot.xAxis, 1.0);
	//customPlot.yAxis->setAutoTickStep(false);
    //customPlot.yAxis->setTickStep(chart->xAxis->tickStep());
	customPlot.replot();
}
// -------------------------------------------------------- //
