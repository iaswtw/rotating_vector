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
    clearSinOrdinates();
    clearCosOrdinates();


    QRandomGenerator randomGen(10);
    hzScrollingBackground.generateRandomAnglePoints(      -200, 1700, 0, 800, randomGen);
    hzScrollingBackground.generateRandomBackgroundPoints( -200, 1900, 0, 800, randomGen);
    hzScrollingBackground.generateRandomTimePoints(       -200, 1700, 0, 800, randomGen);

    vtScrollingBackground.generateRandomAnglePoints(      0, 500, 0, 800, randomGen);
    vtScrollingBackground.generateRandomBackgroundPoints( 0, 500, 0, 800, randomGen);
    vtScrollingBackground.generateRandomTimePoints(       0, 500, 0, 800, randomGen);
}

void RenderWidget::clearSinOrdinates()
{
    for (int i = 0; i < NUM_ORDINATES; i++)
    {
        sinOrdinates[i] = 0;
    }
}

void RenderWidget::clearCosOrdinates()
{
    for (int i = 0; i < NUM_ORDINATES; i++)
    {
        cosOrdinates[i] = 0;
    }
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

    VectorDrawingCoordinates v;

    v.vector_origin_x = width() - data->xAxisOffFromRight + 20 + data->amplitude;
    v.vector_origin_y = data->xAxisOffFromTop;

    v.vector_width  = int(data->amplitude * cos(data->curAngleInRadians));
    v.vector_height = int(data->amplitude * sin(data->curAngleInRadians));

    v.vector_tip_x = v.vector_origin_x + v.vector_width;
    v.vector_tip_y = v.vector_origin_y - v.vector_height;

    drawAxis(p, v);
    drawProjectionBoxes(p, v);

    drawBackground(p, v);
    drawSineAndCosinePoints(p, v);

    drawRotatingVector(p, v);
    drawVectorShadow(p, v);

    // For the projection tip circle to show correctly, draw this after drawing sin & cos points and vector projection dotted line.
    drawTipCircles(p, v);
}


void RenderWidget::drawProjectionBoxes(QPainter *p, VectorDrawingCoordinates v)
{
    //-------------------------------------------------------------------
    // Draw rectangle that will have the vector's vertical shadow
    //-------------------------------------------------------------------
    QFont font = QFont();
    font.setPixelSize(13);
//    font.setStretch(2.0);
    p->setFont(font);
    QFontMetrics fm(font);
    QString str;
    QPen pen;

    if (data->showVerticalProjectionBox)
    {
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

        //---------------------------------------------------------
//        int w;
//        str = "V e r t i c a l   p r o j e c t i o n";
//        w = fm.width(str);

//        p->save();
//        p->translate(v.vector_origin_x - data->amplitude - 4,
//                     v.vector_origin_y + w / 2);
//        p->rotate(-90);
//        p->drawText(0, 0, str);
//        p->restore();

        p->drawText(v.vector_origin_x - data->amplitude - data->penWidth - 20 - 15,
                    v.vector_origin_y + data->amplitude,
                    "-1");
        p->drawText(v.vector_origin_x - data->amplitude - data->penWidth - 20 - 15,
                    v.vector_origin_y - data->amplitude,
                    "+1");

    }

    //-------------------------------------------------------------------
    // Draw rectangle that will have the vector's horizontal projection (not shadow. no vector casting horizontal shadow)
    //-------------------------------------------------------------------
    pen.setColor(QColor(150,150,150));
    p->setPen(pen);

    if (data->showHorizontalProjectionBox)
    {
        pen = QPen(QColor(100, 100, 100));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setOpacity(0.2);
        p->drawRect(v.vector_origin_x - data->amplitude - 2,
                    v.vector_origin_y - data->amplitude - 20 - data->penWidth/2 - 2,
                    data->amplitude*2 + data->penWidth + 2,
                    data->penWidth + 2);

        p->setOpacity(1);

//        str = "H o r i z o n t a l   p r o j e c t i o n";
//        w = fm.width(str);

//        p->drawText(v.vector_origin_x - w / 2,
//                    v.vector_origin_y - data->amplitude - 4,
//                    str);
        p->drawText(v.vector_origin_x - data->amplitude,
                    v.vector_origin_y - data->amplitude - data->penWidth - 20,
                    "-1");
        p->drawText(v.vector_origin_x + data->amplitude,
                    v.vector_origin_y - data->amplitude - data->penWidth - 20,
                    "+1");
    }

}

void RenderWidget::drawBackground(QPainter *p, VectorDrawingCoordinates v)
{
    //--------------------------------------------------------------------
    // Draw Horizontal background
    //--------------------------------------------------------------------
    QFont font = QFont();
    font.setPixelSize(30);
    QPen pen = QPen(scrollingBackgroundColor);
    p->setPen(pen);
    p->setBrush(Qt::green);
    p->setFont(font);

    if (data->showSinOnXAxis)
    {
        p->setOpacity(0.2);

        hzScrollingBackground.draw(*p,
                                   0,                                               // x offset
                                   v.vector_origin_y - data->amplitude,               // y offset
                                   v.vector_origin_x - data->amplitude,               // width of rectangle to draw in
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
                    v.vector_origin_y - data->amplitude - 20,
                    v.vector_origin_x - data->amplitude - 30,
                    data->amplitude * 2 + 40);


        p->setOpacity(1);
    }

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
                                   v.vector_origin_x - data->amplitude,               // x offset
                                   0,                                               // y offset
                                   data->amplitude * 2,                             // width of rectangle to draw in
                                   v.vector_origin_y - data->amplitude                // height of rectangle to draw in
        );

        if (!data->isTimePaused)
        {
            vtScrollingBackground.shiftUp(data->timeXInc);
        }

        pen = QPen(cosColor);
        p->setPen(pen);
        p->setBrush(cosColor);
        p->setOpacity(0.05);
        p->drawRect(v.vector_origin_x - data->amplitude - 20,
                    0,
                    data->amplitude * 2 + 40,
                    v.vector_origin_y - data->amplitude - 30);

        p->setOpacity(1);
    }
}

void RenderWidget::drawRotatingVector(QPainter *p, VectorDrawingCoordinates v)
{
    //--------------------------------------------------------------------
    // Draw rotating vector, if enabled
    //--------------------------------------------------------------------
    if (data->drawRotatingVector)
    {
        //----------------------------------------------------
        // Draw vector axis and tracing circle
        QPen pen = QPen(QColor(120, 120, 120));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(vectorSweepColor);
        p->setOpacity(0.1);
        p->drawEllipse(v.vector_origin_x - data->amplitude,
                       v.vector_origin_y - data->amplitude,
                       2 * data->amplitude,
                       2 * data->amplitude
        );

        p->setOpacity(0.3);
        // vector's x axis
        p->drawLine(v.vector_origin_x - data->amplitude - 10,
                    v.vector_origin_y,
                    v.vector_origin_x + data->amplitude + 10,
                    v.vector_origin_y
        );
        // vector's y axis
        p->drawLine(v.vector_origin_x,
                    v.vector_origin_y - data->amplitude - 10,
                    v.vector_origin_x,
                    v.vector_origin_y + data->amplitude + 10
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
        p->drawLine(v.vector_origin_x,
                    v.vector_origin_y,
                    v.vector_tip_x,
                    v.vector_tip_y
        );
        p->setOpacity(1);

        //----------------------------------------------------
        // Draw arc showing the region from 0 degrees that forms the current angle.
        if (data->drawAngleArc)
        {
            pen = QPen(Qt::black);
            pen.setWidth(2);
            pen.setCapStyle(Qt::RoundCap);
            p->setPen(pen);
            p->setBrush(vectorColor);
            p->setOpacity(0.3);
            p->drawArc(v.vector_origin_x - 30,
                       v.vector_origin_y - 30,
                       60,
                       60,
                       0 * 16,
                       int(data->curAngleInDegrees * 16)
            );
        }
    }

}

void RenderWidget::drawAxis(QPainter *p, VectorDrawingCoordinates v)
{
    //-------------------------------------------------------------------
    // Draw X & Y axis
    //--------------------------------------------------------------------
    QPen pen = QPen(QColor(120, 120, 120));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);

    if (data->showSinOnXAxis)
    {
        p->drawLine(0,
                    v.vector_origin_y,
                    width() - data->xAxisOffFromRight,
                    v.vector_origin_y);
    }

    if (data->showCosOnYAxis)
    {
        p->drawLine(v.vector_origin_x,
                    0,
                    v.vector_origin_x,
                    v.vector_origin_y - data->amplitude - 20 - data->penWidth/2);
    }
}

void RenderWidget::drawVectorShadow(QPainter *p, VectorDrawingCoordinates v)
{
    //--------------------------------------------------------------------
    // Draw vector shadow, if enabled
    //--------------------------------------------------------------------
    if (data->drawVerticalShadow)
    {
        //--------------------------------------------------------------------
        // Vertical shadow
        //--------------------------------------------------------------------
        QPen pen = QPen(sinColor);
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
    }

    if (data->drawVerticalProjectionDottedLine)
    {
        //----------------------------------------------------
        // Draw vector tip vertical projection
        QPen pen = QPen(QColor(20, 20, 20));
        pen.setWidth(data->penWidth);
        pen.setStyle(Qt::DotLine);
        p->setPen(pen);
        p->setOpacity(0.2);
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    width() - data->xAxisOffFromRight,
                    data->xAxisOffFromTop - v.vector_height
        );
        p->setOpacity(1);
    }

    if (data->drawHorizontalShadow)
    {
        //--------------------------------------------------------------------
        // Horizontal projection
        //--------------------------------------------------------------------
        QPen pen = QPen(cosColor);
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(cosColor);
        p->setOpacity(1);
        p->drawRect(v.vector_origin_x,
                    v.vector_origin_y - data->amplitude - 20 - data->penWidth/2,
                    data->curWidth,
                    data->penWidth);
        p->setOpacity(1);
    }

    if (data->drawHorizontalProjectionDottedLine)
    {
        //----------------------------------------------------
        // Draw vector tip horizontal projection
        QPen pen = QPen(QColor(20, 20, 20));
        pen.setWidth(data->penWidth);
        pen.setStyle(Qt::DotLine);
        p->setPen(pen);
        p->setOpacity(0.2);
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    v.vector_tip_x,
                    v.vector_origin_y - data->amplitude - 20
        );
        p->setOpacity(1);
    }
}

void RenderWidget::drawSineAndCosinePoints(QPainter *p, VectorDrawingCoordinates v)
{
    //--------------------------------------------------------------------
    // Draw sine points
    //--------------------------------------------------------------------
    //         pen = QPen(QColor(120, 60, 60))
    QPen pen = QPen(sinColor);
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    //         qp.setBrush(QBrush(Qt.black))

    if (data->showSinOnXAxis)
    {
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
    }

    //--------------------------------------------------------------------
    // Draw cosine points
    //--------------------------------------------------------------------
    pen = QPen(cosColor);
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    //         qp.setBrush(QBrush(Qt.black))

    if (data->showCosOnYAxis)
    {
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

        for (int i = NUM_ORDINATES-2; i >= 0; i--)
        {
            p->drawLine(
                        v.vector_origin_x + cosOrdinates[i],
                        v.vector_origin_y - data->amplitude - 20 - i*data->timeXInc,

                        v.vector_origin_x + cosOrdinates[i+1],
                        v.vector_origin_y - data->amplitude - 20 - (i+1)*data->timeXInc
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


void RenderWidget::drawTipCircles(QPainter *p, VectorDrawingCoordinates v)
{
    if (data->drawRotatingVector)
    {
        // Draw a circle at the tip of the vector
        QPen pen = QPen(vectorTipCircleColor);
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(Qt::white);
        p->setOpacity(1);
        p->drawEllipse(v.vector_tip_x - data->penWidth / 2,
                       v.vector_tip_y - data->penWidth / 2,
                       data->penWidth,
                       data->penWidth
        );
        p->setOpacity(1);
    }
    if (data->drawVerticalProjectionTipCircle)
    {
        // Draw a circle at the tip of the vector's vertical projection
        QPen pen = QPen(vectorTipCircleColor);
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(Qt::white);
        p->setOpacity(1);
        p->drawEllipse(width() - data->xAxisOffFromRight - data->penWidth/2,
                       v.vector_tip_y - data->penWidth/2,
                       data->penWidth,
                       data->penWidth
        );
        p->setOpacity(1);
    }
    if (data->drawHorizontalProjectionTipCircle)
    {
        // Draw a circle at the tip of the vector's vertical projection
        QPen pen = QPen(vectorTipCircleColor);
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(Qt::white);
        p->setOpacity(1);
        p->drawEllipse(v.vector_tip_x - data->penWidth/2,
                       v.vector_origin_y - data->amplitude - 20 - data->penWidth/2,
                       data->penWidth,
                       data->penWidth
        );
        p->setOpacity(1);
    }
}
