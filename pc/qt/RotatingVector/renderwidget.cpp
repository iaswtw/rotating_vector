#include "renderwidget.h"
#include <QPen>
#include <QPainter>
#include <QRandomGenerator>
#include "mainwindow.h"
#include <QDir>
#include <QImage>

RenderWidget::RenderWidget(QWidget *parent, MainWindow *data) :
    QWidget(parent)
{
    this->data = data;

    yProjection.setPhase(data->phaseShiftFromSine);

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

void RenderWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    recalculateVectorOrigin();
}

/*
 * E.g. of when recalculation of vector position is required: when its offset from the bottom or right changes, or its amplitude changes.
 */
void RenderWidget::recalculateVectorOrigin()
{
    vectorOrigin.setX(width() - data->amplitude - 130 - data->extraVectorOffsetFromRight);
    vectorOrigin.setY(height() - data->amplitude - 130 - data->extraVectorOffsetFromBottom);

    notifyPositionChange();
}

void RenderWidget::notifyPositionChange()
{
    xProjection.recalculatePosition(vectorOrigin.x(), vectorOrigin.y(), data->amplitude, wallSeparation);
    yProjection.recalculatePosition(vectorOrigin.x(), vectorOrigin.y(), data->amplitude, wallSeparation);
}

void RenderWidget::updatePhaseShiftFromSine()
{
    yProjection.setPhase(data->phaseShiftFromSine);
}

void RenderWidget::lowPassFilterAngleDifference(double newAngleInDegrees)
{
    double difference = fabs(newAngleInDegrees - previousAngleInDegrees);

    // This method is called everytime GUI update is performed, which could be several times between
    // consecutive angle updates. Hence alpha has to be small.
    // E.g. if alpha = 0.5, smoothed angle could get close to 0 if vector speed is slow (vector updates come every 100s or ms),
    //      and GUI update is fast (less delay between GUI timer firing... 10s of ms).
    // This will cause angle to be not displayed at 0, 90, 180 & 270 due to low smoothed angle threshold.
    double alpha = 0.1;
    if (difference > 2)
    {
        // Ignore this difference. Must be due to 0 -> 360 rollover.
    }
    else
    {
        smoothedChangeInAngle = (smoothedChangeInAngle * (1-alpha)) + (difference * alpha);
    }
    //printf("Calculated smoothed angle diff.  difference = %lf\n", difference);
}

void RenderWidget::clearSinOrdinates()
{
    xProjection.clear();
}

void RenderWidget::clearCosOrdinates()
{
    yProjection.clear();
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

    data->renderWidgetPaintEvent();

    QPainter p(this);

    draw(&p);

}


void RenderWidget::draw(QPainter * p)
{
    //----------------------------------------------------------------------------------------------------------
    // Decide if vector (arduino or simulator) is rotating. Use low pass filter on angle difference.
    // if it is rotating, and if current angle is 0, 90, 180 and 270, it will be applied on current ordinate.
    //----------------------------------------------------------------------------------------------------------
    // if time is not paused, we are feeding current angle to sine/cosine plot. LPF to get smoothed angle diff.
    // if time paused, sine/cosine is frozen. smoothed angle diff can stay where it is.
    if (!data->isTimePaused)
    {
        lowPassFilterAngleDifference(data->curAngleInDegrees);
    }
    previousAngleInDegrees = data->curAngleInDegrees;
    isVectorOrArduinoRunning = smoothedChangeInAngle > 0.2;
    //----------------------------------------------------------------------------------------------------------

    QFont font;

    VectorDrawingCoordinates v;

    // Automatically set vector origin coords baseds on window width and height.  Then add offsets to it.
    v.vector_origin_x = vectorOrigin.x();
    v.vector_origin_y = vectorOrigin.y();

    v.vector_width  = int(data->amplitude * cos(data->curAngleInRadians));
    v.vector_height = int(data->amplitude * sin(data->curAngleInRadians));

    v.vector_tip_x = v.vector_origin_x + v.vector_width;
    v.vector_tip_y = v.vector_origin_y - v.vector_height;

    v.xaxis_x = v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth/2;
    v.xaxis_y = v.vector_origin_y;

    v.yaxis_x = v.vector_origin_x;
    v.yaxis_y = v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth/2;

    v.vproj_origin_x = v.vector_origin_x - data->amplitude - wallSeparation;
    v.vproj_origin_y = v.vector_origin_y;

    v.hproj_origin_x = v.vector_origin_x;
    v.hproj_origin_y = v.vector_origin_y - data->amplitude - wallSeparation;


    drawAxis(p);
    drawProjectionBoxes(p);

    drawBackground(p, v);
    drawSineAndCosinePoints(p);
    drawLinesAtImportantOrdinateValues(p);

    drawRotatingVectorComponents(p, v);
    drawRotatingVector(p, v);
    drawVectorProjection(p, v);

    // For the projection tip circle to show correctly, draw this after drawing sin & cos points and vector projection dotted line.
    drawTipCircles(p, v);

    drawObservers(p);
}


void RenderWidget::drawProjectionBoxes(QPainter *p)
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

        xProjection.drawVectorProjectionBoxes(p, data->penWidth);

        p->setOpacity(1);

        //---------------------------------------------------------
//        font.setPixelSize(20);
//        p->setFont(font);
//        p->drawText(v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth - 25,
//                    v.vector_origin_y - data->amplitude - 5,
//                    "+1");
//        p->drawText(v.vector_origin_x - data->amplitude - wallSeparation - data->penWidth - 18,
//                    v.vector_origin_y + data->amplitude + 20,
//                    "-1");

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

        yProjection.drawVectorProjectionBoxes(p, data->penWidth);

        p->setOpacity(1);

        //---------------------------------------------------------
//        p->drawText(v.vector_origin_x - data->amplitude - 23,
//                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth,
//                    "-1");
//        p->drawText(v.vector_origin_x + data->amplitude + 3,
//                    v.vector_origin_y - data->amplitude - wallSeparation - data->penWidth,
//                    "+1");
    }

}

void RenderWidget::drawBackground(QPainter *p, VectorDrawingCoordinates v)
{
    if (data->showScrollingBackgroundText)
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
                                       v.vector_origin_y - data->amplitude,             // y offset
                                       v.vector_origin_x - data->amplitude,             // width of rectangle to draw in
                                       data->amplitude * 2                              // height of rectangle to draw in
            );

            if (!data->isTimePaused)
                hzScrollingBackground.shiftLeft(data->timeXInc);

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
            pen = QPen(scrollingBackgroundColor);
            p->setPen(pen);
            p->setOpacity(0.2);

            vtScrollingBackground.draw(*p,
                                       v.vector_origin_x - data->amplitude,               // x offset
                                       0,                                                 // y offset
                                       data->amplitude * 2,                               // width of rectangle to draw in
                                       v.vector_origin_y - data->amplitude                // height of rectangle to draw in
            );

            if (!data->isTimePaused)
                vtScrollingBackground.shiftUp(data->timeXInc);

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
}

void RenderWidget::drawRotatingVectorComponents(QPainter *p, VectorDrawingCoordinates v)
{
    QPen pen = QPen();
    pen.setWidth(data->penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setOpacity(sinCosOpacity);

    if (data->drawSinComponent)
    {
        pen.setColor(sinColor);
        p->setPen(pen);
        p->setBrush(sinColor);
        // Drop a line from vector tip perpendicular to X axis
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    v.vector_tip_x,
                    v.vector_origin_y);
        // Draw projection on Y Axis
        p->drawLine(v.vector_origin_x,
                    v.vector_tip_y,
                    v.vector_origin_x,
                    v.vector_origin_y);
    }

    if (data->drawCosComponent)
    {
        pen.setColor(cosColor);
        p->setPen(pen);
        p->setBrush(cosColor);
        // Draw a line from vector tip perpendicular to Y axis
        p->drawLine(v.vector_tip_x,
                    v.vector_tip_y,
                    v.vector_origin_x,
                    v.vector_tip_y);
        // Draw projection on X axis
        p->drawLine(v.vector_tip_x,
                    v.vector_origin_y,
                    v.vector_origin_x,
                    v.vector_origin_y);
    }
    p->setOpacity(1);
}


void RenderWidget::drawRotatingVector(QPainter *p, VectorDrawingCoordinates v)
{
    //--------------------------------------------------------------------
    // Draw rotating vector, if enabled
    //--------------------------------------------------------------------
    if (data->drawRotatingVector)
    {
        //----------------------------------------------------
        // Draw vector x & y axis and tracing circle
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

            int arcRadius = data->amplitude / 10;
            p->drawArc(v.vector_origin_x - arcRadius,
                       v.vector_origin_y - arcRadius,
                       2 * arcRadius,
                       2 * arcRadius,
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

void RenderWidget::drawVectorProjection(QPainter *p, VectorDrawingCoordinates v)
{
    QPen pen = QPen();
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);
    p->setOpacity(0.7);

    //--------------------------------------------------------------------
    // VerticalpProjection
    //--------------------------------------------------------------------
    if (data->drawVerticalShadow)
    {
        pen.setColor(sinColor);
        p->setPen(pen);
        p->setBrush(sinColor);
        xProjection.drawVectorProjection(p, data->curAngleInDegrees, data->penWidth);
    }

    //--------------------------------------------------------------------
    // Horizontal projection
    //--------------------------------------------------------------------
    if (data->drawHorizontalShadow)
    {
        pen.setColor(cosColor);
        p->setPen(pen);
        p->setBrush(cosColor);
        yProjection.drawVectorProjection(p, data->curAngleInDegrees, data->penWidth);
    }
    p->setOpacity(1);

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
        // For vertical projection: from wall to vector tip.
        xProjection.drawDottedLineFromVectorTip(p, data->curAngleInDegrees, v.vector_tip_x, v.vector_tip_y);
    }

    if (data->drawHorizontalProjectionDottedLine)
    {
        // For horizontal projection: from ceiling to vector tip.
        yProjection.drawDottedLineFromVectorTip(p, data->curAngleInDegrees, v.vector_tip_x, v.vector_tip_y);
    }
    p->setOpacity(1);
}

void RenderWidget::drawSineAndCosinePoints(QPainter *p)
{
//    printf("smoothedChangeInAngle = %lf\n", smoothedChangeInAngle);
    if (!data->isTimePaused)
    {
        // Feed all projection axis
        xProjection.shift(data->amplitude,
                          data->curAngleInDegrees,
                          isVectorOrArduinoRunning,
                          !data->arduinoSimulator->isCounterClockwise);

        yProjection.shift(data->amplitude,
                          data->curAngleInDegrees,
                          isVectorOrArduinoRunning,
                          !data->arduinoSimulator->isCounterClockwise);
    }

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
        xProjection.drawWave(p, data->timeXInc);
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
        yProjection.drawWave(p, data->timeXInc);
        if (data->showAnglesOnXAndYAxis)
            yProjection.drawAngles(p, data->timeXInc, data->show30And60Angles);
    }

    //--------------------------------------------------------------------
    // Draw cosine on X axis along with sine so as to compare the 90 degree phase shift.
    if (data->showCosOnXAxis)
    {
        pen.setColor(cosColor);
        p->setPen(pen);
        p->setOpacity(sinCosOpacity);
        yProjection.drawWaveWithoutRotation(p, data->timeXInc);
    }

    // Draw angles after potentially drawing Cosine on X axis. This will ensure the marks and angles are on top of sine & cosine lines.
    if (data->showSinOnXAxis)
    {
        if (data->showAnglesOnXAndYAxis)
            xProjection.drawAngles(p, data->timeXInc, data->show30And60Angles);
    }
    p->setOpacity(1);
}


void RenderWidget::drawLinesAtImportantOrdinateValues (QPainter *p)
{
    QPen pen = QPen(QColor(50, 50, 50));
    pen.setWidth(2);
    p->setPen(pen);

    QFont font;
    font.setPixelSize(15);
    p->setFont(font);

    if (data->showSinOnXAxis && data->showAnglesOnXAndYAxis)
        xProjection.drawLinesAtImportantCoordinates(p, true, true, true, true);

    if (data->showCosOnYAxis && data->showAnglesOnXAndYAxis)
        yProjection.drawLinesAtImportantCoordinates(p, true, true, true, true);
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
                       data->penWidth);
    }
    if (data->drawVerticalProjectionTipCircle)
        xProjection.drawTipCircle(p, data->curAngleInDegrees, data->penWidth);      // Draw a circle at the tip of the vector's vertical projection

    if (data->drawHorizontalProjectionTipCircle && data->drawHorizontalShadow)
        yProjection.drawTipCircle(p, data->curAngleInDegrees, data->penWidth);      // Draw a circle at the tip of the vector's horizontal projection

    if (data->showCosOnXAxis && data->drawHorizontalProjectionTipCircle)
        yProjection.drawTipCircleWithoutRotation(p, data->curAngleInDegrees, data->penWidth);

    p->setOpacity(1);
}

void RenderWidget::drawAxis(QPainter *p)
{
    //-------------------------------------------------------------------
    // Draw X & Y axis
    //--------------------------------------------------------------------
    QPen pen = QPen(QColor(80, 80, 80));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->setOpacity(0.7);

    if (data->showSinOnXAxis)
        xProjection.drawAxis(p);

    if (data->showCosOnYAxis)
        yProjection.drawAxis(p);
}

void RenderWidget::drawObservers(QPainter *p)
{
    xProjection.drawObserver(p);
    yProjection.drawObserver(p);
}
