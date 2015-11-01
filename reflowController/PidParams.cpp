#include "PidParams.h"

PidParams::PidParams(double* pidSetpoint, double* pidInput, double* pidOutput) {
	pidSetpoint_ = pidSetpoint;
	pidInput_ = pidInput;
	pidOutput_ = pidOutput;
}

double PidParams::getPidSetpoint() {
	return *pidSetpoint_;
}

double PidParams::getPidInput() {
	return *pidInput_;
}

double PidParams::getPidOutput() {
	return *pidOutput_;
}

void PidParams::setPidSetpoint(double newSetPoint) {
	*pidSetpoint_ = newSetPoint;
}

void PidParams::setPidInput(double newPidInput) {
	*pidInput_ = newPidInput;
}

void PidParams::setPidOutput(double newPidOutput) {
	*pidOutput_ = newPidOutput;
}

