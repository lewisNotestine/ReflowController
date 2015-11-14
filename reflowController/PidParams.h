#pragma once

class PidParams {
private:
	double* pidSetpoint_; 
	double* pidInput_; 
	double* pidOutput_;
public:
	PidParams(double* pidSetpoint, double* pidInput_, double* pidOutput_);

	double getPidSetpoint();
	double getPidInput();
	double getPidOutput();

	void setPidSetpoint(double newSetPoint);
	void setPidInput(double newPidInput);
	void setPidOutput(double newPidOutput);
};

