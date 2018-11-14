#include "renderwidget.h"
#include <QPen>
#include <QPainter>
#include <QRandomGenerator>
#include "mainwindow.h"

RenderWidget::RenderWidget(QWidget *parent, MainWindow *data) :
    QWidget(parent),
    timer(new QTimer(this)),

    xAxisOrdinates(AxisOrdinates(NUM_ORDINATES)),
    yAxisOrdinates(AxisOrdinates(NUM_ORDINATES)),

    vtScrollingBackground(NUM_BACKGROUND_TEXT_POINTS/2, NUM_TIME_TEXT_POINTS/2, NUM_ANGLE_TEXT_POINTS/2),
    hzScrollingBackground(NUM_BACKGROUND_TEXT_POINTS, NUM_TIME_TEXT_POINTS, NUM_ANGLE_TEXT_POINTS)
{
    this->data = data;

    connect(timer, SIGNAL(timeout()), this, SLOT(renderTimerEvent()));

    timer->start(data->timerInterval);

    //----------------------------------------------------------------
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
    xAxisOrdinates.clear();
}

void RenderWidget::clearCosOrdinates()
{
    yAxisOrdinates.clear();
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

    v.vector_origin_x = width() - data->xAxisOffFromRight + wallSeparation + data->amplitude;
    v.vector_origin_y = data->xAxisOffFromTop;

    v.vector_width  = int(data->amplitude * cos(data->curAngleInRadians));
    v.vector_height = int(data->amplitude * sin(data->curAngleInRadians));

    v.vector_tip_x = v.vector_origin_x + v.vector_width;
    v.vector_tip_y = v.vector_origin_y - v.vector_height;

    drawAxis(p, v);
    drawProjectionBoxes(p, v);

    drawBackground(p, v);
    drawSineAndCosinePoints(p, v);

    drawRotatingVectorComponents(p, v);
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
        p->drawRect(v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth/2,
                    v.vector_origin_y - data->amplitude,
                    data->penWidth,
                    data->amplitude*2);
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

        font.setPixelSize(20);
        p->setFont(font);
        p->drawText(v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth - 25,
                    v.vector_origin_y - data->amplitude - 5,
                    "+1");
        p->drawText(v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth - 18,
                    v.vector_origin_y + data->amplitude + 20,
                    "-1");

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
        p->drawRect(v.vector_origin_x - data->amplitude,
                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth/2,
                    data->amplitude*2,
                    data->penWidth);

        p->setOpacity(1);

//        str = "H o r i z o n t a l   p r o j e c t i o n";
//        w = fm.width(str);

//        p->drawText(v.vector_origin_x - w / 2,
//                    v.vector_origin_y - data->amplitude - 4,
//                    str);
        p->drawText(v.vector_origin_x - data->amplitude - 23,
                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth,
                    "-1");
        p->drawText(v.vector_origin_x + data->amplitude + 3,
                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth,
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
                    v.vector_origin_y - data->amplitude - wallSeparation,
                    v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth/2,
                    2 * (data->amplitude + wallSeparation));


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
        p->drawRect(v.vector_origin_x - data->amplitude - wallSeparation,
                    0,
                    2 * (data->amplitude + wallSeparation),
                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth/2);

        p->setOpacity(1);
    }
}

void RenderWidget::drawRotatingVectorComponents(QPainter *p, VectorDrawingCoordinates v)
{
    if (data->drawSinComponent)
    {
        QPen pen = QPen(sinColor);
        pen.setWidth(data->penWidth);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(sinColor);
        p->setOpacity(sinCosOpacity);
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    v.vector_tip_x,
                    v.vector_origin_y);
        p->setOpacity(1);
    }

    if (data->drawCosComponent)
    {
        QPen pen = QPen(cosColor);
        pen.setWidth(data->penWidth);
        pen.setCapStyle(Qt::RoundCap);
        p->setPen(pen);
        p->setBrush(cosColor);
        p->setOpacity(sinCosOpacity);
        p->drawLine(v.vector_origin_x,
                    v.vector_origin_y,
                    v.vector_tip_x,
                    v.vector_origin_y);
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

            QFont font = QFont();
            font.setPixelSize(40);
            p->setFont(font);
            p->setOpacity(0.7);

            QFontMetrics fm(font);
            QString angleString = QString::number(int(round(data->curAngleInDegrees)));
            int w = fm.width(angleString);

            p->drawText(v.vector_origin_x - w/2,
                        v.vector_origin_y + data->amplitude + 50,
                        angleString);
        }
    }

}

void RenderWidget::drawVectorShadow(QPainter *p, VectorDrawingCoordinates v)
{
    QPen pen = QPen();
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);
    p->setOpacity(0.7);

    //--------------------------------------------------------------------
    // Draw vector shadow, if enabled
    //--------------------------------------------------------------------
    if (data->drawVerticalShadow)
    {
        //--------------------------------------------------------------------
        // Vertical shadow
        //--------------------------------------------------------------------
        pen.setColor(sinColor);
        p->setPen(pen);
        p->setBrush(sinColor);
        p->drawRect(width() - data->xAxisOffFromRight - data->penWidth/2,
                    data->xAxisOffFromTop - data->curHeight,
                    data->penWidth,
                    data->curHeight);
        p->setOpacity(1);
    }

    if (data->drawHorizontalShadow)
    {
        //--------------------------------------------------------------------
        // Horizontal projection
        //--------------------------------------------------------------------
        pen.setColor(cosColor);
        p->setPen(pen);
        p->setBrush(cosColor);
        p->drawRect(v.vector_origin_x,
                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth/2,
                    data->curWidth,
                    data->penWidth);
        p->setOpacity(1);
    }

    //--------------------------------------------------------------------
    // Dotted line showing projection
    //--------------------------------------------------------------------
    pen = QPen(QColor(20, 20, 20));
    pen.setWidth(5);
    pen.setStyle(Qt::DotLine);
    p->setPen(pen);
    p->setOpacity(0.1);

    if (data->drawVerticalProjectionDottedLine)
    {
        // Draw vector tip vertical projection
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    width() - data->xAxisOffFromRight,
                    data->xAxisOffFromTop - v.vector_height
        );
    }

    if (data->drawHorizontalProjectionDottedLine)
    {
        // Draw vector tip horizontal projection
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    v.vector_tip_x,
                    v.vector_origin_y - data->amplitude - wallSeparation
        );
    }
    p->setOpacity(1);
}

void RenderWidget::drawSineAndCosinePoints(QPainter *p, VectorDrawingCoordinates v)
{
    //--------------------------------------------------------------------
    // Draw sine points
    //--------------------------------------------------------------------
    QPen pen = QPen(sinColor);
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->setOpacity(sinCosOpacity);
    if (data->showSinOnXAxis)
    {
        if (!data->isTimePaused)
        {
            // Copy value of each point to the previous (older) point.
            xAxisOrdinates.shift(data->curHeight);
        }
        //print("Using current height of " + str(self.current_height))

        // Draw straight lines between consecutive ordinate values.
        xAxisOrdinates.drawLines(p,
                                 RIGHT_TO_LEFT,
                                 v.vector_origin_x - data->amplitude - wallSeparation,
                                 v.vector_origin_y,
                                 data->timeXInc);
    }

    //--------------------------------------------------------------------
    // Draw cosine points
    //--------------------------------------------------------------------
    pen = QPen(cosColor);
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->setOpacity(sinCosOpacity);
    if (data->showCosOnYAxis)
    {
        if (!data->isTimePaused)
        {
            // Copy height of each point to the point on its left (older point)
            yAxisOrdinates.shift(data->curWidth);
        }

        //print("Using current height of " + str(self.current_height))
        yAxisOrdinates.drawLines(p,
                                 BOTTOM_TO_TOP,
                                 v.vector_origin_x,
                                 v.vector_origin_y - data->amplitude - wallSeparation,
                                 data->timeXInc);
    }

    //--------------------------------------------------------------------
    // Draw cosine on X axis along with sine so as to compare the 90 degree phase shift.
    if (data->showCosOnXAxis)
    {
        pen.setColor(cosColor);
        p->setPen(pen);
        p->setOpacity(sinCosOpacity);
        yAxisOrdinates.drawLines(p,
                                 RIGHT_TO_LEFT,
                                 v.vector_origin_x - data->amplitude - wallSeparation,
                                 v.vector_origin_y,
                                 data->timeXInc);
    }

    p->setOpacity(1);
}


void RenderWidget::drawTipCircles(QPainter *p, VectorDrawingCoordinates v)
{
    QPen pen = QPen(vectorTipCircleColor);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->setBrush(Qt::white);
    p->setOpacity(1);

    if (data->drawRotatingVector)
    {
        // Draw a circle at the tip of the vector
        p->drawEllipse(v.vector_tip_x - data->penWidth / 2,
                       v.vector_tip_y - data->penWidth / 2,
                       data->penWidth,
                       data->penWidth
        );
    }
    if (data->drawVerticalProjectionTipCircle)
    {
        // Draw a circle at the tip of the vector's vertical projection
        p->drawEllipse(width() - data->xAxisOffFromRight - data->penWidth/2,
                       v.vector_tip_y - data->penWidth/2,
                       data->penWidth,
                       data->penWidth
        );
    }
    if (data->drawHorizontalProjectionTipCircle)
    {
        // Draw a circle at the tip of the vector's vertical projection
        p->drawEllipse(v.vector_tip_x - data->penWidth/2,
                       v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth/2,
                       data->penWidth,
                       data->penWidth
        );

        if (data->showCosOnXAxis)
        {
            p->drawEllipse(v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth/2,
                           v.vector_origin_y - (v.vector_tip_x - v.vector_origin_x) - data->penWidth/2,
                           data->penWidth,
                           data->penWidth
            );
        }
    }

    p->setOpacity(1);
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
                    v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth/2,
                    v.vector_origin_y);
    }

    if (data->showCosOnYAxis)
    {
        p->drawLine(v.vector_origin_x,
                    0,
                    v.vector_origin_x,
                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth/2);
    }
}

