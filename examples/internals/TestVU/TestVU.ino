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

// #define LED_COLOR_ORDER RGB
// #define NUM_LEDS 50

// #define EC_DEFAULT_UPDATE_PERIOD 20
#define EC_ENABLE_VU_RANGE_EXTENDER_BYPASS 1

//------------------------------------------------------------------------------

#include <EyeCandy.h>
#include <Animation_IO_config.h>
#include <AudioNormalizer.h>
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

ButtonHandler selectButton;

bool autoMode = true;

float audioSample = 0.0;
EC::AudioNormalizer normalizer;

#define PRINT_MEMORY_USAGE 1
#define PRINT_SAMPLE_RATE 0
#define PRINT_PATTERN_RATE 0

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
#if (PRINT_MEMORY_USAGE)
    printMemoryUsage();
#endif
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 15;
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
    levelVu->volume = 64;
    levelVu->vuHueRange = 0.75;
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

void makeCrazierVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = appendVuSourceAnalogPin(scene, EC::BlueprintCrazierVu::fadeRate);

    EC::BlueprintCrazierVu bp(strip, scene, *vuLevelSource);

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

void makeTestVU1(EC::AnimationScene &scene)
{
    auto drawingFct = [](EC::FastLedStrip &strip, EC::TestVU1 &vu)
    {
        strip.normLineAbs(vu.vuLevel, vu.lastVuLevel, CRGB(0, 128, 0));

        strip.optPixel(vu.vuPeakHandler.getVU()) += CRGB(255, 0, 0);
        strip.optPixel(vu.vuDipHandler.getVU()) += CRGB(0, 0, 255);
#if (1)
        strip.optPixel(vu.vuPeakGravityHandler.getVU()) += CRGB(128, 0, 32);
        strip.optPixel(vu.vuDipGravityHandler.getVU()) += CRGB(32, 0, 128);

        strip.optPixel(vu.vuPeakForceHandler.getVU()) += CRGB(128, 64, 0);
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
        strip.normLineAbs(0.0, vu.vuLevelHandler.getVU(), CRGB(0, 64, 0));
        strip.optPixel(vu.vuRangeExtender.getVU()) += CRGB(255, 64, 0);
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
        strip.normLineAbs(lastVuLevel, rawVuLevel, CRGB(16, 32, 8));
        lastVuLevel = rawVuLevel;

        // the green dot shows the average VU level
        const float vuLevelAvg = vu.vuRangeExtender.vuLevelAvg.get();
        strip.normPixel(vuLevelAvg) = CRGB(0, 128, 0);

        const float posDeltaAvg = vu.vuRangeExtender.posDeltaAvg.get();
        strip.normPixel(vuLevelAvg + posDeltaAvg) = CRGB(64, 8, 0);

        const float negDeltaAvg = vu.vuRangeExtender.negDeltaAvg.get();
        strip.normPixel(vuLevelAvg + negDeltaAvg) = CRGB(0, 8, 64);

        // the VU level is scaled so that the red dot will be at the end of the strip
        strip.normPixel(vu.vuRangeExtender.rangeMax) = CRGB(255, 0, 0);
        // the VU level is scaled so that the blue dot will be at the start of the strip
        strip.normPixel(vu.vuRangeExtender.rangeMin) = CRGB(0, 0, 255);
    };

    auto testVU = appendTestVU1(scene, drawingFct, 0);

    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct allAnimations[] = {
    &makeTestVU1,
    // &makeRangeExtenderInternals,
    // &makeRangeExtenderComparison,

    // &makeRawAudioVU,
    &makeVuElements1,
    &makeVuElements2,
    &makeVuElements3,
    &makeVuElements4,
    &makeVuElements5,
    &makeVuElements6,

    // &makeEjectingDotVu,
    // &makeCrazyVu,
    // &makeCrazierVu,
    // &makeTestVU1,

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

    // digitalWrite(LED_BUILTIN, autoMode);
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
    Serial.println(sizeof(EC::VuSourceAnalogPin));

    Serial.print(F("VuSourcePeakHold = "));
    Serial.println(sizeof(EC::VuSourcePeakHold));

    Serial.print(F("VuSourcePeakGravity = "));
    Serial.println(sizeof(EC::VuSourcePeakGravity));

    Serial.print(F("VuSourcePeakForce = "));
    Serial.println(sizeof(EC::VuSourcePeakForce));

    Serial.print(F("VuOverlayLine = "));
    Serial.println(sizeof(EC::VuOverlayLine));

    Serial.print(F("VuOverlayStripe = "));
    Serial.println(sizeof(EC::VuOverlayStripe));

    Serial.print(F("VuOverlayDot = "));
    Serial.println(sizeof(EC::VuOverlayDot));

    Serial.print(F("VuOverlayRainbowLine = "));
    Serial.println(sizeof(EC::VuOverlayRainbowLine));

    Serial.print(F("VuOverlayRainbowStripe = "));
    Serial.println(sizeof(EC::VuOverlayRainbowStripe));

    Serial.print(F("VuOverlayRainbowDot = "));
    Serial.println(sizeof(EC::VuOverlayRainbowDot));

    Serial.print(F("VuOverlayPeakGlitter = "));
    Serial.println(sizeof(EC::VuOverlayPeakGlitter));

    Serial.print(F("Fire2012VU = "));
    Serial.println(sizeof(EC::Fire2012VU<NUM_LEDS>));

    Serial.print(F("RawAudioVU = "));
    Serial.println(sizeof(EC::RawAudioVU));

    Serial.print(F("TestVU1 = "));
    Serial.println(sizeof(EC::TestVU1));
}
#endif

//------------------------------------------------------------------------------
