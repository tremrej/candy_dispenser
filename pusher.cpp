/*!
 * @file pusher.cpp
 *
 * Definition of the class Pusher.
 * It implementes a state machine to control a cycle to push a candy out.
 *
 */

#include "pusher.h"

Pusher::Pusher( int           stepPerRotation
              , float         pitchRadius
              , float         distanceToPush
              , float         distanceAtHome
              , AccelStepper *motor
              , int           switchHome)
{
    stepPerRotation_m = stepPerRotation;
    pitchRadius_m     = pitchRadius;
    motor_m           = motor;
    switchHome_m      = switchHome;

    state_m           = idle_c;
    homeState_m       = notHome_c;
    candyCnt_m        = 0;
    distanceToPush_m = distanceToPush;
    distanceAtHome_m = distanceAtHome;
    nbStepToPush_m = distanceToStep(distanceToPush);
    nbStepAtHome_m = distanceToStep(distanceAtHome);
}

Pusher::~Pusher()
{
    ;
}

Pusher::PusherState_e Pusher::run()
{

    if (state_m == idle_c)
    {
        // Nothing to do.
        // Waiting for a cycle to start
        ;
    }
    else if (state_m == goingToZero_c)
    {
        if (digitalRead(switchHome_m) == LOW)
        {
            // We're at zero, i.e. the limit switch
            motor_m->stop();
            motor_m->setCurrentPosition(0);
            if (distanceAtHome_m == 0.0)
            {
                // Start pushing
                state_m = pushing_c;
            }
            else
            {
                state_m = homing_c;
            }
        }
        else
        {
            // Move slowly toward zero (limit switch)
            motor_m->setSpeed(-stepPerRotation_m/2);
            motor_m->runSpeed();
        }
    }
    else if (state_m == homing_c)
    {
        // Going to predefined home position
        motor_m->moveTo(nbStepAtHome_m);
        motor_m->run();
        if (motor_m->distanceToGo() == 0)
        {
            // At the end,
            // Reverse the direction
            homeState_m = atHome_c;

            // Start pushing
            state_m = pushing_c;
        }
    }
    else if (state_m == pushing_c)
    {
        // Push a candy
        motor_m->moveTo(nbStepToPush_m);
        motor_m->run();
        if (motor_m->distanceToGo() == 0)
        {
            // At the end,
            // Reverse the direction
            state_m = goingBackHome_c;
        }
    }
    else if (state_m == goingBackHome_c)
    {
        // Go back to home
        motor_m->moveTo(nbStepAtHome_m);
        motor_m->run();
        if (motor_m->distanceToGo() == 0 ||
            digitalRead(switchHome_m) == LOW)
        {
             // End of cycle
             state_m = idle_c;
             //motor_m->release();
             candyCnt_m++;
        }
    }

    return state_m;
}

Pusher::PusherState_e Pusher::startCycle()
{
    // If distanceAtHome is zero then we always home the device.
    if (distanceAtHome_m == 0.0)
    {
        state_m = goingToZero_c;
    }
    else
    {
        if (homeState_m == atHome_c && 
            digitalRead(switchHome_m) != LOW)    // Not at the limit switch
        {
            // The device is already home. We can start pushing.
            state_m = pushing_c;
        }
        else
        {
            state_m = goingToZero_c;
        }
    }
        
    return state_m;
}

void Pusher::stop()
{
    state_m = idle_c;
    //motor_m->release();
}

int Pusher::distanceToStep(float distance)
{
    float cir = 3.1416 * (pitchRadius_m * 2.0);
    return (distance / cir) * stepPerRotation_m;
}

