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

	public: 
		SetupHandler(PID* pid, PidParams* pidParams, Adafruit_MAX31855* thermoCouple);

		//Sets up and tests the initial state
		void runSetup();
};



