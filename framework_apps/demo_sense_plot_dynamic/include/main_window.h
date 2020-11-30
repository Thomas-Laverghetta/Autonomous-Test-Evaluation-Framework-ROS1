
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
  void saveFile(std::string path);

public slots:
  void setBounds(qreal _minX, qreal _maxX, qreal _minY, qreal maxY);
  void setResolution(int resol);
  void addPoint(qreal range, qreal heading);

private:
  QWidget centralWidget;
  QVBoxLayout verticalLayout;
  QCustomPlot customPlot;
  qreal minX,maxX,minY,maxY;
  QCPItemLine *arrow;
  int headKeeper;
  int resolution;

  std::vector<qreal> range_values;
  std::vector<qreal> heading_values;
  std::vector<qreal> x_values;
  std::vector<qreal> y_values;

protected:
  void resizeEvent(QResizeEvent *event);
};


#endif
