#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include <QPen>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this)),
    timer(new QTimer()),

    curHeight(0),
    curAngleInRadians(0.0),
    isTimePaused(false),
    xAxisOffFromTop(300),
    xAxisOffFromRight(400),
    amplitude(0),
    penWidth(4),
    timeXInc(1),
    drawRotatingVector(true),
    drawShadow(true),
    timerInterval(20)
{
    setbuf(stdout, NULL);
    ui->setupUi(this);

    //-------------- Connect signals to slots ------------------
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerEvent);

    timer->start(timerInterval);


    // ----------------- Serial port -----------------
    serial->setPortName("COM3");
    serial->setBaudRate(115200);
    serial->setDataBits(QSerialPort::DataBits::Data8);
    if (serial->open(QIODevice::ReadWrite))
    {
        printf("Serial port opened successfully\n");
    }

    ui->penWidth_sb->setValue(penWidth);
    ui->drawShadow_cb->setChecked(drawShadow);
    ui->drawRotatingVector_cb->setChecked(drawRotatingVector);
    ui->xAxisOffFromRight_sb->setValue(xAxisOffFromRight);
    ui->xAxisOffFromTop_sb->setValue(xAxisOffFromTop);
    ui->sineAmplitude_sb->setValue(amplitude);


    printf("Hello world\n");
    fflush(stdout);
//    std::cout << "Hello world" << std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent()
{
//    printf("Timer event\n");
    repaint();
}

void MainWindow::update()
{

}

void MainWindow::paintEvent(QPaintEvent *pe)
{
    QPainter p(ui->screenWidget);

//    p.begin(ui->screenWidget);
    this->draw(&p);
//    p.end();

}

void MainWindow::draw(QPainter * p)
{
    QPen pen = QPen(QColor(120, 120, 120));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->drawLine(0,
                xAxisOffFromTop,
                ui->screenWidget->width() - xAxisOffFromRight,
                xAxisOffFromTop);

}

void MainWindow::sendCmd(const char * pCmd)
{
    serial->write(pCmd, strlen(pCmd));
}

void MainWindow::readData()
{
    const QByteArray data = serial->readAll();
//    console->putData(data);
    for (int i = 0; i < data.length(); i++)
//        printf("%02X", data[i]);
        printf("%c", data[i]);
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
    // TODO
}

void MainWindow::on_pauseVector_btn_clicked()
{
    sendCmd("p\n");
}

void MainWindow::on_pauseVectorAndTime_btn_clicked()
{
    // TODO pause time
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
    sendCmd("g180\n");
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

}

void MainWindow::on_setPcCalOffFrom180ToMinus3p0_btn_clicked()
{

}

void MainWindow::on_setPcCalOffFrom180ToMinus3p5_btn_clicked()
{

}

void MainWindow::on_timeDelay_sb_valueChanged(int arg1)
{

}

void MainWindow::on_sineAmplitude_sb_valueChanged(int arg1)
{

}

void MainWindow::on_penWidth_sb_valueChanged(int arg1)
{

}

void MainWindow::on_xAxisOffFromTop_sb_valueChanged(int arg1)
{

}

void MainWindow::on_xAxisOffFromRight_sb_valueChanged(int arg1)
{

}
