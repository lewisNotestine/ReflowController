#include "SetupHandler.h"
#include "Arduino.h"

SetupHandler::SetupHandler(PID* pid, PidParams* pidParams, Adafruit_MAX31855* thermoCouple) {  
  pid_ = pid; 
  pidParams_ = pidParams;
  thermoCouple_ = thermoCouple;  
}

void SetupHandler::runSetup() {
	
  double initTemp = 0.0;
  pinMode(ReflowOperationState::PIN_GUN, OUTPUT);
  
  //tell the PID to range between 0 and the full window size
  pid_->SetOutputLimits(0, ReflowOperationState::WINDOW_SIZE);

  //turn the PID on
  pid_->SetMode(AUTOMATIC);

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  
  while (initTemp == 0) {
    initTemp = thermoCouple_->readCelsius();
    Serial.print("initTemp = ");
    Serial.print(initTemp);

    if (isnan(initTemp)) {
       Serial.println("reinitializing...");
       initTemp = 0;
    } else if (initTemp < 5 || initTemp > 50) {
       Serial.println("reinitializing...");
       initTemp = 0;
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

}


