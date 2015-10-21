/*
Adapted from Adafruit Max31855 breakout board code.

controls a heat gun based on thermocouple reading. 
Trying different algorithms, like simple on/off and temperature control reading versus PID algorithm.

*/
#pragma once

#include "Adafruit_MAX31855.h"
#include "PID_v1.h"
#include "SetupHandler.h"
#include "ReflowOperationState.h"



// //TODO: these things are in ReflowOperationsState. Get rid of them here.
// int             phaseIndex = 0;  //current state of the reflow.
// unsigned long   windowStartTime;
// unsigned long   currentSecs;
// unsigned long   currentMils;
// unsigned long   startTime;
// unsigned long   lastTime;
// double          timeArray[4] = { PREHEAT_TIME, SOAK_TIME, REFLOW_TIME, COOLDOWN_TIME };
// double          tempArray[4] = { PREHEAT_TEMP, SOAK_TEMP, REFLOW_TEMP, COOLDOWN_TEMP };
// double          dTdtArray[4];
// char            nameArray[4][10];
// char            phaseType[10];  //name of the phase.
// double          phaseTime = 0.0;  //tracks the time.
// double          totalTime = 0.0;
// double          targetTemp;  //current target temperature.
// double          initTemp = 0;
// double          lastTemp = 0;

// bool            gunIsOn = false;
// bool            printed;
// int             lastPrinted;

//Define PID Variables 
double pidSetpoint, pidInput, pidOutput;
PidParams pidParams(&pidSetpoint, &pidInput, &pidOutput);

//PID controller.
PID myPID(&pidInput, &pidOutput, &pidSetpoint, 100, 10, 100, DIRECT);

//thermocouple.
Adafruit_MAX31855 thermocouple(
  ReflowOperationState::PIN_THERMO_CLK, 
  ReflowOperationState::PIN_THERMO_CS, 
  ReflowOperationState::PIN_THERMO_DO);

ReflowOperationState* reflowState = 0;

SetupHandler setupHandler(&myPID);  

LoopHandler loopHandler(&myPid, &pidParams, reflowState, &thermocouple);

void setup() { 
  reflowState = setupHandler.runSetup();
}

void loopFunc() {
  LoopHandler.handleLoop();
}

void loop() {
  if (!ReflowOperationState::TEST_ONLY) {
    loopFunc();
  }
}

