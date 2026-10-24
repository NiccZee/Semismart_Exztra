#if LED_MANAGER_ENABLED
#include <Adafruit_NeoPixel.h>
#endif

// Core Arduino libraries
#include <Arduino.h>
#include <Wire.h>
//#include <EEPROM.h>

/* =======================================================================================================================================
Note on display libraries:
The code is designed to be flexible with different display types. You can choose to use the Adafruit_SSD1306 library
for a smaller OLED display, the Adafruit_SH110X library for a slightly larger OLED, or the Adafruit_ST7796S library
for a full-color TFT display. Each library has its own initialization and drawing methods,
so make sure to include only the one you are using to save memory.

The view_manager.cpp file contains the display initialization code, so if you switch displays,
remember to update that file as well as the corresponding library inclusion here.
4The FreeSansBold18pt7b font is included because it is used in the view_manager for certain screens,
but if you switch to a display that doesn't support it or if you want to save memory,
you can comment it out and use a different font or the default one provided by the display library.

If you want to test on a breadboard with a smaller OLED, you can switch to the Adafruit_SSD1306 library
and update the display initialization accordingly. Just remember to comment out the other display libraries to save memory.
===========================================================================================================================================
*/

// Sensor libraries
#include <Adafruit_SHT4x.h>

// Project configuration
#include "user_config.h"
#include "include/core/system_config.h"

// Core system modules
#include "include/core/state_manager.h"
#include "include/sensors/sensor_manager.h"
#include "include/control/control.h"

// Utility modules
#include "include/utils/storage.h"
#include "include/utils/strings.h"
#include "include/utils/tasks.h"

// Connectivity modules
#if BLUETOOTH_WIFI_ENABLED
#include "include/connectivity/bluetooth_conn.h"
#include "include/connectivity/wifi_conn.h"
#include "include/connectivity/sm_webserver.h"
#endif

// Specialized modules
#include "include/control/pid_manager.h"
#include "include/safety/thermal_security.h"

#if DISPLAY_TYPE_SSD1306
#include "include/display/view_manager_sh1306.h"

#elif DISPLAY_TYPE_SH110X
#include "include/display/view_manager_sh1107.h"

#elif DISPLAY_TYPE_SSD1351
#include "include/display/view_manager_ssd1351.h"

#elif DISPLAY_TYPE_ST7796S
#include "include/display/view_manager_st7796s.h"
#endif

#include "include/interface/gpio_interface.h"
#include "include/sensors/sensors.h"

#if LED_MANAGER_ENABLED
//  Add LEDs  ************************************************************
#include "include/lights/LEDManager.h"
LEDManager ledManager(12, NUMBER_OF_LEDS);  // (pin, number of LEDs)
// End Add LEDs************************************************************
#endif

SystemIntervals systemIntervals;
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

struct Timer {
	uint32_t interval;
	uint32_t last;
	Timer(uint32_t interval_)
	  : interval(interval_), last(0) {}
	bool tick() {
		uint32_t now = millis();
		if (now - last >= interval) {
			last = now;
			return true;
		}
		return false;
	}
};

Timer sensorTimer(2000);
Timer pidTimer(100);
Timer displayTimer(300);
Timer thermalTimer(100);
Timer stateTimer(250);

bool buttonAction = false;
bool buttonPlus = false;
bool buttonMinus = false;
bool buttonActionPrev = false;
bool buttonPlusPrev = false;
bool buttonMinusPrev = false;
uint32_t lastButtonTime = 0;
uint32_t now = -1;
uint32_t ScreenSaveTimer = 0;
bool manualTurnoffHandled = true;

void setup() {
	Wire.begin();

#if BREADBOARD
	Serial.begin(115200);
#endif

#if LED_MANAGER_ENABLED
	// Add LEDs ***************************************************************
	ledManager.begin();  // Initializes strip, brightness, pattern
	// End Add LEDs ************************************************************
#endif

	pinMode(PIN_HEATER, OUTPUT);
	pinMode(PIN_FAN, OUTPUT);
	pinMode(PIN_BUTTON_PLUS, INPUT);
	pinMode(PIN_BUTTON_MINUS, INPUT);
	pinMode(PIN_ACTION_BUTTON, INPUT);

	initStorage();
	sensorManager.begin();
	stateManager.begin();
	viewManager.begin(display);
	initControl();
	initThermalSecurity();
	setupPID();
	initGPIOInterface();

	// /*Comment out this section if you want to test in a breadboard
	extern bool sht4Available;
#if BREADBOARD
	if (sht4Available) {
#else
	if (!sht4Available) {
#endif
		pinMode(LED_BUILTIN, OUTPUT);
		pinMode(LEDR, OUTPUT);
		pinMode(LEDB, OUTPUT);
		static bool LEDFLASH = false;
		while (1) {
			viewManager.setView(ViewID::SENSOR_ERROR);
			viewManager.draw();
			digitalWrite(LED_BUILTIN, LEDFLASH ? HIGH : LOW);
			analogWrite(LEDR, LEDFLASH ? 0 : 255);
			analogWrite(LEDB, LEDFLASH ? 255 : 0);
			LEDFLASH = !LEDFLASH;
			delay(500);
		}
	}
	//End comment out  **/

	if (ScreenSaveTimer == 0) ScreenSaveTimer = millis();


	viewManager.setView(ViewID::SPLASH);
	viewManager.draw();
}

void loop() {
	//	Serial.println("Made it here");

#if LED_MANAGER_ENABLED
	// Add LEDs Check Mode **************************************************
	ledManager.update();  // ← LED animation handler
	                      //************************************************************************
#endif

	if (sensorTimer.tick()) sensorTask();
	if (pidTimer.tick()) {
		pidTask();
		fanTask();
	}
	if (displayTimer.tick()) displayTask();
	if (thermalTimer.tick()) thermalTask();

	// Handle splash screen timer - priority over other state changes
	static uint32_t splashStartTime = 0;
	if (viewManager.getCurrentView() == ViewID::SPLASH) {
		if (splashStartTime == 0) splashStartTime = millis();
		if (millis() - splashStartTime >= systemIntervals.splash) {
			viewManager.setView(ViewID::STANDBY);
			stateManager.setCurrentView(ViewID::STANDBY);
			splashStartTime = 0;

			// If Bluetooth/WiFi is enabled, we can perform a quick scan/connect
			// here to avoid doing it later in the loop and potentially causing
			// delays in sensor reading or display updates. This way, we can ensure
			// connectivity is established while the splash screen is still showing,
			// and it won't interfere with the main loop performance after the splash screen.
			// Note: You will reamain in this state until a WIFI-network has been selected.
			// The connected screen will be the main interface.
#if BLUETOOTH_WIFI_ENABLED
			connectivyTask();
#endif

			// After splash screen, check the last saved system state and restore it (OFF, STANDBY or ON)
			if (stateManager.isPowerLossMemory()) {
				if (readSystemStateEEPROM() == SYSTEM_OFF) {
				} else if (readSystemStateEEPROM() == SYSTEM_STANDBY) {
					stateManager.setSystemState(SYSTEM_STANDBY);
					setStandbyMode(true);
				} else if (readSystemStateEEPROM() == SYSTEM_ON) {
					stateManager.setSystemState(SYSTEM_ON);
					setStandbyMode(false);
					if (stateManager.getMode() == DRY_MODE_BY_TIME) {
						stateManager.setDryStartTime(millis());
						manualTurnoffHandled = false;
						stateManager.setManualTurnedOff(manualTurnoffHandled);
					} else {
						manualTurnoffHandled = true;
						stateManager.setManualTurnedOff(manualTurnoffHandled);
					}
				}
			}
			// This section ends here
		}
	} else if (stateTimer.tick()) updateStateTask();

	handleButtonLogic();
#if BLUETOOTH_WIFI_ENABLED
	if (WiFi.status() == WL_CONNECTED) {
		sswServer.smwebloop();
	}
#endif

	if (stateManager.getScreenTimeout() > 0) {
		static ViewID lastViewBeforeSleep = viewManager.getCurrentView();
		if (millis() - ScreenSaveTimer > (stateManager.getScreenTimeout() * MINUTES_TO_MS)) {
			if (viewManager.getCurrentView() != ViewID::SCREEN_OFF)
				lastViewBeforeSleep = viewManager.getCurrentView();
			viewManager.setView(ViewID::SCREEN_OFF);
		}

		static bool buttonPressed = false;
		if ((buttonAction || buttonPlus || buttonMinus) && !buttonPressed && viewManager.getCurrentView() == ViewID::SCREEN_OFF) {
			ScreenSaveTimer = millis();
			viewManager.setView(lastViewBeforeSleep);
			buttonPressed = true;
		} else if ((buttonAction || buttonPlus || buttonMinus) && !buttonPressed && viewManager.getCurrentView() != ViewID::SCREEN_OFF) {
			ScreenSaveTimer = millis();
			buttonPressed = true;
		} else if (!buttonAction && !buttonPlus && !buttonMinus) {
			buttonPressed = false;
		}
	}


	// When state is SystemOn and in DryTime mode, check if dry duration has elapsed to automatically turn off the system
	if (stateManager.getMode() == DRY_MODE_BY_TIME) {
		if (stateManager.isSystemOn() && stateManager.getRemainingDryTime() == 0) {
			stateManager.setSystemState(SYSTEM_OFF);
			saveSystemStateEEPROM(stateManager.getSystemState());
			setStandbyMode(false);
			handleSystemShutdown();
			now = millis() / MINUTES_TO_MS;
			if (stateManager.getDryStartTimer() > 0)
				stateManager.setDryReStartTimer(millis());
		}

		// When the Start timer has reach 0, we need to turn we turn the system on, in DRY_MODE_BY_TIME, we set the dry start time to the moment of activation, so the timer starts counting down from there.
		if (stateManager.isSystemOff() && stateManager.getDryStartTimer() > 0 && !manualTurnoffHandled && (millis() / MINUTES_TO_MS) - now > stateManager.getDryStartTimer()) {
			stateManager.setSystemState(SYSTEM_ON);
			saveSystemStateEEPROM(stateManager.getSystemState());
			setStandbyMode(false);
			stateManager.setDryStartTime(millis());  // Set the dry start time to the moment of activation
		}
	}
}

void handleButtonLogic() {
	uint32_t now = millis();

	static uint32_t actionPressStartTime = 0;
	static uint32_t PlusButtonPressStartTime = 0;
	static bool longPressActionTaken = false;
	static bool longPressPlusButtonTaken = false;

	buttonAction = digitalRead(PIN_ACTION_BUTTON) == HIGH;
	buttonPlus = digitalRead(PIN_BUTTON_PLUS) == HIGH;
	buttonMinus = digitalRead(PIN_BUTTON_MINUS) == HIGH;

	if (buttonAction && !buttonActionPrev) {
		actionPressStartTime = now;
		longPressActionTaken = false;
	}

	// Check for long press on action button
	if (buttonAction && (now - actionPressStartTime >= 2000) && !longPressActionTaken) {
		// Long press (2s) - toggle ON/OFF or go to ON from OFF/STANDBY
		if (stateManager.getControlMode() == CONTROL_USER_TEMP || stateManager.getControlMode() == CONTROL_USER_HUM) {
			bool turnOn = !stateManager.isSystemOn();
			stateManager.setSystemState(turnOn ? SYSTEM_ON : SYSTEM_OFF);
			saveSystemStateEEPROM(stateManager.getSystemState());
			setStandbyMode(false);
			// When turning ON AND in DryTime mode, reset the dry timer to start counting from the moment of activation
			if (turnOn && stateManager.getMode() == DRY_MODE_BY_TIME) {
				stateManager.setDryStartTime(millis());
				manualTurnoffHandled = false;
				stateManager.setManualTurnedOff(manualTurnoffHandled);
				// If not turned on, handle shutdown and turn off Heater and Fan immediately
			} else if (!turnOn) {
				handleSystemShutdown();
				manualTurnoffHandled = true;
				stateManager.setManualTurnedOff(manualTurnoffHandled);
			}
		}
		longPressActionTaken = true;
	}

	// Check for short press (only if action button was released and long press action was not taken)
	if (!buttonAction && buttonActionPrev && !longPressActionTaken) {
		// Short press - toggle between OFF and STANDBY when system is not ON
		if (!stateManager.isSystemOn()) {
			SystemState newState = stateManager.isSystemOff() ? SYSTEM_STANDBY : SYSTEM_OFF;
			stateManager.setSystemState(newState);
			setStandbyMode(newState == SYSTEM_STANDBY);
			if (newState == SYSTEM_OFF) handleSystemShutdown();
			saveSystemStateEEPROM(stateManager.getSystemState());
		} else if (stateManager.getControlMode() == CONTROL_USER_TEMP || stateManager.getControlMode() == CONTROL_USER_HUM) {
			// System is ON - handle editing mode (only in USER mode)
			ViewID currentView = stateManager.getCurrentView();
			if (currentView == ViewID::TEMP || currentView == ViewID::HUM || currentView == ViewID::MODE
			    || currentView == ViewID::DRY_TIME || currentView == ViewID::DRY_START
			    || currentView == ViewID::POWEROUT || currentView == ViewID::SCREEN_SAVER) {
				stateManager.setEditing(!stateManager.isEditing());
				if (!stateManager.isEditing()) {
					switch (currentView) {
						case ViewID::TEMP: saveTargetTempEEPROM(stateManager.getTargetTemp()); break;
						case ViewID::HUM: saveTargetHumEEPROM(stateManager.getTargetHumidity()); break;
						case ViewID::MODE: saveModeEEPROM(stateManager.getMode()); break;
						case ViewID::SCREEN_SAVER: saveScreenTimeoutEEPROM(stateManager.getScreenTimeout()); break;
						case ViewID::POWEROUT: savePowerLossMemoryEEPROM(stateManager.isPowerLossMemory()); break;
						case ViewID::DRY_TIME: saveDryDurationEEPROM(stateManager.getDryDuration()); break;
						case ViewID::DRY_START: saveDryStartTimeEEPROM(stateManager.getDryStartTimer()); break;
					}
				}
			}
		}
	}
#if BLUETOOTH_WIFI_ENABLED
	if (buttonPlus && !buttonPlusPrev) {
		longPressPlusButtonTaken = false;  // Reset the long press flag when the button is released
		PlusButtonPressStartTime = now;    // Reset the press start time for the next long press detection
	}

	/*
	* Handle long press on plus button in system off mode to reset Bluetooth/WiFi setup and return to splash
	* screen for reconfiguration. This allows users to easily restart the connectivity setup process without
	* needing a separate reset button or power cycle, which can be especially useful if they need to connect
	* to a new network or if there were issues with the previous connection.
	*/
	if (buttonPlus && (now - PlusButtonPressStartTime >= 2000) && !longPressPlusButtonTaken) {
		if (stateManager.isSystemOff() && BLUETOOTH_WIFI_ENABLED) {
			btExztra.resetConnectTimeout();
			wifiExztra.resetWiFi();
			connectivyTask();
			//viewManager.setView(ViewID::SPLASH);
			longPressPlusButtonTaken = true;  // Prevent this from triggering multiple times during the long press
		}
	}

#endif

	// Handle plus button logic - only in USER control mode or when in STANDBY (to allow mode change)
	if (buttonPlus && !longPressPlusButtonTaken && ((stateManager.isSystemOn() && (stateManager.getControlMode() == CONTROL_USER_TEMP || stateManager.getControlMode() == CONTROL_USER_HUM)) || (stateManager.isSystemStandby()))) {
		if (!stateManager.isEditing() && !buttonPlusPrev) {
			if (stateManager.isSystemStandby()) {
				// Only allow mode change in STANDBY state
#if GPIO_INTERFACE_ENABLED
				switch (stateManager.getControlMode()) {
					case CONTROL_USER_TEMP: stateManager.setControlMode(CONTROL_USER_HUM); break;
					case CONTROL_USER_HUM: stateManager.setControlMode(CONTROL_AUTO_TEMP); break;
					case CONTROL_AUTO_TEMP: stateManager.setControlMode(CONTROL_AUTO_HUM); break;
					case CONTROL_AUTO_HUM: stateManager.setControlMode(CONTROL_USER_TEMP); break;
					default: stateManager.setControlMode(CONTROL_USER_TEMP); break;
				}
				saveControlModeEEPROM(stateManager.getControlMode());
#else
				// When GPIO is disabled, only USER mode is available
				switch (stateManager.getControlMode()) {
					case CONTROL_USER_TEMP: stateManager.setControlMode(CONTROL_USER_HUM); break;
					case CONTROL_USER_HUM: stateManager.setControlMode(CONTROL_USER_TEMP); break;
					default: stateManager.setControlMode(CONTROL_USER_TEMP); break;
				}
				saveControlModeEEPROM(CONTROL_USER_TEMP);
#endif
			} else if (stateManager.isSystemOn()) {
				ViewID currentView = stateManager.getCurrentView();
				ViewID nextView;
				switch (currentView) {
					case ViewID::INFO: nextView = ViewID::TEMP; break;
					case ViewID::TEMP: nextView = (stateManager.getMode() == DRY_MODE_BY_HUM) ? ViewID::HUM : ViewID::DRY_TIME; break;
					case ViewID::HUM: nextView = ViewID::INFO; break;
					case ViewID::DRY_TIME: nextView = ViewID::DRY_START; break;
					case ViewID::DRY_START: nextView = ViewID::INFO; break;
					default: nextView = ViewID::INFO; break;
				}
				stateManager.setCurrentView(nextView);
				viewManager.setView(nextView);
			}
		} else if (stateManager.isEditing() && now - lastButtonTime >= 100) {
			switch (stateManager.getCurrentView()) {
				case ViewID::TEMP:
					{
						uint8_t currentTemp = stateManager.getTargetTemp();
						if (currentTemp < TEMP_MAX) stateManager.setTargetTemp(currentTemp + 1);
					}
					break;
				case ViewID::HUM:
					if (stateManager.getTargetHumidity() < HUM_MAX) stateManager.setTargetHumidity(stateManager.getTargetHumidity() + 1);
					break;
				case ViewID::MODE:
					stateManager.setMode(DRY_MODE_BY_HUM);
					break;
				case ViewID::POWEROUT:
					stateManager.setPowerLossMemory(!stateManager.isPowerLossMemory());
					break;
				case ViewID::SCREEN_SAVER:
					if (stateManager.getScreenTimeout() < SCREEN_TIMEOUT_MAX)
						stateManager.setScreenTimeout(stateManager.getScreenTimeout() + 1);
					break;
				case ViewID::DRY_TIME:
					if (stateManager.getDryDuration() < DRY_DURATION_MAX) {
						stateManager.setDryDuration(stateManager.getDryDuration() + DRY_DURATION_STEP);
						stateManager.setDryStartTime(millis());
					}
					break;
				case ViewID::DRY_START:
					if (stateManager.getDryStartTimer() < DRY_START_MAX) {
						stateManager.setDryStartTimer(stateManager.getDryStartTimer() + DRY_START_STEP);
					}
					break;
			}
			lastButtonTime = now;
		}
	}

	// Handle minus button logic - only in USER control mode or when in STANDBY (to allow mode change)
	if (buttonMinus && stateManager.isSystemOn() && (stateManager.getControlMode() == CONTROL_USER_TEMP || stateManager.getControlMode() == CONTROL_USER_HUM)) {
		if (!stateManager.isEditing() && !buttonMinusPrev) {
			ViewID currentView = stateManager.getCurrentView();
			ViewID nextView;
			switch (currentView) {
				case ViewID::INFO: nextView = ViewID::MODE; break;
				case ViewID::MODE: nextView = ViewID::POWEROUT; break;
				case ViewID::POWEROUT: nextView = ViewID::SCREEN_SAVER; break;
				case ViewID::SCREEN_SAVER: nextView = ViewID::INFO; break;
				default: nextView = ViewID::INFO; break;
			}
			stateManager.setCurrentView(nextView);
			viewManager.setView(nextView);
		} else if (stateManager.isEditing() && now - lastButtonTime >= 100) {
			switch (stateManager.getCurrentView()) {
				case ViewID::TEMP:
					{
						uint8_t currentTemp = stateManager.getTargetTemp();
						if (currentTemp > TEMP_MIN) stateManager.setTargetTemp(currentTemp - 1);
					}
					break;
				case ViewID::HUM:
					if (stateManager.getTargetHumidity() > HUM_MIN) stateManager.setTargetHumidity(stateManager.getTargetHumidity() - 1);
					break;
				case ViewID::MODE:
					stateManager.setMode(DRY_MODE_BY_TIME);
					stateManager.setDryStartTime(millis());
					break;
				case ViewID::POWEROUT:
					stateManager.setPowerLossMemory(!stateManager.isPowerLossMemory());
					break;
				case ViewID::SCREEN_SAVER:
					if (stateManager.getScreenTimeout() > 0)
						stateManager.setScreenTimeout(stateManager.getScreenTimeout() - 1);
					break;

				case ViewID::DRY_TIME:
					if (stateManager.getDryDuration() > DRY_DURATION_MIN) {
						stateManager.setDryDuration(stateManager.getDryDuration() - DRY_DURATION_STEP);
						stateManager.setDryStartTime(millis());
					}
					break;
				case ViewID::DRY_START:
					if (stateManager.getDryStartTimer() > DRY_START_MIN) {
						stateManager.setDryStartTimer(stateManager.getDryStartTimer() - DRY_START_STEP);
					}
					break;
			}
			lastButtonTime = now;
		}
	}
#if LED_MANAGER_ENABLED
	// Add LEDs Button Logic **********************************************
	else {
		// Debounce: Detect press → release transition
		static bool buttonMinusWasPressed = false;
		static uint32_t buttonMinusLastChangeTime = 0;
		const uint16_t debounceDelay = 50;  // 50 ms debounce

		// Short press release: Change pattern
		if (!buttonMinus && buttonMinusWasPressed && (now - buttonMinusLastChangeTime > debounceDelay)) {
			if (!longPressActionTaken) {
				ledManager.nextPattern();
			}
			longPressActionTaken = false;  // Reset after release
		}

		if (buttonMinus != buttonMinusWasPressed) {
			buttonMinusLastChangeTime = now;
			buttonMinusWasPressed = buttonMinus;
		}

		// --- Long Press for Brightness with Debounce ---
		static uint32_t buttonMinusDebounceStart = 0;
		static uint32_t buttonMinusPressStartTime = 0;
		static bool buttonMinusStableState = false;

		if (buttonMinus != buttonMinusStableState) {
			// Button state changed, start debounce
			buttonMinusDebounceStart = now;
			buttonMinusStableState = buttonMinus;
		}

		if ((now - buttonMinusDebounceStart) >= debounceDelay) {
			if (buttonMinusStableState) {
				// Button is stably pressed
				if (buttonMinusPressStartTime == 0) {
					buttonMinusPressStartTime = now;
					longPressActionTaken = false;
				}

				// Long press: Change brightness
				if ((now - buttonMinusPressStartTime >= 1500) && !longPressActionTaken) {
					ledManager.nextBrightness();
					longPressActionTaken = true;
				}
			} else {
				// Button is stably released
				buttonMinusPressStartTime = 0;
				longPressActionTaken = false;
			}
		}
	}
#endif
	//  End add LEDs button logic *****************************************

	buttonActionPrev = buttonAction;
	buttonPlusPrev = buttonPlus;
	buttonMinusPrev = buttonMinus;

	// Don't change view if in splash screen
	if (viewManager.getCurrentView() == ViewID::SPLASH) {
		return;
	}

	// Ensure we switch to STANDBY view when system is off, and back to INFO when turned on, but only if not already in a compatible view
	// This prevents unnecessary view changes when the user is actively navigating settings while turning the system on/off
	// If system is turned off, switch to STANDBY view if not already there
	// If system is turned on, only switch to INFO view if currently in STANDBY or if current view is incompatible with the new mode
	// This allows users to remain in their current view (e.g., TEMP, HUM, MODE, DRY_TIME) when turning the system on,
	// without being forced back to INFO, as long as it's compatible with the current mode.
	// For example, if user is in TEMP view and turns the system on, they can stay in TEMP view.
	// But if they are in HUM view and turn on DRY_MODE_BY_TIME, they will be switched to INFO since HUM is not relevant in time-based mode.
	// This logic provides a smoother user experience by respecting the user's current context while ensuring they are always in
	// a relevant view based on the system state and mode.
	// Check system state and update view accordingly, but only if not in SPLASH
	if (!stateManager.isSystemOn()) {
		if (viewManager.getCurrentView() != ViewID::STANDBY) {
			viewManager.setView(ViewID::STANDBY);
			stateManager.setCurrentView(ViewID::STANDBY);
		}
		// If system is off, we want to ensure we're in STANDBY view, but if we're already in a settings view,
		// we can allow that to remain as it is less disruptive than forcing STANDBY. The user can then navigate back to STANDBY if they want.
		// This allows users to see their last settings even when the system is turned off,
		// which can be helpful for reference when they turn it back on.
		// For example, if they were adjusting the target temperature and turned off the system,
		// they can still see the target temperature in the TEMP view when they turn it back on,
		// rather than being forced to switch to STANDBY and then navigate back to TEMP.
		// This logic prioritizes user context and minimizes unnecessary view changes while ensuring that the displayed
		// information is always relevant to the current system state and mode.
		// If user is in a settings view (TEMP, HUM, MODE, DRY_TIME) when turning off, we can allow that to remain as it is less
		// disruptive than forcing STANDBY. The user can then navigate back to STANDBY if they want.
	} else {
		ViewID currentView = viewManager.getCurrentView();
		// Only force INFO view if coming from STANDBY or if view is incompatible with current mode
		if (currentView == ViewID::STANDBY) {
			viewManager.setView(ViewID::INFO);
			stateManager.setCurrentView(ViewID::INFO);

			// If coming from STANDBY, we want to ensure we switch to INFO view, but if we're already in a settings view,
			// we can allow that to remain as it is less disruptive than forcing INFO. The user can then navigate back to INFO if they want.
			// This allows users to see their last settings immediately when they turn the system on,
			// which can be helpful for reference. For example, if they were adjusting the target humidity and turned off the system,
			// they can still see the target humidity in the HUM view when they turn it back on,
			// rather than being forced to switch to INFO and then navigate back to HUM.
			// This logic prioritizes user context and minimizes unnecessary view changes while ensuring that the displayed
			// information is always relevant to the current system state and mode.
		} else if ((stateManager.getMode() == DRY_MODE_BY_HUM && currentView == ViewID::DRY_TIME)
		           || (stateManager.getMode() == DRY_MODE_BY_TIME && currentView == ViewID::HUM)) {
			viewManager.setView(ViewID::INFO);
			stateManager.setCurrentView(ViewID::INFO);
		}
	}
}

#if BLUETOOTH_WIFI_ENABLED
void connectivyTask() {
	// Start non-blocking scans with small timeouts btExztra.startBTSetup();
	// should return immediately or use callbacks wifiExztra.startScanWithTimeout(3000);
	// scan for networks up to 3s, then return
	// optionally attempt auto-connect if credentials exist:

	uint32_t ipConfigScreenTimer = 0;
	if (wifiExztra.hasSavedCredentials()) {
		wifiExztra.connectAsync();
		ipConfigScreenTimer = millis();
	}

	/*
	* A check has to be made that the save SSID also exists in the scan results,
	* otherwise we might end up in a situation where the device tries to connect to a
	* network that is not available, causing long timeouts and a bad user experience.
	* By ensuring that the saved SSID is present in the current scan results before attempting to connect,
	* we can avoid unnecessary connection attempts and provide a smoother experience for the user.
	*/


	// Start non-blocking scans/connect
	static bool wifiSetupStarted = false;
	if (!wifiSetupStarted) {
		btExztra.startBTSetup();  // non-blocking or callback-based
		wifiSetupStarted = true;
	}

	while (WiFi.status() != WL_CONNECTED && digitalRead(PIN_BUTTON_MINUS) == LOW) {
		if (btExztra.isConnectTimeout()) break;
		while (!btExztra.isDeviceConnected()) {
			if (btExztra.isConnectTimeout()) break;
			if (digitalRead(PIN_BUTTON_MINUS) == HIGH) break;  // allow exit with button
			wifiExztra.setSSIDselected(false);
			wifiExztra.clearSSIDList();
			btExztra.connectReceive();
			// update short tasks to keep UI/sensors responsive
			viewManager.draw();  // keep splash animation alive
			//	if (btExztra.isDeviceConnected()) break;
			delay(10);  // small yield to avoid busy loop
		}
		while (btExztra.isDeviceConnected() && !wifiExztra.getSSIDselected()) {
			viewManager.draw();  // keep splash animation alive
			btExztra.connectReceive();
			wifiExztra.WiFi_search();
			// update short tasks to keep UI/sensors responsive
			delay(10);  // small yield to avoid busy loop
		}
		while (btExztra.isDeviceConnected() && wifiExztra.getSSIDselected()) {
			viewManager.draw();  // keep splash animation alive
			btExztra.connectReceive();
			wifiExztra.setSSIDcredentials();
			if (WiFi.status() == WL_CONNECTED) break;
			// update short tasks to keep UI/sensors responsive
			delay(10);  // small yield to avoid busy loop}
		}
	}

	/*
	* Stay on the IP information screen until a button is pressed or the timer has passed
	*/
	while (digitalRead(PIN_BUTTON_MINUS) == LOW
	       && digitalRead(PIN_BUTTON_PLUS) == LOW
	       && digitalRead(PIN_ACTION_BUTTON) == LOW
	       && (ipConfigScreenTimer == 0 || (ipConfigScreenTimer > 0 && millis() - ipConfigScreenTimer < 10000))) {
		viewManager.setView(ViewID::WIFI_CONNECTED);
		viewManager.draw();  // keep splash animation alive
	}

	if (WiFi.status() == WL_CONNECTED) {
		sswServer.smwebsetup();
	}
	viewManager.setView(ViewID::STANDBY);
	stateManager.setCurrentView(ViewID::STANDBY);
}

extern "C" void turnSystemOff() {
	stateManager.setSystemState(SYSTEM_OFF);
	saveSystemStateEEPROM(stateManager.getSystemState());
	setStandbyMode(false);
	handleSystemShutdown();
	if (stateManager.getMode() == DRY_MODE_BY_TIME) {
		manualTurnoffHandled = true;
		stateManager.setManualTurnedOff(manualTurnoffHandled);
	}
}

extern "C" void turnSystemStandby() {
	stateManager.setSystemState(SYSTEM_STANDBY);
	saveSystemStateEEPROM(stateManager.getSystemState());
	setStandbyMode(true);
}

extern "C" void turnSystemOn() {
	stateManager.setSystemState(SYSTEM_ON);
	saveSystemStateEEPROM(stateManager.getSystemState());
	setStandbyMode(false);
	if (stateManager.getMode() == DRY_MODE_BY_TIME) {
		stateManager.setDryStartTime(millis());
		manualTurnoffHandled = false;
		stateManager.setManualTurnedOff(manualTurnoffHandled);
		// If not turned on, handle shutdown and turn off Heater and Fan immediately
	}
}

#endif