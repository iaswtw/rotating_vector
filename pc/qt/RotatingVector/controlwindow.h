#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QDialog>

namespace Ui {
class ControlWindow;
}

class MainWindow;

class ControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ControlWindow(QWidget *parent = nullptr, MainWindow * mw = nullptr);
    ~ControlWindow();

private:
    MainWindow *mw;

    void sendCmd(const char * pCmd);

public:
    Ui::ControlWindow *ui;

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
    void on_drawVerticalShadow_cb_stateChanged(int arg1);
    void on_drawRotatingVector_cb_stateChanged(int arg1);
    void on_showCosOnXAxis_cb_stateChanged(int arg1);
    void on_showCosOnYAxis_cb_stateChanged(int arg1);
    void on_useArduino_cb_stateChanged(int arg1);
    void on_showSinOnXAxis_cb_stateChanged(int arg1);
    void on_goto30_btn_clicked();
    void on_goto45_btn_clicked();
    void on_goto60_btn_clicked();
    void on_angleAdvanceOffset_sb_valueChanged(const QString &arg1);
    void on_showVerticalProjectionBox_cb_stateChanged(int arg1);
    void on_showHorizontalProjectionBox_cb_stateChanged(int arg1);
    void on_drawHorizontalShadow_cb_stateChanged(int arg1);
    void on_clearSinValues_btn_clicked();
    void on_clearCosValues_btn_clicked();
};

#endif // CONTROLWINDOW_H
