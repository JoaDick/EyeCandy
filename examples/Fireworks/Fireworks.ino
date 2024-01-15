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

// #define FIREWORK_DEBUG

#include <EyeCandy.h>
#include <Animation_IO_config.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];
EC::FastLedStrip mainStrip(leds, NUM_LEDS);

#ifdef FIREWORK_DEBUG
const uint8_t PARTICLE_COUNT = 1;
#else
const uint8_t PARTICLE_COUNT = 5;
#endif

EC::FadeOut fadeOut(mainStrip, false, EC::Firework_fadeRate_default());
EC::Firework<PARTICLE_COUNT> firework1(mainStrip, true, 1500);
EC::Firework<PARTICLE_COUNT> firework2(mainStrip, true, 3100);
EC::Firework<PARTICLE_COUNT> firework3(mainStrip, true, 4700);
EC::Firework<PARTICLE_COUNT> firework4(mainStrip, true, 6300);
EC::Firework<PARTICLE_COUNT> firework5(mainStrip, true, 7900);

// run max. 8 Animations simultaneously
EC::AnimationRunnerS animationRunner;

//------------------------------------------------------------------------------

void setup()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    random16_set_seed(analogRead(A3));

    animationRunner.add(fadeOut);
    animationRunner.add(firework1);
#ifndef FIREWORK_DEBUG
    animationRunner.add(firework2);
    animationRunner.add(firework3);
    animationRunner.add(firework4);
    animationRunner.add(firework5);
#endif
}

//------------------------------------------------------------------------------

void loop()
{
    if (animationRunner.process())
    {
        FastLED.show();
    }
    random16_add_entropy(random());
}

//------------------------------------------------------------------------------
