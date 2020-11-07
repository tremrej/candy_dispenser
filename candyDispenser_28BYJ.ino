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
#include "pusher.h"

typedef enum {
    idle_c,
    running_1_c,
    running_2_c,
    running_3_c
} DispenserState_e;

DispenserState_e state_g = idle_c;

// Stepper on shield V1
AF_Stepper motor1(200, 1);    // 200 steps, port 1
AF_Stepper motor2(200, 2);    // 200 steps, port 2

// Stepper on shield V2
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_StepperMotor *motor3 = AFMS.getStepper(200, 1);

#define buttonTrigger A0
#define switchHome1 A1
#define switchHome2 A2
#define switchHome3 A3

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

Pusher pusher1 (400, 28.58, 125, &stepper2, switchHome1);
Pusher pusher2 (400, 28.58, 100, &stepper1, switchHome2);
Pusher pusher3 (400, 28.58, 100.0, &stepper3, switchHome3);


void setup()
{  
   Serial.begin(115200);           // set up Serial library at 9600 bps
   Serial.println("stepper1 test!");

   pinMode(buttonTrigger, INPUT_PULLUP);     // LOW when pressed.
   pinMode(switchHome1, INPUT_PULLUP);        // LOW when home.
   pinMode(switchHome2, INPUT_PULLUP);        // LOW when home.
   pinMode(switchHome3, INPUT_PULLUP);        // LOW when home.
   
   // Init shield V2
   AFMS.begin();  // create with the default frequency 1.6KHz
  
   stepper1.setMaxSpeed(400);	     // INTERLEAVE
   stepper1.setAcceleration(2000);	
   stepper2.setMaxSpeed(400);	     // INTERLEAVE
   stepper2.setAcceleration(2000);	
   stepper3.setMaxSpeed(400);	     // INTERLEAVE
   stepper3.setAcceleration(2000);	

   pusher1.stop();
   pusher2.stop();
   pusher3.stop();
}

void loop()
{  
    if (state_g == idle_c)
    {
        // Check the trigger button
        if (digitalRead(buttonTrigger) == LOW ||
            Serial.read() == 'B')
        {
            // Button pressed
            state_g = running_1_c;
            pusher1.startCycle();
        }
    }
    else if (state_g == running_1_c)
    {
        if (pusher1.run() == Pusher::idle_c)
        {
            // Pusher 1 is done.
            // Let start pusher 2
            state_g = running_2_c;
            pusher2.startCycle();
        }
    } else if (state_g == running_2_c)
    {
        if (pusher2.run() == Pusher::idle_c)
        {
            // Pusher 2 is done.
            // Let start pusher 3
            state_g = running_3_c;
            pusher3.startCycle();
        }
    } else if (state_g == running_3_c)
    {
        if (pusher3.run() == Pusher::idle_c)
        {
            // Pusher 3 is done.
            state_g = idle_c;
        }
    }

}
