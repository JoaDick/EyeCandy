/*******************************************************************************

An example showing how multiple Animations can be rendered side by side.
Including an Overlay, which is partially covering both other Patterns.

Can also be used as inspiration for dividing one LED strip into multiple
virtual LED strips (if you omit the MovingDot and adjust the sub-strip sizes):
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

//------------------------------------------------------------------------------

// #define LED_COLOR_ORDER RGB
// #define NUM_LEDS 50
#include <Animation_IO_config.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];
EC::FastLedStrip mainStrip(leds, NUM_LEDS);

// Rainbow in the lower third
EC::FastLedStrip rainbowStrip = mainStrip.getSubStrip(0, NUM_LEDS / 3);

// RGB Blocks in the rest (upper 2/3)
EC::FastLedStrip rgbBlocksStrip = mainStrip.getSubStrip(rainbowStrip.ledCount(), 0);

// Moving Dot overlay in the middle, using 2/3 of the entire strip
EC::FastLedStrip movingDotStrip = mainStrip.getSubStrip(NUM_LEDS / 6, 2 * NUM_LEDS / 3);

// Patterns
EC::Rainbow rainbow(rainbowStrip);
EC::RgbBlocks rgbBlocks(rgbBlocksStrip);

// Overlays
EC::Meteor movingDotOverlay(movingDotStrip, true);

EC::AnimationSceneStatic animationScene;
EC::AnimationUpdateHandler animationHandler(animationScene);

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    // set up Animations
    rainbow.volume = 100;
    // movingDotOverlay.color = CRGB::SteelBlue;
    movingDotOverlay.bpm = 15;
    movingDotOverlay.overshoot = 0.0;

    animationScene.append(rainbow);
    animationScene.append(rgbBlocks);
    animationScene.append(movingDotOverlay);
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();

    if (animationHandler.process(millis()))
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

        rainbow.volume = hue;
        rgbBlocks.blockSize = hue / 10;
    }
}

//------------------------------------------------------------------------------

void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED_POT), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t animationSpeed = analogValue;
        const uint8_t modelUpdatePeriod = animationSpeed ? 256 - animationSpeed : 0;

        rainbow.setModelUpdatePeriod(modelUpdatePeriod);
        rgbBlocks.setModelUpdatePeriod(8 * modelUpdatePeriod);

        // movingDotOverlay.setModelUpdatePeriod(2 * modelUpdatePeriod);
    }
}

//------------------------------------------------------------------------------
