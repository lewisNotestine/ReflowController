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
   double c = thermocouple_->readCelsius();
   
   Serial.print("current C: ");
   Serial.print(c);
   Serial.print('\n');

  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");    
    reflowState_->incrementPhaseTime();
    reflowState_->incrementTotalTime();     
  } else {

    //set the PID setpoint.
    //get current time in milliseconds, interpolate from current phase. 
    reflowState_->updateTime();

    //increment phaseIndex, change setPoint if necessary.    
    reflowState_->evaluatePhaseAndSetpoint();
    
    //calculate, set the PID input.  
    reflowState_->evaluateTargetTemp();  
    
    //Compute.
    pid_->Compute();

    //turn gun on or off based on PID output.
    reflowState_->evaluateWindowStartTime();
    
    /* PID control*/

    if(pidParams_->getPidOutput() > millis() - reflowState_->getWindowStartTime()) {
      digitalWrite(
        ReflowOperationState::PIN_GUN,
        HIGH);
        reflowState_->setGunState(true);
    } else {
      digitalWrite(ReflowOperationState::PIN_GUN,
        LOW);
        reflowState_->setGunState(false);
    }

    reflowState_->evaluatePrinted();

    //print everything every second, also reset the dTdt comparanda.
    if (!reflowState_->getPrinted()) {

      calculateDTDt(&dT, &dt, &c, &dTdt);

      pidParams_->setPidInput(dTdt);
      Serial.print("pid OUTPUT: ");
      Serial.print(pidParams_->getPidOutput());
      Serial.print("; ");

      Serial.print("dT: ");
      Serial.print(dT);

      Serial.print("; dt: ");
      Serial.println(dt);

      Serial.print("Target dTdt: ");
      Serial.print(reflowState_->getCurrentTargetDTdt());
      Serial.print("; ");

      Serial.print("dTdt: ");
      Serial.print(pidParams_->getPidInput());      
      Serial.print("; ");

      Serial.print("Target Temp = C ");
      Serial.print(reflowState_->getCurrentTargetTemp());
      Serial.print("; ");

      //print the observed temperature.
      Serial.print("Current Temp = C "); 
      Serial.print(c);
      Serial.print("; ");

      reflowState_->setLastTemp(c);

      reflowState_->evaluatePrintedTime();
    }
  }
}

void LoopHandler::calculateDTDt(double* dT, double* dt, double* c, double* dTdtOutput) {
  *dT = *c - reflowState_->getLastTemp();
  *dt = (reflowState_->getCurrentMils() - reflowState_->getLastTime()) / 1000.0;

  if (*dt != 0) {
    *dTdtOutput = *dT / *dt;
  } else {
    *dTdtOutput = 0;
  }
}

