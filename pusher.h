/*!
 * @file pusher.cpp
 *
 * Definition of the class Pusher.
 * It implementes a state machine to control a cycle to push a candy out.
 *
 */

#ifndef pusher_h_
#define pusher_h_

#include <AccelStepper.h>

class Pusher
{
public:

    typedef enum {
        idle_c,
        goingHome_c,
        pushing_c,
        goingBackHome_c
    } PusherState_e;

    Pusher( int           stepPerRotation
          , float         pitchRadius
          , float         distanceToPush
          , AccelStepper *motor
          , int           switchHome);     // Arduino port number of the switch home.

    ~Pusher();

    PusherState_e run();

    PusherState_e startCycle();

    void stop();

    PusherState_e getCurrentState() {return state_m;};

    void resetCnt() {candyCnt_m = 0;};
    int  getCnt() {return candyCnt_m;};

private:

    int distanceToStep(float distance);

    int           stepPerRotation_m;
    float         pitchRadius_m;
    AccelStepper *motor_m;
    int           switchHome_m;         // input port of home limit switch
    float         distanceToPush_m;     // mm

    int           nbStepToPush_m;
    PusherState_e state_m;
    int           candyCnt_m;

};

#endif  //pusher_h_
