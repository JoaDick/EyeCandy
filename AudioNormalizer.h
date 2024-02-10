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

#include "MovingAverage.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for normalizing raw ADC values from an audio signal.
   * Normalizing means eliminating the DC offset, and scaling each sample to the
   * defined range between -1.0 ... +1.0 (lowest / highest possible value).
   */
  class AudioNormalizer
  {
  public:
    /** Constructor
     * @param avgLength  Number of samples to incorporate for DC offset calculation.
     */
    explicit AudioNormalizer(uint16_t avgLength = 5000)
        : _sampleAvg(avgLength, 0.5)
    {
    }

    /** Read one value from ADC and normalize that sample.
     * @param pin  Analog input to read from.
     * @param maxRawValue  Maximum possible value from the ADC.
     * @return  The normalized audio sample, without DC offset.
     *          Output range: -1.0 ... +1.0
     */
    float analogRead(uint8_t pin, uint16_t maxRawValue = 1023)
    {
      return removeDC(::analogRead(pin) / float(maxRawValue));
    }

    /** Remove DC offset from the given \a dcSample.
     * @param dcSample  Pre-scaled audio sample, which still contains the DC offset.
     *                  Must already be scaled to the range 0.0 ... 1.0
     * @return  The normalized audio sample, without DC offset.
     *          Output range: -1.0 ... +1.0
     */
    float removeDC(float dcSample)
    {
      const float dcOffset = _sampleAvg.process(dcSample);
      return 2.0 * (dcSample - dcOffset);
    }

  private:
    MovingAverage _sampleAvg;
  };

}
