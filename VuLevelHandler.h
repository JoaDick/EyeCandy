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

#include <math.h>
#include "MathUtils.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for calculating the volume level of a VU meter.
   * Main feature is the internal transformation to a logarithmic scale (instead
   * of sticking to a linear scale), so both silent and loud audio signals will
   * lead to suitable results with a similar peak-to-peak range. \n
   * Usage: Continously feed in normalized audio samples via addSample() from your
   * loop() function.
   * Eventually, when the VU meter shall be rendered, call capture() to obtain the
   * current VU level. \n
   * This class is internally doing the following:
   * - calculate the RMS volume of all audio samples since last capture() calling
   * - convert RMS value to a dB / full-scale value (ca. -28.5 ... +3.0 dB_FS)
   * - scale that inconvenient dB_FS value to the range 0.0 ... 1.0
   * - smooth out the last few VU values for a less twitchy appearance
   * @note Use #AudioNormalizer for pre-processing the raw ADC values.
   */
  class VuLevelHandler
      : public VuSource
  {
  public:
    /** Noise floor threshold (in dB_FS).
     * Volume levels below that value are treated as silence - like a squelch.
     * Change that value only with care; the default is fine for 10 bit ADCs
     * like in the Arduino.
     */
    float noiseFloor_dB = -28.5;

    /** Constructor.
     * @param sampleCount  Number of audio samples to integrate for calculating the VU level.
     */
    explicit VuLevelHandler(uint16_t sampleCount = 100)
        : _squareAvg(sampleCount)
    {
    }

    /** Get the current VU level.
     * This means the VU level of the last call to capture().
     * @return A normalized VU value between 0.0 ... 1.0, representing the current volume.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0!
     * @see VuSource::getVU()
     */
    float getVU() override { return _vuLevel; }

    /** Feed in normalized audio samples.
     * Call this method frequently in the background.
     */
    void addSample(float audioSample)
    {
      _squareAvg.process(square(audioSample));
    }

    /** Call this method to obtain the current VU level.
     * Call it ONLY when the VU meter Animation shall be rendered.
     * @return A normalized VU value between 0.0 ... 1.0, representing the current volume.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0!
     */
    float capture()
    {
      _vuLevel = 0.0;
      volume_RMS = sqrt(_squareAvg.get());
      if (volume_RMS > 0.0)
      {
        // https://en.wikipedia.org/wiki/DBFS#RMS_levels
        const float volume_dB = 20.0 * log10(volume_RMS) + 3.0;
        if (volume_dB > noiseFloor_dB)
        {
          _vuLevel = (noiseFloor_dB - volume_dB) / noiseFloor_dB;
        }
      }
      return _vuLevel;
    }

    /// The intermediate RMS volume. Only for debugging; don't modify!
    float volume_RMS = 0.0;

  private:
    MovingAverage _squareAvg;
    float _vuLevel = 0.0;
  };

}
