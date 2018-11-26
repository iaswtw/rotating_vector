#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <tuple>
#include <QDir>
#include <QFile>

using namespace std;

#define NUM_ORDINATES                   2300
#define NUM_BACKGROUND_TEXT_POINTS      15
#define NUM_TIME_TEXT_POINTS            8
#define NUM_ANGLE_TEXT_POINTS           8


class MainWindow;


/*************************************************************************************************

 *************************************************************************************************/
struct BackgroundText
{
    int *x;
    int *y;
    QString text;
    int numPoints;

    int maxX;
    int maxY;

public:
    BackgroundText(QString text_, int numPoints_) :
        x(nullptr),
        y(nullptr),
        text(text_),
        numPoints(numPoints_),
        maxX(0),
        maxY(0)
    {
        x = new int[numPoints];
        y = new int[numPoints];
    }
    void genRandomPoints(int minX, int maxX, int minY, int maxY, QRandomGenerator &randomGenerator)
    {
        this->maxX = maxX;
        this->maxY = maxY;

        for (int i=0; i<numPoints; i++)
        {
            x[i] = randomGenerator.bounded(minX, maxX);
            y[i] = randomGenerator.bounded(minY, maxY);
        }
    }

    void draw(QPainter &p, int xOffset, int yOffset, int width, int height)
    {
        for (int i=0; i<numPoints; i++)
        {
            if ((x[i] < width) && (y[i] < height))
            {
                p.drawText(xOffset + x[i], yOffset + y[i], text);
            }
        }
    }

    void shiftLeft(int amount)
    {
        for (int i=0; i<numPoints; i++)
        {
            x[i] -= amount;
            if (x[i] < -200)
            {
                x[i] = maxX;
            }
        }
    }

    void shiftUp(int amount)
    {
        for (int i=0; i<numPoints; i++)
        {
            y[i] -= amount;
            if (y[i] < 0)
            {
                y[i] = maxY;
            }
        }
    }

};


/*************************************************************************************************

 *************************************************************************************************/
struct ScrollingBackground
{
public:
    BackgroundText backgroundText;
    BackgroundText timeText;
    BackgroundText angleText;

    ScrollingBackground(int numBackgroundTextPoints, int numTimeTextPoints, int numAngleTextPoints) :
        backgroundText(BackgroundText("Background", numBackgroundTextPoints)),
        timeText(BackgroundText("Time", numTimeTextPoints)),
        angleText(BackgroundText("Angle", numAngleTextPoints))
    {
    }

    void generateRandomBackgroundPoints(int minX, int maxX, int minY, int maxY, QRandomGenerator &randomGenerator)
    {
        backgroundText.genRandomPoints(minX, maxX, minY, maxY, randomGenerator);
    }
    void generateRandomTimePoints(int minX, int maxX, int minY, int maxY, QRandomGenerator &randomGenerator)
    {
        timeText.genRandomPoints(minX, maxX, minY, maxY, randomGenerator);
    }
    void generateRandomAnglePoints(int minX, int maxX, int minY, int maxY, QRandomGenerator &randomGenerator)
    {
        angleText.genRandomPoints(minX, maxX, minY, maxY, randomGenerator);
    }

    void draw(QPainter &p, int xOffset, int yOffset, int width, int height)
    {
        backgroundText.draw(p, xOffset, yOffset, width, height);
        timeText.draw(p, xOffset, yOffset, width, height);
        angleText.draw(p, xOffset, yOffset, width, height);
    }

    void shiftLeft(int amount)
    {
        backgroundText.shiftLeft(amount);
        timeText.shiftLeft(amount);
        angleText.shiftLeft(amount);
    }

    void shiftUp(int amount)
    {
        backgroundText.shiftUp(amount);
        timeText.shiftUp(amount);
        angleText.shiftUp(amount);
    }
};


struct VectorDrawingCoordinates
{
    int vector_origin_x;
    int vector_origin_y;

    int vector_width;
    int vector_height;

    int vector_tip_x;
    int vector_tip_y;

    // coordinates of rightmost point of the X axis
    int xaxis_x;
    int xaxis_y;

    // coordinates of the bottommost point of the Y axis
    int yaxis_x;
    int yaxis_y;

    // coordinates of the vector origin's vertical projection
    int vproj_origin_x;
    int vproj_origin_y;

    // coordinates of the vector origin's horizontal projection
    int hproj_origin_x;
    int hproj_origin_y;
};


/*************************************************************************************************
 This class manages projection at a given angle. It knows how to draw various parts of a projection
 such as: projection boxes, projection itself, axis, various important ordinate lines, angle values,
 observer, etc.  The functions that do the drawing should be called from the render widget.
 *************************************************************************************************/
struct Projection
{
    double phase;
    int *ordinates;
    int maxOrdinates;
    int *angles;

    int axis_x;
    int axis_y;

    int unrotated_axis_x;
    int unrotated_axis_y;

    int amplitude;

    int vector_origin_x;
    int vector_origin_y;
    int wallSeparation;

    bool isPositionCalculated = false;

    QImage *observerImage = nullptr;

public:
    Projection(double phase_, int maxOrdinates_, QString observerFilename)
    {
        phase = phase_;
        maxOrdinates = maxOrdinates_;
        ordinates = new int[maxOrdinates];
        angles = new int[maxOrdinates];
        observerImage = locateAndInstantiateImage(observerFilename);
        clear();
    }
    void clear()
    {
        for (int i=0; i<maxOrdinates; i++)
        {
            ordinates[i] = 0;
            angles[i] = INT_MIN;
        }
    }

    void setPhase(double phase_)
    {
        phase = phase_;
        if (isPositionCalculated)
            recalculatePosition();
    }

    void recalculatePosition()
    {
        axis_x = vector_origin_x - int((amplitude + wallSeparation) * cos(phase * M_PI / 180.0));
        axis_y = vector_origin_y - int((amplitude + wallSeparation) * sin(phase * M_PI / 180.0));

        unrotated_axis_x = vector_origin_x - int((amplitude + wallSeparation) * cos(0 * M_PI / 180.0));
        unrotated_axis_y = vector_origin_y - int((amplitude + wallSeparation) * sin(0 * M_PI / 180.0));

        isPositionCalculated = true;
    }
    void recalculatePosition(int vector_origin_x, int vector_origin_y, int amplitude_, int wallSeparation)
    {
        this->vector_origin_x = vector_origin_x;
        this->vector_origin_y = vector_origin_y;
        this->amplitude = amplitude_;
        this->wallSeparation = wallSeparation;

        recalculatePosition();
    }

    int getCurrentHeight(int amplitude, double currentAngleInDegrees)
    {
        int currentHeight = int(amplitude * sin((currentAngleInDegrees + phase) * M_PI / 180.0));
        return currentHeight;
    }

    /*
     * Shift ordinate & angle values up by 1 index.  The last value in the array will fall off.
     * Set the given current value as the value for ordinate & angle at index 0.
     */
   void shift(int amplitude, double currentAngleInDegrees, bool isVectorRunning, bool isClockwise)
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

           if ((intAngle % 30) != 0)
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
   void drawWave(QPainter *p, int abscissaScale)
   {
       p->save();
       p->translate(axis_x, axis_y);
       p->rotate(phase);
       for (int i=0; i<maxOrdinates-1; i++)
       {
               p->drawLine(- i*abscissaScale,
                           - ordinates[i],
                           - (i+1)*abscissaScale,
                           - ordinates[i+1]);
       }
       p->restore();
   }

   void drawWaveWithoutRotation(QPainter *p, int abscissaScale)
   {
       p->save();
       p->translate(unrotated_axis_x, unrotated_axis_y);
//       p->rotate(phase);          // no rotation
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
   void drawAngles(QPainter *p, int abscissaScale, bool showMultiplesOf30)
   {
       //-----------------------------------------------------------------------
       // Draw angles if set
       QPen pen = QPen(QColor(50, 50, 50));
       pen.setWidth(2);
       p->setPen(pen);

       QFont font;
       int fontPixelSize = 20;
       font.setPixelSize(fontPixelSize);
       p->setFont(font);


       QFontMetrics fm(font);
       for (int i=0; i<maxOrdinates-1; i++)
       {
           if (angles[i] != INT_MIN)
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
               QString str = QString::number(angles[i]);
               int w = fm.width(str);

               int angle_str_x = axis_x - int((i*abscissaScale) * cos(phase * M_PI / 180.0));
               int angle_str_y = axis_y - int((i*abscissaScale) * sin(phase * M_PI / 180.0));

               int angle_str_x_correction = w/2 + int((w/2 + 10) * sin(phase * M_PI / 180.0));
               int angle_str_y_correction = 10 + int((fontPixelSize) * cos(phase * M_PI / 180.0));

               p->drawText(angle_str_x - angle_str_x_correction,
                           angle_str_y + angle_str_y_correction,
                           str);
           }
       }
   }

    void drawVectorProjection(QPainter *p, double currentAngleInDegrees, int penWidth)
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

    void drawVectorProjectionBoxes(QPainter *p, int penWidth)
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

    void drawDottedLineFromVectorTip(QPainter *p, double currentAngleInDegrees, int vector_tip_x, int vector_tip_y)
    {
        int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

        int proj_x = axis_x + int(currentHeight * cos((90 - phase) * M_PI / 180.0));
        int proj_y = axis_y - int(currentHeight * sin((90 - phase) * M_PI / 180.0));

        p->drawLine(proj_x,
                    proj_y,
                    vector_tip_x,
                    vector_tip_y);

    }

    void drawTipCircle(QPainter *p, double currentAngleInDegrees, int penWidth)
    {
        int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

        int proj_x = axis_x + int(currentHeight * cos((90 - phase) * M_PI / 180.0));
        int proj_y = axis_y - int(currentHeight * sin((90 - phase) * M_PI / 180.0));

        p->drawEllipse(proj_x - penWidth / 2,
                       proj_y - penWidth / 2,
                       penWidth,
                       penWidth);
    }

    void drawTipCircleWithoutRotation(QPainter *p, double currentAngleInDegrees, int penWidth)
    {
        int currentHeight = getCurrentHeight(amplitude, currentAngleInDegrees);

        int proj_x = unrotated_axis_x;
        int proj_y = unrotated_axis_y - int(currentHeight * sin((90) * M_PI / 180.0));

        p->drawEllipse(proj_x - penWidth / 2,
                       proj_y - penWidth / 2,
                       penWidth,
                       penWidth);
    }


    void drawLinesAtImportantCoordinates(QPainter *p, bool show1, bool show0p866, bool show0p707, bool show0p5)
    {
        vector<tuple<double, string, bool>> ordinates = {
            make_tuple(1.0,    "+1.0",    show1),
            make_tuple(0.866,  "+0.866",  show0p866),
            make_tuple(0.707,  "+0.707",  show0p707),
            make_tuple(0.5,    "+0.5",    show0p5  ),

            make_tuple(-0.5,   "-0.5",    show0p5  ),
            make_tuple(-0.707, "-0.707",  show0p707),
            make_tuple(-0.866, "-0.866",  show0p866),
            make_tuple(-1.0,   "-1.0",    show1),
        };

        p->save();
        p->translate(axis_x, axis_y);
        p->rotate(phase);
        for (tuple<double, string, bool> t : ordinates)
        {
            p->setOpacity(0.7);
            if (get<2>(t))
            {
                p->drawText(-70,
                            -int(round(amplitude * get<0>(t))),
                            get<1>(t).c_str());

            }

            //---------------------------------------------------------------------------------------
            // Draw faint horizontal lines at +1, +0.866, +0.5, -0.5, -0.866 and -1.
            //---------------------------------------------------------------------------------------
            p->setOpacity(0.1);
            p->drawLine(-10,
                        -int(round(amplitude * get<0>(t))),
                        -2500,
                        -int(round(amplitude * get<0>(t))));
        }
        p->restore();
    }

    void drawAxis(QPainter *p)
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

    void drawObserver(QPainter *p)
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

            target = QRect(2*(amplitude + wallSeparation) + 30,
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
    QImage* locateAndInstantiateImage(QString filename)
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

};


/*************************************************************************************************

 *************************************************************************************************/
class RenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = nullptr, MainWindow *data = nullptr);
    void updateTimerInterval();
    void clearSinOrdinates();
    void clearCosOrdinates();
    void recalculateVectorOrigin();
    void notifyPositionChange();
    void updatePhaseShiftFromSine();

protected:
    void resizeEvent(QResizeEvent* event);


public slots:
    void renderTimerEvent();

signals:


protected:
    void paintEvent(QPaintEvent *pe);


private:
    void draw                               (QPainter *p);
    void drawProjectionBoxes                (QPainter *p);
    void drawBackground                     (QPainter *p, VectorDrawingCoordinates v);
    void drawRotatingVectorComponents       (QPainter *p, VectorDrawingCoordinates v);
    void drawRotatingVector                 (QPainter *p, VectorDrawingCoordinates v);
    void drawVectorProjection               (QPainter *p, VectorDrawingCoordinates v);
    void drawAxis                           (QPainter *p);
    void drawSineAndCosinePoints            (QPainter *p);
    void drawLinesAtImportantOrdinateValues (QPainter *p);
    void drawTipCircles                     (QPainter *p, VectorDrawingCoordinates v);
    void drawObservers                      (QPainter *p);

    void lowPassFilterAngleDifference(double difference);

    QTimer *timer = new QTimer(this);

    MainWindow *data;

    Projection xProjection = Projection(0, NUM_ORDINATES, "alice.png");
    Projection yProjection = Projection(0, NUM_ORDINATES, "cat.png");

    ScrollingBackground vtScrollingBackground = ScrollingBackground(NUM_BACKGROUND_TEXT_POINTS/2,
                                                                    NUM_TIME_TEXT_POINTS/2,
                                                                    NUM_ANGLE_TEXT_POINTS/2);

    ScrollingBackground hzScrollingBackground = ScrollingBackground(NUM_BACKGROUND_TEXT_POINTS,
                                                                    NUM_TIME_TEXT_POINTS,
                                                                    NUM_ANGLE_TEXT_POINTS);

    QImage *aliceImage = nullptr;
    QImage *catImage = nullptr;

    const QColor sinColor = QColor(120, 220, 120);
    const QColor cosColor = QColor(140, 190, 255);

    const QColor vectorColor = QColor(0, 220, 220);
    const QColor vectorTipCircleColor = QColor(40, 40, 40);
    const QColor vectorSweepColor = QColor(0, 50, 50);
    const QColor scrollingBackgroundColor = QColor(150, 150, 150);

    const int wallSeparation = 30;
    const double sinCosOpacity = 0.7;

    double previousAngleInDegrees = 0;
    double smoothedChangeInAngle = 0;
    bool isVectorOrArduinoRunning = false;      // decides whether angle (0, 90, 180, 270) is set on an ordinate.

    QPoint vectorOrigin = QPoint(0, 0);

};

#endif // RENDERWIDGET_H
