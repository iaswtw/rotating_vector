#include "mainwindow.h"
#include "renderwidget.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include <QPen>
#include <QPainter>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this)),

    curHeight(0),
    curAngleInRadians(0.0),
    curAngleInDegrees(0.0),
    isTimePaused(false),
    xAxisOffFromTop(388),
    xAxisOffFromRight(523),
    amplitude(220),
    penWidth(10),
    timeXInc(1),
    drawRotatingVector(true),
    drawShadow(true),
    timerInterval(20),
    halfSteps(0),
    serialData(new QByteArray())
{
    setbuf(stdout, nullptr);
    ui->setupUi(this);


    renderWidget = new RenderWidget(ui->topWidget, this);
    renderWidget->setObjectName(QStringLiteral("renderWidget"));
    QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Minimum);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(1);
    sizePolicy5.setHeightForWidth(renderWidget->sizePolicy().hasHeightForWidth());
    renderWidget->setSizePolicy(sizePolicy5);
    renderWidget->setMinimumSize(QSize(0, 400));
    renderWidget->setStyleSheet(QStringLiteral("background: green"));

    ui->topWidgetLayout->addWidget(renderWidget);


    //-------------- Connect signals to slots ------------------
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);


    // ----------------- Serial port -----------------
    serial->setPortName("COM3");
    serial->setBaudRate(115200);
    serial->setDataBits(QSerialPort::DataBits::Data8);
    if (serial->open(QIODevice::ReadWrite))
    {
        printf("Serial port opened successfully\n");
    }


    //----------------------------------------------------------------------------
    // Set initial values in GUI widgets
    ui->penWidth_sb->setValue(penWidth);
    ui->drawShadow_cb->setChecked(drawShadow);
    ui->drawRotatingVector_cb->setChecked(drawRotatingVector);
    ui->xAxisOffFromRight_sb->setValue(xAxisOffFromRight);
    ui->xAxisOffFromTop_sb->setValue(xAxisOffFromTop);
    ui->sineAmplitude_sb->setValue(amplitude);
    ui->timeDelay_sb->setValue(timerInterval);

    printf("Hello world\n");
    fflush(stdout);
//    std::cout << "Hello world" << std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::update()
{

}


void MainWindow::sendCmd(const char * pCmd)
{
    serial->write(pCmd, strlen(pCmd));
}

void MainWindow::readData()
{
    QRegExp re("(\\d+\\.\\d+)[ ]+(\\d+)");

    const QByteArray data = serial->readAll();
    serialData->append(data);

    // did we get a comple line?
    if (serialData->contains('\n'))
    {
        // print the line
        int newlineIndex = serialData->indexOf('\n');
        QByteArray line = serialData->left(newlineIndex);

        printf("%s", line.data());


        int pos = re.indexIn(line.data());
        QStringList list = re.capturedTexts();

//        printf("regex match result contains %d results. pos = %d\n", list.size(), pos);
        if ((pos != -1) && (list.size() == 3))
        {
            QString qstr;

            // Extract angle
            qstr = list.at(1);
            curAngleInDegrees = qstr.toDouble();
            printf("Angle: %.2f\n", double(curAngleInDegrees));
            ui->curAngle_le->setText(qstr.toLocal8Bit().constData());


            curAngleInDegrees += ui->angleAdvanceOffset_sb->value();
            curAngleInRadians = curAngleInDegrees * M_PI / 180;
            curHeight = int(amplitude * sin(curAngleInRadians));


            // Extract half steps
            qstr = list.at(2);
            halfSteps = qstr.toInt();
            printf("Half steps: %d\n", halfSteps);
            ui->curHalfSteps_le->setText(qstr.toLocal8Bit().constData());
        }
        printf("\n");



        serialData->remove(0, newlineIndex+1);

    }

}

void MainWindow::on_speed1_btn_clicked()
{
    sendCmd("1\n");
}

void MainWindow::on_speed2_btn_clicked()
{
    sendCmd("2\n");
}

void MainWindow::on_speed3_btn_clicked()
{
    sendCmd("3\n");
}

void MainWindow::on_speed4_btn_clicked()
{
    sendCmd("4\n");
}

void MainWindow::on_speed5_btn_clicked()
{
    sendCmd("5\n");
}

void MainWindow::on_speed6_btn_clicked()
{
    sendCmd("6\n");
}

void MainWindow::on_continue_btn_clicked()
{
    sendCmd("c\n");
    isTimePaused = false;
}

void MainWindow::on_ccwHalfStep_btn_clicked()
{
    sendCmd("<\n");
    sendCmd("h\n");
}

void MainWindow::on_cwHalfStep_btn_clicked()
{
    sendCmd(">\n");
    sendCmd("h\n");
    sendCmd("<\n");
}

void MainWindow::on_releaseCoils_btn_clicked()
{
    sendCmd("r\n");
}

void MainWindow::on_ccwDirection_btn_clicked()
{
    sendCmd("<\n");
}

void MainWindow::on_cwDirection_btn_clicked()
{
    sendCmd(">\n");
}

void MainWindow::on_pauseTime_btn_clicked()
{
    isTimePaused = true;
}

void MainWindow::on_pauseVector_btn_clicked()
{
    sendCmd("p\n");
}

void MainWindow::on_pauseVectorAndTime_btn_clicked()
{
    isTimePaused = true;
    sendCmd("p\n");
}

void MainWindow::on_goto0_btn_clicked()
{
    sendCmd("g0\n");
}

void MainWindow::on_goto90_btn_clicked()
{
    sendCmd("g90\n");
}

void MainWindow::on_goto180_btn_clicked()
{
    QString cmd;
    cmd.append("g");
    double angle = 180 + ui->calAt180_sb->value();
    cmd.append(QString::number(angle));
    cmd.append("\n");

    printf("Sending cmd: %s", cmd.toLocal8Bit().constData());

    sendCmd(cmd.toLocal8Bit().constData());
}

void MainWindow::on_goto270_btn_clicked()
{
    sendCmd("g270\n");
}

void MainWindow::on_gotoAngle_btn_clicked()
{
    QString text = ui->gotoAngle_le->displayText();
    QString cmd;

    cmd.append("g");
    cmd.append(text);
    cmd.append("\n");
    sendCmd(cmd.toLocal8Bit().data());
}

void MainWindow::on_setAs0_btn_clicked()
{
    sendCmd("=0\n");
}

void MainWindow::on_setAs180_btn_clicked()
{
    sendCmd("=180\n");
}

void MainWindow::on_setCurPos_btn_clicked()
{
    QString text = ui->curPos_le->displayText();
    QString cmd;

    cmd.append("=");
    cmd.append(text);
    cmd.append("\n");
    sendCmd(cmd.toLocal8Bit().data());
}

void MainWindow::on_setPcCalOffFrom180ToMinus2p5_btn_clicked()
{
    ui->calAt180_sb->setValue(-2.5);
}

void MainWindow::on_setPcCalOffFrom180ToMinus3p0_btn_clicked()
{
    ui->calAt180_sb->setValue(-3.0);
}

void MainWindow::on_setPcCalOffFrom180ToMinus3p5_btn_clicked()
{
    ui->calAt180_sb->setValue(-3.5);
}

void MainWindow::on_sineAmplitude_sb_valueChanged(int /*arg1*/)
{
    amplitude = ui->sineAmplitude_sb->value();
}

void MainWindow::on_penWidth_sb_valueChanged(int /*arg1*/)
{
    penWidth = ui->penWidth_sb->value();
}

void MainWindow::on_xAxisOffFromTop_sb_valueChanged(int /*arg1*/)
{
    xAxisOffFromTop = ui->xAxisOffFromTop_sb->value();
}

void MainWindow::on_xAxisOffFromRight_sb_valueChanged(int /*arg1*/)
{
    xAxisOffFromRight = ui->xAxisOffFromRight_sb->value();
}

void MainWindow::on_sineAmplitude_sb_valueChanged(const QString &/*arg1*/)
{
    amplitude = ui->sineAmplitude_sb->value();
}

void MainWindow::on_drawShadow_cb_stateChanged(int /*arg1*/)
{
    drawShadow = ui->drawShadow_cb->isChecked();
}

void MainWindow::on_drawRotatingVector_cb_stateChanged(int /*arg1*/)
{
    drawRotatingVector = ui->drawRotatingVector_cb->isChecked();
}

void MainWindow::on_timeDelay_sb_valueChanged(int)
{
    timerInterval = ui->timeDelay_sb->value();
    renderWidget->updateTimerInterval();
}

