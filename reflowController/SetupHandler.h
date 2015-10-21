#pragma once
#include "PID_v1.h"
#include "ReflowOperationState.h"

class SetupHandler 
{
	private:
		ReflowOperationState* reflowState_;
		PID* pid_;		
		

	public: 
		SetupHandler(PID* pid);

		//Sets up the initial state and returns the state after setup. 
		ReflowOperationState* runSetup();
};

