#include <AFMotor.h>
#include <string.h>
#include <math.h>

#define  STEPS_PER_REVOLUTION         200
#define  HALF_STEPS_PER_REVOLUTION    (STEPS_PER_REVOLUTION * 2)

#define  STEPS_AT_RESET_POSITION      (STEPS_PER_REVOLUTION * 3 / 4)      // 270 degrees.  vertical down position of vector.
#define  HALF_STEPS_AT_RESET_POSITION (STEPS_AT_RESET_POSITION * 2)

#define  NUM_HALF_STEPS_PER_DEGREE    (HALF_STEPS_PER_REVOLUTION / 360.0)
#define  NUM_DEGREES_PER_HALF_STEP    (360.0 / HALF_STEPS_PER_REVOLUTION)

#define  CMD_BUF_LEN                  10

// Connect a stepper motor with 48 steps per revolution (7.5 degree)
//   - to motor port #1 (M1 and M2)
//   - to motor port #2 (M3 and M4)
AF_Stepper motor(STEPS_PER_REVOLUTION, 1);

int             rpm                   = 3;
int             halfSteps             = HALF_STEPS_AT_RESET_POSITION;     // one and only variable that maintains position of the motor.
unsigned char   cmdBuf[CMD_BUF_LEN];
unsigned int    cmdBufIndex           = 0;
bool            runMotor              = false;
unsigned int    delayMs;
unsigned char   stepType              = INTERLEAVE;
int             targetHalfSteps        = -1;
bool            isCounterClockwise    = true;
bool            doHalfStep            = false;

/*
 * Executed only once at bootup.
 */
void setup()
{
  Serial.begin(115200);           // set up Serial library at 9600 bps
  Serial.println("Rotating vector application ready");

  calculate_delay_ms();
}

/*
 * 
 */
void calculate_delay_ms()
{
  float ms_in_1_minute          = 1000.0 * 60;
  float steps_per_revolution;
  
  if (stepType == INTERLEAVE) {
    steps_per_revolution = HALF_STEPS_PER_REVOLUTION;
  }
  else {
    // step type is SINGLE or DOUBLE
    steps_per_revolution = STEPS_PER_REVOLUTION;
  }

  float one_rpm_delay_in_ms     = ms_in_1_minute / steps_per_revolution;
  
  delayMs                       = one_rpm_delay_in_ms / rpm;
  
}

/*
 * 
 */
void configure_motor_rpm(int rpm_to_set)
{
  rpm = rpm_to_set;
  calculate_delay_ms();
  
  runMotor = true;
  
  Serial.print("Running motor at ");
  Serial.print(rpm);
  Serial.println(" rpm");
}

/*
 * Read commands from PC. Each command is a character followed by a new line. 
 * If there are more than one non-newline characters before a newline, each of them is interpreted as a command.
 */
void ReadAndExecuteCommand()
{
  bool cmdReady = false;
  unsigned char data;
  
  //---------------------------------------------------------------------------------
  // Read a serial byte, if available.  Save to command buffer. Increment index.
  // If received byte was new line, replace it with a null byte (string terminator).
  // If more bytes received than allocated for, restart saving from beginning of buffer.
  //---------------------------------------------------------------------------------
  if (Serial.available())
  {
    data = Serial.read();
    // Serial.println(data);

    if (data == '\n')
      data = 0;
    
    cmdBuf[cmdBufIndex++] = data;       // save data byte to buffer. increment index.
    
    if (data == 0) {
      cmdReady = true;

      // cmd will be handled before leaving this function.
      // cmdBufIndex will be set to zero immediately after executing the command.
    }
    else {
      if (cmdBufIndex >= CMD_BUF_LEN) {
        cmdBufIndex = 0;    // discard everything we got so far and start over.
      }
    }
  } // if Serial.available()

  //------------------------------------------------
  // If command is ready, handle it.
  //------------------------------------------------
  if (cmdReady)
  {
    // cmdBuf is guaranteed to be a null terminated string
    
    int cmdLen            = strlen((const char*)cmdBuf);
    unsigned char cmdId;

    // any new command invalidates the 'g' command if it was in progress.
    targetHalfSteps = -1;

    if (cmdLen > 0) {
      cmdId = cmdBuf[0];
      
      switch (cmdId)
      {
        case '1':
          configure_motor_rpm(3);
          break;
        case '2':
          configure_motor_rpm(4);
          break;
        case '3':
          configure_motor_rpm(5);
          break;
        case '4':
          configure_motor_rpm(6);
          break;
        case '5':
          configure_motor_rpm(10);
          break;
        case '6':
          configure_motor_rpm(13);
          break;

        //------------------------------------------------------------------------
        // go to a specific angle and stop.
        //------------------------------------------------------------------------
        case 'g':
        {
          float targetAngle = atof((const char*) (cmdBuf + 1));
          // keep delayMs unchanged so that the speed at which this happens is the same.
          targetHalfSteps = int(round(float(targetAngle * NUM_HALF_STEPS_PER_DEGREE)));

          Serial.print("Going to ");
          Serial.print(targetAngle);
          Serial.print(" degrees.  targetHalfSteps = ");
          Serial.println(targetHalfSteps);
          
          runMotor = true;     // run the motor in case we were paused.
          break;
        }

        //------------------------------------------------------------------------
        // select direction of vector rotation
        //------------------------------------------------------------------------
        case '<':
          isCounterClockwise = true;
          break;
        case '>':
          isCounterClockwise = false;
          break;

        //------------------------------------------------------------------------
        // half step only
        //------------------------------------------------------------------------
        case 'h':
          doHalfStep = true;
          runMotor = true;

          // if step type is not half step, it will be temporarily set to half step just before acutally stepping the motor.
          break;

        //------------------------------------------------------------------------
        // calibrate current half step to the given angle in degrees
        //------------------------------------------------------------------------
        case '=':
        {
          int calibrationAngle = atoi((const char*) (cmdBuf + 1));
          Serial.print("Calibrating current position to ");
          Serial.print(calibrationAngle);
          Serial.print(" degrees. ");
          
          halfSteps = int(round(calibrationAngle * NUM_HALF_STEPS_PER_DEGREE));
          Serial.print("New nstep = ");
          Serial.println(halfSteps >> 1);
          break;
        }
        
        //------------------------------------------------------------------------
        // handle commands setting the mode of energizing the coils.
        //------------------------------------------------------------------------
        case 's':
          stepType = SINGLE;
          break;
        case 'd':
          stepType = DOUBLE;
          break;
        case 'i':
          stepType = INTERLEAVE;
          break;      
        
        //------------------------------------------------------------------------
        // Reset 
        //------------------------------------------------------------------------
        case 'r':
          halfSteps = HALF_STEPS_AT_RESET_POSITION;
          motor.release();        // coils will be released. stick will fall to 270 degree position due to gravity.
          runMotor = false;
          Serial.println("Releasing motor coils.  Vector should fall to 270 degree position.");
          break;
        
        //------------------------------------------------------------------------
        // Pause the motor
        //------------------------------------------------------------------------
        case 'p':     
          runMotor = false;
          Serial.println("Pausing motor");
          break;

        //------------------------------------------------------------------------
        // continue running a paused motor
        //------------------------------------------------------------------------
        case 'c':
          runMotor = true;
          Serial.println("Resuming motor at speed set earlier");
          break;


        //------------------------------------------------------------------------
        // print internal variables for debug purpose
        //------------------------------------------------------------------------
        case '?':
          Serial.print("rpm=");
          Serial.print(rpm);
          Serial.print(", halfSteps=");
          Serial.print(halfSteps);
          Serial.print(", runMotor=");
          Serial.print(runMotor);
          Serial.print(", delayMs=");
          Serial.print(delayMs);
          Serial.print(", stepType=");
          Serial.print(stepType);
          Serial.print(", targetHalfSteps=");
          Serial.print(targetHalfSteps);
          Serial.print(", isCounterClockwise=");
          Serial.print(isCounterClockwise);
          Serial.print(", doHalfStep=");
          Serial.print(doHalfStep);
          Serial.println();
          
          break;
        
        default:
          // nothing to do
          break;
          
      } // switch(cmdId)
    } // if (cmdLen > 0)
    
    cmdReady = false;
    cmdBufIndex = 0;
  }
}

void loop()
{
  //----------------------------------------------
  // Turn motor by 1 step followed by the set delay
  //----------------------------------------------
  if (runMotor)
  {
    // Possible values for 2nd argument to .onestep() are
    //    - SINGLE - One coil is energized at a time.
    //    - DOUBLE - Two coils are energized at a time for more torque.
    //    - INTERLEAVE - Alternate between single and double to create a half-step in between.
    //                   This can result in smoother operation, but because of the extra half-step, the speed is reduced by half too.
    //    - MICROSTEP - Adjacent coils are ramped up and down to create a number of 'micro-steps' between each full step.
    //                  This results in finer resolution and smoother rotation, but with a loss in torque.
    //                  Abhir's note: microstepping does not work with motor.onestep()

    char oldStepType;
    
    //-------------------------------------
    // Backup step type if we are going to change it for single stepping.
    //-------------------------------------
    if (doHalfStep)
    {
      oldStepType = stepType;
      stepType = INTERLEAVE;
    }

    //------------------------------------------------------------------
    // Perform a single step in the set direction.
    //------------------------------------------------------------------
    if (isCounterClockwise)
      motor.onestep(BACKWARD, stepType);
     else
      motor.onestep(FORWARD, stepType); 

    
    //------------------------------------------------------------------
    // increment the step count. step count is stored in deci steps.    
    //------------------------------------------------------------------
    switch (stepType)
    {
      case SINGLE:
      case DOUBLE:
        if (isCounterClockwise)
          halfSteps += 2;
        else
          halfSteps -= 2;
        break;

      case INTERLEAVE:
        if (isCounterClockwise)
          halfSteps++;
        else
          halfSteps--;
        break;
      
    }

    //-------------------------------------
    // Restore ste type, in case it was temporarily changed for single stepping.
    //-------------------------------------
    if (doHalfStep)
    {
      stepType = oldStepType;
    }
    
    //----------------------------------------------
    // rebase to 0 if gone over a full revolution.
    //----------------------------------------------
    // for counterclockwise direction, deci steps will go over 200*10
    if (isCounterClockwise)
    {
      if (halfSteps >= HALF_STEPS_PER_REVOLUTION)
        halfSteps -= HALF_STEPS_PER_REVOLUTION;
    }
    else
    {
      // for clockwise direction, deci steps will be decremented, hence will go below zero.
      if (halfSteps <= 0)
        halfSteps += HALF_STEPS_PER_REVOLUTION;
    }
    
    //----------------------------------------------
    // If asked to advance by just 1 degree, pause the motor.
    //----------------------------------------------
    if (doHalfStep)
    {
      doHalfStep = false;     // we are done half stepping
      runMotor = false;         // pause the motor
    }
 
    //----------------------------------------------
    // were we asked to go to a specific angle?  If yes, and if we reached that, stop.
    //----------------------------------------------
    if ((targetHalfSteps != -1) && (targetHalfSteps == halfSteps))
    {
      runMotor = false;
      targetHalfSteps = -1;
      
      Serial.print("Reached ");
      Serial.print(halfSteps * NUM_DEGREES_PER_HALF_STEP);
      Serial.print(" degrees. halfSteps = ");
      Serial.print(halfSteps);
      Serial.println(". Pausing motor.");
    }

    //----------------------------------------------
    // print current position at regular intervals to let PC software know where we are.
    //----------------------------------------------
//    if ((halfSteps % (STEPS_PER_REVOLUTION / 16)) == 0) {      // print rposition 8 times every revolution

      // Print angle in degrees and number of half steps.
      Serial.print(halfSteps * NUM_DEGREES_PER_HALF_STEP);
      Serial.print(" ");
      Serial.println(halfSteps);
      
//    }

    //----------------------------------------------
    // apply a delay that was set earlier based on the desired rpm of the motor.
    //----------------------------------------------
     delay(delayMs);

    //-----------------------------------------------------------------------------------------------------
  } // if (runMotor)

  // Read commands that may have arrived from the PC. Execute them.
  ReadAndExecuteCommand();
}
