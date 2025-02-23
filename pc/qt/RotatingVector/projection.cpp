#include "renderwidget.h"


Projection::Projection(double phase_, int maxOrdinates_, QString observerFilename, QColor color_)
{
    phase = phase_;
    maxOrdinates = maxOrdinates_;
    ordinates = new int[maxOrdinates];
    angles = new int[maxOrdinates];
    observerImage = locateAndInstantiateImage(observerFilename);
    color = color_;
    clear();
}

void Projection::clear()
{
    for (int i=0; i<maxOrdinates; i++)
    {
        ordinates[i] = 0;
        angles[i] = INT_MIN;
    }
}

void Projection::setPhase(double phase_)
{
    phase = phase_;
    if (isPositionCalculated)
        recalculatePosition();
}

void Projection::recalculatePosition()
{
    axis_x = vector_origin_x - int((amplitude + wallSeparation) * cos(phase * M_PI / 180.0));
    axis_y = vector_origin_y - int((amplitude + wallSeparation) * sin(phase * M_PI / 180.0));

    unrotated_axis_x = vector_origin_x - int((amplitude + wallSeparation) * cos(0 * M_PI / 180.0));
    unrotated_axis_y = vector_origin_y - int((amplitude + wallSeparation) * sin(0 * M_PI / 180.0));

    isPositionCalculated = true;
}

QPoint Projection::getAxisPositionFromPhase(double phase_)
{
    int x = vector_origin_x - int((amplitude + wallSeparation) * cos(phase_ * M_PI / 180.0));
    int y = vector_origin_y - int((amplitude + wallSeparation) * sin(phase_ * M_PI / 180.0));

    return QPoint(x, y);
}

void Projection::recalculatePosition(int vector_origin_x, int vector_origin_y, int amplitude_, int wallSeparation)
{
    this->vector_origin_x = vector_origin_x;
    this->vector_origin_y = vector_origin_y;
    this->amplitude = amplitude_;
    this->wallSeparation = wallSeparation;

    recalculatePosition();
}

/*
 * Returns the vertical projection (height) from the point of view of current phase.
 */
int Projection::getCurrentHeight(int amplitude, double currentAngleInDegrees)
{
    int currentHeight = int(amplitude * sin((currentAngleInDegrees + phase) * M_PI / 180.0));
    return currentHeight;
}

/*
 * Returns the horizontal projection (depth) from the point of view of current phase.
 */
int Projection::getCurrentDepth(int amplitude, double currentAngleInDegrees)
{
    int currentDepth = int(amplitude * cos((currentAngleInDegrees + phase) * M_PI / 180.0));
    return currentDepth;
}

/*
 * Shift ordinate & angle values up by 1 index.  The last value (oldest) in the array will fall off.
 * Set the given 'amplitude' and 'currentAngleInDegrees' as the current (newest) value,
 * i.e. the value for ordinate & angle at index 0.
 */
void Projection::shiftAndSet(int amplitude, double currentAngleInDegrees, bool isVectorRunning, bool isClockwise)
{
   int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

   for (int i=maxOrdinates-2; i>=0; i--)
   {
       ordinates[i+1] = ordinates[i];
       angles[i+1] = angles[i];
   }
   ordinates[0] = currentHeight;
   angles[0] = INT_MIN;

   int intAngle = INT_MIN;
   if (isVectorRunning)
   {
       intAngle= int(round(currentAngleInDegrees));
       if (isClockwise)
           intAngle = (intAngle - 360) % 360;

       // don't set angle is not a multiple of 30 or 45.
       if (((intAngle % 30) != 0) && ((intAngle % 45) != 0))
           intAngle = INT_MIN;

       if (intAngle == 360)
           intAngle = 0;
    }
   // If a current angle is specified, set it as the angle for current ordinate only if
   // the previous two ordinates didn't have any angle set.
   if (intAngle != INT_MIN)
   {
       if ((angles[1] == INT_MIN) &&
           (angles[2] == INT_MIN) &&
           (angles[3] == INT_MIN) &&
           (angles[4] == INT_MIN) &&
           (angles[5] == INT_MIN))
       {
           angles[0] = intAngle;
       }
   }
}
// Draw straight lines between consecutive ordinate values.
void Projection::drawWave(QPainter *p, int abscissaScale, int penWidth)
{
   drawWave(p, abscissaScale, penWidth, phase);
}

void Projection::drawWave(QPainter *p, int abscissaScale, int penWidth, double phaseRotation)
{
   QPoint axis_pos = getAxisPositionFromPhase(phaseRotation);

   p->save();
   p->translate(axis_pos.x(), axis_pos.y());
   p->rotate(phaseRotation);

   QPen pen = QPen(color);
   pen.setWidth(penWidth);
   pen.setCapStyle(Qt::RoundCap);
   p->setPen(pen);
   p->setOpacity(waveOpacity);

   for (int i=0; i<maxOrdinates-1; i++)
   {
           p->drawLine(- i*abscissaScale,
                       - ordinates[i],
                       - (i+1)*abscissaScale,
                       - ordinates[i+1]);
   }
   p->restore();

}

// Draw angle marks & angle value for ordinates on which they are set.
// Also draw a line showing +1 and -1 limits.
void Projection::drawAngles(QPainter *p, int abscissaScale, bool showMultiplesOf30, bool showInRadians)
{
    // Draw angles if set
    QPen pen = QPen(QColor(50, 50, 50));
    pen.setWidth(2);
    p->setPen(pen);

    QFont font;
    int fontPixelSize = 20;
    font.setPixelSize(fontPixelSize);
    p->setFont(font);
    p->setOpacity(0.3);

    QFontMetrics fm(font);
    for (int i=0; i<maxOrdinates-1; i++)
    {
        if (angles[i] != INT_MIN)        // if a valid angle is set on this ordinate...
        {
            if (!showMultiplesOf30 && ((angles[i] % 90) != 0))
                continue;

            p->save();
            p->translate(axis_x, axis_y);
            p->rotate(phase);
            //--------------------------------------------------
            // draw tiny division on X axis
            p->setOpacity(0.3);
            p->drawLine(- i*abscissaScale,
                        - 1,
                        - i*abscissaScale,
                        + 1);

            // drop perpendicular from the ordinate value to x axis
            p->setOpacity(0.1);
            p->drawLine(- i*abscissaScale,
                        0,
                        - i*abscissaScale,
                        - ordinates[i]);
            //--------------------------------------------------
            // Draw longer division at 0 / 360 degress
            if ((angles[i] == 0) || (angles[i] == 360))
            {
                // draw a thin faint line from top to bottom
                QPen pen = QPen(QColor(200, 50, 50));
                pen.setWidth(2);
                p->setPen(pen);
                p->setOpacity(0.3);
                p->drawLine(- i*abscissaScale,
                            - amplitude - 10,
                            - i*abscissaScale,
                            + amplitude + 10);
            }
            p->restore();

            //--------------------------------------------------
            // Draw angle number. This does not use translated coordinate system.
            QString str;
            int w, h;       // width and height of the rendered angle. fractional angles in radians will cause increased height.
            int w1 = 0;

            if (showInRadians)
            {
                w1 = fm.horizontalAdvance("O");     // get width of 1 dummy character
                std::tuple<int, int> wh = _getRadianAngleDisplayWidthAndHeight(angles[i], w1, fontPixelSize);
                w = std::get<0>(wh);
                h = std::get<1>(wh);
            }
            else
            {
                str = QString::number(angles[i]);
                w = fm.horizontalAdvance(str);
                h = fontPixelSize;
            }


            int angle_str_x = axis_x - int((i*abscissaScale) * cos(phase * M_PI / 180.0));
            int angle_str_y = axis_y - int((i*abscissaScale) * sin(phase * M_PI / 180.0));

            int angle_str_x_correction = w/2 + int((w/2 + 10) * sin(phase * M_PI / 180.0));

            // Note - this correction factor doesn't vertically center the caption on Y axis.
            int angle_str_y_correction = int(20 * cos(phase * M_PI / 180.0))  -
                                         int(((fontPixelSize/2) * sin(phase * M_PI / 180.0)));

            if (showInRadians)
            {
                _drawRadianAngle(p,
                                 angle_str_x - angle_str_x_correction,
                                 angle_str_y + angle_str_y_correction,
                                 angles[i],
                                 w1);
            }
            else
            {
                p->drawText(angle_str_x - angle_str_x_correction,
                            angle_str_y + angle_str_y_correction,
                            str);
            }
        }
    }
}


std::tuple<int, int> Projection::_getRadianAngleDisplayWidthAndHeight(int angleInDegree, int w1, int h1)
{
    // w1 = single character width in current font
    // h1 = single character height in current font

    switch (angleInDegree)
    {
    case 0: return std::make_tuple(w1, h1);              // 0

    case 30: return std::make_tuple(w1, 2 * h1);             // 𝛑/6
    case 45: return std::make_tuple(w1, 2 * h1);             // 𝛑/4
    case 60: return std::make_tuple(2 * w1, 2 * h1);             // 2𝛑/6

    case 90: return std::make_tuple(w1, 2 * h1);             // 𝛑/2

    case 120: return std::make_tuple(2 * w1, 2 * h1);        // 4𝛑/6
    case 135: return std::make_tuple(2 * w1, 2 * h1);        // 3𝛑/4
    case 150: return std::make_tuple(2 * w1, 2 * h1);        // 5𝛑/6

    case 180: return std::make_tuple(w1, h1);            // 𝛑

    case 210: return std::make_tuple(2 * w1, 2 * h1);        // 7𝛑/6
    case 225: return std::make_tuple(2 * w1, 2 * h1);        // 5𝛑/4
    case 240: return std::make_tuple(2 * w1, 2 * h1);        // 8𝛑/6

    case 270: return std::make_tuple(2 * w1, 2 * h1);        // 3𝛑/4

    case 300: return std::make_tuple(3 * w1, 2 * h1);        // 10𝛑/6
    case 315: return std::make_tuple(2 * w1, 2 * h1);        // 7𝛑/4
    case 330: return std::make_tuple(3 * w1, 2 * h1);        // 11𝛑/6


    case 360: return std::make_tuple(w1, h1);            // 0
    }

    return std::make_tuple(0, 0);
}

int Projection::_getRadianAngleDisplayWidth(int angleInDegree, int w1, int h1)
{
    return std::get<0>(_getRadianAngleDisplayWidthAndHeight(angleInDegree, w1, h1));
}

int Projection::_getRadianAngleDisplayHeight(int angleInDegree, int w1, int h1)
{
    return std::get<1>(_getRadianAngleDisplayWidthAndHeight(angleInDegree, w1, h1));
}

// w1 = single character width
void Projection::_drawRadianAngle(QPainter *p, int x, int y, int angleInDegree, int w1)
{
    int denominator = 1;
    int multiplier = 1;
    int h1 = p->font().pixelSize();
    int w = _getRadianAngleDisplayWidth(angleInDegree, w1, h1);;      // width of the angle display in radians

    switch (angleInDegree)
    {
        case 0:     multiplier = 1;     denominator = 1;    break;

        case 30:    multiplier = 1;     denominator = 6;    break;
        case 45:    multiplier = 1;     denominator = 4;    break;
        case 60:    multiplier = 2;     denominator = 6;    break;
        case 90:    multiplier = 1;     denominator = 2;    break;

        case 120:   multiplier = 4;     denominator = 6;    break;
        case 135:   multiplier = 3;     denominator = 4;    break;
        case 150:   multiplier = 5;     denominator = 6;    break;
        case 180:   multiplier = 1;     denominator = 1;    break;

        case 210:   multiplier = 7;     denominator = 6;    break;
        case 225:   multiplier = 5;     denominator = 4;    break;
        case 240:   multiplier = 8;     denominator = 6;    break;
        case 270:   multiplier = 3;     denominator = 2;    break;

        case 300:   multiplier = 10;    denominator = 6;    break;
        case 315:   multiplier = 7;     denominator = 4;    break;
        case 330:   multiplier = 11;    denominator = 6;    break;
        case 360:   multiplier = 1;     denominator = 1;    break;
    }

    switch (angleInDegree)
    {
        case 0:     p->drawText(x, y, "0");     break;
        case 180:   p->drawText(x, y, "𝛑");     break;
        default:
            if (multiplier != 1)
            {
                int ym = y;                 // y value of multipler is different if denominator is not 1.
                if (denominator != 1)
                    ym += h1 / 2 + 2;

                QString str = QString::number(multiplier);
                p->drawText(x, ym, str);

                QFontMetrics fm(p->font());
                x += fm.horizontalAdvance(str) + 2;
            }

            p->drawText(x-1, y, "𝛑");

            if (denominator != 1)
            {
                p->drawLine(x - 2,
                            y + 3,
                            x + 12,
                            y + 3);
                y += h1 + 1;
                p->drawText(x, y, QString::number(denominator));
            }
            break;
    }
}




void Projection::drawVectorProjection(QPainter *p, double currentAngleInDegrees, int penWidth)
{
    int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

    p->save();
    p->translate(axis_x, axis_y);
    p->rotate(phase);

    p->drawRect(-penWidth/2,
                0,
                penWidth,
                -currentHeight);

    p->restore();
}

void Projection::drawVectorComponentInVectorSweepCircle(QPainter *p, double currentAngleInDegrees, int penWidth)
{
    int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);
    int currentDepth = getCurrentDepth(amplitude, currentAngleInDegrees);

    p->save();
    p->translate(vector_origin_x, vector_origin_y);
    p->rotate(phase);

    QPen pen = QPen(color);
    pen.setWidth(penWidth);
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->setOpacity(projectionOpacity);

    p->drawLine(0,
                0,
                0,
                -currentHeight);

    p->drawLine(currentDepth,
                0,
                currentDepth,
                -currentHeight);

    p->restore();
}

void Projection::drawVectorProjectionBoxes(QPainter *p, int penWidth)
{
    p->save();
    p->translate(axis_x, axis_y);
    p->rotate(phase);

    p->drawRect(-penWidth/2,
                -amplitude,
                penWidth,
                amplitude*2);

    p->restore();
}

void Projection::drawDottedLineFromVectorTip(QPainter *p, double currentAngleInDegrees, int vector_tip_x, int vector_tip_y)
{
    int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

    int proj_x = axis_x + int(currentHeight * cos((90 - phase) * M_PI / 180.0));
    int proj_y = axis_y - int(currentHeight * sin((90 - phase) * M_PI / 180.0));

    p->drawLine(proj_x,
                proj_y,
                vector_tip_x,
                vector_tip_y);

}

void Projection::drawTipCircle(QPainter *p, double currentAngleInDegrees, int penWidth)
{
    int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

    int proj_x = axis_x + int(currentHeight * cos((90 - phase) * M_PI / 180.0));
    int proj_y = axis_y - int(currentHeight * sin((90 - phase) * M_PI / 180.0));

    p->drawEllipse(proj_x - penWidth / 2,
                   proj_y - penWidth / 2,
                   penWidth,
                   penWidth);
}

void Projection::drawTipCircleWithoutRotation(QPainter *p, double currentAngleInDegrees, int penWidth)
{
    int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

    int proj_x = unrotated_axis_x;
    int proj_y = unrotated_axis_y - int(currentHeight * sin((90) * M_PI / 180.0));

    p->drawEllipse(proj_x - penWidth / 2,
                   proj_y - penWidth / 2,
                   penWidth,
                   penWidth);
}


void Projection::drawLinesAtImportantCoordinates(QPainter *p,
                                                 bool show1, bool show0p866, bool show0p707, bool show0p5,
                                                 bool show1Caption, bool show0p866Caption, bool show0p707Caption, bool show0p5Caption)
{
    vector<tuple<double, string, bool, bool>> ordinates = {
        make_tuple(1.0,    "+1.0",    show1,      show1Caption),
        make_tuple(0.866,  "+0.866",  show0p866,  show0p866Caption),
        make_tuple(0.707,  "+0.707",  show0p707,  show0p707Caption),
        make_tuple(0.5,    "+0.5",    show0p5,    show0p5Caption),

        make_tuple(-0.5,   "-0.5",    show0p5,    show1Caption),
        make_tuple(-0.707, "-0.707",  show0p707,  show0p707Caption),
        make_tuple(-0.866, "-0.866",  show0p866,  show0p866Caption),
        make_tuple(-1.0,   "-1.0",    show1,      show0p5Caption),
    };

    p->save();
    p->translate(axis_x, axis_y);
    p->rotate(phase);
    for (tuple<double, string, bool, bool> t : ordinates)
    {
        if (get<2>(t))      // is ordinate line enabled?
        {
            //---------------------------------------------------------------------------------------
            // Draw faint horizontal lines at current ordinate
            //---------------------------------------------------------------------------------------
            p->setOpacity(0.1);
            p->drawLine(-10,
                        -int(round(amplitude * get<0>(t))),
                        -2500,
                        -int(round(amplitude * get<0>(t))));

            //---------------------------------------------------------------------------------------
            // is caption enabled?
            if (get<3>(t))
            {
                p->setOpacity(0.7);
                p->drawText(-70,
                            -int(round(amplitude * get<0>(t))),
                            get<1>(t).c_str());
            }
        }

    }
    p->restore();
}

void Projection::drawLineThroughVectorSweepCircle(QPainter *p)
{
    int x_component = int((2 * amplitude + 20) * cos(phase * M_PI / 180.0));
    int y_component = int((2 * amplitude + 20) * sin(phase * M_PI / 180.0));

    int p1_x = vector_origin_x - x_component/2;
    int p1_y = vector_origin_y - y_component/2;

    int p2_x = vector_origin_x + x_component/2;
    int p2_y = vector_origin_y + y_component/2;

    p->drawLine(p1_x, p1_y, p2_x, p2_y);
}

void Projection::drawPhaseArcFromGivenPhase(QPainter *p, double givenPhaseInDegrees, int penWidth)
{
    double spanAngle = givenPhaseInDegrees - phase;

    int reducedAmplitude = amplitude - penWidth/2;

    p->save();
    QPen pen(p->pen());
    pen.setWidth(penWidth);
    pen.setColor(color);
    p->setPen(pen);

    p->drawArc(vector_origin_x - reducedAmplitude,
               vector_origin_y - reducedAmplitude,
               2 * reducedAmplitude,
               2 * reducedAmplitude,
               int(-givenPhaseInDegrees * 16),
               int(spanAngle * 16));

    p->restore();

    //-----------------------------------------------------------------------------------------------
    // Draw text along the phase arc
    //-----------------------------------------------------------------------------------------------

    int leadAngle = int(phase) % 360;
    int lagAngle = (360 - int(phase)) % 360;

    QString text                = "  " + QString::number(leadAngle) + "° lead  ";
    QString complementarytext   = "  " + QString::number(lagAngle) + "° lag  ";


    p->save();

    QFont font(p->font());
    font.setPixelSize(22);
    p->setFont(font);
    p->setOpacity(1);

    bool clockwise = (phase >= 0) && (phase <= 180) ? false : true;
    bool alignment = !clockwise;

    // Lead caption
    drawCircularText(p,
                     vector_origin_x,
                     vector_origin_y,
                     amplitude + 15,
                     clockwise,
                     -phase,
                     alignment,
                     text);
    // Lag caption
    drawCircularText(p,
                     vector_origin_x,
                     vector_origin_y,
                     amplitude + 15,
                     clockwise,
                     -phase,
                     !alignment,
                     complementarytext);
    p->restore();

}


void Projection::drawCircularText(QPainter *p, int x, int y, int radius, bool clockwise, double angleInDegrees, bool alignStart, QString& text)
{
    QFont font(p->font());
    QFontMetrics fm(font);
//    int pixelSize = font.pixelSize();
    int directionMultiplier = clockwise ? -1  : 1;

    // what angle will we start rendering characters?
    double angleInRadians = angleInDegrees * M_PI / 180;
    if (!alignStart)
        angleInRadians -= directionMultiplier * fm.horizontalAdvance(text) / double(radius);

    radius = radius + (directionMultiplier * 6);            // adjust radius based on text direction

    for (int i=0; i<text.size(); i++)
    {
        int xOff = int(round(radius * cos(angleInRadians)));
        int yOff = int(round(radius * sin(angleInRadians)));

        p->save();
        p->translate(x + xOff, y - yOff);
        p->rotate((-angleInRadians * 180 / M_PI) - (directionMultiplier * 90));
        p->drawText(0, 0, QString(text[i].unicode()));
        p->restore();

        int w = fm.horizontalAdvance(QString(text[i].unicode()));
        angleInRadians += directionMultiplier * double(w) / radius;
    }
}

void Projection::drawAxis(QPainter *p)
{
    p->save();
    p->translate(axis_x, axis_y);
    p->rotate(phase);
    p->drawLine(0,
                0,
                -2500,
                0);
    p->restore();
}

void Projection::drawObserver(QPainter *p)
{
    QRect target;
    QRect source;

    p->save();
    p->translate(axis_x, axis_y);
    p->rotate(phase);

    p->setOpacity(0.7);
    if (observerImage)
    {
        int w = observerImage->width();
        int h = observerImage->height();

        target = QRect(2*(amplitude + wallSeparation) + 35,
                       - h/2,
                       w,
                       h);

        source = QRect(0, 0, w, h);

        p->drawImage(target, *observerImage, source);
    }

    p->restore();
}

/*
 * Search the file in current directory and ..\RotatingVector directory.
 */
QImage* Projection::locateAndInstantiateImage(QString filename)
{
    QImage *image = nullptr;
    QString curDir = QDir::currentPath();
    QFile file(filename);

    if (file.exists())
    {
        image = new QImage(file.fileName());
    }
    else
    {
        file.setFileName(curDir + "\\..\\RotatingVector\\" + filename);
        if (file.exists())
        {
            image = new QImage(file.fileName());
        }
    }
    return image;
}
