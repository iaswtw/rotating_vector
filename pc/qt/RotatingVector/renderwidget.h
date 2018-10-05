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

    int ordinates[NUM_ORDINATES];
    Point bkTextPoints[NUM_BACKGROUND_TEXT_POINTS];
    Point timeTextPoints[NUM_TIME_TEXT_POINTS];
};

#endif // RENDERWIDGET_H
