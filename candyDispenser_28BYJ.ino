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
#include <Adafruit_MotorShield.h>


// Stepper on shield V1
AF_Stepper motor1(200, 1);    // 200 steps, port 1
AF_Stepper motor2(200, 2);    // 200 steps, port 2

// Stepper on shield V2
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_StepperMotor *motor3 = AFMS.getStepper(200, 1);



// Circonference nominal (20 teeth, 9 pitch)
#define circonference  (3.1416 * 28.68)
int distanceToStep(int distance)
{
    // rayon de pas pour 20 dents est 28.68 mm
    // 200 step par tour
    float cir = 3.1416 * (28.68 * 2.0);
    return (distance / cir) * 400;
}

int distanceToStepBYJ(float distance)
{
    // rayon de pas pour 6 dents est 8.6 mm
    float cir = 3.1416 * (8.6 * 2.0);
    float ttt = (distance / cir) * 513.0;
    return (int) ttt;
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
void forwardstep1() {  
  motor1.onestep(FORWARD, INTERLEAVE);
}
void backwardstep1() {  
  motor1.onestep(BACKWARD, INTERLEAVE);
}
void releaseStep1() {
    motor1.release();
}


void forwardstep2() {  
  motor2.onestep(FORWARD, INTERLEAVE);
}
void backwardstep2() {  
  motor2.onestep(BACKWARD, INTERLEAVE);
}
void releaseStep2() {
    motor2.release();
}

void forwardstep3() {  
  motor3->onestep(FORWARD, INTERLEAVE);
}
void backwardstep3() {  
  motor3->onestep(BACKWARD, INTERLEAVE);
}
void releaseStep3() {
    motor3->release();
}



AccelStepper stepper1(forwardstep1, backwardstep1, releaseStep1); // use functions to step
AccelStepper stepper2(forwardstep2, backwardstep2, releaseStep2); // use functions to step
AccelStepper stepper3(forwardstep3, backwardstep3, releaseStep3); // use functions to step


void setup()
{  
   Serial.begin(115200);           // set up Serial library at 9600 bps
   Serial.println("stepper1 test!");

   pinMode(buttonTrigger, INPUT_PULLUP);     // LOW when pressed.
   pinMode(switchHome, INPUT_PULLUP);        // LOW when home.
   
   // Init shield V2
   AFMS.begin();  // create with the default frequency 1.6KHz


  
   //stepper1.setMaxSpeed(200);	     // SINGLE, DOUBLE
   stepper1.setMaxSpeed(400);	     // INTERLEAVE
   stepper1.setAcceleration(2000);	
   stepper2.setMaxSpeed(400);	     // INTERLEAVE
   stepper2.setAcceleration(2000);	
   stepper3.setMaxSpeed(400);	     // INTERLEAVE
   stepper3.setAcceleration(2000);	
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
            stepper2.moveTo(+0);
            stepper2.run();
            if (stepper2.distanceToGo() == 0 ||
                digitalRead(switchHome) == LOW)
            {
                 // End of cycle
                 state_g = idle_c;
                 stepper1.release();
                 stepper2.release();
                 stepper3.release();
            }
        }
        else if (direction_g == push_c)
        {
            // Push a candy
            stepper2.moveTo(distanceToStep(120));
            //stepper2.moveTo(133);
            stepper2.run();
            if (stepper2.distanceToGo() == 0)
            {
                // At the end,
                // Reverse the direction
                direction_g = pull_c;
            }
        }
    }
    else if (state_g == home_c)
    {
        if (digitalRead(switchHome) == LOW)
        {
            // We're at home
            // Begin a cycle
            stepper2.stop();
            stepper2.setCurrentPosition(0);
            state_g = active_c;
            direction_g == push_c;
        }
        else
        {
            // Move slowly toward home
            stepper2.setSpeed(-200);   // INTERLEAVE
            stepper2.runSpeed();
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

}
