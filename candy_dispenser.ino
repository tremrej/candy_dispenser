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

// To compile:
// Board type Arduino Nano (Old bootloader)

#include <AccelStepper.h>
#include "pusher.h"

typedef enum {
    idle_c,
    running_1_c,
    running_2_c,
    running_3_c
} DispenserState_e;

DispenserState_e state_g = idle_c;

#define buttonTrigger A0
#define buttonTrigger2 11    // pin servo on Eleksmana v5.2
#define switchHome1 A1
#define switchHome2 A2
#define switchHome3 A3

#define motor1StepPin 2    // X
#define motor2StepPin 3    // Y
#define motor3StepPin 4    // Z
#define motor1DirPin  5
#define motor2DirPin  6
#define motor3DirPin  7

#define motorEnablePin 8


AccelStepper stepper1 = AccelStepper(AccelStepper::DRIVER, motor1StepPin, motor1DirPin);
AccelStepper stepper2 = AccelStepper(AccelStepper::DRIVER, motor2StepPin, motor2DirPin);
AccelStepper stepper3 = AccelStepper(AccelStepper::DRIVER, motor3StepPin, motor3DirPin);

//Pusher pusher1 (-400*8, 28.58, 125,  50, &stepper1, switchHome1);      // citrouille
Pusher pusher1 (-400*8, 28.58, 100,   0, &stepper1, switchHome1);        // Kitkat
Pusher pusher2 (-400*8, 28.58, 100,   0, &stepper2, switchHome2);        // Aero
Pusher pusher3 (-400*8, 28.58, 130.0, 0, &stepper3, switchHome3);        // Rocket


void setup()
{  
   Serial.begin(115200);           // set up Serial library at 9600 bps
   Serial.println("Distributeur de bonbons!");

   pinMode(buttonTrigger, INPUT_PULLUP);     // LOW when pressed.
   pinMode(buttonTrigger2, INPUT_PULLUP);     // LOW when pressed.
   pinMode(switchHome1, INPUT_PULLUP);        // LOW when home.
   pinMode(switchHome2, INPUT_PULLUP);        // LOW when home.
   pinMode(switchHome3, INPUT_PULLUP);        // LOW when home.

   pinMode(motorEnablePin, OUTPUT);
   pinMode(LED_BUILTIN, OUTPUT);
   
   stepper1.setMaxSpeed(4000);	     // INTERLEAVE
   stepper1.setAcceleration(30000);	
   stepper2.setMaxSpeed(4000);	     // INTERLEAVE
   stepper2.setAcceleration(30000);	
   stepper3.setMaxSpeed(4000);	     // INTERLEAVE
   stepper3.setAcceleration(30000);	

   pusher1.stop();
   pusher2.stop();
   pusher3.stop();

   digitalWrite(motorEnablePin, HIGH);
}

void loop()
{  

/*
        digitalWrite(LED_BUILTIN, HIGH);
        stepper1.moveTo(400*8);
        stepper1.runToPosition();
        
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(motorEnablePin, HIGH);
        //delay(2000);
        digitalWrite(motorEnablePin, LOW);
        stepper1.moveTo(0);
        stepper1.runToPosition();

        stepper2.moveTo(400*8);
        stepper2.runToPosition();
        stepper2.moveTo(0);
        stepper2.runToPosition();

        stepper3.moveTo(400*8);
        stepper3.runToPosition();
        stepper3.moveTo(0);
        stepper3.runToPosition();
*/

    if (state_g == idle_c)
    {
        // Check the trigger button
        if (digitalRead(buttonTrigger) == LOW ||
            digitalRead(buttonTrigger2) == LOW ||
            Serial.read() == 'B')
        {
            // Button pressed
            digitalWrite(motorEnablePin, LOW);
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
            digitalWrite(motorEnablePin, HIGH);
        }
    }

}
