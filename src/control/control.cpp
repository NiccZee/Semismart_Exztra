#include "../../include/control/control.h"
#include "../../include/utils/storage.h"
#include "../../include/core/state_manager.h"
#include "../../include/sensors/sensor_manager.h"

extern StateManager stateManager;
extern SensorManager sensorManager;

ControlSystem controlSystem;

bool HeatCoolOffFlag = false;

//uint8_t getProportionalFanSpeed() {
//	return (controlSystem.heaterPower <= 0) ? FAN_MIN_PWM_WHEN_HEATING : map(controlSystem.heaterPower, 0, 255, FAN_MIN_PWM_WHEN_HEATING, 255);
//}

uint8_t getProportionalFanSpeed() {
	int hp = constrain(controlSystem.heaterPower, 0, 255);
	int out = map(hp, 0, 255, FAN_MIN_PWM_WHEN_HEATING, 255);
	return (uint8_t)constrain(out, FAN_MIN_PWM_WHEN_HEATING, 255);
}


ControlSystem::ControlSystem() :
	fanState(FAN_OFF),
	stateStartTime(0),
	heaterPrevState(false),
	heaterPower(0),
	isStandby(true),
	lastThermalCheck(0),
	currentFanSpeed(0),
	targetFanSpeed(0),
	fanTransitionStartTime(0),
	fanTransitionActive(false) {
}

void ControlSystem::init() {
	pinMode(PIN_HEATER, OUTPUT);
	pinMode(PIN_FAN, OUTPUT);
	analogWrite(PIN_HEATER, 0);
	analogWrite(PIN_FAN, FAN_SPEED_OFF);
	heaterPower = 0;
}

void ControlSystem::setHeaterPower(int power) {
	bool wasOn = heaterPower > 0;
	heaterPower = constrain(power, 0, 255);
	bool isOn = heaterPower > 0;
	analogWrite(PIN_HEATER, isHeaterAllowed() ? heaterPower : 0);
	if (wasOn && !isOn) heaterPrevState = true;
}

void ControlSystem::updateFanState() {
	uint32_t currentTime = millis();
	uint32_t stateDuration = currentTime - stateStartTime;
	static uint32_t startFanCycleTime = 0;
	float heaterTemp = readThermistorTemperature() / 10.0f;

	// Thermal security has highest priority and can force COOLDOWN state
	if (isCooldownRequired() && fanState != FAN_COOLDOWN) {
		setFanState(FAN_COOLDOWN);
	}
	switch (fanState) {
	case FAN_OFF:
		if (stateManager.isSystemOn()) {
			setFanState(FAN_ON);
		}
		else if (stateManager.isSystemStandby()) {
			setFanState(FAN_STANDBY_CYCLING_ON);
		}
		else {
			setFanState(FAN_STANDBY_OFF);
		}
		break;
	case FAN_ON:
		if (!stateManager.isSystemOn()) {
			if (heaterPrevState) {
				setFanState(FAN_COOLDOWN);
			}
			else if (stateManager.isSystemStandby()) {
				setFanState(FAN_STANDBY_CYCLING_ON);
			}
			else {
				setFanState(FAN_STANDBY_OFF);
			}
		}
		break;
	case FAN_COOLDOWN:
		if (stateManager.isSystemOn()) {
			// If system turns ON while in cooldown, change to FAN_ON
			setFanState(FAN_ON);
		}
		else if (stateDuration >= FAN_COOLDOWN_TIME && !isCooldownRequired()) {
			heaterPrevState = false;
			if (stateManager.isSystemStandby()) {
				setFanState(FAN_STANDBY_CYCLING_ON);
			}
			else {
				setFanState(FAN_STANDBY_OFF);
			}
		}
		break;
	case FAN_STANDBY_CYCLING_ON:
		if (stateManager.getControlMode() == CONTROL_AUTO_TEMP || stateManager.getControlMode() == CONTROL_USER_TEMP) {
			if ((sensorManager.getTemperature() < TEMP_MAX)
				&& (sensorManager.getTemperature() < stateManager.getTargetTemp())
				&& !HeatCoolOffFlag) {
				setHeaterPower(updateHeaterControl());
			}
			else if ((sensorManager.getTemperature() > stateManager.getTargetTemp())
				&& !HeatCoolOffFlag) {
				setHeaterPower(0);
				HeatCoolOffFlag = true;
			}
			else if ((heaterTemp < sensorManager.getTemperature()) 
				&& HeatCoolOffFlag) {
				setHeaterPower(0);
				HeatCoolOffFlag = false;
				setFanState(FAN_STANDBY_CYCLING_OFF);
			}
		}

		if (stateManager.getControlMode() == CONTROL_AUTO_HUM || stateManager.getControlMode() == CONTROL_USER_HUM) {
			if ((sensorManager.getTemperature() < TEMP_MAX)
				&& (sensorManager.getHumidity() > stateManager.getTargetHumidity())
				&& !HeatCoolOffFlag) {
				setHeaterPower(updateHeaterControl());
			}
			else if ((sensorManager.getHumidity() < stateManager.getTargetHumidity())
				&& !HeatCoolOffFlag) {
				setHeaterPower(0);
				startFanCycleTime = currentTime;
				HeatCoolOffFlag = true;
			}
			else if ((heaterTemp < sensorManager.getTemperature()) 
				&& HeatCoolOffFlag) {
				setFanState(FAN_STANDBY_CYCLING_OFF);
				HeatCoolOffFlag = false;
			}
		}

		if (stateManager.isSystemOn()) {
			setFanState(FAN_ON);
		}
		else if (stateManager.isSystemOff()) {
			setFanState(FAN_STANDBY_OFF);
		}
		//else if (stateDuration >= FAN_CYCLE_ON_TIME) {
		//	setFanState(FAN_STANDBY_CYCLING_OFF);
		//}
		break;
	case FAN_STANDBY_CYCLING_OFF:
		if (stateManager.isSystemOn()) {
			setFanState(FAN_ON);
		}
		else if (stateManager.isSystemOff()) {
			setFanState(FAN_STANDBY_OFF);
		}
		else if ((stateManager.getControlMode() == CONTROL_AUTO_TEMP || stateManager.getControlMode() == CONTROL_USER_TEMP)
			&& (sensorManager.getTemperature() < (stateManager.getTargetTemp() - 3.0f))) {
			setFanState(FAN_STANDBY_CYCLING_ON);
		}
		else if ((stateManager.getControlMode() == CONTROL_AUTO_HUM || stateManager.getControlMode() == CONTROL_USER_HUM)
			&& (sensorManager.getHumidity() > (stateManager.getTargetHumidity() + 2.0f))) {
			setFanState(FAN_STANDBY_CYCLING_ON);
		}
		//else if (stateDuration >= FAN_CYCLE_OFF_TIME) {
		//	setFanState(FAN_STANDBY_CYCLING_ON);
		//}
		break;
	case FAN_STANDBY_OFF:
		if (stateManager.isSystemOn()) {
			setFanState(FAN_ON);
		}
		else if (stateManager.isSystemStandby()) {
			setFanState(FAN_STANDBY_CYCLING_ON);
		}
		// Stay in FAN_STANDBY_OFF until system state changes
		break;
	}
	uint8_t fanSpeed;
	switch (fanState) {
	case FAN_ON: fanSpeed = getProportionalFanSpeed(); break;
	case FAN_COOLDOWN: fanSpeed = FAN_SPEED_SECURITY_COOLDOWN; break;
	case FAN_STANDBY_CYCLING_ON: fanSpeed = FAN_SPEED_STANDBY_CYCLING_PWM; break;
	default: fanSpeed = FAN_SPEED_OFF; break;
	}
	setFanSpeedWithTransition(fanSpeed);
}

void ControlSystem::setFanState(FanState newState) {
	if (fanState != newState) fanState = newState, stateStartTime = millis();
}

void ControlSystem::handleSystemShutdown() {
	setHeaterPower(0);
}

void ControlSystem::setStandbyMode(bool standby) {
	if (isStandby != standby) isStandby = standby;
}

bool ControlSystem::isFanCooldownActive() const { return fanState == FAN_COOLDOWN; }
bool ControlSystem::isFanOn() const { return (fanState != FAN_OFF && fanState != FAN_STANDBY_CYCLING_OFF); }
bool ControlSystem::isHeaterOn() const {
	return heaterPower > 0 && isHeaterAllowed();
}
FanState ControlSystem::getFanState() const { return fanState; }


void ControlSystem::setFanSpeedWithTransition(uint8_t targetSpeed) {
	if (targetSpeed != targetFanSpeed) targetFanSpeed = targetSpeed, fanTransitionStartTime = millis(), fanTransitionActive = true;
}

void ControlSystem::updateFanTransition() {
	if (!fanTransitionActive) return;
	uint32_t currentTime = millis();
	uint32_t elapsed = currentTime - fanTransitionStartTime;
	if (elapsed >= FAN_TRANSITION_TIME) {
		currentFanSpeed = targetFanSpeed;
		fanTransitionActive = false;
	}
	else {
		float progress = (float)elapsed / FAN_TRANSITION_TIME;
		currentFanSpeed = currentFanSpeed + (targetFanSpeed - currentFanSpeed) * progress;
	}
	analogWrite(PIN_FAN, currentFanSpeed);
}

uint8_t ControlSystem::getCurrentFanSpeed() const {
	return currentFanSpeed;
}

