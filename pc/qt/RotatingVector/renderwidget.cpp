#include "renderwidget.h"
#include <QPen>
#include <QPainter>
#include <QRandomGenerator>
#include "mainwindow.h"

RenderWidget::RenderWidget(QWidget *parent, MainWindow *data) :
    QWidget(parent),
    timer(new QTimer(this)),
    vtScrollingBackground(NUM_BACKGROUND_TEXT_POINTS/2, NUM_TIME_TEXT_POINTS/2, NUM_ANGLE_TEXT_POINTS/2),
    hzScrollingBackground(NUM_BACKGROUND_TEXT_POINTS, NUM_TIME_TEXT_POINTS, NUM_ANGLE_TEXT_POINTS)
{
    this->data = data;

    connect(timer, SIGNAL(timeout()), this, SLOT(renderTimerEvent()));

    timer->start(data->timerInterval);


    //----------------------------------------------------------------
    for (int i = 0; i < NUM_ORDINATES; i++)
    {
        sinOrdinates[i] = 0;
        cosOrdinates[i] = 0;
    }


    QRandomGenerator randomGen(10);
    hzScrollingBackground.generateRandomAnglePoints(      -200, 1700, 0, 800, randomGen);
    hzScrollingBackground.generateRandomBackgroundPoints( -200, 1900, 0, 800, randomGen);
    hzScrollingBackground.generateRandomTimePoints(       -200, 1700, 0, 800, randomGen);

    vtScrollingBackground.generateRandomAnglePoints(      0, 500, 0, 800, randomGen);
    vtScrollingBackground.generateRandomBackgroundPoints( 0, 500, 0, 800, randomGen);
    vtScrollingBackground.generateRandomTimePoints(       0, 500, 0, 800, randomGen);
}

void RenderWidget::updateTimerInterval()
{
    timer->start(data->timerInterval);
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
    QFont font;

    int vector_origin_x = width() - data->xAxisOffFromRight + 20 + data->amplitude;
    int vector_origin_y = data->xAxisOffFromTop;

    int vector_width  = int(data->amplitude * cos(data->curAngleInRadians));
    int vector_height = int(data->amplitude * sin(data->curAngleInRadians));

    int vector_tip_x = vector_origin_x + vector_width;
    int vector_tip_y = vector_origin_y - vector_height;


    //-------------------------------------------------------------------
    // Draw X & Y axis
    //--------------------------------------------------------------------
    QPen pen = QPen(QColor(120, 120, 120));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->drawLine(0,
                vector_origin_y,
                width() - data->xAxisOffFromRight,
                vector_origin_y);

    if (data->showCosOnYAxis)
    {
        p->drawLine(vector_origin_x,
                    0,
                    vector_origin_x,
                    vector_origin_y - data->amplitude - 20 - data->penWidth/2);
    }

    //-------------------------------------------------------------------
    // Draw rectangle that will have the vector's vertical shadow
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


    if (data->showCosOnYAxis)
    {
        //-------------------------------------------------------------------
        // Draw rectangle that will have the vector's horizontal projection (not shadow. no vector casting horizontal shadow)
        //-------------------------------------------------------------------
        pen = QPen(QColor(100, 100, 100));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setOpacity(0.2);
        p->drawRect(vector_origin_x - data->amplitude - 2,
                    vector_origin_y - data->amplitude - 20 - data->penWidth/2 - 2,
                    data->amplitude*2 + data->penWidth + 2,
                    data->penWidth + 2);

        p->setOpacity(1);
    }

    //---------------------------------------------------------
    int w;
    QString str;
    pen.setColor(QColor(150,150,150));
    p->setPen(pen);
    font = QFont();
    font.setPixelSize(13);
//    font.setStretch(2.0);
    p->setFont(font);
    QFontMetrics fm(font);

    if (data->showCosOnYAxis)
    {
        str = "H o r i z o n t a l   p r o j e c t i o n";
        w = fm.width(str);

        p->drawText(vector_origin_x - w / 2,
                    vector_origin_y - data->amplitude - 4,
                    str);
        p->drawText(vector_origin_x - data->amplitude,
                    vector_origin_y - data->amplitude - data->penWidth - 20,
                    "-1");
        p->drawText(vector_origin_x + data->amplitude,
                    vector_origin_y - data->amplitude - data->penWidth - 20,
                    "+1");
    }

    //---------------------------------------------------------
    str = "V e r t i c a l   p r o j e c t i o n";
    w = fm.width(str);

    p->save();
    p->translate(vector_origin_x - data->amplitude - 4,
                 vector_origin_y + w / 2);
    p->rotate(-90);
    p->drawText(0, 0, str);
    p->restore();

    p->drawText(vector_origin_x - data->amplitude - data->penWidth - 20 - 15,
                vector_origin_y + data->amplitude,
                "-1");
    p->drawText(vector_origin_x - data->amplitude - data->penWidth - 20 - 15,
                vector_origin_y - data->amplitude,
                "+1");




    //--------------------------------------------------------------------
    // Draw vector shadow, if enabled
    //--------------------------------------------------------------------
    if (data->drawShadow)
    {
        //--------------------------------------------------------------------
        // Vertical shadow
        //--------------------------------------------------------------------
        pen = QPen(sinColor);
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(sinColor);
        p->setOpacity(1);
        p->drawRect(width() - data->xAxisOffFromRight - data->penWidth/2,
                    data->xAxisOffFromTop - data->curHeight,
                    data->penWidth,
                    data->curHeight);
        p->setOpacity(1);

        if (data->showCosOnYAxis)
        {
            //--------------------------------------------------------------------
            // Horizontal projection
            //--------------------------------------------------------------------
            pen = QPen(cosColor);
            pen.setWidth(2);
            pen.setCapStyle(Qt::RoundCap);
            p->setPen(pen);
            p->setBrush(cosColor);
            p->setOpacity(1);
            p->drawRect(vector_origin_x,
                        vector_origin_y - data->amplitude - 20 - data->penWidth/2,
                        data->curWidth,
                        data->penWidth);
            p->setOpacity(1);
        }

    }
    //--------------------------------------------------------------------
    // Draw rotating vector, if enabled
    //--------------------------------------------------------------------
    if (data->drawRotatingVector)
    {
        //----------------------------------------------------
        // Draw vector axis and tracing circle
        pen = QPen(QColor(120, 120, 120));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(vectorSweepColor);
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
        pen = QPen(vectorColor);
        pen.setWidth(data->penWidth);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(vectorColor);
        p->setOpacity(1);
        p->drawLine(vector_origin_x,
                    vector_origin_y,
                    vector_tip_x,
                    vector_tip_y
        );
        p->setOpacity(1);

        //----------------------------------------------------
        // Draw vector tip vertical projection
        pen = QPen(QColor(20, 20, 20));
        pen.setWidth(data->penWidth);
        pen.setStyle(Qt::DotLine);
        p->setPen(pen);
        p->setOpacity(0.2);
        p->drawLine(vector_tip_x,
                    vector_tip_y,
                    width() - data->xAxisOffFromRight,
                    data->xAxisOffFromTop - vector_height
        );
        p->setOpacity(1);

        if (data->showCosOnYAxis)
        {
            //----------------------------------------------------
            // Draw vector tip horizontal projection
            pen = QPen(QColor(20, 20, 20));
            pen.setWidth(data->penWidth);
            pen.setStyle(Qt::DotLine);
            p->setPen(pen);
            p->setOpacity(0.2);
            p->drawLine(vector_tip_x,
                        vector_tip_y,
                        vector_tip_x,
                        vector_origin_y - data->amplitude - 20
            );
            p->setOpacity(1);
        }
    }

    //--------------------------------------------------------------------
    // Draw Horizontal background
    //--------------------------------------------------------------------
    font = QFont();
    font.setPixelSize(30);
    pen = QPen(scrollingBackgroundColor);
    p->setPen(pen);
    p->setBrush(Qt::green);
    p->setFont(font);
    p->setOpacity(0.2);

    hzScrollingBackground.draw(*p,
                               0,                                               // x offset
                               vector_origin_y - data->amplitude,               // y offset
                               vector_origin_x - data->amplitude,               // width of rectangle to draw in
                               data->amplitude * 2                              // height of rectangle to draw in
    );

    if (!data->isTimePaused)
    {
        hzScrollingBackground.shiftLeft(data->timeXInc);
    }

    pen = QPen(cosColor);
    p->setPen(pen);
    p->setBrush(sinColor);
    p->setOpacity(0.05);
    p->drawRect(0,
                vector_origin_y - data->amplitude - 20,
                vector_origin_x - data->amplitude - 30,
                data->amplitude * 2 + 40);


    p->setOpacity(1);


    if (data->showCosOnYAxis)
    {
        //--------------------------------------------------------------------
        // Draw Vertical background
        //--------------------------------------------------------------------
        font = QFont();
        font.setPixelSize(30);
        pen = QPen(scrollingBackgroundColor);
        p->setPen(pen);
        p->setFont(font);
        p->setOpacity(0.2);

        vtScrollingBackground.draw(*p,
                                   vector_origin_x - data->amplitude,               // x offset
                                   0,                                               // y offset
                                   data->amplitude * 2,                             // width of rectangle to draw in
                                   vector_origin_y - data->amplitude                // height of rectangle to draw in
        );

        if (!data->isTimePaused)
        {
            vtScrollingBackground.shiftUp(data->timeXInc);
        }

        pen = QPen(cosColor);
        p->setPen(pen);
        p->setBrush(cosColor);
        p->setOpacity(0.05);
        p->drawRect(vector_origin_x - data->amplitude - 20,
                    0,
                    data->amplitude * 2 + 40,
                    vector_origin_y - data->amplitude - 30);

        p->setOpacity(1);
    }


    //--------------------------------------------------------------------
    // Draw sine points
    //--------------------------------------------------------------------
    //         pen = QPen(QColor(120, 60, 60))
    pen = QPen(sinColor);
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    //         qp.setBrush(QBrush(Qt.black))

    if (!data->isTimePaused)
    {
        // Copy height of each point to the point on its left (older point)
        for (int i = NUM_ORDINATES-2; i >= 0; i--)
        {
            sinOrdinates[i+1] = sinOrdinates[i];
        }
    }
    //print("Using current height of " + str(self.current_height))
    sinOrdinates[0] = -data->curHeight;            // set height of "current" (rightmost) sample

    for (int i = NUM_ORDINATES-2; i >= 0; i--)
    {
        p->drawLine(width() -        data->xAxisOffFromRight - i*data->timeXInc,
                    sinOrdinates[i] +   data->xAxisOffFromTop,
                    width() -        data->xAxisOffFromRight - (i+1)*data->timeXInc,
                    sinOrdinates[i+1] + data->xAxisOffFromTop
        );
    }


    //--------------------------------------------------------------------
    // Draw cosine points
    //--------------------------------------------------------------------
    pen = QPen(cosColor);
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    //         qp.setBrush(QBrush(Qt.black))

    if (!data->isTimePaused)
    {
        // Copy height of each point to the point on its left (older point)
        for (int i = NUM_ORDINATES-2; i >= 0; i--)
        {
            cosOrdinates[i+1] = cosOrdinates[i];
        }
    }
    //print("Using current height of " + str(self.current_height))
    cosOrdinates[0] = data->curWidth;            // set width of "current" (rightmost) sample

    if (data->showCosOnYAxis)
    {
        for (int i = NUM_ORDINATES-2; i >= 0; i--)
        {
            p->drawLine(
                        vector_origin_x + cosOrdinates[i],
                        vector_origin_y - data->amplitude - 20 - i*data->timeXInc,

                        vector_origin_x + cosOrdinates[i+1],
                        vector_origin_y - data->amplitude - 20 - (i+1)*data->timeXInc
            );
        }
    }

    if (data->showCosOnXAxis)
    {
        pen.setColor(cosColor);
        p->setPen(pen);
        for (int i = NUM_ORDINATES-2; i >= 0; i--)
        {
            p->drawLine(width() - data->xAxisOffFromRight - i*data->timeXInc,
                        - cosOrdinates[i]   + data->xAxisOffFromTop,
                        width() - data->xAxisOffFromRight - (i+1)*data->timeXInc,
                        - cosOrdinates[i+1] + data->xAxisOffFromTop
            );
        }
    }

}
