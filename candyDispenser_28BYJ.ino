// AFMotor_ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
// Requires the AFMotor library
// (https://github.com/adafruit/Adafruit-Motor-Shield-library)
// Caution, does not work with Adafruit Motor Shield V2
// See https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library 
// for examples that work with Adafruit Motor Shield V2.

#include <AccelStepper.h>
#include <AFMotor.h>

AF_Stepper motor1(200, 1);    // 200 steps, port 1

// Circonference nominal (20 teeth, 9 pitch)
#define circonference  (3.1416 * 28.68)
int distanceToStep(int distance)
{
    //return (distance / circonference) * 200;
    return (distance / circonference) * 200;   // INTERLEAVE
}

#define buttonTrigger A0
#define switchHome A1

enum OperationSate_e {
    home_c,       // Go to home position
    idle_c,
    active_c
};

enum Direction_e {
    push_c,
    pull_c
};


// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
void forwardstep() {  
  motor1.onestep(FORWARD, INTERLEAVE);
}
void backwardstep() {  
  motor1.onestep(BACKWARD, INTERLEAVE);
}

AccelStepper stepper(forwardstep, backwardstep); // use functions to step

void setup()
{  
   Serial.begin(115200);           // set up Serial library at 9600 bps
   Serial.println("Stepper test!");

   pinMode(buttonTrigger, INPUT_PULLUP);     // LOW when pressed.
   pinMode(switchHome, INPUT_PULLUP);        // LOW when home.

  
   //stepper.setMaxSpeed(200);	     // SINGLE, DOUBLE
   stepper.setMaxSpeed(400);	     // INTERLEAVE
   stepper.setAcceleration(2000);	
}

Direction_e direction_g = push_c;

OperationSate_e state_g = idle_c;

void loop()
{  

    if (state_g == active_c)
    {
        if (direction_g == pull_c)
        {
            // Go back to zero
            stepper.moveTo(+0);
            stepper.run();
            if (stepper.distanceToGo() == 0)
            {
                 // Reverse the direction
                 state_g = idle_c;
                 motor1.release();
            }
        }
        else if (direction_g == push_c)
        {
            // Push a candy
            //stepper.moveTo(+225);       // INTERLEAVE
            stepper.moveTo(distanceToStep(90));
            stepper.run();
            //Serial.print("dtg:");
            //Serial.println(stepper.distanceToGo());
            //Serial.println(stepper.speed());
            if (stepper.distanceToGo() == 0)
            {
                // At the end,
                // Reverse the direction
                direction_g = 1;
            }
        }
    }
    else if (state_g == home_c)
    {
        if (digitalRead(switchHome) == LOW)
        {
            // We're at home
            // Begin a cycle
            stepper.stop();
            stepper.setCurrentPosition(0);
            state_g = active_c;
        }
        else
        {
            // Move slowly toward home
            //stepper.setSpeed(-100);   // SINGLE, DOUBLE
            stepper.setSpeed(-200);   // INTERLEAVE
            stepper.runSpeed();
        }

    }
    else    // idle
    {
        // Check the trigger button
        if (digitalRead(buttonTrigger) == LOW)
        {
            // Button pressed
            direction_g = push_c;
            //state_g = active_c;
            state_g = home_c;
        }
    }



//        else
//        {
//           stepper.moveTo(0);
//           stepper.run();
//           if (stepper.distanceToGo() == 0)
//           {
//               delay(750);
//               direction_g = 0;
//           }
//        }
}
