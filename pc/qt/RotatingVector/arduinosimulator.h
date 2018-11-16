#ifndef ARDUINOSIMULATOR_H
#define ARDUINOSIMULATOR_H

#include <QObject>
#include <QTimer>

class MainWindow;

class ArduinoSimulator : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoSimulator(QObject *parent = nullptr, MainWindow *mw_ = nullptr);
    void postCmd(QString cmd);

    bool runMotor;
    bool isCounterClockwise;


signals:

public slots:
    void iterationTimerEvent();

private:
    QTimer *timer;
    int iterationInterval;
    int halfSteps;
    int targetHalfSteps;
    bool doHalfStep;
    QStringList cmdList;
    MainWindow *mw;

    void readAndExecuteCommand();
};

#endif // ARDUINOSIMULATOR_H
