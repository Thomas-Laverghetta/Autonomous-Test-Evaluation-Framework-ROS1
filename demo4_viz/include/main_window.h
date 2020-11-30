
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QTimer>

#ifndef Q_MOC_RUN
#include "qcustomplot.h"
#endif


// -------------------- Main Window ------------------- //

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  MainWindow(QWidget* parent = 0);  
  ~MainWindow();
  void keyPressEvent(QKeyEvent* k);

public slots:
  void setBounds(qreal _minX, qreal _maxX, qreal _minY, qreal maxY);
  void drawArrow(qreal x, qreal y, qreal theta);
  void drawObstacle(qreal x, qreal y);
  void drawLine(qreal x0, qreal y0, qreal x1, qreal y1);
  void setMaxRange(qreal size);
  void setFieldOfView(qreal fov);

private:
  QWidget centralWidget;
  QVBoxLayout verticalLayout;
  QCustomPlot customPlot;
  qreal minX,maxX,minY,maxY;
  QCPItemLine *arrow;
  QCPItemLine *arrow_fov1;
  QCPItemLine *arrow_fov2;
  qreal max_range;
  qreal field_of_view;

protected:
  void resizeEvent(QResizeEvent *event);
};


#endif