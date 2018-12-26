#include "mainwindow.h"
#include "renderwidget.h"
#include "controlwindow.h"
#include "ui_mainwindow.h"
#include "ui_controlwindow.h"
#include "aboutdialog.h"
#include <stdio.h>
#include <iostream>
#include <QPen>
#include <QPainter>
#include <math.h>
#include <QDir>
#include <QMessageBox>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //---------------------------------------------------------------------------------
    // Following values are set to I don't have to click checkboxes everytime I start the app for testing.
    // Comment these values for actual demonstration.
    showVerticalProjectionBox = true;
    showHorizontalProjectionBox = true;

    drawVerticalShadow = true;
    drawHorizontalShadow = true;

    drawVerticalProjectionTipCircle = true;
    drawHorizontalProjectionTipCircle = true;

    drawVerticalProjectionDottedLine = true;
    drawHorizontalProjectionDottedLine = true;

    showSinOnXAxis = true;
    showCosOnYAxis = true;
    showCosOnXAxis = true;

    drawAngleArc = true;
    showAnglesOnXAndYAxis = true;
//    show30And60Angles = true;
    //---------------------------------------------------------------------------------

    setbuf(stdout, nullptr);
    ui->setupUi(this);
    QMainWindow::showMaximized();
    setWindowTitle("Rotating Vector");

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
    fflush(stdout);

    oneTimeTimer.setSingleShot(true);
    connect(&oneTimeTimer, SIGNAL(timeout()), this, SLOT(oneTimeTimerHandler()));
    oneTimeTimer.start(500);

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::oneTimeTimerHandler()
{
//    cw->setLowestSpeed();
//    cw->continueVectorAndUnpauseTime();
}

void MainWindow::setControlWindow(ControlWindow *cw)
{
    this->cw = cw;
    showControlWindowCentered();
}

void MainWindow::renderWidgetPaintEvent()
{
    if (!useArduino)
    {
        arduinoSimulator->tick();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space)
    {
        if (event->modifiers() & Qt::ControlModifier)
            cw->continueVectorAndUnpauseTime();
        else
            cw->pauseVectorAndTime();
    }
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

void MainWindow::showControlWindowCentered()
{
    cw->show();
    cw->move(200, 50);
}

void MainWindow::on_actionControl_Panel_triggered()
{
    showControlWindowCentered();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog;
    aboutDialog.setModal(true);
    aboutDialog.exec();

//    QMessageBox messageBox;
//    messageBox.about(this, "About Rotating Vector", "Version 0.1.1\nAuthor: Abhir Joshi");
}
