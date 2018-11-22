#ifndef ARDUINOSIMULATOR_H
#define ARDUINOSIMULATOR_H

#include <QObject>
#include <QTimer>

class MainWindow;

#define SMALLEST_ANGLE_INCREMENT                (0.2)
#define SPEED_1_ANGLE_INCREMENT                 (SMALLEST_ANGLE_INCREMENT * 1)
#define SPEED_2_ANGLE_INCREMENT                 (SMALLEST_ANGLE_INCREMENT * 2)
#define SPEED_3_ANGLE_INCREMENT                 (SMALLEST_ANGLE_INCREMENT * 3)
#define SPEED_4_ANGLE_INCREMENT                 (SMALLEST_ANGLE_INCREMENT * 4)
#define SPEED_5_ANGLE_INCREMENT                 (SMALLEST_ANGLE_INCREMENT * 5)
#define SPEED_6_ANGLE_INCREMENT                 (SMALLEST_ANGLE_INCREMENT * 6)

class ArduinoSimulator : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoSimulator(QObject *parent = nullptr, MainWindow *mw_ = nullptr);
    void postCmd(QString cmd);

    bool runMotor = false;
    bool isCounterClockwise = true;

signals:

public slots:
    void tick();

private:
    double angleIncrementInDegrees = SPEED_3_ANGLE_INCREMENT;
    double currentAngleInDegrees = 0.0;
    int halfSteps = 0;
    int targetHalfSteps = -1;
    bool doHalfStep = false;

    QStringList cmdList;
    MainWindow *mw;

    void readAndExecuteCommand();
};

#endif // ARDUINOSIMULATOR_H
