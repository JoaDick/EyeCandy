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

#define PRINT_MEMORY_USAGE 0

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

/** Make a VU that takes the average of some (absolute) audio samples as VU value.
 * Not really looking very appealing, and the fundamentally bad part is that the
 * VU is always higly dependant on the audio level at the analog input.
 * --> Fail; not useful :-(
 */
void makeSampleAvgVU(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normLineRel(0.0, vu.vuLevelAvg, CRGB(0, 128, 0));
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->smoothingFactor = 0;

    // animationDuration = 10;
}

/** Same as makeSampleAvgVU(), but with a floating average over the last few VU values.
 * That's an attempt to make the VU appear smoother and less twitchy.
 * But no matter how - it's still not looking very appealing.
 * --> Next fail!
 */
void makeSampleAvgVU_smoothed(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normLineRel(0.0, vu.vuLevelAvg, CRGB(0, 128, 0));
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->smoothingFactor = 3;

    // animationDuration = 10;
}

/** Make a VU that takes the RMS (Root Mean Square) of audio samples as VU value.
 * Interestingly, this is looking very similar to makeSampleAvgVU(), so smoothing this
 * VU can probably be skipped... However, let's try to compare Average and RMS directly.
 * @see https://audiosorcerer.com/post/rms-in-audio/#4-what-is-rms-in-audio
 */
void makeSampleRmsVU(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normLineRel(0.0, vu.vuLevelRms, CRGB(0, 0, 128));
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotRms = true;
    vu->smoothingFactor = 0;

    // animationDuration = 10;
}

/** Same as makeSampleRmsVU(), but with a floating average over the last few VU values (smoothing).
 * Not really much helpful, but mainly for completeness of the comparisons.
 */
void makeSampleRmsVU_smoothed(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normLineRel(0.0, vu.vuLevelRms, CRGB(0, 0, 128));
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotRms = true;
    vu->smoothingFactor = 3;

    // animationDuration = 10;
}

/** Make a VU that displays the averaged VU values and RMS VU values as dots.
 * Just to compare them. It seems like RMS level is only slightly higher than avg level.
 */
void makeAvgVsRmsVU(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.normPixel(vu.vuLevelRms) += CRGB(0, 0, 128);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplotRms = true;
    vu->smoothingFactor = 0;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Same as makeAvgVsRmsVU(), but with smoothing.
 * This also shows clearly that RMS level is only slightly higher than avg level.
 * In Conclusion, the additional math for RMS over averaging might not be worth the
 * effort (at least not from just a visual standpoint).
 */
void makeAvgVsRmsVU_smoothed(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.normPixel(vu.vuLevelRms) += CRGB(0, 0, 128);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplotRms = true;
    vu->smoothingFactor = 3;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Same as makeAvgVsRmsVU(), but with a peak detection.
 * So we can better compare them and see the difference.
 */
void makeAvgVsRmsVU_peak(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.normPixel(vu.vuLevelRms) += CRGB(0, 0, 128);
        strip.optPixel(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 255, 0);
        strip.optPixel(vu.vuPeakHandlerRms.getVU()) += CRGB(0, 0, 255);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplotRms = true;
    vu->smoothingFactor = 0;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Same as makeAvgVsRmsVU_peak(), but with smoothing.
 * Just for completeness. Nevertheless, the main problem still exists:
 * The VU is still higly dependant on the audio level at the analog input.
 */
void makeAvgVsRmsVU_peak_smoothed(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.normPixel(vu.vuLevelRms) += CRGB(0, 0, 128);
        strip.optPixel(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 255, 0);
        strip.optPixel(vu.vuPeakHandlerRms.getVU()) += CRGB(0, 0, 255);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplotRms = true;
    vu->smoothingFactor = 3;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Make a VU that displays the averaged VU values as dots on a linear and logarithmic scale.
 * Let's try a different approach, since the difference between averaged
 * and RMS values isn't really big. \n
 * --> Quite twitchy... Probably a bit of smoothing should always be applied.
 */
void makeAvgVsAvgLog(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 32, 0);
        strip.normPixel(vu.vuLevelAvg_log) += CRGB(32, 128, 0);
        strip.optPixel(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 64, 0);
        strip.optPixel(vu.vuPeakHandlerAvg_log.getVU()) += CRGB(64, 255, 0);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplot_log = true;
    vu->smoothingFactor = 0;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Same as makeAvgVsAvgLog(), but with smoothing.
 * --> This Looking really promising - and also far less dependant on the audio level
 * at the analog input.
 */
void makeAvgVsAvgLog_smoothed(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 32, 0);
        strip.normPixel(vu.vuLevelAvg_log) += CRGB(32, 128, 0);
        strip.optPixel(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 64, 0);
        strip.optPixel(vu.vuPeakHandlerAvg_log.getVU()) += CRGB(64, 255, 0);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplot_log = true;
    vu->smoothingFactor = 3;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Same as makeAvgVsAvgLog(), plus RMS stuff.
 * Let's see the difference between avg and RMS on a logarithmic scale. Smoothing is
 * also enabled, because the twitchy stuff without it won't be helpful. \n
 * --> Here the difference between RMS and avg still isn't too much, but much more
 * visible. So the additional effort for RMS could probably be worth it.
 */
void makeAvgLogVsRmsLogVU(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normPixel(vu.vuLevelAvg) += CRGB(0, 32, 0);
        strip.normPixel(vu.vuLevelRms) += CRGB(0, 0, 32);
        strip.normPixel(vu.vuLevelAvg_log) += CRGB(32, 128, 0);
        strip.normPixel(vu.vuLevelRms_log) += CRGB(16, 0, 128);
        strip.optPixel(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 64, 0);
        strip.optPixel(vu.vuPeakHandlerRms.getVU()) += CRGB(0, 0, 64);
        strip.optPixel(vu.vuPeakHandlerAvg_log.getVU()) += CRGB(64, 255, 0);
        strip.optPixel(vu.vuPeakHandlerRms_log.getVU()) += CRGB(32, 0, 255);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotAvg = true;
    vu->enableTeleplotRms = true;
    vu->enableTeleplot_log = true;
    vu->smoothingFactor = 5;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/** Make the final VU - RMS on a logarithmic scale with a peak detection.
 * --> I'd call this one a success :-) \n
 * The only open thing is that this is still somehow dependant on the audio level
 * at the analog input. But that's going to be another investigation...
 */
void makeRmsLogVU(EC::AnimationScene &scene)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.normLineAbs(0.0, vu.vuLevelRms_log, CRGB(0, 32, 128));
        strip.optPixel(vu.vuPeakHandlerRms_log.getVU()) += CRGB(64, 0, 255);
    };

    auto vu = scene.append(new EC::LowLevelAudioPlaygroundVU(audioSample, {leds, NUM_LEDS}, drawFunction));
    vu->enableTeleplotRms = true;
    vu->enableTeleplot_log = true;
    vu->smoothingFactor = 5;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

/// An outlook what is possible.
void makeEssentialVU(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = scene.append(new EC::EssentialVU(audioSample, strip, false));
    vu->vuBarColor = CRGB(64, 0, 32);
    vu->peakDotColor = CRGB(64, 32, 0);
    vu->vuPeakHandler.peakHold = 500;
    vu->fadeRate = 0;

    // animationDuration = 10;
}

void makeTestVU1(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto vu = scene.append(new EC::TestVU1(audioSample, strip, false));
    // vu->vuBarColor = CRGB(64, 0, 32);
    // vu->peakDotColor = CRGB(64, 32, 0);
    // vu->vuPeakHandler.peakHold = 500;
    // vu->enableRangeExtender = true;
    // vu->fadeRate = 0;

    // animationDuration = 10;
    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct allAnimations[] = {
    &makeRawAudioVU,

    &makeSampleAvgVU,
    &makeSampleAvgVU_smoothed,
    &makeSampleRmsVU,
    &makeSampleRmsVU_smoothed,
    &makeAvgVsRmsVU,
    &makeAvgVsRmsVU_smoothed,
    &makeAvgVsRmsVU_peak,
    &makeAvgVsRmsVU_peak_smoothed,
    &makeAvgVsAvgLog,
    &makeAvgVsAvgLog_smoothed,
    &makeAvgLogVsRmsLogVU,
    &makeRmsLogVU,
    &makeEssentialVU,

    nullptr};

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
        const uint8_t animationDelay = animationSpeed ? 256 - animationSpeed : 0;

        static uint16_t lastSpeed = 0;
        if (animationSpeed != lastSpeed)
        {
            // Serial.print("speed: ");
            // Serial.print(animationSpeed);
            // Serial.print(" delay: ");
            // Serial.println(animationDelay);
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

    Serial.print(F("TestVU1 = "));
    Serial.println(sizeof(EC::TestVU1));
}
#endif

//------------------------------------------------------------------------------
