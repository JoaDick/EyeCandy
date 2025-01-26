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

//------------------------------------------------------------------------------

// #define LED_COLOR_ORDER RGB
// #define NUM_LEDS 50
#include <Animation_IO_config.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];
EC::FastLedStrip mainStrip(leds, NUM_LEDS);

// because the content is duplicated, we draw only in the lower part of the strip
EC::FastLedStrip workingStrip = mainStrip.getHalfStrip();

// the background pattern (explicitly)
EC::BgFadeToBlack background(workingStrip, 50);
// the main animation (as overlay)
EC::Meteor mainAnimation(workingStrip, true, 20, 0.05);
// overlay for adding some glitter (in the lower part only)
EC::Glitter glitterOverlay(workingStrip.getHalfStrip(), true);

// overlay for doing the Kaleidoscope magic
EC::Kaleidoscope kaleidoscopeOverlay(mainStrip);

EC::AnimationSceneStatic animationScene;
EC::AnimationUpdateHandler animationHandler(animationScene);

//------------------------------------------------------------------------------

void setup()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    // configure the core Animation + Overlay
    mainAnimation.fadeRate = 20;
    glitterOverlay.color = CRGB(64, 64, 0);
    glitterOverlay.effectRate = 100;

    // add the background pattern
    animationScene.append(background);
    // add the main animation
    animationScene.append(mainAnimation);
    // add the overlay
    animationScene.append(glitterOverlay);
    // Kaleidoscope should be added as last one
    animationScene.append(kaleidoscopeOverlay);
}

//------------------------------------------------------------------------------

void loop()
{
    if (animationHandler.process(millis()))
    {
        EVERY_N_SECONDS(15)
        {
            // toggle this every few seconds to show the difference
            kaleidoscopeOverlay.mirrored ^= true;
        }
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
