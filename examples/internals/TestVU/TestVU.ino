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

#define EC_ENABLE_VU_RANGE_EXTENDER_BYPASS 1
// #define EC_TESTVU1_DEBUG 1

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

/** Example to show the usage of VU Animations building blocks.
 * - Input --> Line
 * - Input --> PeakHold --> Dot
 */
void makeVuElements1(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 0));
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

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

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

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

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

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 20));

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

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 100));

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

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, 50));

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

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintEjectingDotVu::fadeRate));
    EC::BlueprintEjectingDotVu bp(strip, scene, *vuLevelSource);

    // autoMode = false;
}

void makeCrazyVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintCrazyVu::fadeRate));
    EC::BlueprintCrazyVu bp(strip, scene, *vuLevelSource);

    // autoMode = false;
}

void makeCrazierVu(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vuLevelSource = scene.append(new EC::VuSourceAnalogPin(audioSample, strip, EC::BlueprintCrazierVu::fadeRate));
    EC::BlueprintCrazierVu bp(strip, scene, *vuLevelSource);

    // autoMode = false;
}

//------------------------------------------------------------------------------

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

    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct allAnimations[] = {
    // &makeTestVU1,

    &makeRawAudioVU,
    &makeVuElements1,
    &makeVuElements2,
    &makeVuElements3,
    &makeVuElements4,
    &makeVuElements5,
    &makeVuElements6,

    // &makeEjectingDotVu,
    // &makeCrazyVu,
    // &makeCrazierVu,
    &makeTestVU1,

    nullptr};

EC::AnimationChanger animationChanger(allAnimations);
// EC::AnimationChangerSoft animationChanger(allAnimations);

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
}
#endif

//------------------------------------------------------------------------------
