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
    // random16_set_seed(analogRead(A3));
    // randomSeed(random16_get_seed());

    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);
    pinMode(PIN_MIC, INPUT);
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

/// Show the raw audio input as VU Anumation.
void makeRawAudioVU(EC::AnimationScene &scene)
{
    auto vu = scene.append(new EC::RawAudioVU(audioSample, {leds, NUM_LEDS}));
    // vu->enableTeleplot = true;

    // animationDuration = 10;
}

//------------------------------------------------------------------------------

#define USE_GLOBAL_VU_LEVEL_SOURCE 0

#if (USE_GLOBAL_VU_LEVEL_SOURCE)
EC::VuSourceAnalogPin globalVuLevelSource(audioSample, {leds, NUM_LEDS}, 0);
#endif

inline EC::VuSourceAnalogPin *appendVuSourceAnalogPin(EC::AnimationScene &scene,
                                                      uint8_t fadeRate)
{
#if (USE_GLOBAL_VU_LEVEL_SOURCE)
    auto vuLevelSource = scene.append(globalVuLevelSource);
    vuLevelSource->fadeRate = fadeRate;
    return vuLevelSource;
#else
    return scene.append(new EC::VuSourceAnalogPin(audioSample, {leds, NUM_LEDS}, fadeRate));
#endif
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> Line
 * - Input --> PeakHold --> Dot
 */
void makeVuElements1(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, 0);

    auto vuPeakSource = scene.append(new EC::VuSourcePeakHold(*vuLevelSource));

    auto levelVu = scene.append(new EC::VuOverlayLine(strip, *vuLevelSource));
    auto peakVu = scene.append(new EC::VuOverlayDot(strip, *vuPeakSource));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> RainbowLine
 * - Input --> PeakGravity / Ball --> RainbowDot / Input, 5%
 */
void makeVuElements2(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, 50);

    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuPeakSource->vuPeakHandler.presetPunchedBall();

    auto levelVu = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource));
    auto peakVu = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource, *vuLevelSource, 0.05));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> Stripe / green
 * - Input --> PeakHold1 --> Dot / red
 * - Input --> PeakHold2 / Dip --> Dot / blue
 */
void makeVuElements3(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, 50);

    auto vuPeakSource = scene.append(new EC::VuSourcePeakHold(*vuLevelSource));
    vuPeakSource->vuPeakHandler.peakHold = 500;
    vuPeakSource->vuPeakHandler.peakDecay = 3000;

    auto vuDipSource = scene.append(new EC::VuSourcePeakHold(*vuLevelSource));
    vuDipSource->vuPeakHandler = vuPeakSource->vuPeakHandler;
    vuDipSource->vuPeakHandler.enableDipMode = true;

    auto levelVu = scene.append(new EC::VuOverlayStripe(strip, *vuLevelSource, CRGB(0, 128, 0)));
    auto peakVu = scene.append(new EC::VuOverlayDot(strip, *vuPeakSource, CRGB(255, 0, 0)));
    auto dipVu = scene.append(new EC::VuOverlayDot(strip, *vuDipSource, CRGB(0, 0, 255)));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> RainbowStripe
 * - Input --> PeakGravity1 / Bubble --> RainbowDot / Input
 * - Input --> PeakGravity2 / Bubble, Dip --> RainbowDot / Input
 */
void makeVuElements4(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, 20);

    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuPeakSource->vuPeakHandler.presetFloatingBubble();

    auto vuDipSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuDipSource->vuPeakHandler = vuPeakSource->vuPeakHandler;
    vuDipSource->vuPeakHandler.enableDipMode = true;

    auto levelVu = scene.append(new EC::VuOverlayRainbowStripe(strip, *vuLevelSource));
    auto peakVu = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource, *vuLevelSource));
    auto dipVu = scene.append(new EC::VuOverlayRainbowDot(strip, *vuDipSource, *vuLevelSource));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> RainbowLine / PeakForce
 * - Input --> PeakForce --> Dot / green, 5%
 * - Input --> Glitter
 */
void makeVuElements5(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, 100);

    auto vuPeakSource = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));

    auto levelVu = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource, *vuPeakSource));
    levelVu->color.volume = 64;
    levelVu->color.hueRange = 0.75;
    auto peakVu = scene.append(new EC::VuOverlayDot(strip, *vuPeakSource, CRGB(0, 64, 0), 0.05));

    auto peakGlitter = scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));

    // autoMode = false;
}

/** Example to show the usage of VU Animations building blocks.
 * - Input --> PeakForce --> Stripe / light green
 * - Input --> Glitter / red'ish
 * - Input --> Glitter / blue'ish, Dip
 */
void makeVuElements6(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, 50);

    auto vuPeakSource = scene.append(new EC::VuSourcePeakForce(*vuLevelSource));

    auto levelVu = scene.append(new EC::VuOverlayStripe(strip, *vuPeakSource, CRGB(0, 32, 0)));

    auto peakGlitter = scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource, CRGB(255, 64, 64)));
    auto dipGlitter = scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource, CRGB(64, 64, 255)));
    dipGlitter->vuPeakHandler.enableDipMode = true;

    // autoMode = false;
}

//------------------------------------------------------------------------------

void makeEjectingDotVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, EC::BlueprintEjectingDotVu::fadeRate);

    EC::BlueprintEjectingDotVu bp(strip, scene, *vuLevelSource);
    // autoMode = false;
}

void makeCrazyVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, EC::BlueprintCrazyVu::fadeRate);

    EC::BlueprintCrazyVu bp(strip, scene, *vuLevelSource);
    // autoMode = false;
}

void makeBeyondCrazyVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, EC::BlueprintBeyondCrazyVu::fadeRate);

    EC::BlueprintBeyondCrazyVu bp(strip, scene, *vuLevelSource);
    // autoMode = false;
}

void makeBallLightningVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::TriggerPattern());
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto levelVu = scene.append(new EC::BallLightningVU(strip, *vuLevelSource));
    // autoMode = false;
}

void makeBlackHoleVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);
    auto workStrip = strip.getHalfStrip(true);

    scene.append(new EC::BgFadeToBlack(20, workStrip, 40));
    scene.append(new EC::BgRotate(workStrip, true));
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));

    auto levelVu = scene.append(new EC::VuOverlayRainbowStripe(workStrip, *vuLevelSource));
    levelVu->color.hueRange = 0.5;
    levelVu->color.volume = 255;

    scene.append(new EC::Kaleidoscope(strip));
    // autoMode = false;
}

void makeDancingJellyfishVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, EC::DancingJellyfishVU::fadeRate);
    auto levelVu = scene.append(new EC::DancingJellyfishVU(strip, *vuLevelSource));
    // autoMode = false;
}

void makeFlowingBeatVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::TriggerPattern(EC::FlowingBeatVU::patternUpdatePeriod));
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto levelVu = scene.append(new EC::FlowingBeatVU(strip, *vuLevelSource));
    // autoMode = false;
}

void makeLightbulbVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::TriggerPattern());
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto levelVu = scene.append(new EC::LightbulbVU(strip, *vuLevelSource));
    // autoMode = false;
}

void makeRainingVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::BgFadeToBlack(20, strip, 20));
    scene.append(new EC::BgRotate(strip, true));
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));

    auto levelVu = scene.append(new EC::VuOverlayRainbowDot(strip, *vuLevelSource));
    levelVu->color.hueRange = 0.67;
    levelVu->color.volume = 192;

    auto peakGlitter = scene.append(new EC::VuOverlayPeakGlitter(strip, *vuLevelSource));
    peakGlitter->vuPeakHandler.peakHold = 500;
    peakGlitter->vuPeakHandler.peakDecay = 500;
    // autoMode = false;
}

void makeRetroPartyVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::TriggerPattern());
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));
    auto levelVu = scene.append(new EC::RetroPartyVU(strip, *vuLevelSource));
    // autoMode = false;
}

//------------------------------------------------------------------------------

#define USE_GLOBAL_TEST_VU 0

#if (USE_GLOBAL_TEST_VU)
EC::TestVU1 globalTestVU(audioSample, {leds, NUM_LEDS}, nullptr);
#endif

EC::TestVU1 *appendTestVU1(EC::AnimationScene &scene,
                           EC::TestVU1::DrawingFct drawingFct,
                           uint8_t fadeRate = 0)
{
#if (USE_GLOBAL_TEST_VU)
    auto testVU = scene.append(globalTestVU);
    testVU->drawingFct = drawingFct;
    testVU->fadeRate = fadeRate;
    return testVU;
#else
    return scene.append(new EC::TestVU1(audioSample, {leds, NUM_LEDS}, drawingFct, fadeRate));
#endif
}

void makeTestVU_1(EC::AnimationScene &scene)
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

#if (1)
    auto testVU = appendTestVU1(scene, drawingFct, 0);
#else
    auto testVU = scene.append(new EC::TestVU1(audioSample, {leds, NUM_LEDS}, drawingFct, 0));
    testVU->vuRangeExtender.bypass = true;
#endif

    // autoMode = false;
}

void makeRangeExtenderComparison(EC::AnimationScene &scene)
{
    auto drawingFct = [](EC::FastLedStrip &strip, EC::TestVU1 &vu)
    {
        strip.n_lineAbs(0.0, vu.vuLevelHandler.getVU(), CRGB(0, 64, 0));
        strip.n_pixelOpt(vu.vuRangeExtender.getVU()) += CRGB(255, 64, 0);
    };

    auto testVU = appendTestVU1(scene, drawingFct);
    // autoMode = false;
}

void makeRangeExtenderInternals(EC::AnimationScene &scene)
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

    auto testVU = appendTestVU1(scene, drawingFct, 0);
    autoMode = false;
}

void makeDraftVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::TriggerPattern(EC::FlowingBeatVU::patternUpdatePeriod));
    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample));

#if (0)
    auto levelVu = scene.append(new EC::FlowingBeatVU(strip, *vuLevelSource));
#else
    auto levelVu = scene.append(new EC::FlowingBeatVU(strip.getHalfStrip(), *vuLevelSource));
    scene.append(new EC::Kaleidoscope(strip));
#endif
    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct allAnimations[] = {
    // &makeRawAudioVU,
    &makeDraftVU,
    // &makeRangeExtenderInternals,
    // &makeRangeExtenderComparison,

    &makeVuElements1,
    &makeVuElements2,
    &makeVuElements3,
    &makeVuElements4,
    &makeVuElements5,
    &makeVuElements6,

    // &makeEjectingDotVu,
    // &makeCrazyVu,
    // &makeBeyondCrazyVu,
    // &makeTestVU_1,

    // &makeRetroPartyVU,
    // &makeLightbulbVU,
    &makeRainingVU,
    &makeBlackHoleVU,
    &makeDancingJellyfishVU,
    &makeFlowingBeatVU,
    &makeBallLightningVU,

    nullptr};

#if (0)
EC::AnimationChanger animationChanger(allAnimations);
#else
EC::AnimationChangerSoft animationChanger(allAnimations);
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
    audioSample = normalizer.analogRead(PIN_MIC);

    // EC::logAudioSample(audioSample);

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

    Serial.print(F("VuSourceAnalogPin = "));
    Serial.println((int)sizeof(EC::VuSourceAnalogPin));

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
