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
#include <AudioNormalizer.h>
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

// run max. 16 Animations simultaneously
EC::AnimationRunnerM animationRunner;

ButtonHandler selectButton;

bool autoMode = true;

float audioSample = 0.0;
AudioNormalizer normalizer;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_MIC, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    analogReference(EXTERNAL);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
    Serial.println();
    Serial.println(F("Welcome to EyeCandy"));
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
uint16_t animationDuration = defaultAnimationDuration;

void makeBouncingDotVU(EC::AnimationRepo &repo)
{
    auto vuPeak = new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample);
    vuPeak->enableVuBar = false;
    vuPeak->vuHueRange = 0.5;

    auto glitter = new EC::PeakGlitterVU(leds, NUM_LEDS, audioSample, true);
    glitter->glitterColor = CRGB(128, 64, 0);

    repo.add(vuPeak);
    repo.add(glitter);
}

void makeDancingDotVU(EC::AnimationRepo &repo)
{
    auto vu = new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample);
    vu->enablePeakDot = false;
    vu->fadeRate = 0;
    vu->volume = 64;
    repo.add(vu);
    repo.add(new EC::DancingDotVU(leds, NUM_LEDS, audioSample, true));
}

void makeDoubleBouncingDotVU(EC::AnimationRepo &repo)
{
    auto vu1 = new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample, true);
    vu1->enableVuBar = false;
    vu1->volume = 255;
    vu1->vuHueRange = 0.4;
    vu1->baseHueStep = -0.17;
    vu1->vuLevelHandler.smoothingFactor = 6;

    auto vu2 = new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample, true);
    vu2->mirrored = true;
    vu2->enableVuBar = false;
    vu2->volume = 255;
    vu2->vuHueRange = 0.6;
    vu2->baseHueStep = 0.11;
    vu2->vuLevelHandler.smoothingFactor = 9;

    repo.add(new EC::FadeOut(leds, NUM_LEDS, 50));
    repo.add(vu1);
    repo.add(vu2);
}

void makeDoubleDancingDotVU1(EC::AnimationRepo &repo)
{
    auto vu1 = new EC::DancingDotVU(leds, NUM_LEDS, audioSample, true);
    vu1->peakDotColor = CRGB(255, 0, 0);

    auto vu2 = new EC::DancingDotVU(leds, NUM_LEDS, audioSample, true);
    vu2->mirrored = true;
    vu2->peakDotColor = CRGB(0, 255, 0);
    vu2->vuPeakHandler.inertia = 0.55;
    vu2->vuPeakHandler.friction = 0.14;

    auto kaleidoscope = new EC::Kaleidoscope(leds, NUM_LEDS);
    auto pride = new EC::Pride2015(leds, kaleidoscope->remainLedCount());
    // pride->mirrored = true;

    repo.add(pride);
    repo.add(kaleidoscope);
    repo.add(new EC::FadeOut(leds, NUM_LEDS, true, 100));
    repo.add(vu1);
    repo.add(vu2);
}

void makeDoubleDancingDotVU2(EC::AnimationRepo &repo)
{
    auto vu1 = new EC::DancingDotVU(leds, NUM_LEDS, audioSample, true);
    vu1->peakDotColor = CHSV(20, 255, 255);

    auto vu2 = new EC::DancingDotVU(leds, NUM_LEDS, audioSample, true);
    vu2->mirrored = true;
    vu2->peakDotColor = CHSV(20 + 128, 255, 255);
    vu2->vuPeakHandler.inertia = 0.55;
    vu2->vuPeakHandler.friction = 0.14;

    repo.add(new EC::FloatingBlobs(leds, NUM_LEDS));
    repo.add(new EC::FadeOut(leds, NUM_LEDS, true, 230));
    repo.add(vu1);
    repo.add(vu2);
}

void makeFireVU(EC::AnimationRepo &repo)
{
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, NUM_LEDS);
#ifndef FIRE2012VU_DEBUG
    auto fireChanger = new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire);
#else
    auto fireChanger = new EC::Fire2012VU<NUM_LEDS>(leds, NUM_LEDS, audioSample, *fire);
#endif
    repo.add(fire);
    repo.add(fireChanger);
}

void makeFlareVU(EC::AnimationRepo &repo)
{
    const uint16_t fireLedCount = NUM_LEDS / 2 + NUM_LEDS / 10;
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, fireLedCount);
    fire->mirrored = true;
    fire->animationDelay = 10;

    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire));
    repo.add(fire);
    repo.add(new EC::Kaleidoscope(leds, NUM_LEDS));
}

void makeFlareDoubleVU(EC::AnimationRepo &repo)
{
    auto kaleidoscope1 = new EC::Kaleidoscope(leds, NUM_LEDS);
    const uint16_t remainLedCount1 = NUM_LEDS / 2 + NUM_LEDS / 10;

    auto kaleidoscope2 = new EC::Kaleidoscope(leds, remainLedCount1);
    const uint16_t remainLedCount2 = (remainLedCount1 + 1) / 2;

    auto fire = new EC::Fire2012<NUM_LEDS>(leds, remainLedCount2);
    fire->animationDelay = 10;

    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire));
    repo.add(fire);
    repo.add(kaleidoscope2);
    repo.add(kaleidoscope1);
}

void makeFlareInwardVU(EC::AnimationRepo &repo)
{
    const uint16_t ledCount1 = (NUM_LEDS + 1) / 2;
    const uint16_t ledCount2 = NUM_LEDS - ledCount1;

    auto fire1 = new EC::Fire2012<NUM_LEDS>(leds, ledCount1);
    fire1->animationDelay = 11;

    auto fire2 = new EC::Fire2012<NUM_LEDS>(leds + ledCount1, ledCount2);
    fire2->mirrored = true;
    fire2->animationDelay = 13;

    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire1));
    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire2));
    repo.add(fire1);
    repo.add(fire2);
}

void makeRainbowBubbleVU(EC::AnimationRepo &repo)
{
    auto vu = new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample);
    vu->vuPeakHandler.a0 = 0.5;
    vu->vuPeakHandler.v0 = 0.1;
    vu->vuHueRange = 0.67;
    repo.add(vu);
}

void makeRainbowBubbleCenteredVU(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope(leds, NUM_LEDS);
    auto vu = new EC::RainbowLevelVU(leds, kaleidoscope->remainLedCount(), audioSample);
    vu->vuPeakHandler.a0 = 0.5;
    vu->vuPeakHandler.v0 = 0.1;
    vu->vuHueRange = 0.67;
    vu->mirrored = true;
    repo.add(vu);
    repo.add(kaleidoscope);
}

void makeRainbowBubbleInwardVU(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope(leds, NUM_LEDS);
    auto vu = new EC::RainbowLevelVU(leds, kaleidoscope->remainLedCount(), audioSample);
    vu->vuPeakHandler.a0 = 0.5;
    vu->vuPeakHandler.v0 = 0.1;
    vu->vuHueRange = 0.67;
    repo.add(vu);
    repo.add(kaleidoscope);
}

void makeRainbowLevelCenteredVU(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope(leds, NUM_LEDS);
    auto vu = new EC::RainbowLevelVU(leds, kaleidoscope->remainLedCount(), audioSample);
    vu->mirrored = true;
    repo.add(vu);
    repo.add(kaleidoscope);
}

void makeRainbowLevelInwardVU(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope(leds, NUM_LEDS);
    auto vu = new EC::RainbowLevelVU(leds, kaleidoscope->remainLedCount(), audioSample);
    repo.add(vu);
    repo.add(kaleidoscope);
}

// ---------- VU sequence ----------

void makeVuSequence1(EC::AnimationRepo &repo)
{
    repo.add(new EC::PeakGlitterVU(leds, NUM_LEDS, audioSample));
    animationDuration = 8;
}

void makeVuSequence2(EC::AnimationRepo &repo)
{
    auto baseVU = new EC::EssentialVU(leds, NUM_LEDS, audioSample);
    baseVU->enableVuBar = false;
    baseVU->fadeRate = 0;
    repo.add(baseVU);
    repo.add(new EC::PeakGlitterVU(leds, NUM_LEDS, audioSample, true));
    animationDuration = 12;
}

void makeVuSequence3(EC::AnimationRepo &repo)
{
    auto baseVU = new EC::EssentialVU(leds, NUM_LEDS, audioSample);
    baseVU->fadeRate = 0;
    repo.add(baseVU);
    animationDuration = 16;
}

void makeVuSequence4(EC::AnimationRepo &repo)
{
    repo.add(new EC::EssentialVU(leds, NUM_LEDS, audioSample));
    repo.add(new EC::PeakGlitterVU(leds, NUM_LEDS, audioSample, true));
    animationDuration = 10;
}

void makeVuSequence5(EC::AnimationRepo &repo)
{
    repo.add(new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample));
    repo.add(new EC::PeakGlitterVU(leds, NUM_LEDS, audioSample, true));
    animationDuration = 10;
}

void makeVuSequence6(EC::AnimationRepo &repo)
{
    makeBouncingDotVU(repo);
    animationDuration = 10;
}

void makeVuSequence7(EC::AnimationRepo &repo)
{
    repo.add(new EC::RainbowLevelVU(leds, NUM_LEDS, audioSample));
    // animationDuration = 15;
}

void makeVuSequence8(EC::AnimationRepo &repo)
{
    makeRainbowBubbleVU(repo);
    // animationDuration = 20;
}

void makeVuSequence9(EC::AnimationRepo &repo)
{
    makeRainbowLevelInwardVU(repo);
    // animationDuration = 15;
}

void makeVuSequence10(EC::AnimationRepo &repo)
{
    makeRainbowLevelCenteredVU(repo);
    // animationDuration = 15;
}

void makeVuSequence11(EC::AnimationRepo &repo)
{
    makeRainbowBubbleCenteredVU(repo);
    // animationDuration = 15;
}

void makeVuSequence12(EC::AnimationRepo &repo)
{
    makeRainbowBubbleInwardVU(repo);
    // animationDuration = 20;
}

void makeVuSequence13(EC::AnimationRepo &repo)
{
    makeDancingDotVU(repo);
    // animationDuration = 15;
}

void makeVuSequence14(EC::AnimationRepo &repo)
{
    makeDoubleBouncingDotVU(repo);
    // animationDuration = 15;
}

void makeVuSequence15(EC::AnimationRepo &repo)
{
    makeDoubleDancingDotVU1(repo);
    // animationDuration = 20;
}

void makeVuSequence16(EC::AnimationRepo &repo)
{
    makeDoubleDancingDotVU2(repo);
    // animationDuration = 15;
}

void makeVuSequence17(EC::AnimationRepo &repo)
{
    makeFireVU(repo);
    animationDuration = 30;
}

void makeVuSequence18(EC::AnimationRepo &repo)
{
    makeFlareInwardVU(repo);
}

void makeVuSequence19(EC::AnimationRepo &repo)
{
    makeFlareVU(repo);
}

void makeVuSequence20(EC::AnimationRepo &repo)
{
    makeFlareDoubleVU(repo);
    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationBuilderFct nextAnimation = nullptr;

EC::AnimationBuilderFct allAnimations[] = {
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
    &makeVuSequence18,
    &makeVuSequence19,
    &makeVuSequence20,
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

    handleAnimationChange(currentMillis);
    animationChanger.process(currentMillis);
    FastLED.show();
}

//------------------------------------------------------------------------------
