/*
Adapted from Adafruit Max31855 breakout board code.

controls a heat gun based on thermocouple reading. 
Trying different algorithms, like simple on/off and temperature control reading versus PID algorithm.

*/

#include "Adafruit_MAX31855.h"
#include "PID_v1.h"


const int gunPin = 11;
const int thermoDO = 3;
const int thermoCS = 4;
const int thermoCLK = 5;

//Times are given as t (not period).
const double TEMP_THRESHOLD = 2.0;

const double PREHEAT_TIME = 120; //470;
const double PREHEAT_TEMP = 80;  //125;

const double SOAK_TIME = 40; //575;
const double SOAK_TEMP = 60;  //160;

const double REFLOW_TIME = 60; //675;
const double REFLOW_TEMP = 80;     //210;

const double COOLDOWN_TIME = 120;   //800;
const double COOLDOWN_TEMP = 60;   //160;
const int SECONDS = 120;



int             phaseIndex = 0;  //current state of the reflow.
int             windowSize = 200;
unsigned long   windowStartTime;
unsigned long   currentSecs;
unsigned long   currentMils;
unsigned long   startTime;
unsigned long   lastTime;
double          timeArray[4];
double          tempArray[4];
double          dTdtArray[4];
char            nameArray[4][10];
char            phaseType[10];  //name of the phase.
double          phaseTime = 0.0;  //tracks the time.
double          totalTime = 0.0;
double          targetTemp;  //current target temperature.
double          initTemp = 0;
double          lastTemp = 0;

bool            gunIsOn = false;
bool            printed;
int             lastPrinted;

//Define PID Variables 
double pidSetpoint, pidInput, pidOutput;

//PID controller.
PID myPID(&pidInput, &pidOutput, &pidSetpoint, 100, 10, 100, DIRECT);
//PID soakPID(&pidInput, &pidOutput, &pidSetpoint, 20, 1, 20, DIRECT);
//PID reflowPID(&pidInput, &pidOutput, &pidSetpoint, 20, 1, 20, DIRECT);
//PID cooldownPID(&pidInput, &pidOutput, &pidSetpoint, 20, 1, 20, DIRECT);

//thermocouple.
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);
  
void setup() { 

  pinMode(gunPin, OUTPUT);
  
  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, windowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  timeArray[0] = PREHEAT_TIME;
  tempArray[0] = PREHEAT_TEMP;
  strcpy(nameArray[0], "Preheat");
  //dTdtArray is populated after initTemp is recorded.
  
  timeArray[1] = SOAK_TIME;
  tempArray[1] = SOAK_TEMP;
  strcpy(nameArray[1], "Soak");
  dTdtArray[1] = (tempArray[1] - tempArray[0]) / (timeArray[1] - timeArray[0]);
  
  timeArray[2] = REFLOW_TIME;
  tempArray[2] = REFLOW_TEMP;
  strcpy(nameArray[2], "Reflow");
  dTdtArray[2] = (tempArray[2] - tempArray[1]) / (timeArray[2] - timeArray[1]);
  
  timeArray[3] = COOLDOWN_TIME;
  tempArray[3] = COOLDOWN_TEMP;
  strcpy(nameArray[3], "Cooldown");
  dTdtArray[3] = (tempArray[3] - tempArray[2]) / (timeArray[3] - timeArray[2]);  
  
  
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
  digitalWrite(gunPin, HIGH);
  Serial.println(digitalRead(gunPin));
  delay(1000);
  
  Serial.println("gun OFF");
  digitalWrite(gunPin, LOW);
  Serial.println(digitalRead(gunPin));
  delay(1000);
  
  Serial.println("gun ON");
  digitalWrite(gunPin, HIGH);
  Serial.println(digitalRead(gunPin));
  delay(1000);
  
  Serial.println("gun OFF");
  digitalWrite(gunPin, LOW);
  Serial.println(digitalRead(gunPin));
  delay(1000);
  
  windowStartTime = millis();
  startTime = windowStartTime;
  //pidSetpoint = 40.0;
  pidSetpoint = dTdtArray[0];
}

void loopFunc() {

   double dT;
   double dt;
   
   double dTdt = 0;    //rate of temp change current.
   double c = thermocouple.readCelsius();
   
  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");
    phaseTime += 1.0;
    totalTime += 1.0;
     
  } else {


    //set the PID setpoint.
    //get current time in milliseconds, interpolate from current phase. 
    currentMils = (millis() - startTime);
    currentSecs = (millis() - startTime) / 1000;

    //increment phaseIndex, change setPoint if necessary.
    if (currentSecs >= timeArray[phaseIndex]) {
      phaseIndex++;
      pidSetpoint = dTdtArray[phaseIndex];
    }

    //calculate, set the PID input.
    if (phaseIndex == 0) {
      targetTemp = (currentSecs * dTdtArray[phaseIndex]) + initTemp;
    } else {
      targetTemp = ((currentSecs - timeArray[phaseIndex - 1]) * dTdtArray[phaseIndex]) + tempArray[phaseIndex - 1];
    }

    /*changing these to update every second*/
    /*dT = c - initTemp;
    dt = (currentMils - startTime) / 1000.0;

    //dT = c - lastTemp;
    //dt = (currentMils - lastTime) / 1000.0;

    if (dt != 0) {
      dTdt = dT / dt;
      /*if (dTdt == -1) {
        dTdt = dT / dt;  //initialize the measurement.
      } else {
        dTdt = (dTdt + (dT / dt))/ 2.0; //use a moving average to get rid of noise or bias.
      }
      
    } else {
      dTdt = 0;
    }*/

    
    //Compute.
    myPID.Compute();

    //turn gun on or off based on PID output.
    if(millis() - windowStartTime > windowSize) { //time to shift the Relay Window
      windowStartTime += windowSize;
    }
    
    /* PID control*/

    if(pidOutput > millis() - windowStartTime) {
      digitalWrite(gunPin,HIGH);
      gunIsOn = true;
    } else {
      digitalWrite(gunPin,LOW);
      gunIsOn = false;
    }

    printed = (currentSecs == lastPrinted);

    //print everything every second, also reset the dTdt comparanda.
    if (!printed) {
      dT = c - lastTemp;
      dt = (currentMils - lastTime) / 1000.0;

      //dT = c - lastTemp;
      //dt = (currentMils - lastTime) / 1000.0;

      if (dt != 0) {
        dTdt = dT / dt;
        /*if (dTdt == -1) {
          dTdt = dT / dt;  //initialize the measurement.
        } else {
          dTdt = (dTdt + (dT / dt))/ 2.0; //use a moving average to get rid of noise or bias.
        }*/
        
      } else {
        dTdt = 0;
      }

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
      //Serial.print(c - targetTemp);
      Serial.print("; ");

      Serial.print("Target Temp = C ");
      Serial.print(targetTemp);
      Serial.print("; ");

      //print the observed temperature.
      Serial.print("Current Temp = C "); 
      Serial.print(c);
      Serial.print("; ");

      //pidInput = dTdt;

      lastTemp = c;
      lastTime = currentMils;
      lastPrinted = currentSecs;
    }
  }
}

void loop() {
  //loopFunc();
}
