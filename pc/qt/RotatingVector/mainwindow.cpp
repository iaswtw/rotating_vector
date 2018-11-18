#include "mainwindow.h"
#include "renderwidget.h"
#include "controlwindow.h"
#include "ui_mainwindow.h"
#include "ui_controlwindow.h"
#include <stdio.h>
#include <iostream>
#include <QPen>
#include <QPainter>
#include <math.h>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this)),
    arduinoSimulator(nullptr),

    curAngleInRadians(0.0),
    curAngleInDegrees(0.0),
    isTimePaused(false),
    xAxisOffFromTop(663),
    xAxisOffFromRight(661),
    amplitude(220),
    curHeight(0),
    curWidth(amplitude),
    penWidth(12),
    timeXInc(1),
    drawRotatingVector(true),
    drawAngleArc(true),

    drawSinComponent(false),
    drawCosComponent(false),

    drawVerticalShadow(false),
    drawHorizontalShadow(false),

    drawVerticalProjectionDottedLine(false),
    drawHorizontalProjectionDottedLine(false),

    drawVerticalProjectionTipCircle(false),
    drawHorizontalProjectionTipCircle(false),

    showVerticalProjectionBox(false),
    showHorizontalProjectionBox(false),

    showSinOnXAxis(false),
    showCosOnYAxis(false),
    showCosOnXAxis(false),

    showAnglesOnXAndYAxis(false),
    showScrollingBackgroundText(false),
    show30And60Angles(false),

    timerInterval(30),
    halfSteps(0),
    useArduino(false),

    serialData(new QByteArray())
{
    //---------------------------------------------------------------------------------
    // Following values are set to I don't have to click checkboxes everytime I start the app for testing.
    // Comment these values for actual demonstration.
//    showVerticalProjectionBox = true;
//    showHorizontalProjectionBox = true;

//    drawVerticalShadow = true;
//    drawHorizontalShadow = true;

//    drawVerticalProjectionTipCircle = true;
//    drawHorizontalProjectionTipCircle = true;

//    drawVerticalProjectionDottedLine = true;
//    drawHorizontalProjectionDottedLine = true;

//    showSinOnXAxis = true;
//    showCosOnYAxis = true;
//    showCosOnXAxis = true;

//    drawAngleArc = true;
//    showAnglesOnXAndYAxis = true;
//    show30And60Angles = true;
    //---------------------------------------------------------------------------------

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
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);


    // ----------------- Serial port -----------------
    serial->setPortName("COM3");
    serial->setBaudRate(115200);
    serial->setDataBits(QSerialPort::DataBits::Data8);
    if (serial->open(QIODevice::ReadWrite))
    {
        printf("Serial port opened successfully\n");
    }

    arduinoSimulator = new ArduinoSimulator(this, this);

    printf("Current Dir: %s\n", QDir::currentPath().toStdString().c_str());
    //printf("Hello world\n");
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

void MainWindow::setControlWindow(ControlWindow *cw)
{
    this->cw = cw;
}


void MainWindow::readSerialData()
{
    const QByteArray data = serial->readAll();
    serialData->append(data);

    // did we get a comple line?
    if (serialData->contains('\n'))
    {
        // print the line
        int newlineIndex = serialData->indexOf('\n');
        QByteArray line = serialData->left(newlineIndex);

        // we read the serial data unconditionally, but process it only if 'use arduino' is selected.
        if (useArduino)
        {
            processSerialLine(line);
        }

        serialData->remove(0, newlineIndex+1);
    }
}


void MainWindow::processSerialLine(QByteArray line)
{
    QRegExp re("(\\d+\\.\\d+)[ ]+(\\d+)");

    //printf("%s", line.data());

    int pos = re.indexIn(line.data());
    QStringList list = re.capturedTexts();

//        printf("regex match result contains %d results. pos = %d\n", list.size(), pos);
    if ((pos != -1) && (list.size() == 3))
    {
        QString qstr;

        // Extract angle
        qstr = list.at(1);
        curAngleInDegrees = qstr.toDouble();

        //printf("Angle: %.2f\n", double(curAngleInDegrees));
        cw->ui->curAngle_le->setText(qstr.toLocal8Bit().constData());       // set current angle in GUI

        if (useArduino)
            curAngleInDegrees += cw->ui->angleAdvanceOffset_sb->value();

        curAngleInRadians = curAngleInDegrees * M_PI / 180;
        curHeight = int(amplitude * sin(curAngleInRadians));
        curWidth  = int(amplitude * cos(curAngleInRadians));

        // Extract half steps
        qstr = list.at(2);
        halfSteps = qstr.toInt();
        //printf("Half steps: %d\n", halfSteps);
        cw->ui->curHalfSteps_le->setText(qstr.toLocal8Bit().constData());
    }
    //printf("\n");
}
