#pragma once
#include "PidParams.h"
#include "Arduino.h"

/*
For encapsulating the state of the program. Can be passed between handlers.
CONSIDER: perhaps factor out other states for separation of concerns? see what happens.
*/
class ReflowOperationState {
private:
    //TODO: make these into an enum?
    static const int PHASES = 4;
    static const int INDEX_PREHEAT = 0;
    static const int INDEX_SOAK = 1;
    static const int INDEX_REFLOW = 2;
    static const int INDEX_COOLDOWN = 3;

    PidParams*      pidParams_;
	int             phaseIndex_;  //current state of the reflow.
	unsigned long   windowStartTime_;
	unsigned long   currentSecs_;  //TODO. get rid of this as its own variable. 
	unsigned long   currentMils_;
	unsigned long   startTime_; //TASK: find how this is different from windowStartTime.
	unsigned long   lastTime_; 
	double         timeArray_[4];
	double         tempArray_[4];
	double*         dTdtArray_;
	char*          nameArray_[4];
	char*          	phaseName_;  
	double          phaseTime_;  
	double          totalTime_;
	double          targetTemp_;  
	double          initTemp_;
	double          lastTemp_;

	bool            gunIsOn_;
	bool            printed_;
	int             lastPrinted_;

    void setPidSetpoint(double newSetpoint);

    void setTargetTemp(double newTargetTemp);

	
protected:
	ReflowOperationState();
	
public:

    //TODO: put the consts into another class? 
    //only run the setup, not the loop. 
    static const bool TEST_ONLY = true;
    static const int PIN_GUN = 11;
    static const int PIN_THERMO_DO = 3;
    static const int PIN_THERMO_CS = 4;
    static const int PIN_THERMO_CLK = 5;

    //Times are given as t (not period).
    static const double    TEMP_THRESHOLD = 2.0;
    static const double    PREHEAT_TIME = 120; 
    static const double    PREHEAT_TEMP = 80;  
    static const double    SOAK_TIME = 40; 
    static const double    SOAK_TEMP = 60;  
    static const double    REFLOW_TIME = 60; 
    static const double    REFLOW_TEMP = 80; 
    static const double    COOLDOWN_TIME = 120;
    static const double    COOLDOWN_TEMP = 60;
    static const int       SECONDS = 120;
    static const int       WINDOW_SIZE = 200;    

    ReflowOperationState(PidParams* pidParams,        
        unsigned long   currentMils,                
        double          initTemp,        
        bool            gunIsOn);

    int getPhaseIndex();

    double getDTdt(int reflowPhaseIndex);

    double getCurrentDTdt();

    double getTargetTemp(int phaseIndex);

    double getCurrentTargetTemp();

    double getPhaseTime();

    double getTotalTime();

    double getCurrentMils();

    double getStartTime();

    double getCurrentSecs();

    double getTimeAt(int timeIndex);

    long getWindowStartTime();

    void incrementPhaseIndex();

    void incrementPhaseTime();

    void incrementTotalTime();

    void updateTime();

    void evaluatePhaseAndSetpoint();

    void evaluateTargetTemp();

    void evaluateWindowStartTime();

    void setGunState(bool gunIsOn);

    bool getPrinted();

    void setPrinted(bool newPrinted);

    void setLastTemp(double temp);

    double getLastTemp();

    void evaluatePrinted();

    void evaluatePrintedTime();

    unsigned long getLastTime();
};

