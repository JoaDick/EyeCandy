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

#include <FastLED.h>

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for calculating a moving average (exponential weighted).
   * - Inspired by https://stackoverflow.com/a/50854247
   * - Optimized algorithm using only one division
   * - Length = 0 results in bypassing
   */
  class MovingAverage
  {
  public:
    /// Number of old values to incorporate.
    uint16_t avgLength;

    /** Constructor.
     * @param avgLength  Number of old values to incorporate.
     * @param initValue  Initial value for the average.
     */
    explicit MovingAverage(uint16_t avgLength, float initValue = 0.0)
        : avgLength(avgLength), avgValue(initValue)
    {
    }

    /// Get the current average value.
    float get()
    {
      return avgValue;
    }

    /// Override the current average value with the given \a newAvgValue.
    void override(float newAvgValue)
    {
      avgValue = newAvgValue;
    }

    /** Process the given \a newValue.
     * @return Updated average value.
     */
    float process(float newValue)
    {
      avgValue *= avgLength;
      avgValue += newValue;
      avgValue /= avgLength + 1;
      return avgValue;
    }

    /// Current average value.
    float avgValue;
  };

  //------------------------------------------------------------------------------

  float beatsinF(float bpm, float lowest, float highest, uint32_t timebase = 0, float phaseOffset = 0.0)
  {
    float x = beatsin88(bpm * 256, 0, 0xFFFF, timebase, phaseOffset * 0xFFFF);
    x /= 0xFFFF;
    x *= (highest - lowest);
    x += lowest;
    return x;
  }

}
