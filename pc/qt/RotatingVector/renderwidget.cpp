#include "renderwidget.h"
#include <QPen>
#include <QPainter>
#include <QRandomGenerator>
#include "mainwindow.h"

RenderWidget::RenderWidget(QWidget *parent, MainWindow *data) : QWidget(parent),
    timer(new QTimer(this))
{
    this->data = data;

    connect(timer, SIGNAL(timeout()), this, SLOT(renderTimerEvent()));

    timer->start(data->timerInterval);

    QRandomGenerator randomGen(0);

    //----------------------------------------------------------------
    for (int i = 0; i < NUM_BACKGROUND_TEXT_POINTS; i++)
    {
        bkTextPoints[i].x = randomGen.bounded(0, 1900);
        bkTextPoints[i].y = randomGen.bounded(0, 800);
        bkTextPoints[i].text = "Background";
    }
    for (int i = 0; i < NUM_TIME_TEXT_POINTS; i++)
    {
        timeTextPoints[i].x = randomGen.bounded(-200, 1700);
        timeTextPoints[i].y = randomGen.bounded(-200, 1700);
        timeTextPoints[i].text = "Time";
    }



}

void RenderWidget::renderTimerEvent()
{
//    printf("Timer event\n");
    repaint();
}

void RenderWidget::paintEvent(QPaintEvent *pe)
{
    QWidget::paintEvent(pe);
    QPainter p(this);

    draw(&p);

}

void RenderWidget::draw(QPainter * p)
{
    //-------------------------------------------------------------------
    // Draw X axis
    //--------------------------------------------------------------------
    QPen pen = QPen(QColor(120, 120, 120));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->drawLine(0,
                data->xAxisOffFromTop,
                width() - data->xAxisOffFromRight,
                data->xAxisOffFromTop);

    //-------------------------------------------------------------------
    // Draw rectangle that will have the vector's shadow
    //-------------------------------------------------------------------
    pen = QPen(QColor(100, 100, 100));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->setOpacity(0.2);
    p->drawRect(width() - data->xAxisOffFromRight - data->penWidth/2 - 2,
                data->xAxisOffFromTop - data->amplitude - data->penWidth/2 - 2,
                data->penWidth + 2,
                data->amplitude*2 + data->penWidth + 2);
    p->setOpacity(1);

    //--------------------------------------------------------------------
    // Draw vector shadow, if enabled
    //--------------------------------------------------------------------
    if (data->drawShadow)
    {
        pen = QPen(QColor(220, 20, 20));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(QColor(220, 20, 20));
        p->setOpacity(1);
        p->drawRect(width() - data->xAxisOffFromRight - data->penWidth/2,
                    data->xAxisOffFromTop - data->curHeight,
                    data->penWidth,
                    data->curHeight);
        p->setOpacity(1);
    }
    //--------------------------------------------------------------------
    // Draw rotating vector, if enabled
    //--------------------------------------------------------------------
    if (data->drawRotatingVector)
    {
//        vector_origin = Point()
        int vector_origin_x = width() - data->xAxisOffFromRight + 20 + data->amplitude;
        int vector_origin_y = data->xAxisOffFromTop;

        int vector_width  = int(data->amplitude * cos(data->curAngleInRadians));
        int vector_height = int(data->amplitude * sin(data->curAngleInRadians));

//        vector_tip = Point()
        int vector_tip_x = vector_origin_x + vector_width;
        int vector_tip_y = vector_origin_y - vector_height;


        //----------------------------------------------------
        // Draw vector axis and tracing circle
        pen = QPen(QColor(120, 120, 120));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(QColor(220, 20, 20));
        p->setOpacity(0.1);
        p->drawEllipse(vector_origin_x - data->amplitude,
                       vector_origin_y - data->amplitude,
                       2 * data->amplitude,
                       2 * data->amplitude
        );

        p->setOpacity(0.3);
        // vector's x axis
        p->drawLine(vector_origin_x - data->amplitude - 10,
                    vector_origin_y,
                    vector_origin_x + data->amplitude + 10,
                    vector_origin_y
        );
        // vector's y axis
        p->drawLine(vector_origin_x,
                    vector_origin_y - data->amplitude - 10,
                    vector_origin_x,
                    vector_origin_y + data->amplitude + 10
        );

        p->setOpacity(1);


        //----------------------------------------------------
        // Draw vector itself.
        pen = QPen(QColor(220, 20, 20));
        pen.setWidth(data->penWidth);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(QColor(220, 20, 20));
        p->setOpacity(1);
        p->drawLine(vector_origin_x,
                    vector_origin_y,
                    vector_tip_x,
                    vector_tip_y
        );
        p->setOpacity(1);


        //----------------------------------------------------
        // Draw vector tip projection
        pen = QPen(QColor(20, 20, 20));
        pen.setWidth(data->penWidth);
        pen.setStyle(Qt::DotLine);
        p->setPen(pen);
        p->setOpacity(0.2);
        p->drawLine(
                    vector_tip_x,
                    vector_tip_y,
                    width() - data->xAxisOffFromRight,
                    data->xAxisOffFromTop - vector_height
        );
        p->setOpacity(1);
    }

    //--------------------------------------------------------------------
    // Draw background
    //--------------------------------------------------------------------
    QFont font = QFont();
    font.setPixelSize(30);
    pen = QPen(QColor(100, 100, 100));
    p->setPen(pen);
    p->setBrush(Qt::green);
    p->setFont(font);
    p->setOpacity(0.2);
    for (int i = 0; i < NUM_BACKGROUND_TEXT_POINTS; i++)
    {
        p->drawText(bkTextPoints[i].x, bkTextPoints[i].y, bkTextPoints[i].text);

        //--------------------------------------------------
        // If time isn't paused, shift all background objects left
        if (!data->isTimePaused)
        {
            bkTextPoints[i].x -= data->timeXInc;
            if (bkTextPoints[i].x <= -150)
                bkTextPoints[i].x = 2000;
        }
    }
    for (int i = 0; i < NUM_TIME_TEXT_POINTS; i++)
    {
        p->drawText(timeTextPoints[i].x, timeTextPoints[i].y, timeTextPoints[i].text);

        //--------------------------------------------------
        // If time isn't paused, shift all background objects left
        if (!data->isTimePaused)
        {
            timeTextPoints[i].x -= data->timeXInc;
            if (timeTextPoints[i].x <= -150)
                timeTextPoints[i].x = 2000;
        }
    }
    p->setOpacity(1);

}
