#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include "renderwidget.h"
#include "arduinosimulator.h"

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
    void processSerialLine(QByteArray line);
    void renderWidgetPaintEvent();

public slots:
    void readSerialData();
    void oneTimeTimerHandler();

private slots:

protected:

private:
    void sendCmd(const char * pCmd);

    ControlWindow *cw;

public:
    Ui::MainWindow *ui;
    QSerialPort *serial = new QSerialPort(this);
    RenderWidget *renderWidget;
    ArduinoSimulator *arduinoSimulator = nullptr;

    double curAngleInRadians = 0.0;
    double curAngleInDegrees = 0.0;
    bool isTimePaused = false;
    int xAxisOffFromTop = 663;
    int xAxisOffFromRight = 661;
    int amplitude = 220;
    int curHeight = 0;
    int curWidth = amplitude;
    int penWidth = 12;
    int timeXInc = 1;
    bool drawRotatingVector = true;
    bool drawAngleArc = true;

    bool drawSinComponent = false;
    bool drawCosComponent = false;

    bool drawVerticalShadow = false;
    bool drawHorizontalShadow = false;

    bool drawVerticalProjectionDottedLine = false;
    bool drawHorizontalProjectionDottedLine = false;

    bool drawVerticalProjectionTipCircle = false;
    bool drawHorizontalProjectionTipCircle = false;

    bool showVerticalProjectionBox = false;
    bool showHorizontalProjectionBox = false;

    bool showSinOnXAxis = false;
    bool showCosOnYAxis = false;
    bool showCosOnXAxis = false;

    bool showAnglesOnXAndYAxis = false;
    bool showScrollingBackgroundText = false;
    bool show30And60Angles = false;

    int timerInterval = 20;
    int halfSteps = 0;
    bool useArduino = false;

    QByteArray *serialData = new QByteArray();
    QTimer oneTimeTimer;

};

#endif // MAINWINDOW_H
