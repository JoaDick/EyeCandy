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

#include "MathUtils.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for normalizing raw ADC values from an analog input pin.
   * Normalizing means eliminating the DC offset, and scaling each sample to the defined range
   * between -1.0 ... +1.0 (lowest / highest regular value).
   */
  class AdcSampleNormalizer
  {
  public:
    /** Constructor.
     * @param maxRawValue  Maximum possible value from the ADC.
     *                     Example: Arduino's 10 bit ADC --> 1023
     * @param avgLength  Number of samples to incorporate for DC offset calculation.
     */
    explicit AdcSampleNormalizer(uint16_t maxRawValue = 1023,
                                 uint16_t avgLength = 5000)
        : _maxRawValue(maxRawValue), _sampleAvg(avgLength, 0.5)
    {
    }

    /** Process the given raw value from ADC, and return it as normalized sample.
     * @param rawSample  Analog value from ADC.
     * @return The normalized audio sample, without DC offset, and in the range -1.0 ... +1.0
     * @note Be aware that an overloaded / clipped / too loud audio signal may momentarily exceed
     * these limits!
     */
    float process(uint16_t rawSample)
    {
      const float dcSample = rawSample / float(_maxRawValue); // range  0.0 ... 1.0
      const float dcOffset = _sampleAvg.process(dcSample);    // about  0.5
      const float acSample = dcSample - dcOffset;             // range -0.5 ... +0.5
      return 2.0 * acSample;                                  // range -1.0 ... +1.0
    }

  private:
    const uint16_t _maxRawValue;
    MovingAverage _sampleAvg;
  };

}
