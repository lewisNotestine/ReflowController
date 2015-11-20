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
PID pid(&pidInput, &pidOutput, &pidSetpoint, 20, 1, 100, DIRECT);

//thermocouple.
Adafruit_MAX31855 thermocouple(
  ReflowOperationState::PIN_THERMO_CLK, 
  ReflowOperationState::PIN_THERMO_CS, 
  ReflowOperationState::PIN_THERMO_DO);

ReflowOperationState reflowState;

SetupHandler setupHandler(&pid, &pidParams, &thermocouple);  

LoopHandler loopHandler(&pid, &pidParams, &reflowState, &thermocouple);

void setup() { 
  Serial.begin(4800);
    
  setupHandler.runSetup();  
  reflowState = ReflowOperationState(
  	&pidParams,
  	millis(),
  	thermocouple.readCelsius(),
  	digitalRead(ReflowOperationState::PIN_GUN));
}

void loop() {
  if (!ReflowOperationState::TEST_ONLY) {
    loopHandler.handleLoop();
  }
}



