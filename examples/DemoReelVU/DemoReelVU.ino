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

//------------------------------------------------------------------------------

#include <EyeCandy.h>
#include <AudioNormalizer.h>
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// #define LED_COLOR_ORDER RGB
// #define NUM_LEDS 50
#include <Animation_IO_config.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

ButtonHandler selectButton;

bool autoMode = true;

float audioSample = 0.0;
EC::AudioNormalizer normalizer;

//------------------------------------------------------------------------------

void setup()
{
    random16_set_seed(analogRead(A3));
    randomSeed(random16_get_seed());

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

// ---------- VUs ----------

void makeRawAudioVU(EC::AnimationScene &scene)
{
    auto vu = scene.append(new EC::RawAudioVU(audioSample, {leds, NUM_LEDS}));
    // animationDuration = 10;
}

// ---

void makeDancingDotVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    auto vuPeakSource = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));

    auto levelVu = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource));
    levelVu->color.volume = 64;

    auto peakVu = scene.append(new EC::VuOverlayStripe(strip, *vuPeakSource));
}

void makeDoubleDancingDotVU1(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pride2015(strip.getHalfStrip(/*true*/)));
    scene.append(new EC::Kaleidoscope(strip));
    scene.append(new EC::BgFadeToBlack(strip, true, 150));

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto vuPeakSource = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));
    vuPeakSource->vuPeakHandler.mass = 0.5;

    auto vu1 = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource, *vuLevelSource, 0.075));
    vu1->color.bpm = 0.7;

    auto vu2 = scene.append(new EC::VuOverlayRainbowDot(strip.getReversedStrip(), *vuPeakSource, *vuLevelSource, 0.075));
}

void makeDoubleDancingDotVU2(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::FloatingBlobs(strip));
    scene.append(new EC::BgFadeToBlack(strip, true, 230));

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto vuPeakSource1 = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));
    vuPeakSource1->vuPeakHandler.coupling = 50.0;
    auto vuPeakSource2 = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));
    vuPeakSource2->vuPeakHandler.friction = 5.0;

    auto vu1 = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource1, 0.125));
    vu1->color.bpm = 0.7;

    auto vu2 = scene.append(new EC::VuOverlayRainbowDot(strip.getReversedStrip(), *vuPeakSource2, 0.125));
}

void makeManyDancingDotVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    // auto levelVu = scene.append(new EC::VuOverlayStripe(strip, *vuLevelSource, CRGB(128, 0, 0)));

    float mass = 1.0;
    float friction = 5.0;
    for (uint8_t i = 1; i <= 8; ++i)
    {
        auto vuPeakSource = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));
        vuPeakSource->vuPeakHandler.mass = mass;
        vuPeakSource->vuPeakHandler.friction = friction;

        bool flipped = i & 0x01;
        // flipped = false;
        auto peakVu = scene.append(new EC::VuOverlayRainbowStripe(strip.getSubStrip(0, 0, flipped), *vuPeakSource));
        peakVu->color.hueRange = 0.75;

        // mass += i * 0.25;
        friction += i * 3.5;
        // mass += 0.75;
        // friction += 5.0;
    }
}

void makePeakMothsVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    auto vuLevelSource1 = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));

    auto levelVu = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource1));
    levelVu->color.volume = 64;

    auto peakVu1 = scene.append(new EC::VuOverlayRainbowDot(strip, *vuLevelSource1));
    peakVu1->color.hueRange = 0.75;
    peakVu1->color.volume = 255;

    EC::VuSource *vuSource = &vuLevelSource1->asVuSource();
    for (uint8_t i = 1; i <= 4; ++i)
    {
        auto vuPeakSource = scene.append(new EC::VuSourcePeakForce(*vuSource));
        vuPeakSource->vuPeakHandler.mass = 1.55;
        // vuPeakSource->vuPeakHandler.friction = 10.0;
        // vuPeakSource->vuPeakHandler.coupling = 100.0;

        auto peakVu = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource));
        peakVu->color = peakVu1->color;

        vuSource = &vuPeakSource->asVuSource();
    }
}

// ---

void makeFireVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = scene.append(new EC::Fire2012<NUM_LEDS>(strip));
    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    scene.append(new EC::Fire2012VU<NUM_LEDS>(*fire, *vuSource));
}

// ---

void makeFlareVU(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    const int16_t ledCount = strip.ledCount();
    const int16_t fireLedCount = ledCount / 2 + ledCount / 10;
    EC::FastLedStrip fireStrip = strip.getSubStrip(0, fireLedCount, true);

    auto fire = scene.append(new EC::Fire2012<NUM_LEDS>(fireStrip));
    fire->setModelUpdatePeriod(10);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    scene.append(new EC::Fire2012VU<NUM_LEDS>(*fire, *vuSource));
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
    auto fire1 = scene.append(new EC::Fire2012<NUM_LEDS>(fireStrip1));
    fire1->setModelUpdatePeriod(11);

    EC::FastLedStrip fireStrip2 = strip.getSubStrip(fireStrip1.ledCount(), 0, true);
    auto fire2 = scene.append(new EC::Fire2012<NUM_LEDS>(fireStrip2));
    fire2->setModelUpdatePeriod(13);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    scene.append(new EC::Fire2012VU<NUM_LEDS>(*fire1, *vuSource));
    scene.append(new EC::Fire2012VU<NUM_LEDS>(*fire2, *vuSource));
}

// ---

void makeMeteorTrailVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::BgMeteorFadeToBlack(strip, false));
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto levelVu = scene.append(new EC::VuOverlayRainbowStripe(strip, *vuLevelSource));
    levelVu->color.hueRange = 0.67;
    levelVu->color.volume = 255;

    // autoMode = false;
}

// ---

void makePeakGlitterVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::VuOverlayPeakGlitter::fadeRate_default()));
    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuSource));
}

// ---

void makeRainbowBallVU(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintRainbowVU::fadeRate));

    EC::BlueprintRainbowVU bp(strip, scene, *vuLevelSource);
    bp.peakSource->vuPeakHandler.presetPunchedBall();
    // bp.setVuRange(0.67);
}

void makeRainbowBallVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeRainbowBallVU(strip, scene);
}

void makeRainbowBalllVU_inward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeRainbowBallVU(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowBalllVU_outward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeRainbowBallVU(strip.getHalfStrip(true), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

// ---

void makeRainbowBubbleVU(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintRainbowVU::fadeRate));

    EC::BlueprintRainbowVU bp(strip, scene, *vuLevelSource);
    bp.peakSource->vuPeakHandler.presetFloatingBubble();
    bp.setVuRange(0.67);
}

void makeRainbowBubbleVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeRainbowBubbleVU(strip, scene);
}

void makeRainbowBubbleVU_inward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeRainbowBubbleVU(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeRainbowBubbleVU_outward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeRainbowBubbleVU(strip.getHalfStrip(true), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

// ---

void makeEjectingDotVu(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintEjectingDotVu::fadeRate));
    EC::BlueprintEjectingDotVu bp(strip, scene, *vuLevelSource);
}

void makeEjectingDotVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeEjectingDotVu(strip, scene);
}

void makeEjectingDotVu_inward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeEjectingDotVu(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeEjectingDotVu_outward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeEjectingDotVu(strip.getHalfStrip(true), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

// ---

void makeFranticVu(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintFranticVu::fadeRate));
    EC::BlueprintFranticVu bp(strip, scene, *vuLevelSource);
}

void makeFranticVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeFranticVu(strip, scene);
}

void makeFranticVu_inward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeFranticVu(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeFranticVu_outward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeFranticVu(strip.getHalfStrip(true), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

// ---

void makeCrazyVu(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintCrazyVu::fadeRate));
    EC::BlueprintCrazyVu bp(strip, scene, *vuLevelSource);
}

void makeCrazyVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeCrazyVu(strip, scene);
}

void makeCrazyVu_inward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeCrazyVu(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeCrazyVu_outward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeCrazyVu(strip.getHalfStrip(true), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

// ---

void makeBeyondCrazyVu(EC::FastLedStrip strip, EC::AnimationScene &scene)
{
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintBeyondCrazyVu::fadeRate));
    EC::BlueprintBeyondCrazyVu bp(strip, scene, *vuLevelSource);
}

void makeBeyondCrazyVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeBeyondCrazyVu(strip, scene);
}

void makeBeyondCrazyVu_inward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeBeyondCrazyVu(strip.getHalfStrip(), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

void makeBeyondCrazyVu_outward(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    makeBeyondCrazyVu(strip.getHalfStrip(true), scene);
    scene.append(new EC::Kaleidoscope(strip));
}

// ---------- VU sequence ----------

void makeVuIntro1(EC::AnimationScene &scene)
{
    makePeakGlitterVU(scene);
    animationDuration = 8;
}

void makeVuIntro2(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    auto peakSource = scene.append(new EC::VuSourcePeakHold(*vuLevelSource));
    scene.append(new EC::VuOverlayDot(strip, *peakSource));
    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));
    animationDuration = 12;
}

void makeVuIntro3(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    EC::BlueprintBasicVU bp(strip, scene, *vuLevelSource);
    animationDuration = 16;
}

void makeVuIntro4(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintBasicVU::fadeRate));
    EC::BlueprintBasicVU bp(strip, scene, *vuLevelSource);
    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));
    animationDuration = 10;
}

void makeVuIntro5(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));

    EC::BlueprintRainbowVU bp(strip, scene, *vuLevelSource);
    bp.setVuRange(0.15);

    scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));
    animationDuration = 10;
}

void makeVuIntro6(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

#if (1)
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));
#else
    scene.append(new EC::BgMeteorFadeToBlack(strip, false));
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
#endif
    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuPeakSource->vuPeakHandler.presetPunchedBall();

    auto vuPeak = scene.append(new EC::VuOverlayRainbowStripe(strip, *vuPeakSource));
    vuPeak->color.hueRange = 0.5;

    auto glitter = scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));

    animationDuration = 10;
}

void makeVuSequence13(EC::AnimationScene &scene)
{
    makeDancingDotVU(scene);
    // animationDuration = 15;
}

void makeVuSequence14(EC::AnimationScene &scene)
{
    makePeakMothsVU(scene);
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

void makeVuSequence16a(EC::AnimationScene &scene)
{
    makeManyDancingDotVU(scene);
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

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct nextAnimation = nullptr;

EC::AnimationSceneBuilderFct allAnimations[] = {
    // &makeRawAudioVU,

    &makeVuIntro1,
    &makeVuIntro2,
    &makeVuIntro3,
    &makeVuIntro4,
    &makeVuIntro5,
    &makeVuIntro6,

    &makeRainbowBallVU,
    &makeRainbowBubbleVU,
    &makeEjectingDotVu,

    &makeRainbowBalllVU_inward,
    &makeRainbowBubbleVU_inward,
    &makeEjectingDotVu_inward,

    &makeRainbowBalllVU_outward,
    &makeRainbowBubbleVU_outward,
    &makeEjectingDotVu_outward,

    &makeMeteorTrailVU,

    &makeFranticVu,
    &makeCrazyVu,
    &makeBeyondCrazyVu,

    &makeFranticVu_inward,
    &makeCrazyVu_inward,
    &makeBeyondCrazyVu_inward,

    &makeFranticVu_outward,
    &makeCrazyVu_outward,
    &makeBeyondCrazyVu_outward,

    &makeVuSequence13,
    &makeVuSequence14,
    &makeVuSequence15,
    &makeVuSequence16,
    // &makeVuSequence16a,

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
    if (animationChanger.process(currentMillis))
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
