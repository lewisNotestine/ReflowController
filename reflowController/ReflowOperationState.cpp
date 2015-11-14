#include "ReflowOperationState.h"

ReflowOperationState::ReflowOperationState() {}
ReflowOperationState::ReflowOperationState(PidParams* pidParams,        
    unsigned long   startingMils,        
    double          initTemp,
    bool            gunIsOn) :
        pidParams_(pidParams),
        phaseIndex_(0),
        windowStartTime_(startingMils),        
        startTime_(startingMils),
        lastTime_(startingMils),
        timeArray_({ PREHEAT_TIME, SOAK_TIME, REFLOW_TIME, COOLDOWN_TIME }),
        tempArray_({ PREHEAT_TEMP, SOAK_TEMP, REFLOW_TEMP, COOLDOWN_TEMP }),        
        phaseTime_(0),
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

double ReflowOperationState::getCurrentMils() {
    return millis() - startTime_;
}

double ReflowOperationState::getStartTime() {
    return startTime_;
}

double ReflowOperationState::getCurrentSecs() {
    return getCurrentMils() / 1000 ;
}

double ReflowOperationState::getTimeAt(int timeIndex) {
    return timeArray_[timeIndex];
}

unsigned long ReflowOperationState::getWindowStartTime() {
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

bool ReflowOperationState::getGunState() {
    return gunIsOn_;
}

//If enough time has elapsed or if we've never printed before.
bool ReflowOperationState::shouldReEvaluatePidSetpointAndPrintOutput() {
    return getCurrentMils() - lastPrinted_ > PIDINTERVAL_MILLIS || lastPrinted_ == 0.0;
}

void ReflowOperationState::setLastTemp(double temp) {
    lastTemp_ = temp;
}

double ReflowOperationState::getLastTemp() {
    return lastTemp_;
}

void ReflowOperationState::recordJustPrinted() {
    lastTime_ = getCurrentMils();
    lastPrinted_ = getCurrentMils();
}

unsigned long ReflowOperationState::getLastTime() {
    return lastTime_;
}
