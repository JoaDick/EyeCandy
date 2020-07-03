/*******************************************************************************

An example showing how Animations can be multiplied like a Kaleidoscope.

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

//#define LED_COLOR_ORDER     RGB
//#define LED_COLOR_ORDER     RBG
#define LED_COLOR_ORDER GRB
//#define LED_COLOR_ORDER     GBR
//#define LED_COLOR_ORDER     BRG
//#define LED_COLOR_ORDER     BGR

#define LED_PIN 6

#define PIN_BUTTON_FLIP 2
#define PIN_COLOR A0
#define PIN_SPEED A1

//------------------------------------------------------------------------------

#define LED_TYPE WS2812B
#define NUM_LEDS 90

CRGB leds[NUM_LEDS];

//------------------------------------------------------------------------------

// Patterns
EC::Glitter_FL glitter_FL(leds, NUM_LEDS, false);
EC::Rainbow_FL rainbow_FL(leds, NUM_LEDS);

// Overlays
EC::Kaleidoscope_FL kaleidoscopeOverlay_FL(leds, NUM_LEDS);
EC::MovingDot_FL movingDotOverlay_FL(leds, NUM_LEDS, true);

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_BUTTON_FLIP, INPUT_PULLUP);
    pinMode(PIN_COLOR, INPUT_PULLUP);
    pinMode(PIN_SPEED, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    // get remaining size for the core Animations
    const uint16_t remainLedCount = kaleidoscopeOverlay_FL.remainLedCount();

    // Glitter in the lower 1/5
    glitter_FL.setLedCount(remainLedCount / 5);
    glitter_FL.foregroundColor = CRGB(64, 64, 64);
    glitter_FL.effectRate = 50;
    glitter_FL.fadeRate = 200;

    // Rainbow in the upper 4/5
    rainbow_FL.setLedOffset(remainLedCount / 5, 4 * remainLedCount / 5);
    rainbow_FL.mirrored = true;
    rainbow_FL.animationDelay = 20;
    rainbow_FL.deltahue = 4;
    rainbow_FL.volume = 128;
    rainbow_FL.moreRed = false;

    // moving dot overlay shall cover lower 90% of the remaining strip
    movingDotOverlay_FL.setLedCount(9 * remainLedCount / 10);
    movingDotOverlay_FL.foregroundColor = CRGB::Red;
    movingDotOverlay_FL.animationDelay = 0;
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    bool mustShow = false;

    // Base Animations
    mustShow = glitter_FL.process(mustShow);
    mustShow = rainbow_FL.process(mustShow);

    // Overlay
    mustShow = movingDotOverlay_FL.process(mustShow);

    // eventually the Kaleidoscope on top of all others
    mustShow = kaleidoscopeOverlay_FL.process(mustShow);

    if (mustShow)
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------

template <typename IN_TYPE, typename OUT_TYPE>
OUT_TYPE constrainAndMap(const IN_TYPE &x,
                         const IN_TYPE &minThreshold, const IN_TYPE &maxThreshold,
                         const OUT_TYPE &outMin, const OUT_TYPE &outMax)
{
    return map(constrain(x, minThreshold, maxThreshold), minThreshold, maxThreshold, outMin, outMax);
}

//------------------------------------------------------------------------------

void updateColor()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_COLOR), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t hue = analogValue;

        glitter_FL.effectRate = hue;

        movingDotOverlay_FL.foregroundColor = CHSV(hue, 255, 255);
    }
}

//------------------------------------------------------------------------------

void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t animationSpeed = analogValue;
        const uint8_t animationDelay = animationSpeed ? 256 - animationSpeed : 0;

        movingDotOverlay_FL.animationDelay = 2 * animationDelay;
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool flipped = !digitalRead(PIN_BUTTON_FLIP);

    kaleidoscopeOverlay_FL.mirrored = !flipped;
}

//------------------------------------------------------------------------------
