#pragma once
#include "PID_v1.h"
#include "ReflowOperationState.h"
#include "Adafruit_MAX31855.h"
#include "PidParams.h"

class SetupHandler 
{
	private:		
		PID* pid_;	
		PidParams* pidParams_;
		Adafruit_MAX31855* thermoCouple_;	
		double initTemp_;


	public: 
		SetupHandler(PID* pid, PidParams* pidParams, Adafruit_MAX31855* thermoCouple);

		//Sets up the initial state and returns the state after setup. 
		ReflowOperationState runSetup();
};


