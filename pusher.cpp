/*!
 * @file pusher.cpp
 *
 * Definition of the class Pusher.
 * It implemented a state machine to control a cycle to push a candy out.
 *
 */

#include "pusher.h"

Pusher::Pusher( int           stepPerRotation
              , float         pitchRadius
              , float         distanceToPush
              , AccelStepper *motor
              , int           switchHome)
{
    stepPerRotation_m = stepPerRotation;
    pitchRadius_m     = pitchRadius;
    motor_m           = motor;
    switchHome_m      = switchHome;

    state_m           = idle_c;
    candyCnt_m        = 0;
    nbStepToPush_m = distanceToStep(distanceToPush);
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
    else if (state_m == goingHome_c)
    {
        if (digitalRead(switchHome_m) == LOW)
        {
            // We're at home
            motor_m->stop();
            motor_m->setCurrentPosition(0);
            // Start pushing
            state_m = pushing_c;
        }
        else
        {
            // Move slowly toward home
            motor_m->setSpeed(-stepPerRotation_m/2);
            motor_m->runSpeed();
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
        // Go back to zero
        motor_m->moveTo(+0);
        motor_m->run();
        if (motor_m->distanceToGo() == 0 ||
            digitalRead(switchHome_m) == LOW)
        {
             // End of cycle
             state_m = idle_c;
             motor_m->release();
             candyCnt_m++;
        }
    }

    return state_m;
}

Pusher::PusherState_e Pusher::startCycle()
{
    state_m = goingHome_c;
    return state_m;
}

void Pusher::stop()
{
    state_m = idle_c;
    motor_m->release();
}

int Pusher::distanceToStep(float distance)
{
    float cir = 3.1416 * (pitchRadius_m * 2.0);
    return (distance / cir) * stepPerRotation_m;
}

