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
// #define NUM_LEDS 50
#include <Animation_IO_config.h>
#include <AudioNormalizer.h>
#include <ButtonHandler.h>

// !!! This sketch is currently broken and needs repair :-( !!!

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

#define EnableStaticPatterns 0 // fixme!

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

// run max. 8 Animations simultaneously
EC::AnimationRunnerS animationRunner;

ButtonHandler selectButton;

bool autoMode = true;

float audioSample = 0.0;
EC::AudioNormalizer normalizer;

#define PRINT_MEMORY_USAGE 1

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);
    pinMode(PIN_MIC, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    analogReference(EXTERNAL);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
    Serial.println();
    // Serial.println(F("Welcome to EyeCandy"));
#if (PRINT_MEMORY_USAGE)
    printMemoryUsage();
#endif

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

const uint16_t defaultAnimationDuration = 30;
uint16_t animationDuration = defaultAnimationDuration;

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
    rainbowTwinkle.setModelUpdatePeriod(25);
}

void makeAnimation3(EC::AnimationRepo &repo)
{
    repo.add(fire2012);
    fire2012.COOLING = 155;
    fire2012.SPARKING = 75;
    fire2012.setModelUpdatePeriod(10);
    repo.add(bouncingBallsOverlay);
    bouncingBallsOverlay.mirrored = true;
}
#endif

// ---------- Animations ----------

void makeBalls(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::BouncingBalls<>(strip, false));
}

void makeBubbles(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

#if (0)
    repo.add(new EC::Bubbles(strip));
#else
    repo.add(new EC::Pacifica(strip));
    repo.add(new EC::FadeOut(strip, true, 150));
    repo.add(new EC::Bubbles(strip, true));
#endif
}

void makeFire(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = new EC::Fire2012<NUM_LEDS>(strip);
    auto fireChanger = new EC::Fire2012Changer<NUM_LEDS>(*fire);

    repo.add(fireChanger);
    repo.add(fire);
}

void makeFireworks(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::FadeOut(strip, false, EC::Firework_fadeRate_default()));
    repo.add(new EC::Firework<>(strip, true, 1500));
    repo.add(new EC::Firework<>(strip, true, 3100));
    repo.add(new EC::Firework<>(strip, true, 4700));
    repo.add(new EC::Firework<>(strip, true, 6300));
    repo.add(new EC::Firework<>(strip, true, 7900));
}

void makeFlare(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    const uint16_t fireLedCount = NUM_LEDS / 2 + NUM_LEDS / 10;
    EC::FastLedStrip fireStrip = strip.getSubStrip(0, fireLedCount, true);

    auto fire = new EC::Fire2012<NUM_LEDS>(fireStrip);
    fire->SPARKING = 75;
    fire->setModelUpdatePeriod(10);

    repo.add(fire);
    repo.add(new EC::Kaleidoscope(strip));
}

void makePacifica(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::Pacifica(strip));
}

void makePride(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::Pride2015(strip));
}

void makeWaterfall(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::Waterfall(strip));
}

// ---------- VUs ----------

void makeBouncingDotVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuPeak = new EC::RainbowLevelVU(audioSample, strip, false);
    vuPeak->enableVuBar = false;
    vuPeak->vuHueRange = 0.5;

    // auto glitter = new EC::PeakGlitterVU(audioSample, strip, true, CRGB(128, 64, 0));
    auto glitter = new EC::PeakGlitterVU(audioSample, strip, true);
    glitter->color = CRGB(128, 64, 0);

    repo.add(vuPeak);
    repo.add(glitter);
}

void makeDancingDotVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = new EC::RainbowLevelVU(audioSample, strip, false);
    vu->enablePeakDot = false;
    vu->fadeRate = 0;
    vu->volume = 64;
    repo.add(vu);
    repo.add(new EC::DancingDotVU(audioSample, strip, true /*, CRGB::Red*/));
}

void makeDoubleBouncingDotVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu1 = new EC::RainbowLevelVU(audioSample, strip, true);
    vu1->enableVuBar = false;
    vu1->volume = 255;
    vu1->vuHueRange = 0.4;
    vu1->baseHueStep = -0.17;
    vu1->vuLevelHandler.smoothingFactor = 6;

    auto vu2 = new EC::RainbowLevelVU(audioSample, strip.getReversedStrip(), true);
    vu2->enableVuBar = false;
    vu2->volume = 255;
    vu2->vuHueRange = 0.6;
    vu2->baseHueStep = 0.11;
    vu2->vuLevelHandler.smoothingFactor = 9;

    repo.add(new EC::FadeOut(strip, false, 50));
    repo.add(vu1);
    repo.add(vu2);
}

void makeDoubleDancingDotVU1(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu1 = new EC::DancingDotVU(audioSample, strip, true /*, CRGB(255, 0, 0)*/);
    vu1->color = CRGB(255, 0, 0);

    auto vu2 = new EC::DancingDotVU(audioSample, strip.getReversedStrip(), true /*, CRGB(0, 255, 0)*/);
    vu2->color = CRGB(0, 255, 0);
    vu2->vuPeakHandler.inertia = 0.55;
    vu2->vuPeakHandler.friction = 0.14;

    repo.add(new EC::Pride2015(strip.getHalfStrip(/*true*/)));
    repo.add(new EC::Kaleidoscope(strip));
    repo.add(new EC::FadeOut(strip, true, 100));
    repo.add(vu1);
    repo.add(vu2);
}

void makeDoubleDancingDotVU2(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu1 = new EC::DancingDotVU(audioSample, strip, true /*, CHSV(20, 255, 255)*/);
    vu1->color = CHSV(20, 255, 255);

    auto vu2 = new EC::DancingDotVU(audioSample, strip.getReversedStrip(), true /*, CHSV(20 + 128, 255, 255)*/);
    vu2->color = CHSV(20 + 128, 255, 255);
    vu2->vuPeakHandler.inertia = 0.55;
    vu2->vuPeakHandler.friction = 0.14;

    repo.add(new EC::FloatingBlobs(strip));
    repo.add(new EC::FadeOut(strip, true, 230));
    repo.add(vu1);
    repo.add(vu2);
}

void makeEssentialVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::EssentialVU(audioSample, strip, false));
}

void makeFireVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = new EC::Fire2012<NUM_LEDS>(strip);
    auto fireChanger = new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire);
    repo.add(fire);
    repo.add(fireChanger);
}

void makePeakGlitterVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::PeakGlitterVU(audioSample, strip, false));
}

void makeRainbowBubbleVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = new EC::RainbowLevelVU(audioSample, strip, false);
    vu->vuPeakHandler.a0 = 0.5;
    vu->vuPeakHandler.v0 = 0.1;
    vu->vuHueRange = 0.67;
    repo.add(vu);
}

void makeRainbowBubbleCenteredVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = new EC::RainbowLevelVU(audioSample, strip.getHalfStrip(true), false);
    vu->vuPeakHandler.a0 = 0.5;
    vu->vuPeakHandler.v0 = 0.1;
    vu->vuHueRange = 0.67;
    repo.add(vu);
    repo.add(new EC::Kaleidoscope(strip));
}

void makeRainbowBubbleInwardVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = new EC::RainbowLevelVU(audioSample, strip.getHalfStrip(), false);
    vu->vuPeakHandler.a0 = 0.5;
    vu->vuPeakHandler.v0 = 0.1;
    vu->vuHueRange = 0.67;
    repo.add(vu);
    repo.add(new EC::Kaleidoscope(strip));
}

void makeRainbowLevelVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::RainbowLevelVU(audioSample, strip, false));
}

void makeRainbowLevelCenteredVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::RainbowLevelVU(audioSample, strip.getHalfStrip(true), false));
    repo.add(new EC::Kaleidoscope(strip));
}

void makeRainbowLevelInwardVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::RainbowLevelVU(audioSample, strip.getHalfStrip(), false));
    repo.add(new EC::Kaleidoscope(strip));
}

// ---------- VU sequence ----------

void makeVuSequence1(EC::AnimationRepo &repo)
{
    makePeakGlitterVU(repo);
    animationDuration = 5;
}

void makeVuSequence2(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto baseVU = new EC::EssentialVU(audioSample, strip, false);
    baseVU->enableVuBar = false;
    baseVU->fadeRate = 0;
    repo.add(baseVU);
    repo.add(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 10;
}

void makeVuSequence3(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto baseVU = new EC::EssentialVU(audioSample, strip, false);
    baseVU->fadeRate = 0;
    repo.add(baseVU);
    animationDuration = 10;
}

void makeVuSequence4(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::EssentialVU(audioSample, strip, false));
    repo.add(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 10;
}

void makeVuSequence5(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    repo.add(new EC::RainbowLevelVU(audioSample, strip, false));
    repo.add(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 15;
}

void makeVuSequence6(EC::AnimationRepo &repo)
{
    makeBouncingDotVU(repo);
    animationDuration = 10;
}

void makeVuSequence7(EC::AnimationRepo &repo)
{
    makeRainbowLevelVU(repo);
    animationDuration = 15;
}

void makeVuSequence8(EC::AnimationRepo &repo)
{
    makeRainbowBubbleVU(repo);
    animationDuration = 20;
}

void makeVuSequence9(EC::AnimationRepo &repo)
{
    makeRainbowLevelInwardVU(repo);
    animationDuration = 15;
}

void makeVuSequence10(EC::AnimationRepo &repo)
{
    makeRainbowLevelCenteredVU(repo);
    animationDuration = 15;
}

void makeVuSequence11(EC::AnimationRepo &repo)
{
    makeRainbowBubbleCenteredVU(repo);
    animationDuration = 15;
}

void makeVuSequence12(EC::AnimationRepo &repo)
{
    makeRainbowBubbleInwardVU(repo);
    animationDuration = 15;
}

void makeVuSequence13(EC::AnimationRepo &repo)
{
    makeDancingDotVU(repo);
    animationDuration = 15;
}

void makeVuSequence14(EC::AnimationRepo &repo)
{
    makeDoubleBouncingDotVU(repo);
    animationDuration = 15;
}

void makeVuSequence15(EC::AnimationRepo &repo)
{
    makeDoubleDancingDotVU1(repo);
    animationDuration = 15;
}

void makeVuSequence16(EC::AnimationRepo &repo)
{
    makeDoubleDancingDotVU2(repo);
    animationDuration = 15;
}

void makeVuSequence17(EC::AnimationRepo &repo)
{
    makeFireVU(repo);
    // animationDuration = 15;
    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationBuilderFct allAnimations[] = {
#if (EnableStaticPatterns)
    &makeAnimation0,
    &makeAnimation1,
    &makeAnimation2,
    &makeAnimation3,
#else
    // &makeFireworks,

    &makeVuSequence1,
    &makeVuSequence2,
    &makeVuSequence3,
    &makeVuSequence4,
    &makeVuSequence5,
    &makeVuSequence6,
    &makeVuSequence7,
    &makeVuSequence8,
    &makeVuSequence9,
    &makeVuSequence10,
    &makeVuSequence11,
    &makeVuSequence12,
    &makeVuSequence13,
    &makeVuSequence14,
    &makeVuSequence15,
    &makeVuSequence16,
    &makeVuSequence17,

    &makePride,
    &makePacifica,
    &makeBubbles,
    &makeWaterfall,
    &makeFire,
    &makeFlare,
#endif
    nullptr};

EC::AnimationChangerSoft animationChanger(animationRunner, allAnimations);

//------------------------------------------------------------------------------

void handleAnimationChange(uint32_t currentMillis = millis())
{
    static uint32_t lastChangeTime = 0;

    bool mustChange = false;
    if (autoMode)
    {
        if (currentMillis > lastChangeTime + animationDuration * 1000)
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
        animationDuration = defaultAnimationDuration;
        animationChanger.selectNext();
        lastChangeTime = currentMillis;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    const uint32_t currentMillis = millis();
    audioSample = normalizer.analogRead(PIN_MIC);

#if (0)
    Serial.print(" -:");
    Serial.print(-10.0);
    Serial.print(" +:");
    Serial.print(10.0);
    Serial.print(" raw:");
    Serial.print(10.0 * audioSample);
    Serial.println();
#endif

    updateColor();
    updateSpeed();
    updateFlip();

    handleAnimationChange(currentMillis);

    if (animationChanger.process(currentMillis))
    {
#if (0)
        static bool toggleFlag = false;
        toggleFlag ^= true;
        leds[0] = toggleFlag ? CRGB(0, 10, 0) : CRGB::Black;
#endif
    }
    FastLED.show();
    // printSampleRate(currentMillis);
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
        const uint8_t modelUpdatePeriod = animationSpeed ? 256 - animationSpeed : 0;

        static uint16_t lastSpeed = 0;
        if (animationSpeed != lastSpeed)
        {
            lastSpeed = analogValue;
            Serial.print("speed: ");
            Serial.print(animationSpeed);
            Serial.print(" delay: ");
            Serial.println(modelUpdatePeriod);
        }

#if (EnableStaticPatterns)
        fire2012.SPARKING = animationSpeed;
        movingDot.setModelUpdatePeriod(modelUpdatePeriod);
        rainbow.setModelUpdatePeriod(modelUpdatePeriod);
        // rainbowBuiltin.setModelUpdatePeriod(modelUpdatePeriod);
        rainbowTwinkle.setModelUpdatePeriod(modelUpdatePeriod);
        // rgbBlocks.setModelUpdatePeriod(8 * modelUpdatePeriod);
        twinkles.fadeRate = animationSpeed;

        movingDotOverlay.setModelUpdatePeriod(2 * modelUpdatePeriod);
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
        fire2012.setModelUpdatePeriod(15);
    }
    else
    {
        fire2012.setModelUpdatePeriod(0);
        fire2012.gPal = EC::Fire2012_gPal_default();
    }
#endif
}

//------------------------------------------------------------------------------

void printSampleRate(uint32_t currentMillis)
{
    static uint32_t nextSampleRateCheck = 1000;
    static uint32_t sampleRateCounter = 0;

    ++sampleRateCounter;
    if (currentMillis >= nextSampleRateCheck)
    {
        uint16_t sampleRate = sampleRateCounter;
        sampleRateCounter = 0;
        nextSampleRateCheck = currentMillis + 1000;

        Serial.print(sampleRate);
        Serial.println(F(" Hz sample rate"));
    }
}

//------------------------------------------------------------------------------

#if (PRINT_MEMORY_USAGE)
void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

    Serial.print(F("BouncingBalls<> = "));
    Serial.println(sizeof(EC::BouncingBalls<>));

    Serial.print(F("Bubbles = "));
    Serial.println(sizeof(EC::Bubbles));

    Serial.print(F("FadeOutOverlay = "));
    Serial.println(sizeof(EC::FadeOutOverlay));

    Serial.print(F("FadeOutPattern = "));
    Serial.println(sizeof(EC::FadeOutPattern));

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

    Serial.print(F("DancingDotVU = "));
    Serial.println(sizeof(EC::DancingDotVU));

    Serial.print(F("EssentialVU = "));
    Serial.println(sizeof(EC::EssentialVU));

    Serial.print(F("Fire2012VU = "));
    Serial.println(sizeof(EC::Fire2012VU<NUM_LEDS>));

    Serial.print(F("PeakGlitterVU = "));
    Serial.println(sizeof(EC::PeakGlitterVU));

    Serial.print(F("RainbowLevelVU = "));
    Serial.println(sizeof(EC::RainbowLevelVU));
}
#endif

//------------------------------------------------------------------------------
