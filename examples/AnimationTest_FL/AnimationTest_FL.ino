/*******************************************************************************

A sketch for testing during EyeCandy development.

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
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

// Patterns
EC::BouncingBalls_FL<> bouncingBalls_FL(leds, NUM_LEDS);
// EC::FadeOut_FL fadeOut_FL(leds, NUM_LEDS);
EC::Fire2012_FL<NUM_LEDS> fire2012_FL(leds, NUM_LEDS);
EC::FloatingBlobs_FL floatingBlobs_FL(leds, NUM_LEDS);
EC::Glitter_FL glitter_FL(leds, NUM_LEDS, false);
EC::MovingDot_FL movingDot_FL(leds, NUM_LEDS);
EC::Pride2015_FL pride2015_FL(leds, NUM_LEDS);
EC::Rainbow_FL rainbow_FL(leds, NUM_LEDS);
// EC::RainbowBuiltin_FL rainbowBuiltin_FL(leds, NUM_LEDS);
EC::RainbowTwinkle_FL rainbowTwinkle_FL(leds, NUM_LEDS);
// EC::RgbBlocks_FL rgbBlocks_FL(leds, NUM_LEDS);
// EC::StaticBackground_FL staticBackground_FL(leds, NUM_LEDS, CRGB(0, 10, 0));
EC::Twinkles_FL twinkles_FL(leds, NUM_LEDS);

// Overlays
EC::BouncingBalls_FL<> bouncingBallsOverlay_FL(leds, NUM_LEDS, true);
EC::Glitter_FL glitterOverlay_FL(leds, NUM_LEDS, true);
EC::Kaleidoscope_FL kaleidoscopeOverlay_FL(leds, NUM_LEDS);
EC::MovingDot_FL movingDotOverlay_FL(leds, NUM_LEDS, true);
EC::Twinkles_FL twinklesOverlay_FL(leds, NUM_LEDS, true);

// run max. 16 Animations simultaneously
EC::AnimationRunnerM animations;

ButtonHandler selectButton;

bool autoMode = true;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));
    printMemoryUsage();

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

EC::AnimationBuilderFct nextAnimation = nullptr;

EC::AnimationBuilderFct allAnimations[] = {
    &makeAnimation0,
    &makeAnimation1,
    &makeAnimation2,
    &makeAnimation3,
    nullptr};

EC::AnimationChanger animationChanger(animations, allAnimations);

//------------------------------------------------------------------------------

const uint16_t animationDuration = 10;
void handleAnimationChange()
{
    static uint32_t nextChange = animationDuration * 1000;

    const uint32_t now = millis();

    bool mustChange = false;
    if (autoMode)
    {
        if (now > nextChange)
        {
            mustChange = true;
        }
    }

    // skip to next animation with this button
    // long press = enable auto mode
    switch (selectButton.process(!digitalRead(PIN_SELECT_BTN)))
    {
    case 1:
        if (!autoMode)
        {
            mustChange = true;
        }
        autoMode = false;
        break;
    case 2:
        mustChange = true;
        autoMode = true;
        break;
    case 3:
        autoMode = false;
        mustChange = true;
        break;
    }

    digitalWrite(LED_BUILTIN, autoMode);
    if (mustChange)
    {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        animationChanger.selectNext();
        nextChange = now + animationDuration * 1000;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    handleAnimationChange();

    if (animations.process())
    {
#if (0)
        static bool toggleFlag = false;
        toggleFlag ^= true;
        leds[0] = toggleFlag ? CRGB(0, 10, 0) : CRGB::Black;
#endif
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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

    Serial.print(F("BouncingBalls_FL<> = "));
    Serial.println(sizeof(EC::BouncingBalls_FL<>));

    Serial.print(F("FadeOut_FL = "));
    Serial.println(sizeof(EC::FadeOut_FL));

    Serial.print(F("Fire2012_FL<*> = "));
    Serial.println(sizeof(EC::Fire2012_FL<NUM_LEDS>));

    Serial.print(F("Firework_FL<> = "));
    Serial.println(sizeof(EC::Firework_FL<>));

    Serial.print(F("FloatingBlobs_FL = "));
    Serial.println(sizeof(EC::FloatingBlobs_FL));

    Serial.print(F("Glitter_FL = "));
    Serial.println(sizeof(EC::Glitter_FL));

    Serial.print(F("Kaleidoscope_FL = "));
    Serial.println(sizeof(EC::Kaleidoscope_FL));

    Serial.print(F("MovingDot_FL = "));
    Serial.println(sizeof(EC::MovingDot_FL));

    Serial.print(F("Pride2015_FL = "));
    Serial.println(sizeof(EC::Pride2015_FL));

    Serial.print(F("Rainbow_FL = "));
    Serial.println(sizeof(EC::Rainbow_FL));

    Serial.print(F("RainbowBuiltin_FL = "));
    Serial.println(sizeof(EC::RainbowBuiltin_FL));

    Serial.print(F("RainbowTwinkle_FL = "));
    Serial.println(sizeof(EC::RainbowTwinkle_FL));

    Serial.print(F("RgbBlocks_FL = "));
    Serial.println(sizeof(EC::RgbBlocks_FL));

    Serial.print(F("StaticBackground_FL = "));
    Serial.println(sizeof(EC::StaticBackground_FL));

    Serial.print(F("Twinkles_FL = "));
    Serial.println(sizeof(EC::Twinkles_FL));
}

//------------------------------------------------------------------------------
