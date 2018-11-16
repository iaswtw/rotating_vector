#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QRandomGenerator>
#include <QPainter>

#define NUM_ORDINATES                   1500
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
};

enum AxisOrientation
{
    RIGHT_TO_LEFT,
    BOTTOM_TO_TOP
};

/*************************************************************************************************

 *************************************************************************************************/
struct AxisOrdinates
{
    int *ordinates;
    int maxOrdinates;
    int *angles;

public:
    AxisOrdinates(int maxOrdinates_)
    {
        maxOrdinates = maxOrdinates_;
        ordinates = new int[maxOrdinates];
        angles = new int[maxOrdinates];
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
    /*
     * Shift ordinate & angle values up by 1 index.  The last value in the array will fall off.
     * Set the given current value as the value for ordinate & angle at index 0.
     */
    void shift(int currentValue, int currentAngle=INT_MIN)
    {
        for (int i=maxOrdinates-2; i>=0; i--)
        {
            ordinates[i+1] = ordinates[i];
            angles[i+1] = angles[i];
        }
        ordinates[0] = currentValue;
        angles[0] = INT_MIN;

        // If a current angle is specified, set it as the angle for current ordinate only if
        // the previous two ordinates didn't have any angle set.
        if (currentAngle != INT_MIN)
        {
            if ((angles[1] == INT_MIN) && (angles[2] == INT_MIN))
            {
                angles[0] = currentAngle;
            }
        }
    }
    // Draw straight lines between consecutive ordinate values.
    void drawLines(QPainter *p, AxisOrientation orientation, int xOffset, int yOffset, int abscissaScale)
    {
        for (int i=0; i<maxOrdinates-1; i++)
        {
            if (orientation == RIGHT_TO_LEFT)
            {
                p->drawLine(xOffset - i*abscissaScale,
                            yOffset - ordinates[i],
                            xOffset - (i+1)*abscissaScale,
                            yOffset - ordinates[i+1]);
            }
            else if (orientation == BOTTOM_TO_TOP)
            {
                p->drawLine(xOffset + ordinates[i],
                            yOffset - i*abscissaScale,
                            xOffset + ordinates[i+1],
                            yOffset - (i+1)*abscissaScale);
            }
            else
            {
                qFatal("Unknown axis orientation!");
            }
        }
    }
    // Draw angle marks & angle value for ordinates on which they are set.
    void drawAngles(QPainter *p, AxisOrientation orientation, int xOffset, int yOffset, int abscissaScale, int amplitude)
    {
        //-----------------------------------------------------------------------
        // Draw angles if set
        p->save();
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
            if (orientation == RIGHT_TO_LEFT)
            {
                if (angles[i] != INT_MIN)
                {
                    // draw small vertical line on X axis
                    p->drawLine(xOffset - i*abscissaScale,
                                yOffset - 10,
                                xOffset - i*abscissaScale,
                                yOffset + 10);

                    if ((angles[i] == 0) || (angles[i] == 360))
                    {
                        // draw a thin faint line from top to bottom
                        p->save();
                        p->setOpacity(0.1);
                        p->drawLine(xOffset - i*abscissaScale,
                                    yOffset - amplitude - 10,
                                    xOffset - i*abscissaScale,
                                    yOffset + amplitude + 10);
                        p->restore();
                    }

                    QString str = QString::number(angles[i]);
                    int w = fm.width(str);
                    p->drawText(xOffset - i*abscissaScale - w/2,
                                yOffset + 10 + fontPixelSize,
                                str);
                }
            }
            else if (orientation == BOTTOM_TO_TOP)
            {
                if (angles[i] != INT_MIN)
                {
                    // draw small horizontal line on Y axis
                    p->drawLine(xOffset - 10,
                                yOffset - i*abscissaScale,
                                xOffset + 10,
                                yOffset - i*abscissaScale);

                    if ((angles[i] == 0) || (angles[i] == 360))
                    {
                        // draw a thin faint line from left to right
                        p->save();
                        p->setOpacity(0.1);
                        p->drawLine(xOffset - amplitude - 10,
                                    yOffset - i*abscissaScale,
                                    xOffset + amplitude + 10,
                                    yOffset - i*abscissaScale);
                        p->restore();
                    }

                    QString str = QString::number(angles[i]);
                    int w = fm.width(str);
                    p->drawText(xOffset - 10 - w - 5,
                                yOffset - i*abscissaScale + fontPixelSize/2,
                                str);
                }
            }
            else
            {
                qFatal("Unknown axis orientation!");
            }
        }
        p->restore();
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

public slots:
    void renderTimerEvent();

signals:


protected:
    void paintEvent(QPaintEvent *pe);


private:
    void draw                           (QPainter *p);
    void drawProjectionBoxes            (QPainter *p, VectorDrawingCoordinates v);
    void drawBackground                 (QPainter *p, VectorDrawingCoordinates v);
    void drawRotatingVectorComponents   (QPainter *p, VectorDrawingCoordinates v);
    void drawRotatingVector             (QPainter *p, VectorDrawingCoordinates v);
    void drawVectorProjection           (QPainter *p, VectorDrawingCoordinates v);
    void drawAxis                       (QPainter *p, VectorDrawingCoordinates v);
    void drawSineAndCosinePoints        (QPainter *p, VectorDrawingCoordinates v);
    void drawTipCircles                 (QPainter *p, VectorDrawingCoordinates v);
    void drawAliceAndBob                (QPainter *p, VectorDrawingCoordinates v);

    QImage* locateAndInstantiateImage(QString filename);
//    RotatingVectorData *data;

    QTimer *timer;

    MainWindow *data;

    AxisOrdinates xAxisOrdinates;
    AxisOrdinates yAxisOrdinates;

    ScrollingBackground vtScrollingBackground;
    ScrollingBackground hzScrollingBackground;

    QImage *aliceImage = nullptr;
    QImage *catImage = nullptr;

    // Red and green
//    QColor sinColor = QColor(220, 50, 0);
//    QColor cosColor = QColor(50, 220, 0);
//    QColor vectorColor = QColor(220, 220, 0);

    // Red and blue
//    QColor sinColor = QColor(220, 50, 0);
//    QColor cosColor = QColor(50, 0, 220);
//    QColor vectorColor = QColor(220, 0, 220);

    // Green and Blue
//    const QColor sinColor = QColor(0, 220, 0);
//    const QColor cosColor = QColor(0, 0, 220);

    const QColor sinColor = QColor(120, 220, 120);
    const QColor cosColor = QColor(150, 150, 250);

    const QColor vectorColor = QColor(0, 220, 220);
    const QColor vectorTipCircleColor = QColor(40, 40, 40);
    const QColor vectorSweepColor = QColor(0, 50, 50);
    const QColor scrollingBackgroundColor = QColor(150, 150, 150);

    const int wallSeparation = 30;
    const double sinCosOpacity = 0.7;

};

#endif // RENDERWIDGET_H
