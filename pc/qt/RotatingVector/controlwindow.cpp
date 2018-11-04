#include "controlwindow.h"
#include "ui_controlwindow.h"
#include "mainwindow.h"

ControlWindow::ControlWindow(QWidget *parent, MainWindow * mw) :
    QDialog(parent),
    mw(mw),
    ui(new Ui::ControlWindow)
{
    ui->setupUi(this);

    //----------------------------------------------------------------------------
    // Set initial values in GUI widgets
    ui->penWidth_sb->setValue(mw->penWidth);
    ui->drawShadow_cb->setChecked(mw->drawShadow);
    ui->drawRotatingVector_cb->setChecked(mw->drawRotatingVector);
    ui->xAxisOffFromRight_sb->setValue(mw->xAxisOffFromRight);
    ui->xAxisOffFromTop_sb->setValue(mw->xAxisOffFromTop);
    ui->sineAmplitude_sb->setValue(mw->amplitude);
    ui->timeDelay_sb->setValue(mw->timerInterval);
}

ControlWindow::~ControlWindow()
{
    delete ui;
}

void ControlWindow::sendCmd(const char * pCmd)
{
    mw->serial->write(pCmd, strlen(pCmd));
}

void ControlWindow::on_continue_btn_clicked()
{
    sendCmd("c\n");
    mw->isTimePaused = false;
}

void ControlWindow::on_speed1_btn_clicked()
{
    sendCmd("1\n");
}

void ControlWindow::on_speed2_btn_clicked()
{
    sendCmd("2\n");
}

void ControlWindow::on_speed3_btn_clicked()
{
    sendCmd("3\n");
}

void ControlWindow::on_speed4_btn_clicked()
{
    sendCmd("4\n");
}

void ControlWindow::on_speed5_btn_clicked()
{
    sendCmd("5\n");
}

void ControlWindow::on_speed6_btn_clicked()
{
    sendCmd("6\n");
}

void ControlWindow::on_ccwHalfStep_btn_clicked()
{
    sendCmd("<\n");
    sendCmd("h\n");
}

void ControlWindow::on_cwHalfStep_btn_clicked()
{
    sendCmd(">\n");
    sendCmd("h\n");
    sendCmd("<\n");
}

void ControlWindow::on_releaseCoils_btn_clicked()
{
    sendCmd("r\n");
}

void ControlWindow::on_ccwDirection_btn_clicked()
{
    sendCmd("<\n");
}

void ControlWindow::on_cwDirection_btn_clicked()
{
    sendCmd(">\n");
}

void ControlWindow::on_pauseTime_btn_clicked()
{
    mw->isTimePaused = true;
}

void ControlWindow::on_pauseVector_btn_clicked()
{
    sendCmd("p\n");
}

void ControlWindow::on_pauseVectorAndTime_btn_clicked()
{
    mw->isTimePaused = true;
    sendCmd("p\n");
}

void ControlWindow::on_goto0_btn_clicked()
{
    sendCmd("g0\n");
}

void ControlWindow::on_goto90_btn_clicked()
{
    sendCmd("g90\n");
}

void ControlWindow::on_goto180_btn_clicked()
{
    QString cmd;
    cmd.append("g");
    double angle = 180 + ui->calAt180_sb->value();
    cmd.append(QString::number(angle));
    cmd.append("\n");

    printf("Sending cmd: %s", cmd.toLocal8Bit().constData());

    sendCmd(cmd.toLocal8Bit().constData());
}

void ControlWindow::on_goto270_btn_clicked()
{
    sendCmd("g270\n");
}

void ControlWindow::on_gotoAngle_btn_clicked()
{
    QString text = ui->gotoAngle_le->displayText();
    QString cmd;

    cmd.append("g");
    cmd.append(text);
    cmd.append("\n");
    sendCmd(cmd.toLocal8Bit().data());
}

void ControlWindow::on_setAs0_btn_clicked()
{
    sendCmd("=0\n");
}

void ControlWindow::on_setAs180_btn_clicked()
{
    sendCmd("=180\n");
}

void ControlWindow::on_setCurPos_btn_clicked()
{
    QString text = ui->curPos_le->displayText();
    QString cmd;

    cmd.append("=");
    cmd.append(text);
    cmd.append("\n");
    sendCmd(cmd.toLocal8Bit().data());
}

void ControlWindow::on_setPcCalOffFrom180ToMinus2p5_btn_clicked()
{
    ui->calAt180_sb->setValue(-2.5);
}

void ControlWindow::on_setPcCalOffFrom180ToMinus3p0_btn_clicked()
{
    ui->calAt180_sb->setValue(-3.0);
}

void ControlWindow::on_setPcCalOffFrom180ToMinus3p5_btn_clicked()
{
    ui->calAt180_sb->setValue(-3.5);
}

void ControlWindow::on_sineAmplitude_sb_valueChanged(int /*arg1*/)
{
    mw->amplitude = ui->sineAmplitude_sb->value();
}

void ControlWindow::on_penWidth_sb_valueChanged(int /*arg1*/)
{
    mw->penWidth = ui->penWidth_sb->value();
}

void ControlWindow::on_xAxisOffFromTop_sb_valueChanged(int /*arg1*/)
{
    mw->xAxisOffFromTop = ui->xAxisOffFromTop_sb->value();
}

void ControlWindow::on_xAxisOffFromRight_sb_valueChanged(int /*arg1*/)
{
    mw->xAxisOffFromRight = ui->xAxisOffFromRight_sb->value();
}

void ControlWindow::on_sineAmplitude_sb_valueChanged(const QString &/*arg1*/)
{
    mw->amplitude = ui->sineAmplitude_sb->value();
}

void ControlWindow::on_drawShadow_cb_stateChanged(int /*arg1*/)
{
    mw->drawShadow = ui->drawShadow_cb->isChecked();
}

void ControlWindow::on_drawRotatingVector_cb_stateChanged(int /*arg1*/)
{
    mw->drawRotatingVector = ui->drawRotatingVector_cb->isChecked();
}

void ControlWindow::on_timeDelay_sb_valueChanged(int)
{
    mw->timerInterval = ui->timeDelay_sb->value();
    mw->renderWidget->updateTimerInterval();
}

void ControlWindow::on_showCosOnXAxis_cb_stateChanged(int /*arg1*/)
{
    mw->showCosOnXAxis = ui->showCosOnXAxis_cb->isChecked();
}
