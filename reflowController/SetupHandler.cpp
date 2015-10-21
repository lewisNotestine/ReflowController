#include "SetupHandler.h"
#include "Arduino.h"

SetupHandler::SetupHandler(PID* pid) {  
  pid_ = pid; 
}

ReflowOperationState* SetupHandler::runSetup() {
	
  pinMode(ReflowOperationState::PIN_GUN, OUTPUT);
  
  //tell the PID to range between 0 and the full window size
  pid_->SetOutputLimits(0, ReflowOperationState::WINDOW_SIZE);

  //turn the PID on
  pid_->SetMode(AUTOMATIC);
  
  strcpy(nameArray[0], "Preheat");
  //dTdtArray is populated after initTemp is recorded.
    
  strcpy(nameArray[1], "Soak");
  //dTdtArray[1] = (tempArray[1] - tempArray[0]) / (timeArray[1] - timeArray[0]);
     
  strcpy(nameArray[2], "Reflow");
  //dTdtArray[2] = (tempArray[2] - tempArray[1]) / (timeArray[2] - timeArray[1]);
  
  strcpy(nameArray[3], "Cooldown");
  //dTdtArray[3] = (tempArray[3] - tempArray[2]) / (timeArray[3] - timeArray[2]);  
  
  
  Serial.begin(9600);
  
  
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  
  while (initTemp == 0) {
    initTemp = thermocouple.readCelsius();
    
    if (isnan(initTemp)) {
       Serial.println("reinitializing...");
       initTemp = 0;
    } else if (initTemp < 5 || initTemp > 50) {
       Serial.println("reinitializing...");
       initTemp = 0;
    }
  }
  
  lastTemp = initTemp;
  dTdtArray[0] = (tempArray[0] - initTemp) / timeArray[0];
  
  
  int currentPhase = 0;
  double rate = dTdtArray[currentPhase];
  
  delay(1000);
  
  Serial.println("Testing gun");
  
  Serial.println("gun ON");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    HIGH);
  Serial.println(digitalRead(pinForGun_));
  delay(1000);
  
  Serial.println("gun OFF");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    LOW);
  Serial.println(digitalRead(pinForGun_));
  delay(1000);
  
  Serial.println("gun ON");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    HIGH);
  Serial.println(digitalRead(pinForGun_));
  delay(1000);
  
  Serial.println("gun OFF");
  digitalWrite(
    ReflowOperationState::PIN_GUN,
    LOW);
  Serial.println(digitalRead(pinForGun_));
  delay(1000);
  
  windowStartTime = millis();
  startTime = windowStartTime;
  pidSetpoint = dTdtArray[0];
}

