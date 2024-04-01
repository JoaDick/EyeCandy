#pragma once
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

#include <Arduino.h>
#include <math.h>
#include "AnimationBase.h"
#include "AudioNormalizer.h"
#include "VuPeakHandler.h"

//------------------------------------------------------------------------------

#ifndef VU_TOOLS_ENABLE_TELEPLOT
#define VU_TOOLS_ENABLE_TELEPLOT 0
#endif

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  /** Helper for logging raw audio samples to Teleplot extension of VS Code.
   * \a audioSample must have been normalized via AudioNormalizer.
   * @see https://marketplace.visualstudio.com/items?itemName=alexnesnes.teleplot
   */
  void logAudioSample(float audioSample)
  {
#if (VU_TOOLS_ENABLE_TELEPLOT)
    // Teleplot: raw audio samples
    Serial.print(F(">-:"));
    Serial.println(-1.0);
    Serial.print(F(">+:"));
    Serial.println(1.0);
    Serial.print(F(">raw:"));
    Serial.println(audioSample);
#endif
  }

  //------------------------------------------------------------------------------

  /** Display the raw audio samples as VU.
   * Useful e.g. for roughly checking the audio level at the analog input pin -
   * because that signal is also used as audio source for all other VUs. \n
   * Its amplitude doesn't need to be any high at all, so better keep it rather
   * low than saturated (clipping).
   */
  class RawAudioVU
      : public AnimationBase
  {
  public:
    /** Set to true for logging audio samples over serial line (via Teleplot).
     * Use the Teleplot extension of VS Code for rendering the diagrams.
     * @see https://marketplace.visualstudio.com/items?itemName=alexnesnes.teleplot
     * @see logAudioSample()
     */
    bool enableTeleplot = false;

    /** Constructor
     * @param analogPin  Pin for reading the audio signal.
     * @param ledStrip  The LED strip.
     */
    RawAudioVU(uint8_t analogPin,
               FastLedStrip ledStrip)
        : AnimationBase(ledStrip, false, 25),
          _analogPin(analogPin)
    {
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      const float audioSample = _adcNormalizer.process(analogRead(_analogPin));
      float sample = (audioSample + 1.0) / 2.0;

      strip.n_pixel(1.0) = CRGB(64, 0, 0);
      strip.n_pixel(0.0) = CRGB(0, 0, 64);

      strip.n_lineAbs(_lastSample, sample, CRGB(0, 128, 0));
      strip.n_pixel(0.5) = CRGB(255, 255, 0);

      _lastSample = sample;

#if (VU_TOOLS_ENABLE_TELEPLOT)
      if (enableTeleplot)
      {
        logAudioSample(sample);
      }
#endif

      AnimationBase::processAnimation(currentMillis, wasModified);
    }

  private:
    const uint8_t _analogPin;
    AdcSampleNormalizer _adcNormalizer;
    float _lastSample = 0.5;
  };

  //------------------------------------------------------------------------------

  /** A VU as playground for exploring low level audio processing.
   * @note This VU doesn't manipulate the LED strip by itself.
   * Instead, it requires a custom function (provided via constructor) for
   * rendering the content on the LED strip. \n
   * Its main purpose is to provide a playground for exploring the low level math
   * behind the scenes.
   */
  class LowLevelAudioPlaygroundVU
      : public AnimationModelBase
  {
  public:
    /// Signature of the function for rendering the VU on the LED strip.
    using DrawingFct = void (*)(FastLedStrip &strip, LowLevelAudioPlaygroundVU &vu);

    /** Incorporate that many previous VU values for smoothing the output.
     * 0 means no smoothing.
     */
    uint8_t smoothingFactor = 0;

    /** VU level calculated by averaging (absolute) audio samples.
     * This value can be used by the function for rendering the VU on the LED strip.
     */
    float vuLevelAvg = 0.0;

    /** VU level calculated as RMS of the audio samples.
     * This value can be used by the function for rendering the VU on the LED strip.
     */
    float vuLevelRms = 0.0;

    /// Peak detection for averaged VU level.
    VuPeakHandler vuPeakHandlerAvg;

    /// Peak detection for RMS VU level.
    VuPeakHandler vuPeakHandlerRms;

    /// Same as vuLevelAvg, but on a logarithmic scale.
    float vuLevelAvg_log = 0.0;

    /// Same as vuLevelRms, but on a logarithmic scale.
    float vuLevelRms_log = 0.0;

    /// Same as vuPeakHandlerAvg, but on a logarithmic scale.
    VuPeakHandler vuPeakHandlerAvg_log;

    /// Same as vuPeakHandlerRms, but on a logarithmic scale.
    VuPeakHandler vuPeakHandlerRms_log;

    /** Set to true for logging averaged audio samples over serial line.
     * Use the Teleplot extension of VS Code for rendering the diagrams.
     * @see https://marketplace.visualstudio.com/items?itemName=alexnesnes.teleplot
     */
    bool enableTeleplotAvg = false;

    /// Same as enableTeleplotAvg, but for RMS audio samples.
    bool enableTeleplotRms = false;

    /// Also enable logging of averaged / RMS audio samples on a logarithmic scale.
    bool enableTeleplot_log = false;

    /** Noise floor for conversion to a logarithmic scale.
     * Volume levels below that value are treated as silence - like a squelch.
     */
    float noiseFloor_dB = -28.5;

    /** Constructor
     * @param analogPin  Pin for reading the audio signal.
     * @param ledStrip  The LED strip.
     * @param drawingFct  Pointer to a function for rendering the VU on the LED strip.
     */
    LowLevelAudioPlaygroundVU(uint8_t analogPin,
                              FastLedStrip ledStrip,
                              DrawingFct drawingFct)
        : AnimationModelBase(10, ledStrip, false, 50),
          _analogPin(analogPin), _drawingFct(drawingFct)
    {
      // Calculating the average value every 10ms, resulting in 100Hz refresh rate.
      // The LED strip is also updated every 10ms, resulting in 100 "FPS" for the VU.
      // -- Note: That's the default value; see AnimationBase::patternUpdatePeriod.

      vuPeakHandlerAvg.peakHold = 750;
      vuPeakHandlerAvg.peakDecay = 2500;
      vuPeakHandlerRms.peakHold = vuPeakHandlerAvg.peakHold;
      vuPeakHandlerRms.peakDecay = vuPeakHandlerAvg.peakDecay;

      vuPeakHandlerAvg_log.peakHold = vuPeakHandlerAvg.peakHold;
      vuPeakHandlerAvg_log.peakDecay = vuPeakHandlerAvg.peakDecay;
      vuPeakHandlerRms_log.peakHold = vuPeakHandlerAvg.peakHold;
      vuPeakHandlerRms_log.peakDecay = vuPeakHandlerAvg.peakDecay;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      const float audioSample = _adcNormalizer.process(analogRead(_analogPin));
      _sampleAvgSum += fabs(audioSample);
      _sampleRmsSum += square(audioSample);
      ++_sampleCount;

      AnimationModelBase::processAnimation(currentMillis, wasModified);
    }

    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      if (_sampleCount == 0)
      {
        return;
      }

      const float thisVuLevelAvg = _sampleAvgSum / _sampleCount;
      const float thisVuLevelRms = sqrt(_sampleRmsSum / _sampleCount);

      if (smoothingFactor == 0)
      {
        vuLevelAvg = thisVuLevelAvg;
        vuLevelRms = thisVuLevelRms;
      }
      else
      {
        vuLevelAvg *= smoothingFactor;
        vuLevelAvg += thisVuLevelAvg;
        vuLevelAvg /= smoothingFactor + 1;

        vuLevelRms *= smoothingFactor;
        vuLevelRms += thisVuLevelRms;
        vuLevelRms /= smoothingFactor + 1;
      }

      float thisVuLevelAvg_dB = noiseFloor_dB;
      if (vuLevelAvg > 0.0)
      {
        // https://en.wikipedia.org/wiki/DBFS#RMS_levels
        thisVuLevelAvg_dB = 20.0 * log10(vuLevelAvg) + 3.0;
        if (thisVuLevelAvg_dB < noiseFloor_dB)
        {
          thisVuLevelAvg_dB = noiseFloor_dB;
        }
      }
      vuLevelAvg_log = (noiseFloor_dB - thisVuLevelAvg_dB) / noiseFloor_dB;

      float thisVuLevelRms_dB = noiseFloor_dB;
      if (vuLevelRms > 0.0)
      {
        thisVuLevelRms_dB = 20.0 * log10(vuLevelRms) + 3.0;
        if (thisVuLevelRms_dB < noiseFloor_dB)
        {
          thisVuLevelRms_dB = noiseFloor_dB;
        }
      }
      vuLevelRms_log = (noiseFloor_dB - thisVuLevelRms_dB) / noiseFloor_dB;

      vuPeakHandlerAvg.process(vuLevelAvg, currentMillis);
      vuPeakHandlerRms.process(vuLevelRms, currentMillis);
      vuPeakHandlerAvg_log.process(vuLevelAvg_log, currentMillis);
      vuPeakHandlerRms_log.process(vuLevelRms_log, currentMillis);

      // prepare for next intervall
      _sampleCount = 0;
      _sampleAvgSum = 0.0;
      _sampleRmsSum = 0.0;
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      _drawingFct(strip, *this);
#if (VU_TOOLS_ENABLE_TELEPLOT)
      if (enableTeleplotAvg || enableTeleplotRms)
      {
        Serial.print(F(">-:"));
        Serial.println(0.0);
        Serial.print(F(">+:"));
        Serial.println(1.0);
        Serial.print(F(">avg:"));
        Serial.println(enableTeleplotAvg ? vuLevelAvg : 0.0);
        Serial.print(F(">RMS:"));
        Serial.println(enableTeleplotRms ? vuLevelRms : 0.0);
        Serial.print(F(">avg_log:"));
        Serial.println(enableTeleplot_log && enableTeleplotAvg ? vuLevelAvg_log : 0.0);
        Serial.print(F(">RMS_log:"));
        Serial.println(enableTeleplot_log && enableTeleplotRms ? vuLevelRms_log : 0.0);
      }
#endif
    }

  private:
    const uint8_t _analogPin;
    AdcSampleNormalizer _adcNormalizer;
    DrawingFct _drawingFct;

    uint16_t _sampleCount = 0;
    float _sampleAvgSum = 0.0;
    float _sampleRmsSum = 0.0;
  };

  //------------------------------------------------------------------------------

}
