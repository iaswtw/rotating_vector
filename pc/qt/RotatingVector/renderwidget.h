#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>

#define NUM_ORDINATES                   1500
#define NUM_BACKGROUND_TEXT_POINTS      15
#define NUM_TIME_TEXT_POINTS            15


class MainWindow;


struct Point
{
    int x;
    int y;
    QString text;
};

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

    Point hzBkTextPoints[NUM_BACKGROUND_TEXT_POINTS];
    Point hzTimeTextPoints[NUM_TIME_TEXT_POINTS];
    Point vtBkTextPoints[NUM_BACKGROUND_TEXT_POINTS];
    Point vtTimeTextPoints[NUM_TIME_TEXT_POINTS];

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
