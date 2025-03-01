/*******************************************************************************

MIT License

Copyright (c) 2024 Joachim Dick

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

/// A sketch for VU testing during EyeCandy development.

//------------------------------------------------------------------------------

#define PRINT_MEMORY_USAGE 1
#define PRINT_PATTERN_RATE 0
#define PRINT_SAMPLE_RATE 0

//------------------------------------------------------------------------------

// #define EC_DEFAULT_UPDATE_PERIOD 20
#define EC_ENABLE_VU_RANGE_EXTENDER_BYPASS 1

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
    // random16_set_seed(analogRead(A3));
    // randomSeed(random16_get_seed());

    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    analogReference(EXTERNAL);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
#if (PRINT_MEMORY_USAGE)
    printMemoryUsage();
#endif
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
uint16_t animationDuration = defaultAnimationDuration;

//------------------------------------------------------------------------------

/// Show the raw audio input as VU Animation.
void make_RawAudioVU(EC::SetupEnv &env)
{
    auto &vu = env.add(new EC::RawAudioVU(PIN_MIC, {leds, NUM_LEDS}));
    // vu.enableTeleplot = true;

    // animationDuration = 10;
}

//------------------------------------------------------------------------------

/** Example to show the usage of VU Animations building blocks.
 * - Input --> Line
 * - Input --> PeakHold --> Dot
 */
void make_VuElements1(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(0);

    auto &vuPeakSource = env.add(new EC::VuSourcePeakHold(vuLevelSource));

    auto &levelVu = env.add(new EC::VuOverlayLine(env.strip(), vuLevelSource));
    auto &peakVu = env.add(new EC::VuOverlayDot(env.strip(), vuPeakSource));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> RainbowLine
 * - Input --> PeakGravity / Ball --> RainbowDot / Input, 5%
 */
void make_VuElements2(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(50);

    auto &vuPeakSource = env.add(new EC::VuSourcePeakGravity(vuLevelSource));
    vuPeakSource.vuPeakHandler.presetPunchedBall();

    auto &levelVu = env.add(new EC::VuOverlayRainbowLine(env.strip(), vuLevelSource));
    auto &peakVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource, vuLevelSource, 0.05));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> Stripe / green
 * - Input --> PeakHold1 --> Dot / red
 * - Input --> PeakHold2 / Dip --> Dot / blue
 */
void make_VuElements3(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(50);

    auto &vuPeakSource = env.add(new EC::VuSourcePeakHold(vuLevelSource));
    vuPeakSource.vuPeakHandler.peakHold = 500;
    vuPeakSource.vuPeakHandler.peakDecay = 3000;

    auto &vuDipSource = env.add(new EC::VuSourcePeakHold(vuLevelSource));
    vuDipSource.vuPeakHandler = vuPeakSource.vuPeakHandler;
    vuDipSource.vuPeakHandler.enableDipMode = true;

    auto &levelVu = env.add(new EC::VuOverlayStripe(env.strip(), vuLevelSource, CRGB(0, 128, 0)));
    auto &peakVu = env.add(new EC::VuOverlayDot(env.strip(), vuPeakSource, CRGB(255, 0, 0)));
    auto &dipVu = env.add(new EC::VuOverlayDot(env.strip(), vuDipSource, CRGB(0, 0, 255)));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> RainbowStripe
 * - Input --> PeakGravity1 / Bubble --> RainbowDot / Input
 * - Input --> PeakGravity2 / Bubble, Dip --> RainbowDot / Input
 */
void make_VuElements4(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(20);

    auto &vuPeakSource = env.add(new EC::VuSourcePeakGravity(vuLevelSource));
    vuPeakSource.vuPeakHandler.presetFloatingBubble();

    auto &vuDipSource = env.add(new EC::VuSourcePeakGravity(vuLevelSource));
    vuDipSource.vuPeakHandler = vuPeakSource.vuPeakHandler;
    vuDipSource.vuPeakHandler.enableDipMode = true;

    auto &levelVu = env.add(new EC::VuOverlayRainbowStripe(env.strip(), vuLevelSource));
    auto &peakVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource, vuLevelSource));
    auto &dipVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuDipSource, vuLevelSource));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> RainbowLine / PeakForce
 * - Input --> PeakForce --> Dot / green, 5%
 * - Input --> Glitter
 */
void make_VuElements5(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(100);

    auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));

    auto &levelVu = env.add(new EC::VuOverlayRainbowLine(env.strip(), vuLevelSource, vuPeakSource));
    levelVu.color.volume = 64;
    levelVu.color.hueRange = 0.75;
    auto &peakVu = env.add(new EC::VuOverlayDot(env.strip(), vuPeakSource, CRGB(0, 64, 0), 0.05));

    auto &peakGlitter = env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> PeakForce --> Stripe / light green
 * - Input --> Glitter / red'ish
 * - Input --> Glitter / blue'ish, Dip
 */
void make_VuElements6(EC::SetupEnv &env)
{
    auto &vuLevelSource = env.addVuBackground(50);

    auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));

    auto &levelVu = env.add(new EC::VuOverlayStripe(env.strip(), vuPeakSource, CRGB(0, 32, 0)));

    auto &peakGlitter = env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource, CRGB(255, 64, 64)));
    auto &dipGlitter = env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource, CRGB(64, 64, 255)));
    dipGlitter.vuPeakHandler.enableDipMode = true;

    // autoMode = false;
}

//------------------------------------------------------------------------------

// still needs some tweaking
inline void make_ManyDancingDotVU(EC::SetupEnv &env)
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

void make_TestVU_1(EC::SetupEnv &env)
{
    auto drawingFct = [](EC::FastLedStrip &strip, EC::TestVU1 &vu)
    {
        strip.n_lineAbs(vu.vuLevel, vu.lastVuLevel, CRGB(0, 128, 0));

        strip.n_pixelOpt(vu.vuPeakHandler.getVU()) += CRGB(255, 0, 0);
        strip.n_pixelOpt(vu.vuDipHandler.getVU()) += CRGB(0, 0, 255);
#if (1)
        strip.n_pixelOpt(vu.vuPeakGravityHandler.getVU()) += CRGB(128, 0, 32);
        strip.n_pixelOpt(vu.vuDipGravityHandler.getVU()) += CRGB(32, 0, 128);

        strip.n_pixelOpt(vu.vuPeakForceHandler.getVU()) += CRGB(128, 64, 0);
#endif
    };

    auto &testVU = env.add(new EC::TestVU1(PIN_MIC, env.strip(), drawingFct));
    // testVU.vuRangeExtender.bypass = true;

    // autoMode = false;
}

void make_RangeExtenderComparison(EC::SetupEnv &env)
{
    auto drawingFct = [](EC::FastLedStrip &strip, EC::TestVU1 &vu)
    {
        strip.n_lineAbs(0.0, vu.vuLevelHandler.getVU(), CRGB(0, 64, 0));
        strip.n_pixelOpt(vu.vuRangeExtender.getVU()) += CRGB(255, 64, 0);
    };

    auto &testVU = env.add(new EC::TestVU1(PIN_MIC, env.strip(), drawingFct));
    // autoMode = false;
}

void make_RangeExtenderInternals(EC::SetupEnv &env)
{
    auto drawingFct = [](EC::FastLedStrip &strip, EC::TestVU1 &vu)
    {
        static float lastVuLevel = 0.0;
        const float rawVuLevel = vu.vuLevelHandler.getVU();
        // the dancing thing shows the current VU level
        strip.n_lineAbs(lastVuLevel, rawVuLevel, CRGB(16, 32, 8));
        lastVuLevel = rawVuLevel;

        // the green dot shows the average VU level
        const float vuLevelAvg = vu.vuRangeExtender.vuLevelAvg;
        strip.n_pixel(vuLevelAvg) = CRGB(0, 128, 0);

        const float posDeltaAvg = vu.vuRangeExtender.posDeltaAvg;
        strip.n_pixel(vuLevelAvg + posDeltaAvg) = CRGB(64, 8, 0);

        const float negDeltaAvg = vu.vuRangeExtender.negDeltaAvg;
        strip.n_pixel(vuLevelAvg + negDeltaAvg) = CRGB(0, 8, 64);

        // the VU level is scaled so that the red dot will be at the end of the strip
        strip.n_pixel(vu.vuRangeExtender.rangeMax) = CRGB(255, 0, 0);
        // the VU level is scaled so that the blue dot will be at the start of the strip
        strip.n_pixel(vu.vuRangeExtender.rangeMin) = CRGB(0, 0, 255);
    };

    auto &testVU = env.add(new EC::TestVU1(PIN_MIC, env.strip(), drawingFct));
    autoMode = false;
}

void make_DraftVU(EC::SetupEnv &env)
{
    env.add(new EC::TriggerPattern(EC::FlowingBeatVU::patternUpdatePeriod));
    auto &vuLevelSource = env.addVuSource();

#if (0)
    auto &levelVu = env.add(new EC::FlowingBeatVU(env.strip(), vuLevelSource));
#else
    auto &levelVu = env.add(new EC::FlowingBeatVU(env.strip().getHalfStrip(), vuLevelSource));
    env.add(new EC::Kaleidoscope(env.strip()));
#endif
    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneMakerFct allAnimations[] = {
    // &make_RawAudioVU,
    // &make_DraftVU,
    // &make_RangeExtenderInternals,
    // &make_RangeExtenderComparison,

    &make_VuElements1,
    &make_VuElements2,
    &make_VuElements3,
    &make_VuElements4,
    &make_VuElements5,
    &make_VuElements6,

    // &make_DoubleDancingDotVU1,
    // &make_DoubleDancingDotVU2,
    // &make_ManyDancingDotVU,

    // &make_EjectingDotVU,
    // &make_CrazyVU,
    // &make_BeyondCrazyVU,
    // &make_TestVU_1,

    // &make_RetroPartyVU,
    // &make_LightbulbVU,
    &make_RainingVU,
    &make_BlackHoleVU,
    &make_DancingJellyfishVU,
    &make_FlowingBeatVU,
    &make_BallLightningVU,

    nullptr};

//------------------------------------------------------------------------------

#if (0)
EC::VuAnalogInputPin globalVuSource(PIN_MIC);
EC::VuSource &make_VuSource(EC::SetupEnv &env) { return env.add(globalVuSource); }
#else
EC::VuSource &make_VuSource(EC::SetupEnv &env) { return env.add(new EC::VuAnalogInputPin(PIN_MIC)); }
#endif

EC::AnimationScene mainScene;
EC::SetupEnv animationSetupEnv({leds, NUM_LEDS}, mainScene, &make_VuSource);

#if (0)
EC::AnimationChanger animationChanger(animationSetupEnv, allAnimations);
#else
EC::AnimationChangerSoft animationChanger(animationSetupEnv, allAnimations);
#endif

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

    if (animationChanger.process(currentMillis))
    {
        FastLED.show();
#if (PRINT_PATTERN_RATE)
        printPatternRate(currentMillis);
#endif
        updateColor();
        updateSpeed();
        updateFlip();
    }

#if (PRINT_SAMPLE_RATE)
    printSampleRate(currentMillis);
#endif

    // // this avoids nasty flickering with ESP8266 - don't know why...?!?
    // #ifdef ARDUINO_ARCH_ESP8266
    //     delay(2);
    // #endif
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
            // Serial.print("hue: ");
            // Serial.println(hue);
            lastHue = analogValue;
        }

        // animationChanger.maxBrightness = analogValue;
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
            // Serial.print("speed: ");
            // Serial.print(animationSpeed);
            // Serial.print(" delay: ");
            // Serial.println(modelUpdatePeriod);
            lastSpeed = analogValue;
        }
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

    if (flipped)
    {
    }
    else
    {
    }
}

//------------------------------------------------------------------------------

#if (PRINT_SAMPLE_RATE)
void printSampleRate(uint32_t currentMillis)
{
    static uint32_t nextPrint = 1000;
    static uint32_t counter = 0;

    ++counter;
    if (currentMillis >= nextPrint)
    {
        const uint32_t sampleRate = counter;
        counter = 0;
        nextPrint = currentMillis + 1000;

        Serial.print(sampleRate);
        Serial.println(F(" Hz sample rate"));
    }
}
#endif

#if (PRINT_PATTERN_RATE)
void printPatternRate(uint32_t currentMillis)
{
    static uint32_t nextPrint = 1000;
    static uint32_t counter = 0;

    ++counter;
    if (currentMillis >= nextPrint)
    {
        const uint32_t patternRate = counter;
        counter = 0;
        nextPrint = currentMillis + 1000;

        Serial.print(patternRate);
        Serial.println(F(" Hz pattern rate"));
    }
}
#endif

//------------------------------------------------------------------------------

#if (PRINT_MEMORY_USAGE)
void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

    Serial.print(F("VuAnalogInputPin = "));
    Serial.println((int)sizeof(EC::VuAnalogInputPin));

    Serial.print(F("VuSourcePeakHold = "));
    Serial.println((int)sizeof(EC::VuSourcePeakHold));

    Serial.print(F("VuSourcePeakGravity = "));
    Serial.println((int)sizeof(EC::VuSourcePeakGravity));

    Serial.print(F("VuSourcePeakForce = "));
    Serial.println((int)sizeof(EC::VuSourcePeakForce));

    Serial.print(F("VuOverlayLine = "));
    Serial.println((int)sizeof(EC::VuOverlayLine));

    Serial.print(F("VuOverlayStripe = "));
    Serial.println((int)sizeof(EC::VuOverlayStripe));

    Serial.print(F("VuOverlayDot = "));
    Serial.println((int)sizeof(EC::VuOverlayDot));

    Serial.print(F("VuOverlayRainbowLine = "));
    Serial.println((int)sizeof(EC::VuOverlayRainbowLine));

    Serial.print(F("VuOverlayRainbowStripe = "));
    Serial.println((int)sizeof(EC::VuOverlayRainbowStripe));

    Serial.print(F("VuOverlayRainbowDot = "));
    Serial.println((int)sizeof(EC::VuOverlayRainbowDot));

    Serial.print(F("VuOverlayPeakGlitter = "));
    Serial.println((int)sizeof(EC::VuOverlayPeakGlitter));

    Serial.print(F("BallLightningVU = "));
    Serial.println((int)sizeof(EC::BallLightningVU));

    Serial.print(F("DancingJellyfishVU = "));
    Serial.println((int)sizeof(EC::DancingJellyfishVU));

    Serial.print(F("Fire2012VU = "));
    Serial.println((int)sizeof(EC::Fire2012VU<NUM_LEDS>));

    Serial.print(F("FlowingBeatVU = "));
    Serial.println((int)sizeof(EC::FlowingBeatVU));

    Serial.print(F("RawAudioVU = "));
    Serial.println((int)sizeof(EC::RawAudioVU));

    Serial.print(F("TestVU1 = "));
    Serial.println((int)sizeof(EC::TestVU1));
}
#endif

//------------------------------------------------------------------------------
