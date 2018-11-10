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


/*************************************************************************************************

 *************************************************************************************************/
class RenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = nullptr, MainWindow *data = nullptr);
    void updateTimerInterval();

public slots:
    void renderTimerEvent();

signals:


protected:
    void paintEvent(QPaintEvent *pe);


private:
    void draw(QPainter * p);

//    RotatingVectorData *data;

    QTimer *timer;

    MainWindow *data;

    int sinOrdinates[NUM_ORDINATES];
    int cosOrdinates[NUM_ORDINATES];

//    BackgroundText hzBkTextPoints[NUM_BACKGROUND_TEXT_POINTS];
//    BackgroundText hzTimeTextPoints[NUM_TIME_TEXT_POINTS];
//    BackgroundText hzAngleTextPoints[NUM_ANGLE_TEXT_POINTS];
//    BackgroundText vtBkTextPoints[NUM_BACKGROUND_TEXT_POINTS];
//    BackgroundText vtTimeTextPoints[NUM_TIME_TEXT_POINTS];
//    BackgroundText vtAngleTextPoints[NUM_ANGLE_TEXT_POINTS];

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
    QColor sinColor = QColor(0, 220, 0);
    QColor cosColor = QColor(0, 0, 220);
    QColor vectorColor = QColor(0, 220, 220);
    QColor vectorSweepColor = QColor(0, 50, 50);
    QColor scrollingBackgroundColor = QColor(150, 150, 150);

};

#endif // RENDERWIDGET_H
