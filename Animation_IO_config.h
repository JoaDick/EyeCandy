#pragma once

//------------------------------------------------------------------------------

#ifndef NUM_LEDS
// Number of LEDs in the strip.
#define NUM_LEDS 90
#endif

#ifndef LED_PIN
// Connect the LED Strip to that Arduino pin.
#define LED_PIN 6
#endif

// Connect button to the specified Arduino pin and GND.
// Connect potentiometer's center pin to the specified Arduino pin.
// The other potentiometer pins must be wire to +5V and GND.

// "Select" Button.
#define PIN_SELECT_BTN 2

// Button for mirror effect.
#define PIN_FLIP_BTN 3

// Potentiometer to control Animation's color.
#define PIN_COLOR_POT A0

// Potentiometer to control Animation's speed.
#define PIN_SPEED_POT A1

// Microphone.
#define PIN_MIC A5

//------------------------------------------------------------------------------

// Enable one of these according to your LED strip.
#ifndef LED_COLOR_ORDER
//#define LED_COLOR_ORDER RGB
//#define LED_COLOR_ORDER RBG
#define LED_COLOR_ORDER GRB
//#define LED_COLOR_ORDER GBR
//#define LED_COLOR_ORDER BRG
//#define LED_COLOR_ORDER BGR
#endif

#ifndef LED_TYPE
#define LED_TYPE WS2812B
#endif

//------------------------------------------------------------------------------
