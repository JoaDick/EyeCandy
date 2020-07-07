/*******************************************************************************

Many many Animations!

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
EC::BouncingBalls_FL<3> bouncingBalls_FL(leds, NUM_LEDS, false);
// EC::FadeOut_FL fadeOut_FL(leds, NUM_LEDS);
EC::Fire2012_FL<NUM_LEDS> fire2012_FL(leds, NUM_LEDS);
EC::FloatingBlobs_FL floatingBlobs_FL(leds, NUM_LEDS);
EC::Glitter_FL glitter_FL(leds, NUM_LEDS, false);
EC::Kaleidoscope_FL kaleidoscopeOverlay_FL(leds, NUM_LEDS);
EC::MovingDot_FL movingDot_FL(leds, NUM_LEDS, false);
EC::Pride2015_FL pride2015_FL(leds, NUM_LEDS);
EC::Rainbow_FL rainbow_FL(leds, NUM_LEDS);
// EC::RainbowBuiltin_FL rainbowBuiltin_FL(leds, NUM_LEDS);
EC::RainbowTwinkle_FL rainbowTwinkle_FL(leds, NUM_LEDS);
// EC::RgbBlocks_FL rgbBlocks_FL(leds, NUM_LEDS);
// EC::StaticBackground_FL staticBackground_FL(leds, NUM_LEDS, CRGB(0, 10, 0));
EC::Twinkles_FL twinkles_FL(leds, NUM_LEDS, false);

// Overlays
EC::BouncingBalls_FL<3> bouncingBallsOverlay_FL(leds, NUM_LEDS, true);
EC::Glitter_FL glitterOverlay_FL(leds, NUM_LEDS, true);
EC::MovingDot_FL movingDotOverlay_FL(leds, NUM_LEDS, true);
EC::Twinkles_FL twinklesOverlay_FL(leds, NUM_LEDS, true);

// run max. 10 Animations simultaneously
EC::AnimationRunner<10> animations;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));

    // Base Animation (select one)
    // animations.add(bouncingBalls_FL);
    // animations.add(fadeOut_FL);
    // animations.add(fire2012_FL);
    // animations.add(floatingBlobs_FL);
    // animations.add(glitter_FL);
    // animations.add(movingDot_FL);
    // animations.add(pride2015_FL);
    // animations.add(rainbow_FL);
    // animations.add(rainbowBuiltin_FL);
    // animations.add(rainbowTwinkle_FL);
    // animations.add(rgbBlocks_FL);
    // animations.add(staticBackground_FL);
    // animations.add(twinkles_FL);

    // Overlays
    // animations.add(bouncingBallsOverlay_FL);
    // animations.add(glitterOverlay_FL);
    // animations.add(movingDotOverlay_FL);
    // animations.add(twinklesOverlay_FL);

    // animations.add(kaleidoscopeOverlay_FL);
}

//------------------------------------------------------------------------------

void makeAnimation0(EC::AnimationRepo &repo)
{
    repo.add(pride2015_FL);
    pride2015_FL.resizeStrip(kaleidoscopeOverlay_FL.remainLedCount());
    pride2015_FL.mirrored = true;
    repo.add(kaleidoscopeOverlay_FL);
    // repo.add(movingDotOverlay_FL);
    // repo.add(glitterOverlay_FL);
}

void makeAnimation1(EC::AnimationRepo &repo)
{
    repo.add(floatingBlobs_FL);
    repo.add(glitterOverlay_FL);
}

void makeAnimation2(EC::AnimationRepo &repo)
{
    repo.add(rainbowTwinkle_FL);
    rainbowTwinkle_FL.animationDelay = 25;
}

void makeAnimation3(EC::AnimationRepo &repo)
{
    repo.add(fire2012_FL);
    fire2012_FL.COOLING = 155;
    fire2012_FL.SPARKING = 75;
    fire2012_FL.animationDelay = 10;
    repo.add(bouncingBallsOverlay_FL);
    bouncingBallsOverlay_FL.mirrored = true;
}

//------------------------------------------------------------------------------

EC::AnimationBuilderFct allAnimations[] = {
    &makeAnimation0,
    &makeAnimation1,
    &makeAnimation2,
    &makeAnimation3,
    nullptr};

EC::AnimationChanger animationChanger(animations, allAnimations);

//------------------------------------------------------------------------------

void handleAnimationChange()
{
    static const uint16_t animationDuration = 15;
    static uint32_t changeTrigger = animationDuration * 1000;

    const uint32_t now = millis();

    if (now > changeTrigger)
    {
        animationChanger.selectNext();
        changeTrigger = now + animationDuration * 1000;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    handleAnimationChange();

    if (animations.process())
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
/*
template <typename IN_TYPE, typename OUT_TYPE>
OUT_TYPE constrainAndMap(const IN_TYPE &x,
                         const IN_TYPE &minThreshold, const IN_TYPE &maxThreshold,
                         const OUT_TYPE &outMin, const OUT_TYPE &outMax)
{
    return map(constrain(x, minThreshold, maxThreshold), minThreshold, maxThreshold, outMin, outMax);
}
*/
//------------------------------------------------------------------------------
/*
void updateColor()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_COLOR_POT), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t hue = analogValue;

        static uint16_t lastHue = 0;
        if (hue != lastHue)
        {
            Serial.print("hue: ");
            Serial.println(hue);
            lastHue = analogValue;
        }

        fire2012_FL.COOLING = 255 - hue;
        glitter_FL.effectRate = hue;
        movingDot_FL.foregroundColor = CHSV(hue, 255, 255);
        movingDot_FL.backgroundColor = CHSV(hue + 128, 255, 64);
        rainbow_FL.volume = hue;
        // rainbowBuiltin_FL.deltahue = hue / 10;
        // rgbBlocks_FL.blockSize = hue / 10;
        // staticBackground_FL.backgroundColor = CHSV(hue + 128, 255, 128);
        twinkles_FL.effectRate = hue;

        glitterOverlay_FL.effectRate = hue;
        movingDotOverlay_FL.foregroundColor = CHSV(hue + 64, 255, 255);
        twinklesOverlay_FL.effectRate = hue;
    }
}
*/
//------------------------------------------------------------------------------
/*
void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED_POT), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t animationSpeed = analogValue;
        const uint8_t animationDelay = animationSpeed ? 256 - animationSpeed : 0;

        static uint16_t lastSpeed = 0;
        if (animationSpeed != lastSpeed)
        {
            lastSpeed = analogValue;
            Serial.print("speed: ");
            Serial.print(animationSpeed);
            Serial.print(" delay: ");
            Serial.println(animationDelay);
        }

        fire2012_FL.SPARKING = animationSpeed;
        movingDot_FL.animationDelay = animationDelay;
        rainbow_FL.animationDelay = animationDelay;
        // rainbowBuiltin_FL.animationDelay = animationDelay;
        rainbowTwinkle_FL.animationDelay = animationDelay;
        // rgbBlocks_FL.animationDelay = 8 * animationDelay;
        twinkles_FL.fadeRate = animationSpeed;

        movingDotOverlay_FL.animationDelay = 2 * animationDelay;
    }
}
*/
//------------------------------------------------------------------------------
/*
void updateFlip()
{
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

    rainbow_FL.mirrored = flipped;
    // rgbBlocks_FL.mirrored = flipped;

    if (flipped)
    {
        fire2012_FL.animationDelay = 15;
    }
    else
    {
        fire2012_FL.animationDelay = 0;
        fire2012_FL.gPal = EC::Fire2012_gPal_default();
    }
}
*/
//------------------------------------------------------------------------------
