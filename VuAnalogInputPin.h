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
#include "Animation.h"
#include "AudioNormalizer.h"
#include "VuLevelHandler.h"
#include "VuRangeExtender.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

#if (EC_NEEDS_REWORK)
  /** An Animation-Worker for calculating the current VU level from an analog input pin.
   * This worker should be treated like an Overlay, meaning that the VU value is \e not updated
   * until the underlying Pattern triggered an update.
   */
  class VuAnalogInputPin
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

    /// Make this class usable as a VuSource.
    operator VuSource &() { return asVuSource(); }
    VuSource &asVuSource() { return vuRangeExtender; }

    /** Constructor.
     * @param analogPin  Pin for reading the audio signal.
     * @param sampleCount  Number of audio samples to integrate for calculating the VU level.
     */
    explicit VuAnalogInputPin(uint8_t analogPin,
                              uint16_t sampleCount = 100)
        : vuLevelHandler(sampleCount),
          _analogPin(analogPin)
    {
    }

  private:
    /// @see Animation::processAnimationOLD()
    void processAnimationOLD(uint32_t currentMillis, bool &wasModified) override
    {
      // !!! Won't work anymore !!!

      const float audioSample = _adcNormalizer.process(analogRead(_analogPin));
      vuLevelHandler.addSample(audioSample);

      if (wasModified)
      {
        vuRangeExtender.process(vuLevelHandler.capture());
      }
    }

    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      return 0;
    }

  private:
    const uint8_t _analogPin;
    AdcSampleNormalizer _adcNormalizer;
  };
#endif

} // namespace EC
