#include "ReflowOperationState.h"

ReflowOperationState::ReflowOperationState() {}
ReflowOperationState::ReflowOperationState(PidParams* pidParams,        
    unsigned long   currentMils,        
    double          initTemp,
    bool            gunIsOn) :
        pidParams_(pidParams),
        phaseIndex_(0),
        windowStartTime_(currentMils),        
        currentMils_(currentMils),
        startTime_(currentMils),
        lastTime_(currentMils),
        timeArray_({ PREHEAT_TIME, SOAK_TIME, REFLOW_TIME, COOLDOWN_TIME }),
        tempArray_({ PREHEAT_TEMP, SOAK_TEMP, REFLOW_TEMP, COOLDOWN_TEMP }),        
        phaseTime_(0),
        totalTime_(0),
        targetTemp_(PREHEAT_TEMP),
        initTemp_(initTemp),
        lastTemp_(initTemp),
        gunIsOn_(gunIsOn),
        printed_(false),
        lastPrinted_(0.0)
{
    strcpy(nameArray_[0], "Preheat");  
    strcpy(nameArray_[1], "Soak");
    strcpy(nameArray_[2], "Reflow");
    strcpy(nameArray_[3], "Cooldown");
    phaseName_ = nameArray_[0];
}

void ReflowOperationState::setPidSetpoint(double newSetpoint) {
    pidParams_->setPidSetpoint(newSetpoint);
}

double ReflowOperationState::getTargetDTdt(int reflowPhaseIndex) {
    //lazy-assign the value to dTdtArray because we don't expect it to change much.

    //This may be the case for the reflow phase. dTdt = 0.
    //Reflow phase is hard-coded because it's zero.
    if (reflowPhaseIndex > 0 && tempArray_[reflowPhaseIndex] == tempArray_[reflowPhaseIndex - 1])
    {
        return 0;
    } else {
        return tempArray_[reflowPhaseIndex] / timeArray_[reflowPhaseIndex];
    }
}



double ReflowOperationState::getCurrentTargetDTdt() {
    return getTargetDTdt(getPhaseIndex());
}

int ReflowOperationState::getPhaseIndex() {
    return phaseIndex_;
}

double ReflowOperationState::getPhaseTime() {
    return phaseTime_;
}

double ReflowOperationState::getTotalTime() {
    return totalTime_;
}

double ReflowOperationState::getCurrentMils() {
    return currentMils_;
}

double ReflowOperationState::getStartTime() {
    return startTime_;
}

double ReflowOperationState::getCurrentSecs() {
    return currentMils_ / 1000 ;
}

double ReflowOperationState::getTimeAt(int timeIndex) {
    return timeArray_[timeIndex];
}

long ReflowOperationState::getWindowStartTime() {
    return windowStartTime_;
}

void ReflowOperationState::setTargetTemp(double newTargetTemp) {
    targetTemp_ = newTargetTemp;
}

void ReflowOperationState::incrementPhaseIndex() {
    phaseIndex_++;
}

void ReflowOperationState::incrementPhaseTime() {
    phaseTime_ += 1.0;
}

void ReflowOperationState::incrementTotalTime() {
    totalTime_ += 1.0;
}

void ReflowOperationState::updateTime() {
    currentMils_ = (millis() - startTime_);
}               

void ReflowOperationState::evaluatePhaseAndSetpoint() {
    if (getCurrentSecs() >= getTimeAt(getPhaseIndex())) {
      incrementPhaseIndex();
      setPidSetpoint(getCurrentTargetDTdt());
    }
}

void ReflowOperationState::evaluateTargetTemp() {
    if (getPhaseIndex() == 0) {
      setTargetTemp((getCurrentSecs() * getCurrentTargetDTdt()) + initTemp_);
    } else {
      setTargetTemp( ((getCurrentSecs() - getTimeAt(getPhaseIndex() - 1)) * getCurrentTargetDTdt()) + getTargetTemp(getPhaseIndex() - 1) );      
    }
}

double ReflowOperationState::getTargetTemp(int phaseIndex) {
    return tempArray_[phaseIndex];
}

double ReflowOperationState::getCurrentTargetTemp() {
    return targetTemp_;
}

void ReflowOperationState::evaluateWindowStartTime() {
    if(millis() - windowStartTime_ > ReflowOperationState::WINDOW_SIZE) { //time to shift the Relay Window
      windowStartTime_ += ReflowOperationState::WINDOW_SIZE;
    }
}

void ReflowOperationState::setGunState(bool gunIsOn) {
    gunIsOn_ = gunIsOn;
}

bool ReflowOperationState::getPrinted() {
    return printed_;
}

void ReflowOperationState::setPrinted(bool newPrinted) {
    printed_ = newPrinted;
}

void ReflowOperationState::setLastTemp(double temp) {
    lastTemp_ = temp;
}

double ReflowOperationState::getLastTemp() {
    return lastTemp_;
}

void ReflowOperationState::evaluatePrinted() {
    printed_ = (getCurrentSecs() == lastPrinted_);
}

void ReflowOperationState::evaluatePrintedTime() {
    lastTime_ = getCurrentMils();
    lastPrinted_ = getCurrentSecs();
}

unsigned long ReflowOperationState::getLastTime() {
    return lastTime_;
}

void ReflowOperationState::printCurrentState() {    
    Serial.print("phaseIndex: "); Serial.print(getPhaseIndex()); Serial.print('\n');
    Serial.print("window start time"); Serial.print(getWindowStartTime());Serial.print('\n');    
    Serial.print("current mils"); Serial.print(getCurrentMils()); Serial.print('\n');
    Serial.print("current Secs"); Serial.print(getCurrentSecs()); Serial.print('\n');
    Serial.print("last time"); Serial.print(lastTime_); Serial.print('\n'); 
    
    Serial.println("Time Array");
    for (int i = 0; i < 4; i++) {
        Serial.print("index: "); Serial.print(i); Serial.print(", value: "); Serial.print(timeArray_[i]); Serial.print('\n');
    }
    
    Serial.println("Temp Array");
    for (int i = 0; i < 4; i++) {
        Serial.print("index: "); Serial.print(i); Serial.print(", value: "); Serial.print(tempArray_[i]); Serial.print('\n');
    }
    
    Serial.println("Target dTdt:");
    for (int i = 0; i < 4; i++) {
        Serial.print("index: "); Serial.print(i); Serial.print(", value: "); Serial.print(getTargetDTdt(i)); Serial.print('\n');
    }

    Serial.print("phase time: "); Serial.print(getPhaseTime()); Serial.print('\n');
    Serial.print("total time: "); Serial.print(getTotalTime()); Serial.print('\n');
    Serial.print("Target temp: "); Serial.print(getCurrentTargetTemp()); Serial.print('\n');
    Serial.print("Init Temp: "); Serial.print(initTemp_); Serial.print('\n');
    Serial.print("Last temp: "); Serial.print(getLastTemp()); Serial.print('\n');

    Serial.print("Gun is on: "); Serial.print(gunIsOn_); Serial.print('\n');
    Serial.print("Printed: "); Serial.print(printed_); Serial.print('\n');
    Serial.print("Last printed secs: "); Serial.print(lastPrinted_); Serial.print('\n');

}




