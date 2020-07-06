/*******************************************************************************

A realistic Firework Animation.

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

EC::FadeOut_FL fadeOut_FL(leds, NUM_LEDS);
EC::Firework_FL firework1_FL(leds, NUM_LEDS, true);
EC::Firework_FL firework2_FL(leds, NUM_LEDS, true);
EC::Firework_FL firework3_FL(leds, NUM_LEDS, true);
EC::Firework_FL firework4_FL(leds, NUM_LEDS, true);
EC::Firework_FL firework5_FL(leds, NUM_LEDS, true);

EC::AnimationRunner<6> animations;

//------------------------------------------------------------------------------

void setup()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    animations.add(fadeOut_FL);
    animations.add(firework1_FL);
    animations.add(firework2_FL);
    animations.add(firework3_FL);
    animations.add(firework4_FL);
    animations.add(firework5_FL);

    fadeOut_FL.fadeRate = EC::Firework_FL::fadeRate_default();
    firework1_FL.launchDelay = 1500;
    firework2_FL.launchDelay = 3000;
    firework3_FL.launchDelay = 4500;
    firework4_FL.launchDelay = 6000;
    firework5_FL.launchDelay = 7500;
}

//------------------------------------------------------------------------------

void loop()
{
    if (animations.process())
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
