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

// #define EC_ENABLE_VU_RANGE_EXTENDER_BYPASS 1

#include <EyeCandy.h>
// #define LED_COLOR_ORDER RGB
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

const uint16_t defaultAnimationDuration = 10;
uint16_t animationDuration = defaultAnimationDuration;

//------------------------------------------------------------------------------

void makeRawAudioVU(EC::AnimationScene &scene)
{
    auto vu = scene.append(new EC::RawAudioVU(audioSample, {leds, NUM_LEDS}));
    // vu->enableTeleplot = true;

    // animationDuration = 10;
}

/// An outlook what is possible.
void makeEssentialVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
    auto vu = scene.append(new EC::EssentialVU(strip, *vuSource));
    vu->vuLevelBar.color = CRGB(64, 0, 32);
    vu->vuPeakDot.color = CRGB(64, 32, 0);
    vu->vuPeakSource.vuPeakHandler.peakHold = 500;

    // animationDuration = 10;
    // autoMode = false;
}

void makeTestVU1(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

    auto vu = scene.append(new EC::TestVU1(strip, *vuLevelSource));
    vu->enableVuBar = false;
    vu->enableVuStripe = true;
    vu->enableVuDot = false;
    vu->enablePeakDot = true;
    vu->enablePeakDotMin = true;
    // vu->vuPeakHandler.peakHold = 1000;

    // animationDuration = 10;
    // autoMode = false;
}

void makeCompoundVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));
#if (0)
    auto vuPeakSource = scene.append(new EC::VuSourcePeakHold(*vuLevelSource));
    // vuPeakSource->vuPeakHandler.peakHold = 1000;
#endif
#if (1)
    auto vuPeakSource = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    // vuPeakSource->vuPeakHandler.presetPunchedBall();
    // vuPeakSource->vuPeakHandler.presetFloatingBubble();
#endif

#if (1)
    auto vuLevelBar = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource /*, *vuPeakSource*/));
    auto vuPeakDot = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource, *vuLevelSource));
#endif
#if (0)
    scene.append(new EC::VuOverlayLine(strip, *vuLevelSource));
    scene.append(new EC::VuOverlayDot(strip, *vuPeakSource));
#endif

    // autoMode = false;
}

void makeEjectingDotVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 25));
    // vuLevelSource->vuRangeExtender.bypass = true;

    auto vuPeakSource1 = scene.append(new EC::VuSourcePeakGravity(*vuLevelSource));
    vuPeakSource1->vuPeakHandler.a0 = -0.6;
    vuPeakSource1->vuPeakHandler.v0 = 0.3;

    auto vuPeakSource2 = scene.append(new EC::VuSourcePeakGravity(vuPeakSource1->asVuSource()));
    vuPeakSource2->vuPeakHandler.a0 = 0.2;
    vuPeakSource2->vuPeakHandler.v0 = 0.0;

    auto vuLevelBar = scene.append(new EC::VuOverlayRainbowLine(strip, *vuLevelSource));
    auto vuPeakDot1 = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource1, *vuLevelSource));
    auto vuPeakDot2 = scene.append(new EC::VuOverlayRainbowDot(strip, *vuPeakSource2, *vuLevelSource));

    vuPeakDot1->vuHueRange = 1.0;
    vuPeakDot2->vuHueRange = vuPeakDot1->vuHueRange;

    autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct allAnimations[] = {
    // &makeEjectingDotVu,
    &makeEssentialVU,
    &makeTestVU1,
    &makeCompoundVu,

    &makeRawAudioVU,
    nullptr};

EC::AnimationChangerSoft animationChanger(allAnimations);

//------------------------------------------------------------------------------

/*

240130 Bugfix VuPeakGravityHandler

Sketch uses 16722 bytes (51%) of program storage space. Maximum is 32256 bytes.
Global variables use 789 bytes (38%) of dynamic memory, leaving 1259 bytes for local variables. Maximum is 2048 bytes.


240128 New Dip mode for Peak handlers

Sketch uses 16576 bytes (51%) of program storage space. Maximum is 32256 bytes.
Global variables use 789 bytes (38%) of dynamic memory, leaving 1259 bytes for local variables. Maximum is 2048 bytes.


240128 Eliminated VuBaseFL

Sketch uses 16640 bytes (51%) of program storage space. Maximum is 32256 bytes.
Global variables use 795 bytes (38%) of dynamic memory, leaving 1253 bytes for local variables. Maximum is 2048 bytes.

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
            // Serial.print("hue: ");
            // Serial.println(hue);
            lastHue = analogValue;
        }

        animationChanger.maxBrightness = analogValue;
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

    Serial.print(F("EssentialVU = "));
    Serial.println(sizeof(EC::EssentialVU));

    Serial.print(F("RainbowLevelVU = "));
    Serial.println(sizeof(EC::RainbowLevelVU));

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

    Serial.print(F("VuSourcePeakForce = "));
    Serial.println(sizeof(EC::VuSourcePeakForce));

    Serial.print(F("VuSourcePeakGravity = "));
    Serial.println(sizeof(EC::VuSourcePeakGravity));

    Serial.print(F("VuSourcePeakHold = "));
    Serial.println(sizeof(EC::VuSourcePeakHold));

    Serial.print(F("RawAudioVU = "));
    Serial.println(sizeof(EC::RawAudioVU));

    Serial.print(F("TestVU1 = "));
    Serial.println(sizeof(EC::TestVU1));
}
#endif

//------------------------------------------------------------------------------
