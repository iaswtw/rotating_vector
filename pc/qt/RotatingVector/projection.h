#ifndef PROJECTION_H
#define PROJECTION_H

#include <QPoint>
#include <QPainter>


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
    QColor color;

    const double waveOpacity = 0.7;
    const double projectionOpacity = 0.7;

public:
    Projection(double phase_, int maxOrdinates_, QString observerFilename, QColor color_);
    void clear();
    void setPhase(double phase_);
    void recalculatePosition();
    QPoint getAxisPositionFromPhase(double phase_);
    void recalculatePosition(int vector_origin_x, int vector_origin_y, int amplitude_, int wallSeparation);
    int getCurrentHeight(int amplitude, double currentAngleInDegrees);
    int getCurrentDepth(int amplitude, double currentAngleInDegrees);
    void shiftAndSet(int amplitude, double currentAngleInDegrees, bool isVectorRunning, bool isClockwise);
     void drawWave(QPainter *p, int abscissaScale, int penWidth);
    void drawWave(QPainter *p, int abscissaScale, int penWidth, double phaseRotation);
    void drawAngles(QPainter *p, int abscissaScale, bool showMultiplesOf30, bool showInRadians);
    void drawVectorProjection(QPainter *p, double currentAngleInDegrees, int penWidth);
    void drawVectorComponentInVectorSweepCircle(QPainter *p, double currentAngleInDegrees, int penWidth);
    void drawVectorProjectionBoxes(QPainter *p, int penWidth);
    void drawDottedLineFromVectorTip(QPainter *p, double currentAngleInDegrees, int vector_tip_x, int vector_tip_y);
    void drawTipCircle(QPainter *p, double currentAngleInDegrees, int penWidth);
    void drawTipCircleWithoutRotation(QPainter *p, double currentAngleInDegrees, int penWidth);
    void drawLinesAtImportantCoordinates(QPainter *p, bool show1, bool show0p866, bool show0p707, bool show0p5,
                                         bool show1Caption, bool show0p866Caption, bool show0p707Caption, bool show0p5Caption);
    void drawLineThroughVectorSweepCircle(QPainter *p);
    void drawPhaseArcFromGivenPhase(QPainter *p, double givenPhaseInDegrees, int penWidth);
    void drawCircularText(QPainter *p, int x, int y, int radius, bool clockwise, double angleInDegrees, bool alignStart, QString& text);
    void drawAxis(QPainter *p);

    void drawObserver(QPainter *p);
    QImage* locateAndInstantiateImage(QString filename);

private:
    void _drawRadianAngle(QPainter *p, int x, int y, int angleInRadian, int w1);
    std::tuple<int, int> _getRadianAngleDisplayWidthAndHeight(int angleInDegree, int w1, int h1);
    int _getRadianAngleDisplayWidth(int angleInDegree, int w1, int h1);
    int _getRadianAngleDisplayHeight(int angleInDegree, int w1, int h1);

};


#endif // PROJECTION_H
