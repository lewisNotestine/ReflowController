#include "LoopHandler.h"
#include "Arduino.h"

LoopHandler::LoopHandler(PID* pid, PidParams* pidParams, ReflowOperationState* reflowState, Adafruit_MAX31855* thermocouple) {
	pid_ = pid;
  pidParams_ = pidParams;
	reflowState_ = reflowState;
	thermocouple_ = thermocouple;
}

void LoopHandler::handleLoop() {
   double dT;
   double dt; 
   double dTdt = 0;    //rate of temp change current.
   double c = thermocouple.readCelsius();
   
  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");    
    reflowState_->incrementPhaseTime();
    reflowState_->incrementTotalTime();     
  } else {

    //set the PID setpoint.
    //get current time in milliseconds, interpolate from current phase. 
    reflowState_->updateTime();

    //increment phaseIndex, change setPoint if necessary.
    if (reflowState->getCurrentSecs() >= reflowState_->getTimeAt(phaseIndex)) {
      reflowState_->incrementPhaseIndex();
      pidSetpoint = dTdtArray[phaseIndex];
    }

    //calculate, set the PID input.
    if (phaseIndex == 0) {
      targetTemp = (currentSecs * dTdtArray[phaseIndex]) + initTemp;
    } else {
      targetTemp = ((currentSecs - timeArray[phaseIndex - 1]) * dTdtArray[phaseIndex]) + tempArray[phaseIndex - 1];
    }
    
    //Compute.
    myPID.Compute();

    //turn gun on or off based on PID output.
    if(millis() - windowStartTime > ReflowOperationState::WINDOW_SIZE) { //time to shift the Relay Window
      windowStartTime += ReflowOperationState::WINDOW_SIZE;
    }
    
    /* PID control*/

    if(pidOutput > millis() - windowStartTime) {
      digitalWrite(
        ReflowOperationState::PIN_GUN,
        HIGH);
      gunIsOn = true;
    } else {
      digitalWrite(ReflowOperationState::PIN_GUN,
        LOW);
      gunIsOn = false;
    }

    printed = (currentSecs == lastPrinted);

    //print everything every second, also reset the dTdt comparanda.
    if (!printed) {

      calculateDTDt(&dT, &dt, &c, &dTdt);

      pidInput = dTdt;
      Serial.print("pid OUTPUT: ");
      Serial.print(pidOutput);
      Serial.print("; ");

      Serial.print("dT: ");
      Serial.print(dT);

      Serial.print("; dt: ");
      Serial.println(dt);

      Serial.print("Target dTdt: ");
      Serial.print(dTdtArray[phaseIndex]);
      Serial.print("; ");

      Serial.print("dTdt: ");
      Serial.print(pidInput);      
      Serial.print("; ");

      Serial.print("Target Temp = C ");
      Serial.print(targetTemp);
      Serial.print("; ");

      //print the observed temperature.
      Serial.print("Current Temp = C "); 
      Serial.print(c);
      Serial.print("; ");

      lastTemp = c;
      lastTime = currentMils;
      lastPrinted = currentSecs;
    }
  }
}

void LoopHandler::calculateDTDt(double* dT, double* dt, double* c, double* dTdtOutput) {
  *dT = *c - *lastTemp;
  *dt = (reflowState_->getCurrentMils() - reflowState_->getLastTime()) / 1000.0;

  if (*dt != 0) {
    *dTdtOutput = *dT / *dt;
  } else {
    *dTdtOutput = 0;
  }
}

void LoopHandler::evaluatePhaseIndex() {

}