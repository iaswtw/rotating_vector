#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include "renderwidget.h"

namespace Ui {
class MainWindow;
}

class ControlWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setControlWindow(ControlWindow *cw);

public slots:
    void readSerialData();

private slots:

protected:

private:
    void sendCmd(const char * pCmd);
    void update();
    void processSerialLine(QByteArray line);

    ControlWindow *cw;

public:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    RenderWidget *renderWidget;
    double curAngleInRadians;
    double curAngleInDegrees;
    bool isTimePaused;
    int xAxisOffFromTop;
    int xAxisOffFromRight;
    int amplitude;
    int curHeight;
    int curWidth;
    int penWidth;
    int timeXInc;
    bool drawRotatingVector;
    bool drawShadow;
    bool showCosOnYAxis;
    bool showCosOnXAxis;
    int timerInterval;
    int halfSteps;
    bool useArduino;

    QByteArray *serialData;

};

#endif // MAINWINDOW_H
