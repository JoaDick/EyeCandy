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

/// A sketch telling the story how the basic audio processing of a VU evolved.

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
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    analogReference(EXTERNAL);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 10;
uint16_t animationDuration = defaultAnimationDuration;

//------------------------------------------------------------------------------

void make_RawAudioVU(EC::SetupEnv &env)
{
    auto &vu = env.add(new EC::RawAudioVU(PIN_MIC, env.strip()));
    // vu.enableTeleplot = true;

    // animationDuration = 10;
}

/** Make a VU that takes the average of some (absolute) audio samples as VU value.
 * Not really looking very appealing, and the fundamentally bad part is that the
 * VU is always higly dependant on the audio level at the analog input.
 * --> Fail; not useful :-(
 */
void make_SampleAvgVU(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_lineRel(0.0, vu.vuLevelAvg, CRGB(0, 128, 0));
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.smoothingFactor = 0;

    // animationDuration = 10;
}

/** Same as make_SampleAvgVU(), but with a floating average over the last few VU values.
 * That's an attempt to make_ the VU appear smoother and less twitchy.
 * But no matter how - it's still not looking very appealing.
 * --> Next fail!
 */
void make_SampleAvgVU_smoothed(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_lineRel(0.0, vu.vuLevelAvg, CRGB(0, 128, 0));
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.smoothingFactor = 3;

    // animationDuration = 10;
}

/** Make a VU that takes the RMS (Root Mean Square) of audio samples as VU value.
 * Interestingly, this is looking very similar to make_SampleAvgVU(), so smoothing this
 * VU will probably also look very similar...
 * @see https://audiosorcerer.com/post/rms-in-audio/#4-what-is-rms-in-audio
 */
void make_SampleRmsVU(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_lineRel(0.0, vu.vuLevelRms, CRGB(0, 0, 128));
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotRms = true;
    vu.smoothingFactor = 0;

    // animationDuration = 10;
}

/** Same as make_SampleRmsVU(), but with a floating average over the last few VU values (smoothing).
 * Not really much helpful, but mainly for completeness of the comparisons.
 * However, let's try to compare Average and RMS directly.
 */
void make_SampleRmsVU_smoothed(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_lineRel(0.0, vu.vuLevelRms, CRGB(0, 0, 128));
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotRms = true;
    vu.smoothingFactor = 3;

    // animationDuration = 10;
}

/** Make a VU that displays the averaged VU values and RMS VU values as dots.
 * Just to compare them. It seems like RMS level is only slightly higher than avg level.
 */
void make_AvgVsRmsVU(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.n_pixel(vu.vuLevelRms) += CRGB(0, 0, 128);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplotRms = true;
    vu.smoothingFactor = 0;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Same as make_AvgVsRmsVU(), but with smoothing.
 * This also shows clearly that RMS level is only slightly higher than avg level.
 * In Conclusion, the additional math for RMS over averaging might not be worth the
 * effort (at least not from just a visual viewpoint).
 */
void make_AvgVsRmsVU_smoothed(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.n_pixel(vu.vuLevelRms) += CRGB(0, 0, 128);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplotRms = true;
    vu.smoothingFactor = 3;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Same as make_AvgVsRmsVU(), but with a peak detection.
 * So we can better compare them and see the difference.
 */
void make_AvgVsRmsVU_peak(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.n_pixel(vu.vuLevelRms) += CRGB(0, 0, 128);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 255, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerRms.getVU()) += CRGB(0, 0, 255);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplotRms = true;
    vu.smoothingFactor = 0;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Same as make_AvgVsRmsVU_peak(), but with smoothing.
 * Just for completeness. Nevertheless, the main problem still exists:
 * The VU is still higly dependant on the audio level at the analog input.
 */
void make_AvgVsRmsVU_peak_smoothed(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 128, 0);
        strip.n_pixel(vu.vuLevelRms) += CRGB(0, 0, 128);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 255, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerRms.getVU()) += CRGB(0, 0, 255);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplotRms = true;
    vu.smoothingFactor = 3;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Make a VU that displays the averaged VU values as dots on a linear and logarithmic scale.
 * Let's try a different approach, since the difference between averaged
 * and RMS values isn't really big. \n
 * --> Quite twitchy... Probably a bit of smoothing should always be applied.
 */
void make_AvgVsAvgLog(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 32, 0);
        strip.n_pixel(vu.vuLevelAvg_log) += CRGB(32, 128, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 64, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg_log.getVU()) += CRGB(64, 255, 0);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplot_log = true;
    vu.smoothingFactor = 0;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Same as make_AvgVsAvgLog(), but with smoothing.
 * --> This Looking really promising - and also far less dependant on the audio level
 * at the analog input.
 */
void make_AvgVsAvgLog_smoothed(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 32, 0);
        strip.n_pixel(vu.vuLevelAvg_log) += CRGB(32, 128, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 64, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg_log.getVU()) += CRGB(64, 255, 0);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplot_log = true;
    vu.smoothingFactor = 3;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Same as make_AvgVsAvgLog(), plus RMS stuff.
 * Let's see the difference between avg and RMS on a logarithmic scale. Smoothing is
 * also enabled, because the twitchy stuff without smoothing won't be helpful. \n
 * --> Here the difference between RMS and avg still isn't too much, but clearly more
 * visible. So the additional effort for RMS could probably be worth it.
 */
void make_AvgLogVsRmsLogVU(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_pixel(vu.vuLevelAvg) += CRGB(0, 32, 0);
        strip.n_pixel(vu.vuLevelRms) += CRGB(0, 0, 32);
        strip.n_pixel(vu.vuLevelAvg_log) += CRGB(32, 128, 0);
        strip.n_pixel(vu.vuLevelRms_log) += CRGB(16, 0, 128);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg.getVU()) += CRGB(0, 64, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerRms.getVU()) += CRGB(0, 0, 64);
        strip.n_pixelOpt(vu.vuPeakHandlerAvg_log.getVU()) += CRGB(64, 255, 0);
        strip.n_pixelOpt(vu.vuPeakHandlerRms_log.getVU()) += CRGB(32, 0, 255);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotAvg = true;
    vu.enableTeleplotRms = true;
    vu.enableTeleplot_log = true;
    vu.smoothingFactor = 5;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

/** Make the final VU - RMS on a logarithmic scale with peak detection.
 * --> I'd call this one a success :-) \n
 * The only open thing is that this is still somehow dependant on the audio level
 * at the analog input. But that's going to be another investigation...
 */
void make_RmsLogVU(EC::SetupEnv &env)
{
    auto drawFunction = [](EC::FastLedStrip &strip, EC::LowLevelAudioPlaygroundVU &vu)
    {
        strip.n_lineAbs(0.0, vu.vuLevelRms_log, CRGB(0, 32, 128));
        strip.n_pixelOpt(vu.vuPeakHandlerRms_log.getVU()) += CRGB(64, 0, 255);
    };

    auto &vu = env.add(new EC::LowLevelAudioPlaygroundVU(PIN_MIC, env.strip(), drawFunction));
    vu.enableTeleplotRms = true;
    vu.enableTeleplot_log = true;
    vu.smoothingFactor = 5;
    vu.fadeRate = 0;

    // animationDuration = 10;
}

//------------------------------------------------------------------------------

EC::AnimationSceneMakerFct allAnimations[] = {
    &make_RawAudioVU,
    &make_SampleAvgVU,
    &make_SampleAvgVU_smoothed,
    &make_SampleRmsVU,
    &make_SampleRmsVU_smoothed,
    &make_AvgVsRmsVU,
    &make_AvgVsRmsVU_smoothed,
    &make_AvgVsRmsVU_peak,
    &make_AvgVsRmsVU_peak_smoothed,
    &make_AvgVsAvgLog,
    &make_AvgVsAvgLog_smoothed,
    &make_AvgLogVsRmsLogVU,
    &make_RmsLogVU,

    nullptr};

//------------------------------------------------------------------------------

EC::AnimationScene mainScene;
EC::SetupEnv animationSetupEnv({leds, NUM_LEDS}, mainScene);
EC::AnimationChangerSoft animationChanger(animationSetupEnv, allAnimations);

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
