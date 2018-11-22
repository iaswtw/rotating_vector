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
    void update();

    ControlWindow *cw;

public:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    RenderWidget *renderWidget;
    ArduinoSimulator *arduinoSimulator;

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
    bool drawAngleArc;

    bool drawSinComponent;
    bool drawCosComponent;

    bool drawVerticalShadow;
    bool drawHorizontalShadow;

    bool drawVerticalProjectionDottedLine;
    bool drawHorizontalProjectionDottedLine;

    bool drawVerticalProjectionTipCircle;
    bool drawHorizontalProjectionTipCircle;

    bool showVerticalProjectionBox;
    bool showHorizontalProjectionBox;

    bool showSinOnXAxis;
    bool showCosOnYAxis;
    bool showCosOnXAxis;

    bool showAnglesOnXAndYAxis;
    bool showScrollingBackgroundText;
    bool show30And60Angles;

    int timerInterval;
    int halfSteps;
    bool useArduino;

    QByteArray *serialData;
    QTimer oneTimeTimer;

};

#endif // MAINWINDOW_H
