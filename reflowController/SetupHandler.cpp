#include "SetupHandler.h"
#include "Arduino.h"

SetupHandler::SetupHandler(PID* pid, PidParams* pidParams, Adafruit_MAX31855* thermoCouple) {  
  pid_ = pid; 
  pidParams_ = pidParams;
  thermoCouple_ = thermoCouple;
}

ReflowOperationState SetupHandler::runSetup() {
	
  pinMode(ReflowOperationState::PIN_GUN, OUTPUT);
  
  //tell the PID to range between 0 and the full window size
  pid_->SetOutputLimits(0, ReflowOperationState::WINDOW_SIZE);

  //turn the PID on
  pid_->SetMode(AUTOMATIC);
  
  Serial.begin(9600);
  
  
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  
  while (initTemp_ == 0) {
    initTemp_ = thermoCouple_->readCelsius();

    if (isnan(initTemp_)) {
       Serial.println("reinitializing...");
       initTemp_ = 0;
    } else if (initTemp_ < 5 || initTemp_ > 50) {
       Serial.println("reinitializing...");
       initTemp_ = 0;
    }
  }
  
  delay(1000);
  
  Serial.println("Testing gun");
  
  Serial.println("gun ON");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    HIGH);
  Serial.println(digitalRead(ReflowOperationState::PIN_GUN));
  delay(1000);
  
  Serial.println("gun OFF");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    LOW);
  Serial.println(digitalRead(ReflowOperationState::PIN_GUN));
  delay(1000);
  
  Serial.println("gun ON");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    HIGH);
  Serial.println(digitalRead(ReflowOperationState::PIN_GUN));
  delay(1000);
  
  Serial.println("gun OFF");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    LOW);
  Serial.println(digitalRead(ReflowOperationState::PIN_GUN));
  delay(1000);
  
  return ReflowOperationState(
        pidParams_,        
        millis(),    
        initTemp_,
        digitalRead(ReflowOperationState::PIN_GUN));
}


