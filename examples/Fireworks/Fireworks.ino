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

EC::Firework<PARTICLE_COUNT> firework1(mainStrip, false, 1500);
EC::Firework<PARTICLE_COUNT> firework2(mainStrip, true, 3100);
EC::Firework<PARTICLE_COUNT> firework3(mainStrip, true, 4700);
EC::Firework<PARTICLE_COUNT> firework4(mainStrip, true, 6300);
EC::Firework<PARTICLE_COUNT> firework5(mainStrip, true, 7900);

EC::AnimationSceneStatic animationScene;
EC::AnimationUpdateHandler animationHandler(animationScene);

//------------------------------------------------------------------------------

void setup()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    random16_set_seed(analogRead(A3));

    animationScene.append(firework1);
#ifndef FIREWORK_DEBUG
    animationScene.append(firework2);
    animationScene.append(firework3);
    animationScene.append(firework4);
    animationScene.append(firework5);
#endif
}

//------------------------------------------------------------------------------

void loop()
{
    if (animationHandler.process(millis()))
    {
        FastLED.show();
    }
    random16_add_entropy(random());
}

//------------------------------------------------------------------------------
