
#include <QtGui>
#include <QMessageBox>

#ifndef Q_MOC_RUN
#include <iostream>
#include "main_window.h"
#endif


MainWindow::MainWindow(QWidget* parent):  QMainWindow(parent)
{
	customPlot.addGraph();
	customPlot.graph()->setLineStyle((QCPGraph::LineStyle)0);
	customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));

	max_range = 10.0f;

	arrow = new QCPItemLine(&customPlot);
	arrow_fov1 = new QCPItemLine(&customPlot);
	arrow_fov2 = new QCPItemLine(&customPlot);

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

  qreal centerX = (maxX + minX) / 2.0f;
  qreal widthX = (maxX - minX);
  qreal centerY = (maxY + minY) / 2.0f;
  qreal widthY = (maxY - minY);  

  customPlot.xAxis->setRange(centerX, widthX, Qt::AlignCenter);
  customPlot.yAxis->setRange(centerY, widthY, Qt::AlignCenter);
  customPlot.replot();
}

void MainWindow::drawArrow(qreal x, qreal y, qreal theta)
{
	arrow->start->setCoords(x,y);
	arrow_fov1->start->setCoords(x,y);
	arrow_fov2->start->setCoords(x,y);

	// arrow->end->setCoords(x + cos(theta*M_PI/180.0f)*max_range*0.7f, y + sin(theta*M_PI/180.0f)*max_range*0.7f);
	arrow->end->setCoords(x + cos(theta*M_PI/180.0f)*max_range*1.0f, y + sin(theta*M_PI/180.0f)*max_range*1.0f);
	
	arrow_fov1->end->setCoords(x + cos((theta-field_of_view)*M_PI/180.0f)*max_range*1.0f, y + sin((theta-field_of_view)*M_PI/180.0f)*max_range*1.0f);

	arrow_fov2->end->setCoords(x + cos((theta+field_of_view)*M_PI/180.0f)*max_range*1.0f, y + sin((theta+field_of_view)*M_PI/180.0f)*max_range*1.0f);

	arrow->setHead(QCPLineEnding::esSpikeArrow);
	// arrow->setHead(QCPLineEnding::esBar);
	// arrow->setHead(QCPLineEnding::esNone);

	arrow_fov1->setHead(QCPLineEnding::esNone);

	arrow_fov2->setHead(QCPLineEnding::esNone);

	// // For wheel visualization
	// customPlot.graph(0)->setData(QVector<double>(),QVector<double>());
	// customPlot.graph(0)->addData(x + cos((theta-90.0f)*M_PI/180.0f)*5.0f, y + sin((theta-90.0f)*M_PI/180.0f)*5.0f);
	// customPlot.graph(0)->addData(x + cos((theta+90.0f)*M_PI/180.0f)*5.0f, y + sin((theta+90.0f)*M_PI/180.0f)*5.0f);

	// For chasis visualization
	customPlot.graph(0)->setData(QVector<double>(),QVector<double>());
	customPlot.graph(0)->addData(x, y);

	customPlot.replot();
}

void MainWindow::drawObstacle(qreal x, qreal y)
{		
	customPlot.addGraph();
	customPlot.graph()->setLineStyle((QCPGraph::LineStyle)0);
	customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, max_range));
	customPlot.graph()->addData(x,y);
	customPlot.replot();
}

void MainWindow::setMaxRange(qreal size)
{
	max_range = size;
}

void MainWindow::setFieldOfView(qreal fov)
{
	field_of_view = fov;
}

void MainWindow::drawLine(qreal x0, qreal y0, qreal x1, qreal y1)
{
	customPlot.addGraph();
	customPlot.graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, max_range));
	customPlot.graph()->addData(x0,y0);
	customPlot.graph()->addData(x1,y1);
	customPlot.replot();
}

// void MainWindow::drawLine(qreal x0, qreal y0, qreal x1, qreal y1)
// {
// 	customPlot.addGraph();
// 	customPlot.graph()->setLineStyle((QCPGraph::LineStyle)1);
// 	customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
// 	customPlot.graph()->addData(x0,y0);
// 	customPlot.graph()->addData(x1,y1);

// 	customPlot.replot();
// }

// void MainWindow::clearPlot()
// {
// 	customPlot.clearGraphs();
// 	customPlot.replot();
// }

// void MainWindow::drawPoint(qreal _x, qreal _y)
// {
// 	static QTime time(QTime::currentTime());
// 	double key = time.elapsed() / 1000.0;
// 	static double lastPointKey = 0;

// 	if(customPlot.graphCount() == 0)
// 		customPlot.addGraph();

// 	customPlot.graph()->setLineStyle((QCPGraph::LineStyle)0);
// 	customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

// //	if(key - lastPointKey > 0.1)
// //    {		

// 		//printf("_x = %f | _y = %f\n", _x, _y);
		
// 		this->customPlot.graph()->addData(_x,_y);
		
// 		customPlot.replot();

// 		lastPointKey = key;
// //	}
// }

void MainWindow::keyPressEvent(QKeyEvent* k)
{
	switch(tolower(k->key()))
	{
		case('r'):		// reset plot
		{
			
			break;
		}
		case('q'):		// quit
		{
			QApplication::exit();
			break;
		}
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


