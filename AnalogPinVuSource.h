#pragma once
/*******************************************************************************

MIT License

Copyright (c) 2025 Joachim Dick

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

#include "Animation.h"
#include "AudioNormalizer.h"
#include "VuLevelHandler.h"
#include "VuRangeExtender.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** An Animation-Worker for calculating the current VU level from an analog input pin.
   * Create a (static) instance in your sketch, and pass that to SetupEnv when it requests for a
   * VuSource via callback.
   * Call readAudioSample() from the main loop as often as possible to capture audio samples.
   */
  class AnalogPinVuSource
      : public Animation
  {
  public:
    /** Usually there's nothing to configure here.
     * Publicly accessible mainly for debugging.
     */
    VuLevelHandler vuLevelHandler;

    /** Usually there's nothing to configure here.
     * Publicly accessible mainly for debugging.
     */
    VuRangeExtender vuRangeExtender;

    /// Make this class usable as a VuSource (to circumvent multiple inheritance).
    operator VuSource &() { return asVuSource(); }
    VuSource &asVuSource() { return _vuBuffer; }

    /** Constructor.
     * @param analogPin  Pin for reading the audio signal.
     * @param sampleCount  Number of audio samples to integrate for calculating the VU level.
     */
    explicit AnalogPinVuSource(uint8_t analogPin,
                               uint16_t sampleCount = 100)
        : vuLevelHandler(sampleCount), _analogPin(analogPin)
    {
    }

    /** Read an audio sample from the analog input pin.
     * This method shall be called from the main loop as often as possible (i.e. in every cycle).
     */
    void readAudioSample()
    {
      const float audioSample = _adcNormalizer.process(analogRead(_analogPin));
      vuLevelHandler.addSample(audioSample);
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      // just capture and store the current VU value
      const float rawVuValue = vuLevelHandler.capture();
      _vuBuffer.vuValue = vuRangeExtender.process(rawVuValue);
      return 0;
    }

  private:
    const uint8_t _analogPin;
    AdcSampleNormalizer _adcNormalizer;
    BufferingVuSource _vuBuffer;
  };

} // namespace EC
