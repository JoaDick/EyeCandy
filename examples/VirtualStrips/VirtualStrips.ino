/*******************************************************************************

An example for dividing one LED strip into multiple virtual LED strips.
Imagine you're having an installation with 2 separate LED strips.
Both shall show their own Animation, completely independent from each other.
But you're having only ONE pin available on the Arduino (or ESP01)...
The solution is to connect DOUT of the first strip to DIN of the second strip
and upload this sketch.

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

static const uint16_t ledCount1 = 50;
static const uint16_t ledCount2 = 40;

// the physical LED strip
static const uint16_t totalLedCount = ledCount1 + ledCount2;
CRGB leds[totalLedCount];

// virtual LED strip 1
CRGB *ledStrip1 = leds;

// virtual LED strip 2
CRGB *ledStrip2 = leds + ledCount1;

//------------------------------------------------------------------------------

// Animation on virtual LED strip 1
EC::Fire2012<ledCount1> fire2012(ledStrip1, ledCount1);

// Animation on virtual LED strip 2
EC::Pride2015 pride2015(ledStrip2, ledCount2);

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

    // special Animation settings
    fire2012.COOLING = 90;
    fire2012.animationDelay = 50;
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
        const uint8_t hue = analogValue;

        fire2012.COOLING = 255 - hue;
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

        fire2012.SPARKING = animationSpeed;
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

    fire2012.mirrored = flipped;
}

//------------------------------------------------------------------------------
