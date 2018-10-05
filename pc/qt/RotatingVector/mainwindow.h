#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include "renderwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readData();

private slots:
    void on_speed1_btn_clicked();

    void on_speed3_btn_clicked();

    void on_speed2_btn_clicked();

    void on_speed4_btn_clicked();

    void on_speed5_btn_clicked();

    void on_speed6_btn_clicked();

    void on_continue_btn_clicked();

    void on_ccwHalfStep_btn_clicked();

    void on_cwHalfStep_btn_clicked();

    void on_releaseCoils_btn_clicked();

    void on_ccwDirection_btn_clicked();

    void on_cwDirection_btn_clicked();

    void on_pauseTime_btn_clicked();

    void on_pauseVector_btn_clicked();

    void on_pauseVectorAndTime_btn_clicked();

    void on_goto0_btn_clicked();

    void on_goto90_btn_clicked();

    void on_goto180_btn_clicked();

    void on_goto270_btn_clicked();

    void on_gotoAngle_btn_clicked();

    void on_setAs0_btn_clicked();

    void on_setAs180_btn_clicked();

    void on_setCurPos_btn_clicked();

    void on_setPcCalOffFrom180ToMinus2p5_btn_clicked();

    void on_setPcCalOffFrom180ToMinus3p0_btn_clicked();

    void on_setPcCalOffFrom180ToMinus3p5_btn_clicked();

    void on_timeDelay_sb_valueChanged(int arg1);

    void on_sineAmplitude_sb_valueChanged(int arg1);

    void on_penWidth_sb_valueChanged(int arg1);

    void on_xAxisOffFromTop_sb_valueChanged(int arg1);

    void on_xAxisOffFromRight_sb_valueChanged(int arg1);


    void on_sineAmplitude_sb_valueChanged(const QString &arg1);

    void on_drawShadow_cb_stateChanged(int arg1);

    void on_drawRotatingVector_cb_stateChanged(int arg1);

protected:

private:
    void sendCmd(const char * pCmd);
    void update();

public:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    RenderWidget *renderWidget;
    int curHeight;
    double curAngleInRadians;
    double curAngleInDegrees;
    bool isTimePaused;
    int xAxisOffFromTop;
    int xAxisOffFromRight;
    int amplitude;
    int penWidth;
    int timeXInc;
    bool drawRotatingVector;
    bool drawShadow;
    int timerInterval;
    int halfSteps;

    QByteArray *serialData;

};

#endif // MAINWINDOW_H
