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
#include "AnimationBaseFL.h"

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
    // Teleplot: raw audio samples
    Serial.print(">-:");
    Serial.println(-1.0);
    Serial.print(">+:");
    Serial.println(1.0);
    Serial.print(">raw:");
    Serial.println(audioSample);
  }

  //------------------------------------------------------------------------------

  /** Display the raw audio samples as VU.
   * Useful e.g. for roughly checking the audio level at the analog input pin -
   * because that signal is also used as audio source for all other VUs. \n
   * Its amplitude doesn't need to be any high at all, so better keep it rather
   * low than saturated (clipping).
   */
  class RawAudioVU
      : public AnimationBaseFL
  {
  public:
    /** Set to true for logging audio samples over serial line (via Teleplot).
     * @see logAudioSample()
     */
    bool enableTeleplot = false;

    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     */
    RawAudioVU(float &audioSource,
               FastLedStrip ledStrip)
        : AnimationBaseFL(ledStrip, false, 25), _audioSource(audioSource)
    {
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      float sample = (_audioSource + 1.0) / 2.0;

      strip.normPixel(1.0) = CRGB(64, 0, 0);
      strip.normPixel(0.0) = CRGB(0, 0, 64);

      strip.normLineAbs(_lastSample, sample, CRGB(0, 128, 0));
      strip.normPixel(0.5) = CRGB(255, 255, 0);

      _lastSample = sample;

      if (enableTeleplot)
      {
        logAudioSample(sample);
      }

      AnimationBaseFL::processAnimation(currentMillis, wasModified);
    }

  private:
    float &_audioSource;

    float _lastSample = 0.5;
  };

  //------------------------------------------------------------------------------

  /** A VU that takes the average of some (absolute) audio samples as VU value.
   * There's also a floating average over the last few VU values (by default), as
   * an attempt to make the VU appear smoother and less twitchy.
   * But no matter how - it's not really looking very appealing. \n
   * But the fundamentally bad part is that the VU is always higly dependant on
   * the audio level at the analog input. \n
   * --> Fail; not useful :-(
   */
  class SampleAvgVU
      : public AnimationBaseFL
  {
  public:
    /** Incorporate that many previous VU values for smoothing the output.
     * 0 means no smoothing.
     */
    uint8_t smoothingFactor = 3;

    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     */
    SampleAvgVU(float &audioSource,
                FastLedStrip ledStrip)
        : AnimationBaseFL(ledStrip, false, 50), _audioSource(audioSource)
    {
      // Calculate the average value every 10ms, resulting in 100Hz refresh rate.
      animationDelay = 10;
      // The LED strip is also updated every 10ms, resulting in 100 "FPS" for the VU.
      // -- Note: That's the default value; see AnimationBase::patternDelay.
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      _sampleAvgSum += fabs(_audioSource);
      ++_sampleCount;

      AnimationBaseFL::processAnimation(currentMillis, wasModified);
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      if (_sampleCount == 0)
      {
        return;
      }

      const float thisVuLevel = _sampleAvgSum / _sampleCount;
      _sampleAvgSum = 0.0;
      _sampleCount = 0;

      if (smoothingFactor == 0)
      {
        _vuLevel = thisVuLevel;
      }
      else
      {
        _vuLevel *= smoothingFactor;
        _vuLevel += thisVuLevel;
        _vuLevel /= smoothingFactor + 1;
      }
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      strip.normLineRel(0.0, _vuLevel, CRGB(0, 128, 0));
    }

  private:
    float &_audioSource;

    float _vuLevel = 0.0;

    float _sampleAvgSum = 0.0;
    uint16_t _sampleCount = 0;
  };

  //------------------------------------------------------------------------------

}
