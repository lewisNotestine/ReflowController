#pragma once
#include "Adafruit_MAX31855.h"
#include "PID_v1.h"
#include "ReflowOperationState.h"
#include "PidParams.h"

class LoopHandler {
private:

  static const int MARGIN_TEMP_LOWER = 2;
  static const int MARGIN_TEMP_UPPER = 3;

	PID* pid_;
	PidParams* pidParams_;
	ReflowOperationState* reflowState_;
	Adafruit_MAX31855* thermocouple_;

  
	void setTargetTemp();
	void calculateDTDt(double* dT, double* dt, double* c, double* dTdtOutput);
public:
	LoopHandler(PID* pid, PidParams* pidParams, ReflowOperationState* reflowState, Adafruit_MAX31855* thermocouple);
	void handleLoop();

};


