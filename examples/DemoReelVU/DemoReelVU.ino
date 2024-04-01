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

//------------------------------------------------------------------------------

void setup()
{
    random16_set_seed(analogRead(A3));
    randomSeed(random16_get_seed());

    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    pinMode(LED_BUILTIN, OUTPUT);
#endif

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

/// Show the raw audio input as VU Animation.
void make_RawAudioVU(EC::SetupEnv &env)
{
    auto &vu = env.add(new EC::RawAudioVU(PIN_MIC, {leds, NUM_LEDS}));
    // animationDuration = 10;
}

// ---

void make_DancingDotVU(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(0);
    auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));

    auto &levelVu = env.add(new EC::VuOverlayRainbowLine(env.strip(), vuLevelSource));
    levelVu.color.volume = 64;

    auto &peakVu = env.add(new EC::VuOverlayStripe(env.strip(), vuPeakSource));
}

void make_DoubleDancingDotVU1(EC::SetupEnv &env)
{
    env.add(new EC::Pride2015(env.strip().getHalfStrip(/*true*/)));
    env.add(new EC::Kaleidoscope(env.strip()));
    env.add(new EC::BgFadeToBlack(env.strip(), true, 150));

    auto &vuLevelSource = env.addVuSource();
    auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));
    vuPeakSource.vuPeakHandler.mass = 0.5;

    auto &vu1 = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource, vuLevelSource, 0.075));
    vu1.color.bpm = 0.7;

    auto &vu2 = env.add(new EC::VuOverlayRainbowDot(env.strip().getReversedStrip(), vuPeakSource, vuLevelSource, 0.075));
}

void make_DoubleDancingDotVU2(EC::SetupEnv &env)
{
    env.add(new EC::FloatingBlobs(env.strip()));
    env.add(new EC::BgFadeToBlack(env.strip(), true, 230));

    auto &vuLevelSource = env.addVuSource();
    auto &vuPeakSource1 = env.add(new EC::VuSourcePeakForce(vuLevelSource));
    vuPeakSource1.vuPeakHandler.coupling = 50.0;
    auto &vuPeakSource2 = env.add(new EC::VuSourcePeakForce(vuLevelSource));
    vuPeakSource2.vuPeakHandler.friction = 5.0;

    auto &vu1 = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource1, 0.125));
    vu1.color.bpm = 0.7;

    auto &vu2 = env.add(new EC::VuOverlayRainbowDot(env.strip().getReversedStrip(), vuPeakSource2, 0.125));
}

void make_ManyDancingDotVU(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(0);
    // auto &levelVu = env.add(new EC::VuOverlayStripe(env.strip(), vuLevelSource, CRGB(128, 0, 0)));

    float mass = 1.0;
    float friction = 5.0;
    for (uint8_t i = 1; i <= 8; ++i)
    {
        auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));
        vuPeakSource.vuPeakHandler.mass = mass;
        vuPeakSource.vuPeakHandler.friction = friction;

        bool flipped = i & 0x01;
        // flipped = false;
        auto &peakVu = env.add(new EC::VuOverlayRainbowStripe(env.strip().getSubStrip(0, 0, flipped), vuPeakSource));
        peakVu.color.hueRange = 0.75;

        // mass += i * 0.25;
        friction += i * 3.5;
        // mass += 0.75;
        // friction += 5.0;
    }
}

void make_PeakMothsVU(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(0);
    auto &vuLevelSource1 = env.add(new EC::VuSourcePeakForce(vuLevelSource));

    auto &levelVu = env.add(new EC::VuOverlayRainbowLine(env.strip(), vuLevelSource1));
    levelVu.color.volume = 64;

    auto &peakVu1 = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuLevelSource1));
    peakVu1.color.hueRange = 0.75;
    peakVu1.color.volume = 255;

    EC::VuSource *vuSource = &vuLevelSource1.asVuSource();
    for (uint8_t i = 1; i <= 4; ++i)
    {
        auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(*vuSource));
        vuPeakSource.vuPeakHandler.mass = 1.55;
        // vuPeakSource.vuPeakHandler.friction = 10.0;
        // vuPeakSource.vuPeakHandler.coupling = 100.0;

        auto &peakVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource));
        peakVu.color = peakVu1.color;

        vuSource = &vuPeakSource.asVuSource();
    }
}

// ---

void make_FireVU(EC::SetupEnv &env)
{
    auto &fire = env.add(new EC::Fire2012<NUM_LEDS>(env.strip()));
    auto &vuSource = env.addVuSource();
    env.add(new EC::Fire2012VU<NUM_LEDS>(fire, vuSource));
}

// ---

void make_FlareVU(EC::SetupEnv &env)
{
    const int16_t ledCount = env.strip().ledCount();
    const int16_t fireLedCount = ledCount / 2 + ledCount / 10;
    EC::FastLedStrip fireStrip = env.strip().getSubStrip(0, fireLedCount, true);

    auto &fire = env.add(new EC::Fire2012<NUM_LEDS>(fireStrip));
    fire.setModelUpdatePeriod(10);

    auto &vuSource = env.addVuSource();
    env.add(new EC::Fire2012VU<NUM_LEDS>(fire, vuSource));
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_FlareDoubleVU(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_FlareVU(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_FlareInwardVU(EC::SetupEnv &env)
{
    EC::FastLedStrip fireStrip1 = env.strip().getHalfStrip();
    auto &fire1 = env.add(new EC::Fire2012<NUM_LEDS>(fireStrip1));
    fire1.setModelUpdatePeriod(11);

    EC::FastLedStrip fireStrip2 = env.strip().getSubStrip(fireStrip1.ledCount(), 0, true);
    auto &fire2 = env.add(new EC::Fire2012<NUM_LEDS>(fireStrip2));
    fire2.setModelUpdatePeriod(13);

    auto &vuSource = env.addVuSource();
    env.add(new EC::Fire2012VU<NUM_LEDS>(fire1, vuSource));
    env.add(new EC::Fire2012VU<NUM_LEDS>(fire2, vuSource));
}

// ---

void make_BallLightningVU(EC::SetupEnv &env)
{
    env.add(new EC::TriggerPattern());
    auto &vuLevelSource = env.addVuSource();
    env.add(new EC::BallLightningVU(env.strip(), vuLevelSource));
}

// ---

void make_BlackHoleVU(EC::SetupEnv &env)
{
    auto workStrip = env.strip().getHalfStrip(true);

    env.add(new EC::BgFadeToBlack(20, workStrip, 40));
    env.add(new EC::BgRotate(workStrip, true));
    auto &vuLevelSource = env.addVuSource();

    auto &levelVu = env.add(new EC::VuOverlayRainbowStripe(workStrip, vuLevelSource));
    levelVu.color.hueRange = 0.5;
    levelVu.color.volume = 255;

    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---

void make_DancingJellyfishVU(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(EC::DancingJellyfishVU::fadeRate_default);
    env.add(new EC::DancingJellyfishVU(env.strip(), vuLevelSource));
}

// ---

void make_FlowingBeatVU(EC::SetupEnv &env)
{
    env.add(new EC::TriggerPattern(EC::FlowingBeatVU::patternUpdatePeriod));
    auto &vuLevelSource = env.addVuSource();
    env.add(new EC::FlowingBeatVU(env.strip(), vuLevelSource));
}

// ---

void make_LightbulbVU(EC::SetupEnv &env)
{
    env.add(new EC::TriggerPattern());
    auto &vuLevelSource = env.addVuSource();
    env.add(new EC::LightbulbVU(env.strip(), vuLevelSource));
}

// ---

void make_MeteorTrailVU(EC::SetupEnv &env)
{
    env.add(new EC::BgMeteorFadeToBlack(env.strip(), false));
    auto &vuLevelSource = env.addVuSource();
    auto &levelVu = env.add(new EC::VuOverlayRainbowStripe(env.strip(), vuLevelSource));
    levelVu.color.hueRange = 0.67;
    levelVu.color.volume = 255;
}

// ---

void make_PeakGlitterVU(EC::SetupEnv &env)
{
    auto &vuSource = env.addVuBackground(EC::VuOverlayPeakGlitter::fadeRate_default);
    env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuSource));
}

// ---

void make_RainingVU(EC::SetupEnv &env)
{
    env.add(new EC::BgFadeToBlack(20, env.strip(), 20));
    env.add(new EC::BgRotate(env.strip(), true));
    auto &vuLevelSource = env.addVuSource();

    auto &levelVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuLevelSource));
    levelVu.color.hueRange = 0.67;
    levelVu.color.volume = 192;

    auto &peakGlitter = env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource));
    peakGlitter.vuPeakHandler.peakHold = 500;
    peakGlitter.vuPeakHandler.peakDecay = 500;
}

// ---

void make_RetroPartyVU(EC::SetupEnv &env)
{
    env.add(new EC::TriggerPattern());
    auto &vuLevelSource = env.addVuSource();
    env.add(new EC::RetroPartyVU(env.strip(), vuLevelSource));
}

// ---

void make_RainbowBallVU(EC::SetupEnv &env)
{
    EC::BlueprintRainbowVU bp(env);
    bp.peakSource->vuPeakHandler.presetPunchedBall();
    // bp.setVuRange(0.67);
}

void make_RainbowBalllVU_inward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_RainbowBallVU(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_RainbowBalllVU_outward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip(true);
    make_RainbowBallVU(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---

void make_RainbowBubbleVU(EC::SetupEnv &env)
{
    EC::BlueprintRainbowVU bp(env);
    bp.peakSource->vuPeakHandler.presetFloatingBubble();
    bp.setVuRange(0.67);
}

void make_RainbowBubbleVU_inward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_RainbowBubbleVU(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_RainbowBubbleVU_outward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip(true);
    make_RainbowBubbleVU(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---

void make_EjectingDotVu(EC::SetupEnv &env)
{
    EC::BlueprintEjectingDotVu bp(env);
}

void make_EjectingDotVu_inward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_EjectingDotVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_EjectingDotVu_outward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip(true);
    make_EjectingDotVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---

void make_FranticVu(EC::SetupEnv &env)
{
    EC::BlueprintFranticVu bp(env);
}

void make_FranticVu_inward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_FranticVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_FranticVu_outward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip(true);
    make_FranticVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---

void make_CrazyVu(EC::SetupEnv &env)
{
    EC::BlueprintCrazyVu bp(env);
}

void make_CrazyVu_inward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_CrazyVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_CrazyVu_outward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip(true);
    make_CrazyVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---

void make_BeyondCrazyVu(EC::SetupEnv &env)
{
    EC::BlueprintBeyondCrazyVu bp(env);
}

void make_BeyondCrazyVu_inward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip();
    make_BeyondCrazyVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_BeyondCrazyVu_outward(EC::SetupEnv &env)
{
    auto subEnv = env.clone_halfStrip(true);
    make_BeyondCrazyVu(subEnv);
    env.add(new EC::Kaleidoscope(env.strip()));
}

// ---------- VU sequence ----------

void make_VuIntro1(EC::SetupEnv &env)
{
    make_PeakGlitterVU(env);
    animationDuration = 8;
}

void make_VuIntro2(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(0);
    auto &peakSource = env.add(new EC::VuSourcePeakHold(vuLevelSource));
    env.add(new EC::VuOverlayDot(env.strip(), peakSource));
    env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource));
    animationDuration = 12;
}

void make_VuIntro3(EC::SetupEnv &env)
{
    EC::BlueprintBasicVU bp(env, 0);
    animationDuration = 16;
}

void make_VuIntro4(EC::SetupEnv &env)
{
    EC::BlueprintBasicVU bp(env);
    env.add(new EC::VuOverlayPeakGlitter(env.strip(), bp.getVuSource()));
    animationDuration = 10;
}

void make_VuIntro5(EC::SetupEnv &env)
{
    EC::BlueprintRainbowVU bp(env, 0);
    bp.setVuRange(0.15);

    env.add(new EC::VuOverlayPeakGlitter(env.strip(), bp.getVuSource()));
    animationDuration = 10;
}

void make_VuIntro6(EC::SetupEnv &env)
{
#if (1)
    auto &vuLevelSource = env.addVuBackground(50);
#else
    env.add(new EC::BgMeteorFadeToBlack(env.strip(), false));
    auto &vuLevelSource = env.addVuSource();
#endif
    auto &vuPeakSource = env.add(new EC::VuSourcePeakGravity(vuLevelSource));
    vuPeakSource.vuPeakHandler.presetPunchedBall();

    auto &vuPeak = env.add(new EC::VuOverlayRainbowStripe(env.strip(), vuPeakSource));
    vuPeak.color.hueRange = 0.5;

    auto &glitter = env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource));

    animationDuration = 10;
}

void make_VuSequence13(EC::SetupEnv &env)
{
    make_DancingDotVU(env);
    // animationDuration = 15;
}

void make_VuSequence14(EC::SetupEnv &env)
{
    make_PeakMothsVU(env);
    // animationDuration = 15;
}

void make_VuSequence15(EC::SetupEnv &env)
{
    make_DoubleDancingDotVU1(env);
    // animationDuration = 20;
}

void make_VuSequence16(EC::SetupEnv &env)
{
    make_DoubleDancingDotVU2(env);
    // animationDuration = 15;
}

void make_VuSequence16a(EC::SetupEnv &env)
{
    make_ManyDancingDotVU(env);
    // animationDuration = 15;
}

void make_VuSequence17(EC::SetupEnv &env)
{
    make_FireVU(env);
    animationDuration = 30;
}

void make_VuSequence18(EC::SetupEnv &env)
{
    make_FlareInwardVU(env);
}

void make_VuSequence19(EC::SetupEnv &env)
{
    make_FlareVU(env);
}

void make_VuSequence20(EC::SetupEnv &env)
{
    make_FlareDoubleVU(env);
}

//------------------------------------------------------------------------------

EC::AnimationSceneMakerFct allAnimations[] = {
    // &make_RawAudioVU,

    &make_VuIntro1,
    &make_VuIntro2,
    &make_VuIntro3,
    &make_VuIntro4,
    &make_VuIntro5,
    &make_VuIntro6,

    &make_RainbowBallVU,
    &make_RainbowBubbleVU,
    &make_EjectingDotVu,

    &make_RainbowBalllVU_inward,
    &make_RainbowBubbleVU_inward,
    &make_EjectingDotVu_inward,

    // &make_RainbowBalllVU_outward,
    // &make_RainbowBubbleVU_outward,
    // &make_EjectingDotVu_outward,

    &make_MeteorTrailVU,

    &make_FranticVu,
    &make_CrazyVu,
    &make_BeyondCrazyVu,

    // &make_FranticVu_inward,
    // &make_CrazyVu_inward,
    // &make_BeyondCrazyVu_inward,

    // &make_FranticVu_outward,
    // &make_CrazyVu_outward,
    &make_BeyondCrazyVu_outward,

    &make_VuSequence13,
    &make_VuSequence14,
    // &make_VuSequence15,
    &make_VuSequence16,
    // &make_VuSequence16a,

    &make_RetroPartyVU,
    &make_LightbulbVU,
    &make_RainingVU,
    // &make_BlackHoleVU,
    &make_DancingJellyfishVU,
    &make_FlowingBeatVU,
    // &make_BallLightningVU,

    &make_VuSequence17,
    // &make_VuSequence18,
    &make_VuSequence19,
    // &make_VuSequence20,
    nullptr};

//------------------------------------------------------------------------------

EC::VuSource &makeVuSource(EC::SetupEnv &env) { return env.add(new EC::VuAnalogInputPin(PIN_MIC)); }
EC::AnimationScene mainScene;
EC::SetupEnv animationSetupEnv({leds, NUM_LEDS}, mainScene, &makeVuSource);
EC::AnimationChangerSoft2 animationChanger(animationSetupEnv, allAnimations);

//------------------------------------------------------------------------------

/*

improved BlueprintBeyondCrazyVu
Sketch uses 31420 bytes (97%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31404 bytes (97%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31468 bytes (97%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31462 bytes (97%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31444 bytes (97%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31462 bytes (97%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31746 bytes (98%) of program storage space. Maximum is 32256 bytes.
Global variables use 1035 bytes (50%) of dynamic memory, leaving 1013 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31780 bytes (98%) of program storage space. Maximum is 32256 bytes.
Global variables use 1033 bytes (50%) of dynamic memory, leaving 1015 bytes for local variables. Maximum is 2048 bytes.

Re-enable autoMode:
Sketch uses 31802 bytes (98%) of program storage space. Maximum is 32256 bytes.
Global variables use 1033 bytes (50%) of dynamic memory, leaving 1015 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 31774 bytes (98%) of program storage space. Maximum is 32256 bytes.
Global variables use 1033 bytes (50%) of dynamic memory, leaving 1015 bytes for local variables. Maximum is 2048 bytes.

Baseline:
Sketch uses 32106 bytes (99%) of program storage space. Maximum is 32256 bytes.
Global variables use 1027 bytes (50%) of dynamic memory, leaving 1021 bytes for local variables. Maximum is 2048 bytes.

*/

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

#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    digitalWrite(LED_BUILTIN, autoMode);
#endif
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

    handleAnimationChange(currentMillis);

    if (animationChanger.process(currentMillis))
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
