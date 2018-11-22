#include "arduinosimulator.h"
#include <QTimer>
#include "mainwindow.h"

#define  STEPS_PER_REVOLUTION         200
#define  HALF_STEPS_PER_REVOLUTION    (STEPS_PER_REVOLUTION * 2)

#define  STEPS_AT_RESET_POSITION      (STEPS_PER_REVOLUTION * 3 / 4)      // 270 degrees.  vertical down position of vector.
#define  HALF_STEPS_AT_RESET_POSITION (STEPS_AT_RESET_POSITION * 2)

#define  NUM_HALF_STEPS_PER_DEGREE    (HALF_STEPS_PER_REVOLUTION / 360.0)
#define  NUM_DEGREES_PER_HALF_STEP    (360.0 / HALF_STEPS_PER_REVOLUTION)


ArduinoSimulator::ArduinoSimulator(QObject *parent, MainWindow *mw_) :
    QObject(parent),
    runMotor(false),
    isCounterClockwise(true),
    angleIncrementInDegrees(0.1),
    halfSteps(0),
    targetHalfSteps(-1),
    doHalfStep(false),
    mw(mw_)
{
}

void ArduinoSimulator::postCmd(QString cmd)
{
    cmdList.append((cmd));
}

void ArduinoSimulator::readAndExecuteCommand()
{
    if (cmdList.size() > 0)
    {
        QString cmd = cmdList.at(0);
        cmdList.removeAt(0);

        if (cmd.contains('\n'))
        {
            cmd.replace("\n", "\0");
            if (cmd == "c")
            {
                printf("Received continue command\n");
                runMotor = true;
            }
            else if (cmd == "p")
            {
                runMotor = false;
            }
            else if (cmd == "r")
            {
                runMotor = false;
                halfSteps = 0;
            }
            else if (cmd.at(0) == 'g')
            {
                double targetAngle = cmd.right(cmd.length() - 1).toDouble();
                targetHalfSteps = int(round(targetAngle * NUM_HALF_STEPS_PER_DEGREE));
                printf("******* Target half steps = %d\n", targetHalfSteps);
                runMotor = true;
            }

            //----------------------------------------------------
            // Set absolute angle
            //----------------------------------------------------
            else if (cmd.at(0) == "=")
            {
                int calibrationAngle = cmd.right(cmd.length() - 1).toInt();
                printf("Calibrating current position to: %d\n", calibrationAngle);

                halfSteps = int(round(calibrationAngle * NUM_HALF_STEPS_PER_DEGREE));
                printf("New half steps = %d\n", halfSteps);
            }


            //----------------------------------------------------
            // set direction
            //----------------------------------------------------
            else if (cmd.at(0) == "<")
            {
                isCounterClockwise = true;
            }
            else if (cmd.at(0) == ">")
            {
                isCounterClockwise = false;
            }


            //----------------------------------------------------
            // proceed by 1 half step in the set direction
            //----------------------------------------------------
            else if (cmd.at(0) == "h")
            {
                doHalfStep = true;
                runMotor = true;
            }

            //----------------------------------------------------
            // Various speed settings
            //----------------------------------------------------
            else if (cmd == "1")
            {
                runMotor = true;
                angleIncrementInDegrees = SPEED_1_ANGLE_INCREMENT;
            }
            else if (cmd == "2")
            {
                runMotor = true;
                angleIncrementInDegrees = SPEED_2_ANGLE_INCREMENT;
            }
            else if (cmd == "3")
            {
                runMotor = true;
                angleIncrementInDegrees = SPEED_3_ANGLE_INCREMENT;
            }
            else if (cmd == "4")
            {
                runMotor = true;
                angleIncrementInDegrees = SPEED_4_ANGLE_INCREMENT;
            }
            else if (cmd == "5")
            {
                runMotor = true;
                angleIncrementInDegrees = SPEED_5_ANGLE_INCREMENT;
            }
            else if (cmd == "6")
            {
                runMotor = true;
                angleIncrementInDegrees = SPEED_6_ANGLE_INCREMENT;
            }
        }

    }
}

void ArduinoSimulator::tick()
{
    if (runMotor)
    {
        if (isCounterClockwise)
        {
            if (doHalfStep)
                currentAngleInDegrees += 0.9;
            else
                currentAngleInDegrees += angleIncrementInDegrees;
        }
        else
        {
            if (doHalfStep)
                currentAngleInDegrees -= 0.9;
            else
                currentAngleInDegrees -= angleIncrementInDegrees;
        }

        //----------------------------------------------
        // rebase to 0 if gone over/under a full revolution.
        //----------------------------------------------
        if (isCounterClockwise)
        {
            if (currentAngleInDegrees >= 360)
                currentAngleInDegrees -= 360.0;
        }
        else
        {
            if (currentAngleInDegrees < 0)
                currentAngleInDegrees += 360.0;
        }

        //------------------------------------------------------
        // if asked to advance by 1 half step, we just did above. pause now.
        //------------------------------------------------------
        if (doHalfStep)
        {
            doHalfStep = false;
            runMotor = false;
            mw->isTimePaused = true;
        }

        halfSteps = int(round(currentAngleInDegrees * NUM_HALF_STEPS_PER_DEGREE));

        //----------------------------------------------
        // were we asked to go to a specific angle?  If yes, and if we reached that, stop.
        //----------------------------------------------
        if ((targetHalfSteps != -1) && (targetHalfSteps == halfSteps))
        {
            runMotor = false;
            targetHalfSteps = -1;
            mw->isTimePaused = true;
        }


        QString statusString;
        statusString.sprintf("%.2f %d\n", currentAngleInDegrees, halfSteps);

        if (!mw->useArduino)
        {
            mw->processSerialLine(statusString.toUtf8());
        }
    }

    readAndExecuteCommand();
}
