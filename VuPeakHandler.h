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

namespace EC
{

  //------------------------------------------------------------------------------

  /** Helper class for calculating the peak dot position of a VU meter.
   * Just provide the output of a #VuLevelHandler instance, and get the position
   * of the corresponding peak dot via getVU().
   */
  class VuPeakHandler
      : public VuSource
  {
  public:
    /// Delay (in ms) before the dot starts to fall down.
    uint16_t peakHold = 200;

    /** Falling speed equivalent.
     * Time (in ms) the dot would need to fall down the entire strip.
     */
    uint16_t peakDecay = 1500;

    /** VU levels below that level will set the dot's position to 0.
     * Render the dot only when its position is greater than 0.0, so it will
     * disappear during silence.
     */
    float peakThreshold = 0.005;

    /** Get the current VU level.
     * This means the curent position of the peak dot.
     * @return A normalized VU value between 0.0 ... 1.0, representing the dot's position.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0 or less than 0.0!
     * @see VuSource::getVU()
     */
    float getVU() override
    {
      return _peakLevel;
    }

    /** Calculate the peak dot's position for the given \a vuLevel.
     * @param vuLevel  The current VU level.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @retval true   A new peak has been detected.
     * @retval false  No peak detected.
     * Use getVU() to get the position of the peak dot.
     * A new peak is reported, when the current \a vuLevel value is below the
     * previous one for the first time.
     */
    bool process(float vuLevel,
                 uint32_t currentMillis)
    {
      bool peakDetected = false;
      // rising?
      if (vuLevel >= _peakLevel)
      {
        _lastPeakTime = 0;
        _maxPeakValue = vuLevel;
        _peakLevel = vuLevel;
      }
      // falling
      else
      {
        // new peak detected?
        if (_lastPeakTime == 0)
        {
          peakDetected = true;
          _lastPeakTime = currentMillis;
        }
        // peak hold?
        else if (currentMillis <= _lastPeakTime + peakHold)
        {
          // nothing to do
        }
        // peak decay
        else
        {
          const uint16_t decayTime = currentMillis - (_lastPeakTime + peakHold);
          if (decayTime < peakDecay)
          {
            const float decayFactor = float(peakDecay - decayTime) / peakDecay;
            _peakLevel = _maxPeakValue * decayFactor;
          }
          else
          {
            _peakLevel = 0.0;
          }
        }
      }

      // peak too low?
      if (_peakLevel <= peakThreshold)
      {
        _peakLevel = 0.0;
        // peakDetected = false;
        _lastPeakTime = 0;
      }

      return peakDetected;
    }

  private:
    uint32_t _lastPeakTime = 0;
    float _maxPeakValue = 0.0;
    float _peakLevel = 0.0;
  };

  //------------------------------------------------------------------------------

  /** Helper class for calculating the inverse peak dot position of a VU meter.
   * In essence the same as VuPeakHandler, but it is following the minimum VU
   * level instead of the maximum VU level (as a "normal" VU does). \n
   * This thing was born during development as a utility to track the dynamic
   * range of an audio signal. But the effect is really interesting, so it can
   * of course also be used for creating really exceptional VUs. \n
   * Just give it a try!
   */
  class VuPeakHandlerInv
      : public VuSource
  {
  public:
    /// Delay (in ms) before the dot starts to fall down.
    uint16_t &peakHold = _peakHandler.peakHold;

    /** Falling speed equivalent.
     * Time (in ms) the dot would need to fall down the entire strip.
     */
    uint16_t &peakDecay = _peakHandler.peakDecay;

    /** VU levels below that level will set the dot's position to 0.
     * Render the dot only when its position is greater than 0.0, so it will
     * disappear during silence.
     */
    float &peakThreshold = _peakHandler.peakThreshold;

    /** Get the current VU level.
     * This means the curent position of the peak dot.
     * @return A normalized VU value between 0.0 ... 1.0, representing the dot's position.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0 or less than 0.0!
     * @see VuSource::getVU()
     */
    float getVU() override
    {
      return 1.0 - _peakHandler.getVU();
    }

    /** Calculate the peak dot's position.
     * @param vuLevel  The current VU level.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @retval true   A new peak has been detected.
     * @retval false  No peak detected.
     * Use getVU() to get the position of the peak dot.
     * A new peak is reported, when the current \a vuLevel value is above the
     * previous one for the first time.
     */
    bool process(float vuLevel,
                 uint32_t currentMillis)
    {
      return _peakHandler.process(1.0 - vuLevel, currentMillis);
    }

  private:
    VuPeakHandler _peakHandler;
  };

  //------------------------------------------------------------------------------

}
