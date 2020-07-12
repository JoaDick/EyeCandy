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

#define EnableStaticPatterns 0

#if (EnableStaticPatterns)
// Patterns
EC::BouncingBalls<> bouncingBalls(leds, NUM_LEDS);
// EC::FadeOut fadeOut(leds, NUM_LEDS);
EC::Fire2012<NUM_LEDS> fire2012(leds, NUM_LEDS);
EC::FloatingBlobs floatingBlobs(leds, NUM_LEDS);
EC::Glitter glitter(leds, NUM_LEDS, false);
EC::MovingDot movingDot(leds, NUM_LEDS);
EC::Pride2015 pride2015(leds, NUM_LEDS);
EC::Rainbow rainbow(leds, NUM_LEDS);
// EC::RainbowBuiltin rainbowBuiltin(leds, NUM_LEDS);
EC::RainbowTwinkle rainbowTwinkle(leds, NUM_LEDS);
// EC::RgbBlocks rgbBlocks(leds, NUM_LEDS);
// EC::StaticBackground staticBackground(leds, NUM_LEDS, CRGB(0, 10, 0));
EC::Twinkles twinkles(leds, NUM_LEDS);

// Overlays
EC::BouncingBalls<> bouncingBallsOverlay(leds, NUM_LEDS, true);
EC::Glitter glitterOverlay(leds, NUM_LEDS, true);
EC::Kaleidoscope kaleidoscopeOverlay(leds, NUM_LEDS);
EC::MovingDot movingDotOverlay(leds, NUM_LEDS, true);
EC::Twinkles twinklesOverlay(leds, NUM_LEDS, true);
#endif

// run max. 16 Animations simultaneously
EC::AnimationRunnerM animationRunner;

ButtonHandler selectButton;

bool autoMode = false;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));
    printMemoryUsage();

#if (EnableStaticPatterns)
    // Base Animation (select one)
    // animationRunner.add(bouncingBalls);
    // animationRunner.add(fadeOut);
    // animationRunner.add(fire2012);
    // animationRunner.add(floatingBlobs);
    // animationRunner.add(glitter);
    // animationRunner.add(movingDot);
    // animationRunner.add(pride2015);
    // animationRunner.add(rainbow);
    // animationRunner.add(rainbowBuiltin);
    // animationRunner.add(rainbowTwinkle);
    // animationRunner.add(rgbBlocks);
    // animationRunner.add(staticBackground);
    // animationRunner.add(twinkles);

    // Overlays
    // animationRunner.add(bouncingBallsOverlay);
    // animationRunner.add(glitterOverlay);
    // animationRunner.add(movingDotOverlay);
    // animationRunner.add(twinklesOverlay);

    // animationRunner.add(kaleidoscopeOverlay);
#endif
}

//------------------------------------------------------------------------------

#if (EnableStaticPatterns)
void makeAnimation0(EC::AnimationRepo &repo)
{
    repo.add(pride2015);
    pride2015.resizeStrip(kaleidoscopeOverlay.remainLedCount());
    pride2015.mirrored = true;
    repo.add(kaleidoscopeOverlay);
    // repo.add(movingDotOverlay);
    // repo.add(glitterOverlay);
}

void makeAnimation1(EC::AnimationRepo &repo)
{
    repo.add(floatingBlobs);
    repo.add(glitterOverlay);
}

void makeAnimation2(EC::AnimationRepo &repo)
{
    repo.add(rainbowTwinkle);
    rainbowTwinkle.animationDelay = 25;
}

void makeAnimation3(EC::AnimationRepo &repo)
{
    repo.add(fire2012);
    fire2012.COOLING = 155;
    fire2012.SPARKING = 75;
    fire2012.animationDelay = 10;
    repo.add(bouncingBallsOverlay);
    bouncingBallsOverlay.mirrored = true;
}
#endif

void makeFire(EC::AnimationRepo &repo)
{
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, NUM_LEDS);
    auto fireChanger = new EC::Fire2012Changer<NUM_LEDS>(*fire);

    repo.add(fireChanger);
    repo.add(fire);
}

void makeFireworks(EC::AnimationRepo &repo)
{
    repo.add(new EC::FadeOut(leds, NUM_LEDS, EC::Firework_fadeRate_default()));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 1500));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 3000));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 4500));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 6000));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 7500));
}

void makeFlare(EC::AnimationRepo &repo)
{
    const uint16_t fireLedCount = NUM_LEDS / 2 + NUM_LEDS / 10;
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, fireLedCount);
    fire->SPARKING = 75;
    fire->animationDelay = 10;
    fire->mirrored = true;

    repo.add(fire);
    repo.add(new EC::Kaleidoscope(leds, NUM_LEDS));
}

void makePacifica(EC::AnimationRepo &repo)
{
    repo.add(new EC::Pacifica(leds, NUM_LEDS));
}

void makeWaterfall(EC::AnimationRepo &repo)
{
    repo.add(new EC::Waterfall(leds, NUM_LEDS));
}

//------------------------------------------------------------------------------

EC::AnimationBuilderFct nextAnimation = nullptr;

EC::AnimationBuilderFct allAnimations[] = {
    &makePacifica,
    &makeWaterfall,
    &makeFire,
    &makeFlare,
#if (EnableStaticPatterns)
    &makeAnimation0,
    &makeAnimation1,
    &makeAnimation2,
    &makeAnimation3,
#endif
    &makeFireworks,
    nullptr};

EC::AnimationChangerSoft animationChanger(animationRunner, allAnimations);

//------------------------------------------------------------------------------

const uint16_t animationDuration = 10;
void handleAnimationChange()
{
    static uint32_t nextChangeTime = animationDuration * 1000;

    const uint32_t now = millis();

    bool mustChange = false;
    if (autoMode)
    {
        if (now > nextChangeTime)
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
        animationChanger.selectNext();
        nextChangeTime = now + animationDuration * 1000;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    handleAnimationChange();

    if (animationChanger.process())
    {
#if (0)
        static bool toggleFlag = false;
        toggleFlag ^= true;
        leds[0] = toggleFlag ? CRGB(0, 10, 0) : CRGB::Black;
#endif
    }
    FastLED.show();
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

        animationChanger.maxBrightness = analogValue;

#if (EnableStaticPatterns)
        fire2012.COOLING = 255 - hue;
        glitter.effectRate = hue;
        movingDot.foregroundColor = CHSV(hue, 255, 255);
        movingDot.backgroundColor = CHSV(hue + 128, 255, 64);
        rainbow.volume = hue;
        // rainbowBuiltin.deltahue = hue / 10;
        // rgbBlocks.blockSize = hue / 10;
        // staticBackground.backgroundColor = CHSV(hue + 128, 255, 128);
        twinkles.effectRate = hue;

        glitterOverlay.effectRate = hue;
        movingDotOverlay.foregroundColor = CHSV(hue + 64, 255, 255);
        twinklesOverlay.effectRate = hue;
#endif
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

#if (EnableStaticPatterns)
        fire2012.SPARKING = animationSpeed;
        movingDot.animationDelay = animationDelay;
        rainbow.animationDelay = animationDelay;
        // rainbowBuiltin.animationDelay = animationDelay;
        rainbowTwinkle.animationDelay = animationDelay;
        // rgbBlocks.animationDelay = 8 * animationDelay;
        twinkles.fadeRate = animationSpeed;

        movingDotOverlay.animationDelay = 2 * animationDelay;
#endif
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

#if (EnableStaticPatterns)
    rainbow.mirrored = flipped;
    // rgbBlocks.mirrored = flipped;

    if (flipped)
    {
        fire2012.animationDelay = 15;
    }
    else
    {
        fire2012.animationDelay = 0;
        fire2012.gPal = EC::Fire2012_gPal_default();
    }
#endif
}

//------------------------------------------------------------------------------

void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

    Serial.print(F("BouncingBalls<> = "));
    Serial.println(sizeof(EC::BouncingBalls<>));

    Serial.print(F("FadeOut = "));
    Serial.println(sizeof(EC::FadeOut));

    Serial.print(F("Fire2012<*> = "));
    Serial.println(sizeof(EC::Fire2012<NUM_LEDS>));
    Serial.print(F("Fire2012Changer = "));
    Serial.println(sizeof(EC::Fire2012Changer<NUM_LEDS>));

    Serial.print(F("Firework<> = "));
    Serial.println(sizeof(EC::Firework<>));
    Serial.print(F("FireworkParticle = "));
    Serial.println(sizeof(EC::FireworkParticle));

    Serial.print(F("FloatingBlobs = "));
    Serial.println(sizeof(EC::FloatingBlobs));

    Serial.print(F("Glitter = "));
    Serial.println(sizeof(EC::Glitter));

    Serial.print(F("Kaleidoscope = "));
    Serial.println(sizeof(EC::Kaleidoscope));

    Serial.print(F("MovingDot = "));
    Serial.println(sizeof(EC::MovingDot));

    Serial.print(F("Pacifica = "));
    Serial.println(sizeof(EC::Pacifica));

    Serial.print(F("Pride2015 = "));
    Serial.println(sizeof(EC::Pride2015));

    Serial.print(F("Rainbow = "));
    Serial.println(sizeof(EC::Rainbow));

    Serial.print(F("RainbowBuiltin = "));
    Serial.println(sizeof(EC::RainbowBuiltin));

    Serial.print(F("RainbowTwinkle = "));
    Serial.println(sizeof(EC::RainbowTwinkle));

    Serial.print(F("RgbBlocks = "));
    Serial.println(sizeof(EC::RgbBlocks));

    Serial.print(F("StaticBackground = "));
    Serial.println(sizeof(EC::StaticBackground));

    Serial.print(F("Twinkles = "));
    Serial.println(sizeof(EC::Twinkles));

    Serial.print(F("Waterfall = "));
    Serial.println(sizeof(EC::Waterfall));
    Serial.print(F("WaterfallDroplet = "));
    Serial.println(sizeof(EC::WaterfallDroplet));
}

//------------------------------------------------------------------------------
