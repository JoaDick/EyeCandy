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
#include <Animation_IO_config.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

// Patterns
EC::Glitter_FL glitter_FL(leds, NUM_LEDS, false);
EC::Rainbow_FL rainbow_FL(leds, NUM_LEDS);

// Overlays
EC::Kaleidoscope_FL kaleidoscopeOverlay_FL(leds, NUM_LEDS);
EC::MovingDot_FL movingDotOverlay_FL(leds, NUM_LEDS, true);

// run 4 Animations simultaneously
EC::AnimationRunner<4> animations;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    // set up Animations to run
    animations.add(glitter_FL);
    animations.add(rainbow_FL);
    animations.add(movingDotOverlay_FL);
    // Kaleidoscope should be the last one
    animations.add(kaleidoscopeOverlay_FL);

    // get remaining size for the core Animations
    const uint16_t remainLedCount = kaleidoscopeOverlay_FL.remainLedCount();

    // Glitter in the lower 1/5
    glitter_FL.resizeStrip(remainLedCount / 5);
    glitter_FL.foregroundColor = CRGB(64, 64, 64);
    glitter_FL.effectRate = 50;
    glitter_FL.fadeRate = 200;

    // Rainbow in the upper 4/5
    rainbow_FL.resizeStrip(4 * remainLedCount / 5, remainLedCount / 5);
    rainbow_FL.mirrored = true;
    rainbow_FL.animationDelay = 20;
    rainbow_FL.deltahue = 4;
    rainbow_FL.volume = 128;
    rainbow_FL.moreRed = false;

    // moving dot overlay shall cover lower 90% of the remaining strip
    movingDotOverlay_FL.resizeStrip(9 * remainLedCount / 10);
    movingDotOverlay_FL.foregroundColor = CRGB::Red;
    movingDotOverlay_FL.animationDelay = 0;
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    if (animations.process())
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
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_COLOR_POT), 50, 900, 0, 256);

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
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED_POT), 50, 900, 0, 256);

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
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

    kaleidoscopeOverlay_FL.mirrored = !flipped;
}

//------------------------------------------------------------------------------
