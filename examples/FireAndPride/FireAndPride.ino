/*******************************************************************************

A nice example that combines Fire2012 and Pride2015 with a Kaleioscope.

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

// Animations
EC::Fire2012<NUM_LEDS> fire2012(leds, NUM_LEDS);
EC::Pride2015 pride2015(leds, NUM_LEDS);
EC::Kaleidoscope kaleidoscopeOverlay(leds, NUM_LEDS);

// run max. 8 Animations simultaneously
EC::AnimationRunnerS animationRunner;

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
    animationRunner.add(fire2012);
    animationRunner.add(pride2015);
    // Kaleidoscope should be the last one
    animationRunner.add(kaleidoscopeOverlay);

    // calculate new Animation sizes
    const uint16_t ledCount = kaleidoscopeOverlay.remainLedCount();
    const uint16_t ledCount1 = ledCount / 2;
    const uint16_t ledCount2 = ledCount - ledCount1;
    // store new Animation sizes
    fire2012.resizeStrip(ledCount1);
    pride2015.resizeStrip(ledCount2, ledCount1);

    // special Animation settings
    pride2015.mirrored = true;
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    if (animationRunner.process())
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
        fire2012.COOLING = 255 - analogValue;
    }
}

//------------------------------------------------------------------------------

void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED_POT), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        fire2012.SPARKING = analogValue;
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

    if (flipped)
    {
        fire2012.animationDelay = 15;
    }
    else
    {
        fire2012.animationDelay = 0;
    }
}

//------------------------------------------------------------------------------
