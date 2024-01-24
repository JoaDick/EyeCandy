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

void makeRawAudioVU(EC::AnimationScene &scene)
{
    auto vu = scene.append(new EC::RawAudioVU(audioSample, {leds, NUM_LEDS}));
    // animationDuration = 10;
}

void makeBouncingDotVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuPeak = scene.append(new EC::RainbowLevelVU(audioSample, strip, false));
    vuPeak->enableVuBar = false;
    vuPeak->vuHueRange = 0.5;

    // auto glitter = scene.append(new EC::PeakGlitterVU(audioSample, strip, true, CRGB(128, 64, 0));
    auto glitter = scene.append(new EC::PeakGlitterVU(audioSample, strip, true));
    glitter->color = CRGB(128, 64, 0);
}

void makeBouncingDotVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));
    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuPeakSource->vuPeakHandler.presetPunchedBall();

    auto vuPeak = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource));
    vuPeak->vuHueRange = 0.5;

    // auto glitter = scene.append(new EC::VuOverlayPeakGlitter(strip, vuLevelSource, CRGB(128, 64, 0));
    auto glitter = scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));
    glitter->color = CRGB(128, 64, 0);
}

void makeDancingDotVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = scene.append(new EC::RainbowLevelVU(audioSample, strip, false));
    vu->enablePeakDot = false;
    vu->fadeRate = 0;
    vu->volume = 64;

    scene.append(new EC::DancingDotVU(audioSample, strip, true /*, CRGB::Red*/));
}

void makeDancingDotVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));

    auto vu = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource));
    // vu->fadeRate = 0;
    vu->volume = 64;

    scene.append(new EC::DancingDotVU(audioSample, strip, true /*, CRGB::Red*/));
}

void makeDoubleBouncingDotVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu1 = scene.append(new EC::RainbowLevelVU(audioSample, strip, false));
    vu1->fadeRate = 50;
    vu1->enableVuBar = false;
    vu1->volume = 255;
    vu1->vuHueRange = 0.4;
    vu1->baseHueStep = -0.17;
    vu1->vuLevelHandler.smoothingFactor = 6;

    auto vu2 = scene.append(new EC::RainbowLevelVU(audioSample, strip.getReversedStrip(), true));
    vu2->enableVuBar = false;
    vu2->volume = 255;
    vu2->vuHueRange = 0.6;
    vu2->baseHueStep = 0.11;
    vu2->vuLevelHandler.smoothingFactor = 9;
}

void makeDoubleBouncingDotVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));
    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuPeakSource->vuPeakHandler.presetPunchedBall();

    auto vu1 = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource));
    vu1->volume = 255;
    vu1->vuHueRange = 0.4;
    vu1->baseHueStep = -0.17;
    // vu1->vuLevelHandler.smoothingFactor = 6;

    auto vu2 = scene.append(new EC::VuOverlayRainbowDot(strip.getReversedStrip(), *vuPeakSource));
    vu2->volume = 255;
    vu2->vuHueRange = 0.6;
    vu2->baseHueStep = 0.11;
    // vu2->vuLevelHandler.smoothingFactor = 9;
}

void makeDoubleDancingDotVU1(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pride2015(strip.getHalfStrip(/*true*/)));
    scene.append(new EC::Kaleidoscope(strip));
    scene.append(new EC::FadeOutOverlay(strip, 100));

    auto vu1 = scene.append(new EC::DancingDotVU(audioSample, strip, true /*, CRGB(255, 0, 0)*/));
    vu1->color = CRGB(255, 0, 0);

    auto vu2 = scene.append(new EC::DancingDotVU(audioSample, strip.getReversedStrip(), true /*, CRGB(0, 255, 0)*/));
    vu2->color = CRGB(0, 255, 0);
    vu2->vuPeakHandler.inertia = 0.55;
    vu2->vuPeakHandler.friction = 0.14;
}

void makeDoubleDancingDotVU2(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::FloatingBlobs(strip));
    scene.append(new EC::FadeOutOverlay(strip, 230));

    auto vu1 = scene.append(new EC::DancingDotVU(audioSample, strip, true /*, CHSV(20, 255, 255)*/));
    vu1->color = CHSV(20, 255, 255);

    auto vu2 = scene.append(new EC::DancingDotVU(audioSample, strip.getReversedStrip(), true /*, CHSV(20 + 128, 255, 255)*/));
    vu2->color = CHSV(20 + 128, 255, 255);
    vu2->vuPeakHandler.inertia = 0.55;
    vu2->vuPeakHandler.friction = 0.14;
}

void makeEssentialVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::EssentialVU(audioSample, strip, false));
}

void makeEssentialVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::EssentialVU2::fadeRate_default()));
    scene.append(new EC::EssentialVU2(strip, *vuSource));
}

void makeFireVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = new EC::Fire2012<NUM_LEDS>(strip);
    auto fireChanger = new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire);

    scene.append(fire);
    scene.append(fireChanger);
}

void makeFlareVU(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    const uint16_t fireLedCount = strip.ledCount() / 2 + strip.ledCount() / 10;
    EC::FastLedStrip fireStrip = strip.getSubStrip(0, fireLedCount, true);
    auto fire = new EC::Fire2012<NUM_LEDS>(fireStrip);
    fire->modelUpdatePeriod = 10;

    scene.append(fire);
    scene.append(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire));
    scene.append(new EC::Kaleidoscope(strip));
}

void makeFlareVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    makeFlareVU(strip, scene);
}

void makeFlareDoubleVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    makeFlareVU(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeFlareInwardVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    EC::FastLedStrip fireStrip1 = strip.getHalfStrip();
    auto fire1 = new EC::Fire2012<NUM_LEDS>(fireStrip1);
    fire1->modelUpdatePeriod = 11;
    scene.append(fire1);
    scene.append(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire1));

    EC::FastLedStrip fireStrip2 = strip.getSubStrip(fireStrip1.ledCount(), 0, true);
    auto fire2 = new EC::Fire2012<NUM_LEDS>(fireStrip2);
    fire2->modelUpdatePeriod = 13;
    scene.append(fire2);
    scene.append(new EC::Fire2012VU<NUM_LEDS>(audioSample, *fire2));
}

void makePeakGlitterVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::PeakGlitterVU(audioSample, strip, false));
}

void makePeakGlitterVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::VuOverlayPeakGlitter::fadeRate_default()));
    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuSource));
}

void makeRainbowBubbleVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = scene.append(new EC::RainbowLevelVU(audioSample, strip, false));
    vu->vuPeakHandler.presetFloatingBubble();
    vu->vuHueRange = 0.67;
}

void makeRainbowBubbleVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip, *vuLevelSource));
    vu->vuPeakSource.vuPeakHandler.presetFloatingBubble();
    vu->vuLevelBar.vuHueRange = 0.67;
    vu->vuPeakDot.vuHueRange = vu->vuLevelBar.vuHueRange;
}

void makeRainbowBubbleCenteredVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = scene.append(new EC::RainbowLevelVU(audioSample, strip.getHalfStrip(true), false));
    vu->vuPeakHandler.presetFloatingBubble();
    vu->vuHueRange = 0.67;

    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowBubbleCenteredVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip.getHalfStrip(true), *vuLevelSource));
    vu->vuPeakSource.vuPeakHandler.presetFloatingBubble();
    vu->vuLevelBar.vuHueRange = 0.67;
    vu->vuPeakDot.vuHueRange = vu->vuLevelBar.vuHueRange;

    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowBubbleInwardVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = scene.append(new EC::RainbowLevelVU(audioSample, strip.getHalfStrip(), false));
    vu->vuPeakHandler.presetFloatingBubble();
    vu->vuHueRange = 0.67;

    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowBubbleInwardVU_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip.getHalfStrip(), *vuLevelSource));
    vu->vuPeakSource.vuPeakHandler.presetFloatingBubble();
    vu->vuLevelBar.vuHueRange = 0.67;
    vu->vuPeakDot.vuHueRange = vu->vuLevelBar.vuHueRange;

    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowLevelVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip, *vuLevelSource));
}

void makeRainbowLevelCenteredVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip.getHalfStrip(true), *vuLevelSource));
    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowLevelInwardVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip.getHalfStrip(), *vuLevelSource));
    scene.append(new EC::Kaleidoscope(strip));
}

// ---------- VU sequence ----------

void makeVuSequence1(EC::AnimationScene &scene)
{
    makePeakGlitterVU(scene);
    animationDuration = 8;
}

void makeVuSequence1_a(EC::AnimationScene &scene)
{
    makePeakGlitterVU_a(scene);
    animationDuration = 8;
}

void makeVuSequence2(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto baseVU = scene.append(new EC::EssentialVU(audioSample, strip, false));
    baseVU->enableVuBar = false;
    baseVU->fadeRate = 0;

    scene.append(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 12;
}

void makeVuSequence2_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    auto baseVU = scene.append(new EC::EssentialVU2(strip, *vuSource));
    baseVU->enableVuLevelBar = false;

    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuSource));
    animationDuration = 12;
}

void makeVuSequence3(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto baseVU = scene.append(new EC::EssentialVU(audioSample, strip, false));
    baseVU->fadeRate = 0;
    animationDuration = 16;
}

void makeVuSequence3_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    auto baseVU = scene.append(new EC::EssentialVU2(strip, *vuSource));
    animationDuration = 16;
}

void makeVuSequence4(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::EssentialVU(audioSample, strip, false));
    scene.append(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 10;
}

void makeVuSequence4_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::EssentialVU2::fadeRate_default()));
    scene.append(new EC::EssentialVU2(strip, *vuSource));
    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuSource));
    animationDuration = 10;
}

void makeVuSequence5(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::RainbowLevelVU(audioSample, strip, false));
    scene.append(new EC::PeakGlitterVU(audioSample, strip, true));
    animationDuration = 10;
}

void makeVuSequence5_a(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::RainbowLevelVU2(strip, *vuLevelSource));
    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));
    animationDuration = 10;
}

void makeVuSequence6(EC::AnimationScene &scene)
{
    makeBouncingDotVU(scene);
    animationDuration = 10;
}

void makeVuSequence6_a(EC::AnimationScene &scene)
{
    makeBouncingDotVU_a(scene);
    animationDuration = 10;
}

void makeVuSequence7(EC::AnimationScene &scene)
{
    makeRainbowLevelVU(scene);
    // animationDuration = 15;
}

void makeVuSequence8(EC::AnimationScene &scene)
{
    makeRainbowBubbleVU(scene);
    // animationDuration = 20;
}

void makeVuSequence8_a(EC::AnimationScene &scene)
{
    makeRainbowBubbleVU_a(scene);
    // animationDuration = 20;
}

void makeVuSequence9(EC::AnimationScene &scene)
{
    makeRainbowLevelInwardVU(scene);
    // animationDuration = 15;
}

void makeVuSequence10(EC::AnimationScene &scene)
{
    makeRainbowLevelCenteredVU(scene);
    // animationDuration = 15;
}

void makeVuSequence11(EC::AnimationScene &scene)
{
    makeRainbowBubbleCenteredVU(scene);
    // animationDuration = 15;
}

void makeVuSequence11_a(EC::AnimationScene &scene)
{
    makeRainbowBubbleCenteredVU_a(scene);
    // animationDuration = 15;
}

void makeVuSequence12(EC::AnimationScene &scene)
{
    makeRainbowBubbleInwardVU(scene);
    // animationDuration = 20;
}

void makeVuSequence12_a(EC::AnimationScene &scene)
{
    makeRainbowBubbleInwardVU_a(scene);
    // animationDuration = 20;
}

void makeVuSequence13(EC::AnimationScene &scene)
{
    makeDancingDotVU(scene);
    // animationDuration = 15;
}

void makeVuSequence13_a(EC::AnimationScene &scene)
{
    makeDancingDotVU_a(scene);
    // animationDuration = 15;
}

void makeVuSequence14(EC::AnimationScene &scene)
{
    makeDoubleBouncingDotVU(scene);
    // animationDuration = 15;
}

void makeVuSequence14_a(EC::AnimationScene &scene)
{
    makeDoubleBouncingDotVU_a(scene);
    // animationDuration = 15;
}

void makeVuSequence15(EC::AnimationScene &scene)
{
    makeDoubleDancingDotVU1(scene);
    // animationDuration = 20;
}

void makeVuSequence16(EC::AnimationScene &scene)
{
    makeDoubleDancingDotVU2(scene);
    // animationDuration = 15;
}

void makeVuSequence17(EC::AnimationScene &scene)
{
    makeFireVU(scene);
    animationDuration = 30;
}

void makeVuSequence18(EC::AnimationScene &scene)
{
    makeFlareInwardVU(scene);
}

void makeVuSequence19(EC::AnimationScene &scene)
{
    makeFlareVU(scene);
}

void makeVuSequence20(EC::AnimationScene &scene)
{
    makeFlareDoubleVU(scene);
    // autoMode = false;
}

void makeCompoundVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    // auto vuPeakSource = scene.append(new EC::VuSourcePeakHold(*vuLevelSource));
    // // vuPeakSource->vuPeakHandler.peakHold = 1000;

    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    // vuPeakSource->vuPeakHandler.presetPunchedBall();
    // vuPeakSource->vuPeakHandler.presetFloatingBubble();

    auto vuLevelBar = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource /*, *vuPeakSource*/));
    auto vuPeakDot = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource, *vuLevelSource));

    // scene.append(new EC::VuOverlayLine(strip, *vuLevelSource));
    // scene.append(new EC::VuOverlayDot(strip, *vuPeakSource));

    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct nextAnimation = nullptr;

EC::AnimationSceneBuilderFct allAnimations[] = {
    // &makeVuSequence14,
    // &makeVuSequence14_a,
    // &makeVuSequence5,
    // &makeVuSequence5_a,
    // &makeVuSequence6,
    // &makeVuSequence6_a,
    // &makeVuSequence8,
    // &makeVuSequence8_a,
    // &makeVuSequence11,
    // &makeVuSequence11_a,
    // &makeVuSequence12,
    // &makeVuSequence12_a,
    // &makeVuSequence13,
    // &makeVuSequence13_a,

    &makeCompoundVu,

    &makeRawAudioVU,
    // &makeVuSequence1,
    &makeVuSequence1_a,
    // &makeVuSequence2,
    &makeVuSequence2_a,
    // &makeVuSequence3,
    &makeVuSequence3_a,
    // &makeVuSequence4,
    &makeVuSequence4_a,
    // &makeVuSequence5,
    &makeVuSequence5_a,
    // &makeVuSequence6,
    &makeVuSequence6_a,
    &makeVuSequence7,
    // &makeVuSequence8,
    &makeVuSequence8_a,
    &makeVuSequence9,
    &makeVuSequence10,
    // &makeVuSequence11,
    &makeVuSequence11_a,
    // &makeVuSequence12,
    &makeVuSequence12_a,
    // &makeVuSequence13,
    &makeVuSequence13_a,
    // &makeVuSequence14,
    &makeVuSequence14_a,
    &makeVuSequence15,
    &makeVuSequence16,
    &makeVuSequence17,
    &makeVuSequence18,
    &makeVuSequence19,
    &makeVuSequence20,
    nullptr};

// EC::AnimationChanger animationChanger(allAnimations);
EC::AnimationChangerSoft animationChanger(allAnimations);

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

    // ----- old -----
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

    Serial.print(F("RawAudioVU = "));
    Serial.println(sizeof(EC::RawAudioVU));

#if (0)
    Serial.println();

    // ----- new -----
    Serial.print(F("EssentialVU2 = "));
    Serial.println(sizeof(EC::EssentialVU2));

    Serial.print(F("RainbowLevelVU2 = "));
    Serial.println(sizeof(EC::RainbowLevelVU2));

    Serial.print(F("VuOverlayDot = "));
    Serial.println(sizeof(EC::VuOverlayDot));

    Serial.print(F("VuOverlayLine = "));
    Serial.println(sizeof(EC::VuOverlayLine));

    Serial.print(F("VuOverlayPeakGlitter = "));
    Serial.println(sizeof(EC::VuOverlayPeakGlitter));

    Serial.print(F("VuOverlayRainbowDot = "));
    Serial.println(sizeof(EC::VuOverlayRainbowDot));

    Serial.print(F("VuOverlayRainbowLine = "));
    Serial.println(sizeof(EC::VuOverlayRainbowLine));

    Serial.print(F("VuSourceAnalogPin = "));
    Serial.println(sizeof(EC::VuSourceAnalogPin));

    Serial.print(F("VuSourcePeakGravity = "));
    Serial.println(sizeof(EC::VuSourcePeakGravity));

    Serial.print(F("VuSourcePeakHold = "));
    Serial.println(sizeof(EC::VuSourcePeakHold));
#endif
}
#endif

//------------------------------------------------------------------------------
