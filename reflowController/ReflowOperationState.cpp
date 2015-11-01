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

double ReflowOperationState::getDTdt(int reflowPhaseIndex) {
    //lazy-assign the value to dTdtArray because we don't expect it to change much.
    if (reflowPhaseIndex == 2 && dTdtArray_[2] != 0) //Reflow phase is hard-coded because it's zero.
    {
        dTdtArray_[2] = 0;
    } else if (dTdtArray_[reflowPhaseIndex] == 0.0) {
        if (reflowPhaseIndex == 0) {        
            dTdtArray_[reflowPhaseIndex] = tempArray_[reflowPhaseIndex] - initTemp_ / timeArray_[reflowPhaseIndex];
        } else if (dTdtArray_[reflowPhaseIndex] == 0) {
            dTdtArray_[reflowPhaseIndex] = (tempArray_[reflowPhaseIndex] - tempArray_[reflowPhaseIndex - 1]) / (timeArray_[reflowPhaseIndex] - timeArray_[reflowPhaseIndex - 1]);
        }
    }

    return dTdtArray_[reflowPhaseIndex];
}

double ReflowOperationState::getCurrentDTdt() {
    return getDTdt(getPhaseIndex());
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
      setPidSetpoint(getDTdt(getPhaseIndex()));
    }
}

void ReflowOperationState::evaluateTargetTemp() {
    if (getPhaseIndex() == 0) {
      setTargetTemp((getCurrentSecs() * getDTdt(getPhaseIndex())) + initTemp_);
    } else {
      setTargetTemp( ((getCurrentSecs() - getTimeAt(getPhaseIndex() - 1)) * getDTdt(getPhaseIndex())) + getTargetTemp(getPhaseIndex() - 1) );      
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





