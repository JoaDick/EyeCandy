#pragma once

//------------------------------------------------------------------------------

#ifndef NUM_LEDS
// Number of LEDs in the strip.
#define NUM_LEDS 120
#endif

// Enable one of these according to your LED strip.
#ifndef LED_COLOR_ORDER
// #define LED_COLOR_ORDER RGB
// #define LED_COLOR_ORDER RBG
#define LED_COLOR_ORDER GRB
// #define LED_COLOR_ORDER GBR
// #define LED_COLOR_ORDER BRG
// #define LED_COLOR_ORDER BGR
#endif

#ifndef LED_TYPE
#define LED_TYPE WS2812B
#endif

//------------------------------------------------------------------------------

// Data pin for LED strip.
#ifndef LED_PIN
#if defined(ARDUINO_ARCH_AVR) // with Arduino boards
#define LED_PIN 6
#elif defined(ARDUINO_ARCH_ESP8266) // with ESP8266 boards
#define LED_PIN 2
#elif defined(ARDUINO_ARCH_ESP32) // with ESP32 boards
#define LED_PIN 16
#else
#error "No pin for LED data defined"
#endif
#endif

// Connect button to the specified Arduino pin and GND.
// Connect potentiometer's center pin to the specified Arduino pin.
// The other potentiometer pins must be wire to +5V and GND.

// Pin for "Select" button.
#ifndef PIN_SELECT_BTN
#if defined(ARDUINO_ARCH_AVR) // with Arduino boards
#define PIN_SELECT_BTN 2
#elif defined(ARDUINO_ARCH_ESP8266) // with ESP8266 boards
#define PIN_SELECT_BTN 0
#elif defined(ARDUINO_ARCH_ESP32) // with ESP32 boards
#define PIN_SELECT_BTN 17
#else
#error "No pin for Select button defined"
#endif
#endif

#ifndef PIN_FLIP_BTN
// Button for mirror effect.
#define PIN_FLIP_BTN 3
#endif

#ifndef PIN_COLOR_POT
// Potentiometer to control Animation's color.
#define PIN_COLOR_POT A0
#endif

#ifndef PIN_SPEED_POT
// Potentiometer to control Animation's speed.
#define PIN_SPEED_POT A1
#endif

// Microphone analog input.
#ifndef PIN_MIC
#if defined(ARDUINO_ARCH_AVR) // with Arduino boards
#define PIN_MIC A5
#elif defined(ARDUINO_ARCH_ESP8266) // with ESP8266 boards
#define PIN_MIC A0
#elif defined(ARDUINO_ARCH_ESP32) // with ESP32 boards
#define PIN_MIC A0
#else
#error "No pin for microphone defined"
#endif
#endif

//------------------------------------------------------------------------------
