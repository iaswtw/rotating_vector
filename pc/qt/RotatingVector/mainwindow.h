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

public slots:
    void readSerialData();

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


    int timerInterval;
    int halfSteps;
    bool useArduino;

    QByteArray *serialData;

};

#endif // MAINWINDOW_H
