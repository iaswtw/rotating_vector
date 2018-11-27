#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <tuple>
#include <QDir>
#include <QFile>
#include <projection.h>


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

    Projection xProjection = Projection(0, NUM_ORDINATES, "alice.png", sinColor);
    Projection yProjection = Projection(0, NUM_ORDINATES, "cat.png", cosColor);

};

#endif // RENDERWIDGET_H
