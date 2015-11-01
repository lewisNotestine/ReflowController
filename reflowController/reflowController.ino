#pragma once
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "PID_v1.h"
#include "SetupHandler.h"
#include "ReflowOperationState.h"
#include "LoopHandler.h"

/*
Adapted from Adafruit Max31855 breakout board code.

controls a heat gun based on thermocouple reading. 
Trying different algorithms, like simple on/off and temperature control reading versus PID algorithm.
*/

//Define PID Variables 
double pidSetpoint, pidInput, pidOutput;
PidParams pidParams(&pidSetpoint, &pidInput, &pidOutput);

//PID controller.
PID pid(&pidInput, &pidOutput, &pidSetpoint, 100, 10, 100, DIRECT);

//thermocouple.
Adafruit_MAX31855 thermocouple(
  ReflowOperationState::PIN_THERMO_CLK, 
  ReflowOperationState::PIN_THERMO_CS, 
  ReflowOperationState::PIN_THERMO_DO);

ReflowOperationState* reflowState = 0;

SetupHandler setupHandler(&pid, &pidParams, &thermocouple);  

LoopHandler loopHandler(&pid, &pidParams, reflowState, &thermocouple);

void setup() { 
  *reflowState = setupHandler.runSetup();
}

void loop() {
  if (!ReflowOperationState::TEST_ONLY) {
    loopHandler.handleLoop();
  }
}


