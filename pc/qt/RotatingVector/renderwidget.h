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

public:
    AxisOrdinates(int maxOrdinates_)
    {
        maxOrdinates = maxOrdinates_;
        ordinates = new int[maxOrdinates];
        clear();
    }
    void clear()
    {
        for (int i=0; i<maxOrdinates; i++)
        {
            ordinates[i] = 0;
        }
    }
    void shift(int currentValue)
    {
        for (int i=maxOrdinates-2; i>=0; i--)
        {
            ordinates[i+1] = ordinates[i];
        }
        ordinates[0] = currentValue;
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
    void draw(QPainter *p);
    void drawProjectionBoxes(QPainter *p, VectorDrawingCoordinates v);
    void drawBackground(QPainter *p, VectorDrawingCoordinates v);
    void drawRotatingVectorComponents(QPainter *p, VectorDrawingCoordinates v);
    void drawRotatingVector(QPainter *p, VectorDrawingCoordinates v);
    void drawVectorShadow(QPainter *p, VectorDrawingCoordinates v);
    void drawAxis(QPainter *p, VectorDrawingCoordinates v);
    void drawSineAndCosinePoints(QPainter *p, VectorDrawingCoordinates v);
    void drawTipCircles(QPainter *p, VectorDrawingCoordinates v);

//    RotatingVectorData *data;

    QTimer *timer;

    MainWindow *data;

    AxisOrdinates xAxisOrdinates;
    AxisOrdinates yAxisOrdinates;

    ScrollingBackground vtScrollingBackground;
    ScrollingBackground hzScrollingBackground;

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

    const QColor sinColor = QColor(80, 220, 80);
    const QColor cosColor = QColor(80, 80, 220);

    const QColor vectorColor = QColor(0, 220, 220);
    const QColor vectorTipCircleColor = QColor(40, 40, 40);
    const QColor vectorSweepColor = QColor(0, 50, 50);
    const QColor scrollingBackgroundColor = QColor(150, 150, 150);

    const int wallSeparation = 30;
    const double sinCosOpacity = 0.7;

};

#endif // RENDERWIDGET_H
