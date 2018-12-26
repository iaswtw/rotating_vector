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
    void showControlWindowCentered();

public slots:
    void readSerialData();
    void oneTimeTimerHandler();

private slots:

    void on_actionControl_Panel_triggered();

    void on_actionAbout_triggered();

protected:
    void keyPressEvent(QKeyEvent *event);

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
    bool showAngleInRadians = false;
    bool showAllOrdinates = false;
    bool show1AndMinus1Ordinates = true;
    bool showOrdinateCaptions = true;

    int extraVectorOffsetFromRight = 0;
    int extraVectorOffsetFromBottom = 0;

    int timerInterval = 20;
    int halfSteps = 0;
    bool useArduino = false;

    int phaseShiftFromSine = 90;
    bool phaseShiftArcAndCaption = false;

    QByteArray *serialData = new QByteArray();
    QTimer oneTimeTimer;

};

#endif // MAINWINDOW_H
