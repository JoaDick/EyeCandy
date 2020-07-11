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

#ifdef FIREWORK_DEBUG
const uint8_t PARTICLE_COUNT = 1;
#else
const uint8_t PARTICLE_COUNT = 5;
#endif

EC::FadeOut_FL fadeOut_FL(leds, NUM_LEDS, EC::Firework_fadeRate_default());
EC::Firework_FL<PARTICLE_COUNT> firework1_FL(leds, NUM_LEDS, true, 1500);
EC::Firework_FL<PARTICLE_COUNT> firework2_FL(leds, NUM_LEDS, true, 3000);
EC::Firework_FL<PARTICLE_COUNT> firework3_FL(leds, NUM_LEDS, true, 4500);
EC::Firework_FL<PARTICLE_COUNT> firework4_FL(leds, NUM_LEDS, true, 6000);
EC::Firework_FL<PARTICLE_COUNT> firework5_FL(leds, NUM_LEDS, true, 7500);

// run max. 8 Animations simultaneously
EC::AnimationRunnerS animations;

//------------------------------------------------------------------------------

void setup()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    random16_set_seed(analogRead(A3));

    animations.add(fadeOut_FL);
    animations.add(firework1_FL);
#ifndef FIREWORK_DEBUG
    animations.add(firework2_FL);
    animations.add(firework3_FL);
    animations.add(firework4_FL);
    animations.add(firework5_FL);
#endif
}

//------------------------------------------------------------------------------

void loop()
{
    if (animations.process())
    {
        FastLED.show();
    }
    random16_add_entropy(random());
}

//------------------------------------------------------------------------------
