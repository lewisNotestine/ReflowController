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

  //turn gun on or off based on PID output.
  reflowState_->evaluateWindowStartTime();

  //increment phaseIndex, change setPoint if necessary.    
  reflowState_->evaluatePhaseAndSetpoint();

  //calculate, set the PID input.  
  reflowState_->evaluateTargetTemp();  

  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");    
  } else {

    if (pid_->Compute()) {

      //TODO: is PIDINTERVAL_MILLIS the right thing to use as a comparison for PID output?
      if(pidParams_->getPidOutput() > 0) {
        digitalWrite(ReflowOperationState::PIN_GUN, HIGH);
        reflowState_->setGunState(true);
      } else {
        digitalWrite(ReflowOperationState::PIN_GUN, LOW);
        reflowState_->setGunState(false);
      }
      
      calculateDTDt(&dT, &dt, &c, &dTdt);    
      pidParams_->setPidInput(dTdt);
  
      Serial.print(pidParams_->getPidOutput());
      Serial.print(",");
      Serial.print(dT);
      Serial.print(",");
      Serial.print(dt);
      Serial.print(",");
      Serial.print(reflowState_->getCurrentTargetDTdt());    
      Serial.print(",");
      Serial.print(pidParams_->getPidInput());          
      Serial.print(",");
      Serial.print(reflowState_->getCurrentTargetTemp());  
      Serial.print(","); 
      Serial.print(c);
      Serial.print(",");
      Serial.print(reflowState_->getGunState());
      
  
      Serial.print('\n');
  
      reflowState_->setLastTemp(c);
  
      reflowState_->recordJustPrinted();
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


