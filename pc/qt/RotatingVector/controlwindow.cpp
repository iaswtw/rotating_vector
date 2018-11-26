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
    ui->drawVerticalShadow_cb->setChecked(mw->drawVerticalShadow);
    ui->drawHorizontalShadow_cb->setChecked(mw->drawHorizontalShadow);

    ui->drawRotatingVector_cb->setChecked(mw->drawRotatingVector);
    ui->extraVectorOffsetFromRight_sb->setValue(mw->extraVectorOffsetFromRight);
    ui->extraVectorOffsetFromBottom_sb->setValue(mw->extraVectorOffsetFromBottom);
    ui->sineAmplitude_sb->setValue(mw->amplitude);
    ui->timeDelay_sb->setValue(mw->timerInterval);

    ui->drawAngleArc_cb->setChecked(mw->drawAngleArc);

    ui->drawSinComponent_cb->setChecked(mw->drawSinComponent);
    ui->drawCosComponent_cb->setChecked(mw->drawCosComponent);

    ui->showVerticalProjectionBox_cb->setChecked(mw->showVerticalProjectionBox);
    ui->showHorizontalProjectionBox_cb->setChecked(mw->showHorizontalProjectionBox);

    ui->drawVerticalProjectionDottedLine_cb->setChecked(mw->drawVerticalProjectionDottedLine);
    ui->drawHorizontalProjectionDottedLine_cb->setChecked(mw->drawHorizontalProjectionDottedLine);

    ui->drawVerticalProjectionTipCircle_cb->setChecked(mw->drawVerticalProjectionTipCircle);
    ui->drawHorizontalProjectionTipCircle_cb->setChecked(mw->drawHorizontalProjectionTipCircle);

    ui->showSinOnXAxis_cb->setChecked(mw->showSinOnXAxis);
    ui->showCosOnXAxis_cb->setChecked(mw->showCosOnXAxis);
    ui->showCosOnYAxis_cb->setChecked(mw->showCosOnYAxis);

    ui->showAnglesOnXAndYAxis_cb->setChecked(mw->showAnglesOnXAndYAxis);
    ui->showScrollingBackgroundText_cb->setChecked(mw->showScrollingBackgroundText);
    ui->show30And60Angles_cb->setChecked(mw->show30And60Angles);

    ui->phaseShiftFromSine_sb->setValue(mw->phaseShiftFromSine);

    ui->useArduino_cb->setChecked(mw->useArduino);



}

ControlWindow::~ControlWindow()
{
    delete ui;
}

void ControlWindow::setLowestSpeed()
{
    sendCmd("1\n");
}

void ControlWindow::continueVectorAndUnpauseTime()
{
    sendCmd("c\n");
    mw->isTimePaused = false;
}

void ControlWindow::sendCmd(const char * pCmd)
{
    if (mw->useArduino)
    {
        mw->serial->write(pCmd, strlen(pCmd));
    }
    else
    {
        mw->arduinoSimulator->postCmd(pCmd);
    }
}

void ControlWindow::on_continue_btn_clicked()
{
    continueVectorAndUnpauseTime();
}

void ControlWindow::on_unpauseVector_btn_clicked()
{
    sendCmd("c\n");
}

void ControlWindow::on_unpauseTime_btn_clicked()
{
    mw->isTimePaused = false;
}

void ControlWindow::on_speed1_btn_clicked()   {    setLowestSpeed(); }
void ControlWindow::on_speed2_btn_clicked()   {    sendCmd("2\n");   }
void ControlWindow::on_speed3_btn_clicked()   {    sendCmd("3\n");   }
void ControlWindow::on_speed4_btn_clicked()   {    sendCmd("4\n");   }
void ControlWindow::on_speed5_btn_clicked()   {    sendCmd("5\n");   }
void ControlWindow::on_speed6_btn_clicked()   {    sendCmd("6\n");   }

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

void ControlWindow::on_releaseCoils_btn_clicked()           {    sendCmd("r\n");   }
void ControlWindow::on_ccwDirection_btn_clicked()           {    sendCmd("<\n");   }
void ControlWindow::on_cwDirection_btn_clicked()            {    sendCmd(">\n");   }
void ControlWindow::on_pauseVector_btn_clicked()            {    sendCmd("p\n");   }

void ControlWindow::on_pauseTime_btn_clicked()
{
    mw->isTimePaused = true;
}


void ControlWindow::on_pauseVectorAndTime_btn_clicked()
{
    mw->isTimePaused = true;
    sendCmd("p\n");
}

void ControlWindow::gotoAngle(double angle)
{
    QString cmd = "g";
    cmd.append(QString::number(angle));
    cmd.append("\n");

    sendCmd(cmd.toStdString().c_str());
    mw->isTimePaused = false;
}

void ControlWindow::on_goto0_btn_clicked()   {   gotoAngle(0);    }
void ControlWindow::on_goto30_btn_clicked()  {   gotoAngle(30);   }
void ControlWindow::on_goto45_btn_clicked()  {   gotoAngle(45);   }
void ControlWindow::on_goto60_btn_clicked()  {   gotoAngle(60);   }
void ControlWindow::on_goto90_btn_clicked()  {   gotoAngle(90);   }
void ControlWindow::on_goto270_btn_clicked() {   gotoAngle(270);  }
void ControlWindow::on_goto120_btn_clicked() {   gotoAngle(120);  }
void ControlWindow::on_goto135_btn_clicked() {   gotoAngle(135);  }
void ControlWindow::on_goto150_btn_clicked() {   gotoAngle(150);  }
void ControlWindow::on_goto210_btn_clicked() {   gotoAngle(210);  }
void ControlWindow::on_goto225_btn_clicked() {   gotoAngle(225);  }
void ControlWindow::on_goto240_btn_clicked() {   gotoAngle(240);  }
void ControlWindow::on_goto300_btn_clicked() {   gotoAngle(300);  }
void ControlWindow::on_goto315_btn_clicked() {   gotoAngle(315);  }
void ControlWindow::on_goto330_btn_clicked() {   gotoAngle(330);  }

void ControlWindow::on_goto180_btn_clicked()
{
    double angle = 180;
    if (mw->useArduino)
        angle += ui->calAt180_sb->value();

    gotoAngle(angle);
}

void ControlWindow::on_gotoAngle_btn_clicked()
{
    QString text = ui->gotoAngle_le->displayText();
    double angle = atof(text.toStdString().c_str());

    gotoAngle(angle);
}

void ControlWindow::on_setAs0_btn_clicked()    {   sendCmd("=0\n");    }
void ControlWindow::on_setAs180_btn_clicked()  {   sendCmd("=180\n");  }

void ControlWindow::on_setCurPos_btn_clicked()
{
    QString text = ui->curPos_le->displayText();
    QString cmd;

    cmd.append("=");
    cmd.append(text);
    cmd.append("\n");
    sendCmd(cmd.toLocal8Bit().data());
}

void ControlWindow::on_setPcCalOffFrom180ToMinus2p5_btn_clicked()   {    ui->calAt180_sb->setValue(-2.5);   }
void ControlWindow::on_setPcCalOffFrom180ToMinus3p0_btn_clicked()   {    ui->calAt180_sb->setValue(-3.0);   }
void ControlWindow::on_setPcCalOffFrom180ToMinus3p5_btn_clicked()   {    ui->calAt180_sb->setValue(-3.5);   }

void ControlWindow::on_penWidth_sb_valueChanged(int)                    { mw->penWidth = ui->penWidth_sb->value();                                              }
void ControlWindow::on_drawVerticalShadow_cb_stateChanged(int)          { mw->drawVerticalShadow = ui->drawVerticalShadow_cb->isChecked();                      }
void ControlWindow::on_drawRotatingVector_cb_stateChanged(int)          { mw->drawRotatingVector = ui->drawRotatingVector_cb->isChecked();                      }
void ControlWindow::on_showCosOnXAxis_cb_stateChanged(int)              { mw->showCosOnXAxis = ui->showCosOnXAxis_cb->isChecked();                              }
void ControlWindow::on_showCosOnYAxis_cb_stateChanged(int)              { mw->showCosOnYAxis = ui->showCosOnYAxis_cb->isChecked();                              }
void ControlWindow::on_useArduino_cb_stateChanged(int)                  { mw->useArduino = ui->useArduino_cb->isChecked();                                      }
void ControlWindow::on_showSinOnXAxis_cb_stateChanged(int)              { mw->showSinOnXAxis = ui->showSinOnXAxis_cb->isChecked();                              }
void ControlWindow::on_showVerticalProjectionBox_cb_stateChanged(int)   { mw->showVerticalProjectionBox = ui->showVerticalProjectionBox_cb->isChecked();        }
void ControlWindow::on_showHorizontalProjectionBox_cb_stateChanged(int) { mw->showHorizontalProjectionBox = ui->showHorizontalProjectionBox_cb->isChecked();    }
void ControlWindow::on_drawHorizontalShadow_cb_stateChanged(int)        { mw->drawHorizontalShadow = ui->drawHorizontalShadow_cb->isChecked();                  }
void ControlWindow::on_clearSinValues_btn_clicked()                     { mw->renderWidget->clearSinOrdinates();                                                }
void ControlWindow::on_clearCosValues_btn_clicked()                     { mw->renderWidget->clearCosOrdinates();                                                }
void ControlWindow::on_drawVerticalProjectionDottedLine_cb_stateChanged(int)   { mw->drawVerticalProjectionDottedLine = ui->drawVerticalProjectionDottedLine_cb->isChecked(); }
void ControlWindow::on_drawHorizontalProjectionDottedLine_cb_stateChanged(int) { mw->drawHorizontalProjectionDottedLine = ui->drawHorizontalProjectionDottedLine_cb->isChecked(); }
void ControlWindow::on_drawVerticalProjectionTipCircle_cb_stateChanged(int)    { mw->drawVerticalProjectionTipCircle = ui->drawVerticalProjectionTipCircle_cb->isChecked(); }
void ControlWindow::on_drawHorizontalProjectionTipCircle_cb_stateChanged(int)  { mw->drawHorizontalProjectionTipCircle = ui->drawHorizontalProjectionTipCircle_cb->isChecked(); }
void ControlWindow::on_drawAngleArc_cb_stateChanged(int)                { mw->drawAngleArc = ui->drawAngleArc_cb->isChecked();                                  }
void ControlWindow::on_drawSinComponent_cb_stateChanged(int)            { mw->drawSinComponent = ui->drawSinComponent_cb->isChecked();                          }
void ControlWindow::on_drawCosComponent_cb_stateChanged(int)            { mw->drawCosComponent = ui->drawCosComponent_cb->isChecked();                          }
void ControlWindow::on_showAnglesOnXAndYAxis_cb_stateChanged(int)       { mw->showAnglesOnXAndYAxis = ui->showAnglesOnXAndYAxis_cb->isChecked();                }
void ControlWindow::on_showScrollingBackgroundText_cb_stateChanged(int) { mw->showScrollingBackgroundText = ui->showScrollingBackgroundText_cb->isChecked();    }
void ControlWindow::on_show30And60Angles_cb_stateChanged(int)           { mw->show30And60Angles = ui->show30And60Angles_cb->isChecked();                        }

void ControlWindow::on_extraVectorOffsetFromBottom_sb_valueChanged(int)
{
    mw->extraVectorOffsetFromBottom = ui->extraVectorOffsetFromBottom_sb->value();
    mw->renderWidget->recalculateVectorOrigin();
}

void ControlWindow::on_extraVectorOffsetFromRight_sb_valueChanged(int)
{
    mw->extraVectorOffsetFromRight = ui->extraVectorOffsetFromRight_sb->value();
    mw->renderWidget->recalculateVectorOrigin();
}

void ControlWindow::on_sineAmplitude_sb_valueChanged(int)
{
    mw->amplitude = ui->sineAmplitude_sb->value();
    mw->renderWidget->recalculateVectorOrigin();
}


void ControlWindow::on_timeDelay_sb_valueChanged(int)
{
    mw->timerInterval = ui->timeDelay_sb->value();
    mw->renderWidget->updateTimerInterval();
}

void ControlWindow::on_angleAdvanceOffset_sb_valueChanged(const QString &)
{
}

void ControlWindow::on_phaseShiftFromSine_sb_valueChanged(int)
{
    mw->phaseShiftFromSine = ui->phaseShiftFromSine_sb->value();
    mw->renderWidget->updatePhaseShiftFromSine();
}
