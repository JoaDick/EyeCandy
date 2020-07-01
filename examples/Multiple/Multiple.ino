/*******************************************************************************

An example showing how multiple Animations can be rendered side by side.
Including an Overlay, which is partially covering both other Patterns.

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
EC::Rainbow_FL rainbow_FL(leds, NUM_LEDS);
EC::RgbBlocks_FL rgbBlocks_FL(leds, NUM_LEDS);

// Overlays
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

    // Rainbow in the lower half
    rainbow_FL.setLedCount(NUM_LEDS / 2);
    rainbow_FL.volume = 128;

    // RGB blocks in the upper half
    rgbBlocks_FL.setLedOffset(NUM_LEDS / 2, NUM_LEDS / 2);

    // Moving dot overlay in the middle, using 2/3 of the entire strip
    movingDotOverlay_FL.setLedOffset(NUM_LEDS / 6, 2 * NUM_LEDS / 3);
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    bool mustShow = false;

    // Base Animations
    mustShow = rainbow_FL.process(mustShow);
    mustShow = rgbBlocks_FL.process(mustShow);

    // Overlay
    mustShow = movingDotOverlay_FL.process(mustShow);

    if (mustShow)
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------

template <typename IN_TYPE, typename OUT_TYPE>
OUT_TYPE constrainAndMap(int x,
                         const IN_TYPE &minThreshold, const IN_TYPE &maxThreshold,
                         const OUT_TYPE &outMin, const OUT_TYPE &outMax)
{
    return map(constrain(x, minThreshold, maxThreshold), minThreshold, maxThreshold, outMin, outMax);
}

//------------------------------------------------------------------------------

uint8_t lastHue = 0;
void updateColor()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_COLOR), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t hue = analogValue;

        rainbow_FL.volume = hue;
        rgbBlocks_FL.blockSize = hue / 10;

        movingDotOverlay_FL.foregroundColor = CHSV(hue + 64, 255, 255);
        movingDotOverlay_FL.backgroundColor = CHSV(hue + 192, 255, 64);
    }
}

//------------------------------------------------------------------------------

uint8_t lastSpeed = 0;
void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t animationSpeed = analogValue;
        const uint8_t animationDelay = animationSpeed ? 256 - animationSpeed : 0;

        rainbow_FL.animationDelay = animationDelay;
        rgbBlocks_FL.animationDelay = 8 * animationDelay;

        movingDotOverlay_FL.animationDelay = 2 * animationDelay;
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool mirrored = !digitalRead(PIN_BUTTON_FLIP);

    rainbow_FL.mirrored = mirrored;
    rgbBlocks_FL.mirrored = mirrored;
}

//------------------------------------------------------------------------------
