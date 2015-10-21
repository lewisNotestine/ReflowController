#include "ReflowOperationState.h"

ReflowOperationState::ReflowOperationState() {}
ReflowOperationState::ReflowOperationState(int phaseIndex,
    unsigned long   windowStartTime,
    unsigned long   currentSecs, //TODO. get rid of this.
    unsigned long   currentMils,
    unsigned long   startTime,
    unsigned long   lastTime,
    double*        	timeArray,
    double*        	tempArray,
    //double*        	dTdtArray,
    char**          nameArray,
    char*           phaseName,
    double          phaseTime,
    double          totalTime,
    double          targetTemp,  
    double          initTemp,
    double          lastTemp,
    bool            gunIsOn,
    bool            printed,
    int             lastPrinted) :
        phaseIndex_(phaseIndex),
        windowStartTime_(windowStartTime),
        currentSecs_(currentSecs),
        currentMils_(currentMils),
        startTime_(startTime),
        lastTime_(lastTime),
        timeArray_(timeArray),
        tempArray_(tempArray),
        //dTdtArray_(dTdtArray),
        nameArray_(nameArray),
        phaseName_(phaseName),
        phaseTime_(phaseTime),
        totalTime_(totalTime),
        targetTemp_(targetTemp),
        initTemp_(initTemp),
        lastTemp_(lastTemp),
        gunIsOn_(gunIsOn),
        printed_(printed),
        lastPrinted_(lastPrinted)
{}

ReflowOperationState* ReflowOperationState::CreateInitialState(unsigned long windowStartTime,
	unsigned long currentSecs,
	unsigned long currentMils,
	unsigned long startTime,
	//double* timeArray,
	//double* tempArray,
	//double* dTdtArray,
	char** nameArray,
	char* phaseName,		
	double targetTemp,		
	bool printed,
	int lastPrinted) {
	
    return new ReflowOperationState(0,// phaseIndex,
        windowStartTime,
        currentSecs,
        currentMils,
        startTime,
        0, //lastTime,
        { PREHEAT_TIME, SOAK_TIME, REFLOW_TIME, COOLDOWN_TIME }, //timeArray,
        { PREHEAT_TEMP, SOAK_TEMP, REFLOW_TEMP, COOLDOWN_TEMP }, //tempArray,
        //dTdtArray, 
        nameArray,
        phaseName,
        0.0, //phaseTime,
        0.0, //totalTime,
        targetTemp,
        0.0, //initTemp,
        0.0, //lastTemp,
        false, //gunIsOn,
        printed,
        lastPrinted);
}

double ReflowOperationState::getdTdt(int reflowPhaseIndex) {
    if (dTdtArray_ == 0) {
        dTdtArray_ = new double[PHASES];
    }

    //lazy-assign the value to dTdtArray because we don't expect it to change much.
    if (dTdtArray_[reflowPhaseIndex] == 0.0) {
        if (reflowPhaseIndex == 0) {        
            dTdtArray_[reflowPhaseIndex] = tempArray_[reflowPhaseIndex] - initTemp_ / timeArray_[reflowPhaseIndex];
        } else if (dTdtArray_[reflowPhaseIndex] == 0) {
            dTdtArray_[reflowPhaseIndex] = (tempArray_[reflowPhaseIndex] - tempArray_[reflowPhaseIndex - 1]) / (timeArray_[reflowPhaseIndex] - timeArray_[reflowPhaseIndex - 1]);
        }
    }

    return dTdtArray_[reflowPhaseIndex];
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
    currentMils = (millis() - startTime);
}





