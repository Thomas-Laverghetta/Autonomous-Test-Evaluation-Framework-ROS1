
#include <QtGui>
#include <QMessageBox>

#ifndef Q_MOC_RUN
#include <iostream>
#include "main_window.h"
#endif


MainWindow::MainWindow(QWidget* parent):  QMainWindow(parent)
{
	//customPlot.addGraph();
	//customPlot.graph()->setLineStyle((QCPGraph::LineStyle)1);
	//customPlot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

	verticalLayout.addWidget(&customPlot);  
	centralWidget.setLayout(&verticalLayout);
	this->setCentralWidget(&centralWidget);  


	setupGraphs();

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

void MainWindow::setupGraphs()
{
	for(int i = 0; i < 360; i++)
	{
		customPlot.addGraph();
	}
}


void MainWindow::drawLine(int sectionIndex, qreal x0, qreal y0, qreal x1, qreal y1)
{
	// customPlot.addGraph();
	customPlot.graph(sectionIndex)->setLineStyle((QCPGraph::LineStyle)1);
	customPlot.graph(sectionIndex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
	customPlot.graph(sectionIndex)->addData(x0,y0);
	customPlot.graph(sectionIndex)->addData(x1,y1);

	customPlot.replot();
}

void MainWindow::clearPlot(int sectionIndex)
{
	// customPlot.clearGraphs();
	customPlot.removeGraph(sectionIndex);
	customPlot.replot();
}

void MainWindow::drawPoint(int sectionIndex, qreal _x, qreal _y)
{
	// static QTime time(QTime::currentTime());
	// double key = time.elapsed() / 1000.0;
	// static double lastPointKey = 0;

	// if(customPlot.graphCount() == 0)
	// 	customPlot.addGraph();

	customPlot.graph(sectionIndex)->setLineStyle((QCPGraph::LineStyle)0);
	customPlot.graph(sectionIndex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

//	if(key - lastPointKey > 0.1)
//    {		

		//printf("_x = %f | _y = %f\n", _x, _y);
		
		this->customPlot.graph(sectionIndex)->addData(_x,_y);
		
		customPlot.replot();

		// lastPointKey = key;
//	}
}

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


