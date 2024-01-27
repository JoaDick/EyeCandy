#pragma once
/*******************************************************************************

MIT License

Copyright (c) 2020 Joachim Dick

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

#include "VuSource.h"

//------------------------------------------------------------------------------

#ifndef EC_ENABLE_VU_RANGE_EXTENDER_BYPASS
/// Enable an option to bypass the VU Range Extender functionality.
#define EC_ENABLE_VU_RANGE_EXTENDER_BYPASS 0
#endif

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for stretching the "interesting" part of a VU meter over the
   * entire LED strip.
   * The #VuLevelHandler is already producing a very suitable output, where the
   * VU effect span is mostly independent of the audio volume. However, the
   * drawback is that this interesting part of the Animation utilizes only about
   * 1/3 in the mid of the LED strip. The lower and upper parts of the strip are
   * almost constantly on (resp. off), with their sizes depending on the audio
   * signal's volume. \n
   * This VuRangeExtender stretches the output of a #VuLevelHandler, so that its
   * relevant part always spans the entire LED strip, without constantly on/off
   * LEDs in the lower and upper part.
   */
  class VuRangeExtender
      : public VuSource
  {
  public:
    /** Incorporate how many previous VU values for smoothing the output.
     * 0 means no smoothing.
     */
    uint8_t smoothingFactor = 5;

    /** Number of VU values to incorporate for average volume computation.
     * Usually there's no need for changing this value.
     */
    uint16_t vuLevelAvgLen = 100;

    /** Number of VU values to incorporate for dynamic range computation.
     * Usually there's no need for changing this value.
     */
    uint16_t rangeAvgLen = 10;

    /** Minimum enforced dynamic range.
     * Usually there's no need for changing this value.
     */
    float dynamicRangeMin = 0.15;

#if (EC_ENABLE_VU_RANGE_EXTENDER_BYPASS)
    /// Set to \c true for disabling the VU Range Extender functionality.
    bool bypass = false;
#endif

    /** Get the current VU level.
     * This means the adjusted VU level of the last call to process().
     * @return A normalized VU value between 0.0 ... 1.0, representing the current volume.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0!
     * @see VuSource::getVU()
     */
    float getVU() override { return _newVuLevel; }

    /** Process VU level adjustment on the given \a vuLevel.
     * Calculates an adjusted VU level which utilizes the entire LED strip.
     * @return A normalized VU value between 0.0 ... 1.0, representing the current volume.
     * The value is constrained to 0.0 ... 1.0, regardless of \a vuLevel.
     */
    float process(float vuLevel)
    {
#if (EC_ENABLE_VU_RANGE_EXTENDER_BYPASS)
      if (bypass)
      {
        _newVuLevel = vuLevel;
        return _newVuLevel;
      }
#endif

      vuLevelAvg = (vuLevelAvg * (vuLevelAvgLen - 1) + vuLevel) / vuLevelAvgLen;
      deltaAvg = (deltaAvg * (rangeAvgLen - 1) + abs(vuLevel - vuLevelAvg)) / rangeAvgLen;

      rangeMax = constrainVU(vuLevelAvg + 2.0 * deltaAvg);
      rangeMin = constrainVU(vuLevelAvg - 2.0 * deltaAvg);

      dynamicRange = rangeMax - rangeMin;
      if (dynamicRange < dynamicRangeMin)
      {
        dynamicRange = dynamicRangeMin;
      }

      _newVuLevel *= smoothingFactor;
      _newVuLevel += (vuLevel - rangeMin) / dynamicRange;
      _newVuLevel /= smoothingFactor + 1;

      _newVuLevel = constrainVU(_newVuLevel);
      return _newVuLevel;
    }

    /// Average input VU level. Only for debugging; don't modify!
    float vuLevelAvg = 0.5;

    /// Average dynamic range. Only for debugging; don't modify!
    float dynamicRange = 0.0;

    /// Only for debugging; don't modify!
    float deltaAvg = 0.0;

    /// Only for debugging; don't modify!
    float rangeMax = 0.0;

    /// Only for debugging; don't modify!
    float rangeMin = 0.0;

  private:
    float _newVuLevel = 0.0;
  };

}
