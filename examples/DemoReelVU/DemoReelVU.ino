/*******************************************************************************

Many many VU-Animations!

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

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

// run max. 16 Animations simultaneously
EC::AnimationRunnerM animationRunner;

ButtonHandler selectButton;

bool autoMode = true;

float audioSample = 0.0;
AudioNormalizer normalizer;

#define PRINT_MEMORY_USAGE 1

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
#if (PRINT_MEMORY_USAGE)
    printMemoryUsage();
#endif
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
uint16_t animationDuration = defaultAnimationDuration;

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
    repo.add(new EC::DancingDotVU(audioSample, strip, true/*, CRGB::Red*/));
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

    auto vu1 = new EC::DancingDotVU(audioSample, strip, true/*, CRGB(255, 0, 0)*/);
    vu1->color = CRGB(255, 0, 0);

    auto vu2 = new EC::DancingDotVU(audioSample, strip.getReversedStrip(), true/*, CRGB(0, 255, 0)*/);
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

    auto vu1 = new EC::DancingDotVU(audioSample, strip, true/*, CHSV(20, 255, 255)*/);
    vu1->color = CHSV(20, 255, 255);

    auto vu2 = new EC::DancingDotVU(audioSample, strip.getReversedStrip(), true/*, CHSV(20 + 128, 255, 255)*/);
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

void makeFlareVU(EC::FastLedStrip strip, EC::AnimationRepo &repo)
{
    const uint16_t fireLedCount = strip.ledCount() / 2 + strip.ledCount() / 10;
    EC::FastLedStrip fireStrip = strip.getSubStrip(0, fireLedCount, true);
    auto fire = new EC::Fire2012<NUM_LEDS>(fireStrip);
    fire->animationDelay = 10;

    repo.add(fire);
    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire));
    repo.add(new EC::Kaleidoscope(strip));
}

void makeFlareVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    makeFlareVU(strip, repo);
}

void makeFlareDoubleVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    makeFlareVU(strip.getHalfStrip(), repo);
    repo.add(new EC::Kaleidoscope(strip));
}

void makeFlareInwardVU(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    EC::FastLedStrip fireStrip1 = strip.getHalfStrip();
    auto fire1 = new EC::Fire2012<NUM_LEDS>(fireStrip1);
    fire1->animationDelay = 11;
    repo.add(fire1);
    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire1));

    EC::FastLedStrip fireStrip2 = strip.getSubStrip(fireStrip1.ledCount(), 0, true);
    auto fire2 = new EC::Fire2012<NUM_LEDS>(fireStrip2);
    fire2->animationDelay = 13;
    repo.add(fire2);
    repo.add(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire2));
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
    animationDuration = 8;
}

void makeVuSequence2(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto baseVU = new EC::EssentialVU(audioSample, strip, false);
    baseVU->enableVuBar = false;
    baseVU->fadeRate = 0;
    repo.add(baseVU);
    repo.add(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 12;
}

void makeVuSequence3(EC::AnimationRepo &repo)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto baseVU = new EC::EssentialVU(audioSample, strip, false);
    baseVU->fadeRate = 0;
    repo.add(baseVU);
    animationDuration = 16;
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
    animationDuration = 10;
}

void makeVuSequence6(EC::AnimationRepo &repo)
{
    makeBouncingDotVU(repo);
    animationDuration = 10;
}

void makeVuSequence7(EC::AnimationRepo &repo)
{
    makeRainbowLevelVU(repo);
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

#if (PRINT_MEMORY_USAGE)
void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

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
