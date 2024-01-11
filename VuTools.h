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
#include "AnimationBaseFL.h"

//------------------------------------------------------------------------------

namespace EC
{

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

      strip.normPixel(0.999) = CRGB(64, 0, 0);
      strip.normPixel(0.0) = CRGB(0, 0, 64);

      strip.normLineAbs(_lastSample, sample, CRGB(0, 128, 0));
      strip.normPixel(0.5) = CRGB(255, 255, 0);

      _lastSample = sample;

      AnimationBaseFL::processAnimation(currentMillis, wasModified);
    }

    float &_audioSource;
    float _lastSample = 0.5;
  };

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

}
