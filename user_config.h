#ifndef USER_CONFIG_H
#define USER_CONFIG_H

/* PSEUDOCODE / PLAN (preprocessor comment):
   - After the available DISPLAY_TYPE_* macros are defined (each 0 or 1),
     compute a macro DISPLAY_TYPE_COUNT that sums them.
   - Use preprocessor checks to enforce that exactly one display type is enabled:
     - If DISPLAY_TYPE_COUNT == 0 -> produce #error: "No display type selected..."
     - If DISPLAY_TYPE_COUNT > 1 -> produce #error: "Multiple display types selected..."
   - Keep all original configuration and notes intact.
   - Place the checks immediately after the display type macros so a clear compile-time
     error points to the configuration issue before other code is processed.

  ===================== Display configuration =====================
  Supported displays: Adafruit_SSD1306, Adafruit_SH110X, Adafruit ST7796S, Adafruit SSD1351
  (Set 1 for the one you are using, and 0 for the others to save memory)
*/
#define DISPLAY_TYPE_SSD1306  0
#define DISPLAY_TYPE_SH110X   1
#define DISPLAY_TYPE_SSD1351  0
#define DISPLAY_TYPE_ST7796S  0

// Compile-time check: ensure exactly one display type is set to 1.
#define DISPLAY_TYPE_COUNT (DISPLAY_TYPE_SSD1306 + DISPLAY_TYPE_SH110X + DISPLAY_TYPE_SSD1351 + DISPLAY_TYPE_ST7796S)

#if DISPLAY_TYPE_COUNT == 0
  #error "No display type selected. Set exactly one DISPLAY_TYPE_* to 1 in user_config.h"
#elif DISPLAY_TYPE_COUNT > 1
  #error "Multiple display types selected. Set exactly one DISPLAY_TYPE_* to 1 in user_config.h"
#endif

// ======================== TEMP SENSOR =========================================
// The Nano ESP32 does not have any 5v-tolerant pins. As the Temp sensor that is 
// connected to the A7-pin is fed with 5V, you need to modify the PCB to
// feed the temp sensor with 3.3V instead. 
// If you don't want to do this modification, this setting turns off the temp-sensor
// completely and you can run your setup without it. It is only a fail-safe aginst
// overheating. There is also a Temp fuse that I believe is safer, as it cuts the
// power to the heater completely at over heating.
// 
// Set TEMP_SENSOR to 0 to run WITHOUT temp sensor.
// Set TEMP_SENSOR to 1 to run WITH the temp sensor and the appropriate modifications.
// ==============================================================================
#define TEMP_SENSOR 1
#define TEMP_FUSE 85 // Temperature in Celsius at which the thermal fuse is stated for (e.g. 70C/16A).
                     // Adjust according to what you have installed.

// ===================== ARDUINO VERSION USED ===========================
// Select which version of the Arduino module you are using.
// If you are using the Arduino Nano R4, set the below definition to 0
// If you are using the Arduino Nano ESP32 Wifi, set the below definition to 1
#define ARDUINO_ESP32 1
 
// ===================== USER CONFIGURABLE SETTINGS =====================
// We can enable additional features like LED animations. Disable if you don't intend 
// to use LED strip to save memory for other features.
#define LED_MANAGER_ENABLED 0	// Enable LED animations (requires WS2812B LEDs)
#define NUMBER_OF_LEDS 120		// Set this to the number of LEDs in your strip (e.g. 120 for 1m of WS2812B at 30 LEDs/meter)

// ===================== Bluetooth Integration =====================
// Disable if you don't need Bluetooth connectivity to save memory
// Can ONLY be used with Arduino NANO ESP32 WiFi, as it otherwise will give compilation error
#if ARDUINO_ESP32
#define BLUETOOTH_WIFI_ENABLED 1 // Set this to 0 for no Wifi on ESP32
#else
#define BLUETOOTH_WIFI_ENABLED 0 // Do not touch this setting. For Nano R4, this can only be 0.
#endif


// ==============================================================================
// ===================== GPIO Interface (Prusa) Integration =====================
// Disable for printer without GPIO interface eg. MK3 series
#define GPIO_INTERFACE_ENABLED 1
// ==============================================================================

// ===================== Breadboard usage =====================
// Set this setting to 1 of you want to use the Arduino on a 
// breadboard to test out functionality or do own coding.
#define BREADBOARD 1
// ==============================================================================

// ===================== Unit configuration =====================
// Temperature unit: Celsius only

// ===================== Fan PWM speeds (Configurable) =====================
// Recommended values for minimum noise and maximum efficiency (0 to 255 max)
#define FAN_SPEED_STANDBY_CYCLING_PWM 255  // 40% - Standby cycling
#define FAN_SPEED_NORMAL_CYCLING_PWM 255   // 60% - Normal operation cycling (BY TIME/BY HUM)
#define FAN_MIN_PWM_WHEN_HEATING 170        // 35% - Minimum fan speed when heater is active

// ===================== Fan control timing =====================
#define FAN_COOLDOWN_TIME 45000     // Cooldown time after heater off
#define FAN_CYCLE_ON_TIME 15000      // Standby cycle on time
#define FAN_CYCLE_OFF_TIME 300000    // Standby cycle off time
#define FAN_TRANSITION_TIME 2000     // Smooth transition time (2 seconds)

#endif // USER_CONFIG_H
