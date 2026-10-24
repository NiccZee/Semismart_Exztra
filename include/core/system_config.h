#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <stdint.h>
#include "../../user_config.h"

// ===================== SYSTEM CONFIGURATION =====================
// These settings should be modified by experiencedusers (be aware)

// ===================== Hardware pins =====================
#define PIN_FAN 3
#define PIN_ACTION_BUTTON 6
#define PIN_BUTTON_PLUS 7
#define PIN_BUTTON_MINUS 8
#define PIN_PRINT_STATUS 9   // Printer status (M262 P0 B0 (set as output), M264 P0 B1/B0 start/stop print)
#define PIN_HEATER 10
#define NTC_PIN A7

// ===================== System timing =====================
#define MAIN_LOOP_DELAY 50          // Main loop delay (ms)

// ===================== Button timing =====================
#define BUTTON_DELAY 200            // Button debounce delay (ms)
#define LONG_PRESS_TIME 1000        // Long press time (ms)
#define DEBOUNCE_DELAY 50

// ===================== Fan system configuration =====================
#define FAN_SPEED_SECURITY_COOLDOWN 255    // 100% - Thermal security cooldown (DO NOT CHANGE)
#define FAN_SPEED_OFF 0                    // 0% - Fan off

// ===================== Safety configuration =====================
#define TEMP_MAX 50     // Maximum 45°C for safe operation
#define TEMP_MIN 15     // Minimum 15°C to avoid condensation
#define DEFAULT_TEMP 35 // 35°C default (conservative for PLA and sensitive filaments)
#define SECURITY_TEMP_SAFE_MAX 60
#define SECURITY_TEMP_SAFE_MIN 0

// ===================== Temperature and humidity limits =====================
#define HUM_MAX 90
#define HUM_MIN 5
#define DEFAULT_HUM 30

#define SECURITY_HUM_SAFE_MAX 85
#define SECURITY_HUM_SAFE_MIN 5

// ===================== Drying duration limits =====================
#define DRY_DURATION_MIN 10           // Minimum duration in minutes (10 min)
#define DRY_DURATION_MAX 720          // Maximum duration in minutes (12 hours)
#define DRY_START_MAX 1440           // Maximum start time in minutes (24 hours)
#define DRY_START_MIN 0               // Minimum start time in seconds
#define DRY_DURATION_DEFAULT 60       // Default duration in minutes
#define DRY_DURATION_STEP 5           // Increment/decrement in minutes
#define DRY_START_STEP 1             // Increment/decrement in minutes

// ===================== PID system configuration =====================
#define PID_KP 2.8    // less aggressive response
#define PID_KI 0.25   // avoid wind-up and overshoot
#define PID_KD 1.5    // better damping and stability
#define PID_DIRECTION NORMAL
#define PID_OUTPUT_MIN 0
#define PID_OUTPUT_MAX 255

// ===================== Time constants and conversions =====================
#define MINUTES_TO_MS 60000           // Minutes to milliseconds conversion
#define MS_TO_SECONDS 1000            // Milliseconds to seconds conversion
#define SECONDS_PER_HOUR 3600         // Seconds in a hour
#define MINUTES_PER_HOUR 60
#define SCREEN_TIMEOUT_MAX 60		 // Maximum screen timeout in minutes (1 hour)

// ===================== Sensor system configuration =====================
#define SENSOR_DEFAULT_TEMP 250       // 25.0°C default (scaled x10)
#define SENSOR_DEFAULT_HUM 500        // 50.0% default (scaled x10)

// ===================== Sensor validation limits =====================
#define SENSOR_TEMP_MIN -500          // Minimum valid temperature (scaled x10)
#define SENSOR_TEMP_MAX 1500          // Maximum valid temperature (scaled x10)

// ===================== Thermal safety limits =====================
#define THERMAL_SECURITY_TEMP_MIN -50.0  // Minimum temperature for thermal validation (°C)
#define THERMAL_SECURITY_TEMP_MAX 150.0  // Maximum temperature for thermal validation (°C)



// ===================== Thermistor configuration =====================
#define THERMISTOR_SERIES_RESISTOR 10000.0  // Series resistor (10KΩ)
#define THERMISTOR_NOMINAL_RESISTANCE 100000.0 // Nominal resistance at 25°C (100KΩ)
#define THERMISTOR_BETA 3950.0              // Beta coefficient for the thermistor
#if ARDUINO_ESP32
#define ADC_MAX 4095.0  // 12-bit ADC
#define VCC 3.3        // Supply voltage for thermistor (3.3V for ESP32)
#else
#define ADC_MAX 1023.0  // 10-bit ADC
#define VCC 5.0        // Supply voltage for thermistor (5V for Nano)
#endif

#define FULL_POWER_TEMP          (SECURITY_MAX_HEATER_TEMP - 4.0f)  // 55.0°C
#define MIN_HEATER_POWER         183   // minimum sustaining power (≈ 15%)
#define HARD_CUTOFF_TEMP         (SECURITY_MAX_HEATER_TEMP + 0.5f)   // 61°C

// ===================== Thermal security system configuration =====================
#define SECURITY_MAX_HEATER_TEMP (TEMP_FUSE - 10.0f)        // Maximum heater temperature (°C)
#define SECURITY_PROTECTION_THRESHOLD (TEMP_FUSE - 5.0f)  // Protection activation threshold (°C)
#define SECURITY_RECOVERY_THRESHOLD (TEMP_FUSE - 2.0f)     // Recovery threshold (°C)

#define SECURITY_PROTECTION_TIME 5000        // Minimum protection time (5 sec)
#define SECURITY_COOLDOWN_TIME 15000         // Cooldown time for thermal protection (30 sec)
#define SECURITY_ERROR_THRESHOLD 5           // Maximum consecutive errors

// ===================== Sensor configuration =====================
#define SECURITY_SENSOR_ERROR_VALUE -999     // Sensor error value
#define SECURITY_MIN_ADC_VALUE 50            // Minimum valid ADC value
#define SECURITY_SENSOR_RESET_TIME 1000      // 1 second for automatic recovery
#define SECURITY_SENSOR_MAX_TEMP_CHANGE 15.0 // Maximum temperature change per reading in sensor
#if ARDUINO_ESP32
#define SECURITY_NO_SENSOR 454
#define SECURITY_MAX_ADC_VALUE 3900           // Maximum valid ADC value
#else
#define SECURITY_NO_SENSOR 113.5
#define SECURITY_MAX_ADC_VALUE 970           // Maximum valid ADC value
#endif

// ===================== Intervals for system =====================
struct SystemIntervals {
    uint16_t sensor = 2000;         // Sensor reading interval (ms)
    uint16_t display = 300;        // Display update interval (ms) – safe minimum for Nano
    uint16_t pid = 100;            // PID update interval (ms)
    uint16_t thermal = 100;        // Thermal check interval (ms)
    uint16_t mainLoop = 50;        // Main loop delay (ms)
    uint16_t button = 200;         // Button debounce delay (ms)
    uint16_t longPress = 1000;     // Long press time (ms)
    uint16_t multiPress = 1500;     // Window for multiple presses (ms)
    uint16_t animation = 250;      // Animation interval (ms)
    uint16_t splash = 2500;        // Splash screen duration (ms)
};

// Global instance of intervals
extern SystemIntervals systemIntervals;

// ===================== System enumerations =====================
// Operation mode enumeration
enum OperationMode : uint8_t {
    DRY_MODE_BY_HUM = 0,    // Control by humidity (without time limit)
    DRY_MODE_BY_TIME = 1    // Time-based mode (max 12hs)
};

// Control mode enumeration
enum ControlMode : uint8_t {
    CONTROL_USER_TEMP = 0,       // User control
    CONTROL_AUTO_TEMP = 1,        // Automatic control (by printer)
	CONTROL_USER_HUM = 2,         // User control for humidity (only in DRY_MODE_BY_HUM)
	CONTROL_AUTO_HUM = 3          // Automatic control for humidity (only in DRY_MODE_BY_HUM)
};

// ===================== System macros =====================
// Temperature unit: Celsius only
#define getTemperatureUnit() str_celsius

// ===================== Conectivity configuration =====================
#define MAX_WIFI_NETWORKS 6 // Maximum number of WiFi networks to display

#endif // SYSTEM_CONFIG_H 
