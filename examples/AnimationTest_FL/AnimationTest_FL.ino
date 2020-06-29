/*******************************************************************************

A sketch for testing during EyeCandy development.

********************************************************************************

MIT License

Copyright (c) 2020 Joachim Dick

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*******************************************************************************/

#include <EyeCandy.h>

// Enable one of these according to your LED strip - see also setup().
//#define LED_COLOR_ORDER     RGB
//#define LED_COLOR_ORDER     RBG
#define LED_COLOR_ORDER GRB
//#define LED_COLOR_ORDER     GBR
//#define LED_COLOR_ORDER     BRG
//#define LED_COLOR_ORDER     BGR

// Connect the LED Strip to that pin.
#define LED_PIN 6

// Connect a pushbutton for selecting the next pattern to that pin (and GND).
#define PIN_BUTTON_NEXT_PATTERN 2

// Connect a pushbutton for selecting the first pattern to that pin (and GND).
#define PIN_BUTTON_FIRST_PATTERN 3

//------------------------------------------------------------------------------

#define LED_TYPE WS2812B
#define NUM_LEDS 90

CRGB leds[NUM_LEDS];

bool lastButtonState = false;

//------------------------------------------------------------------------------

// Patterns
EC::FadeOut_FL fadeOut_FL(leds, NUM_LEDS);
EC::Glitter_FL glitter_FL(leds, NUM_LEDS, false);
EC::MovingDot_FL movingDot_FL(leds, NUM_LEDS, false);
EC::StaticBackground_FL staticBackground_FL(leds, NUM_LEDS, CRGB(0, 10, 0));
EC::RainbowBuiltin_FL rainbowBuiltin_FL(leds, NUM_LEDS);
EC::RgbBlocks_FL rgbBlocks_FL(leds, NUM_LEDS);
EC::Twinkles_FL twinkles_FL(leds, NUM_LEDS);
EC::RainbowTwinkle_FL rainbowTwinkle_FL(leds, NUM_LEDS);

// Overlays
EC::Glitter_FL glitterOverlay_FL(leds, NUM_LEDS, true);
EC::MovingDot_FL movingDotOverlay_FL(leds, NUM_LEDS, true);

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_BUTTON_NEXT_PATTERN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_FIRST_PATTERN, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F("Hello"));
}

//------------------------------------------------------------------------------

void updateColor()
{
    const uint16_t analogValue = analogRead(A0);
    const uint8_t hue = map(analogValue, 0, 1023, 0, 255);

    staticBackground_FL.backgroundColor = CHSV(hue + 128, 255, 128);
    rainbowBuiltin_FL.deltahue = hue / 10;
    movingDot_FL.foregroundColor = CHSV(hue, 255, 255);
    movingDot_FL.backgroundColor = CHSV(hue + 128, 255, 64);
    movingDotOverlay_FL.foregroundColor = CHSV(hue, 255, 255);
    movingDotOverlay_FL.backgroundColor = CHSV(hue + 128, 255, 64);
}

//------------------------------------------------------------------------------

void updateSpeed()
{
    const uint16_t analogValue = analogRead(A1);
    const uint16_t animationDelay = map(analogValue, 0, 1023, 1000, 0);
    const uint16_t speed16 = map(analogValue, 0, 1023, 0, 0xFFFF);
    const uint8_t speed8 = map(analogValue, 0, 1023, 0, 0xFF);

    glitter_FL.effectRate = speed8;
    movingDot_FL.animationDelay = animationDelay * 2;
    movingDotOverlay_FL.animationDelay = animationDelay * 2;
    rainbowBuiltin_FL.animationDelay = animationDelay;
    rainbowTwinkle_FL.wheelDelay = map(analogValue, 0, 1023, 50, 0);
    rgbBlocks_FL.animationDelay = animationDelay;
    twinkles_FL.effectRate = speed8;
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();

    bool mustShow = false;
    //mustShow = true;

    // Base Animation (enable one)
    //mustShow = fadeOut_FL.process(mustShow);
    //mustShow = glitter_FL.process(mustShow);
    //mustShow = staticBackground_FL.process(mustShow);
    mustShow = rainbowBuiltin_FL.process(mustShow);
    //mustShow = movingDot_FL.process(mustShow);
    //mustShow = rgbBlocks_FL.process(mustShow);
    //mustShow = twinkles_FL.process(mustShow);
    //mustShow = rainbowTwinkle_FL.process(mustShow);

    // Overlays
    mustShow = glitterOverlay_FL.process(mustShow);
    mustShow = movingDotOverlay_FL.process(mustShow);

    if (mustShow)
    {
#if(1)
        static bool toggleFlag = false;
        toggleFlag ^= true;
        leds[0] = toggleFlag ? CRGB(0, 10, 0) : CRGB::Black;
#endif
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
