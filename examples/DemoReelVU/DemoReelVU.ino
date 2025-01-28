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
    env.add(new EC::BgMeteorFadeToBlack(env.strip()));
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

void make_VuSequence17(EC::SetupEnv &env)
{
    make_FireVU<NUM_LEDS>(env);
    animationDuration = 30;
}

void make_VuSequence18(EC::SetupEnv &env)
{
    make_FlareInwardVU<NUM_LEDS>(env);
}

void make_VuSequence19(EC::SetupEnv &env)
{
    make_FlareVU<NUM_LEDS>(env);
}

void make_VuSequence20(EC::SetupEnv &env)
{
    make_FlareDoubleVU<NUM_LEDS>(env);
}

//------------------------------------------------------------------------------

/// Show the raw audio input as VU Animation.
void make_RawAudioVU(EC::SetupEnv &env)
{
    auto &vu = env.add(new EC::RawAudioVU(PIN_MIC, {leds, NUM_LEDS}));
    // animationDuration = 10;
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
    &make_EjectingDotVU,

    &make_RainbowBalllVU_inward,
    &make_RainbowBubbleVU_inward,
    &make_EjectingDotVU_inward,

    // &make_RainbowBalllVU_outward,
    // &make_RainbowBubbleVU_outward,
    // &make_EjectingDotVU_outward,

    &make_MeteorTrailVU,

    &make_FranticVU,
    &make_CrazyVU,
    &make_BeyondCrazyVU,

    // &make_FranticVU_inward,
    // &make_CrazyVU_inward,
    // &make_BeyondCrazyVU_inward,

    // &make_FranticVU_outward,
    // &make_CrazyVU_outward,
    &make_BeyondCrazyVU_outward,

    &make_VuSequence13,
    &make_VuSequence14,
    // &make_VuSequence15,
    &make_VuSequence16, // ?

    &make_RetroPartyVU,
    &make_LightbulbVU,
    &make_RainingVU,
    // &make_RainingVU_inward,
    &make_RainingVU_outward,
    &make_BlackHoleVU,
    // &make_BlackAntiHoleVU,
    &make_DancingJellyfishVU,
    // &make_FlowingBeatVU,
    &make_FlowingBeatVU_outward,
    // &make_FlowingBeatVU_inward,
    // &make_BallLightningVU,

    &make_VuSequence17,
    // &make_VuSequence18,
    &make_VuSequence19,
    // &make_VuSequence20,
    nullptr};

//------------------------------------------------------------------------------

EC::AnalogPinVuSource vuSource(PIN_MIC);
EC::VuSource &make_VuSource(EC::SetupEnv &env) { return env.add(vuSource); }
EC::AnimationScene mainScene;
EC::SetupEnv animationSetupEnv({leds, NUM_LEDS}, mainScene, &make_VuSource);
EC::AnimationChangerSoft animationChanger(animationSetupEnv, allAnimations);
EC::AnimationUpdateHandler animationHandler(animationChanger);

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

    vuSource.readAudioSample();

    if (animationHandler.process(currentMillis))
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
