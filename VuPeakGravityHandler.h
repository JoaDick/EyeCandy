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

  /** Helper class for detecting VU peaks.
   * Activate peak detection via arm(). Then call process() periodically; when a peak is detected,
   * it returns the peak's value and disarms. Call arm() again to prepare for detecting the next
   * peak. When disarmed or no peak has been detected yet, 0 is returned. \n
   * This detector checks if consecutive VU values are rising or falling. A transition from rising
   * to falling is reported as peak.
   */
  class VuPeakDetector
  {
  public:
    /** VU levels below that level will be set to 0.
     * Render the peak only when its position is greater than 0, so it will disappear during
     * silence.
     */
    float peakThreshold = 0.005;

    /** Calculate the peak for the given \a vuLevel.
     * After a peak was detected, it must be rearmed via arm().
     * @return When armed, the peak VU value when detected. Otherwise 0
     * @note \a vuLevel is set to 0 if it is below peakThreshold.
     */
    float process(float &vuLevel)
    {
      if (vuLevel <= peakThreshold)
      {
        vuLevel = 0.0;
      }

      float retval = 0.0;

      if (_isRising)
      {
        if (vuLevel < _lastVuLevel)
        {
          _isRising = false;
          if (_isArmed)
          {
            _isArmed = false;
            retval = _lastVuLevel;
          }
        }
      }
      else
      {
        if (vuLevel > _lastVuLevel)
        {
          _isRising = true;
        }
      }

      _lastVuLevel = vuLevel;
      return retval;
    }

    /// Activate peak detection.
    bool arm()
    {
      _isArmed = true;
    }

    /// Check if peak detection is activated.
    bool isArmed()
    {
      return _isArmed;
    }

  private:
    float _lastVuLevel = 0.0;
    bool _isRising = false;
    bool _isArmed = false;
  };

  //------------------------------------------------------------------------------

  /** Peak dot of a VU meter, influenced by gravity.
   * Set #a0 < 0.0 for the behaviour of a falling ball, that's bumped up by the VU bar.
   * Set #a0 > 0.0 for a bubble, floating off the peak.
   */
  class VuPeakGravityHandler
      : public VuSource
  {
  public:
    /// Configure a "Punched Ball" behaviour.
    void presetPunchedBall()
    {
      a0 = -2.75; // Takeoff acceleration.
      v0 = 0.75;  // Takeoff velocity.
    }

    /// Configure a "Floating Bubble" behaviour.
    void presetFloatingBubble()
    {
      a0 = 0.5; // Takeoff acceleration.
      v0 = 0.1; // Takeoff velocity.
    }

    /// Configure a "Peak Hold" alike behaviour (the default).
    void presetPeakHold()
    {
      a0 = -0.5; // Takeoff acceleration.
      v0 = 0.05; // Takeoff velocity.
    }

    /// Takeoff acceleration. Value 0.0 is not allowed.
    float a0 = -0.5;

    /// Takeoff velocity.
    float v0 = 0.05;

    /** Set to \c true for calculating the VU Dip instead of the VU Peak.
     * The commonly known \e peak is following the \e highest VU values, and is \e decreasing
     * during decay phase. \n
     * A \e dip behaves just the the other way around: : it is following the \e lowest VU values,
     * and is \e increasing during decay phase.
     */
    bool enableDipMode = false;

    /** Get the current VU level.
     * This means the curent position of the peak dot.
     * @return A normalized VU value between (typically) 0.0 ... 1.0, representing the dot's position.
     * @note Be aware that the retured value may sometimes be greater than 1.0!
     * @see VuSource::getVU()
     */
    float getVU() override { return _peakLevelOutput; }

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
      if (enableDipMode)
      {
        calculatePeak(1.0 - vuLevel, currentMillis);
        if (pos > 0.0)
        {
          _peakLevelOutput = 1.0 - pos;
        }
      }
      else
      {
        calculatePeak(vuLevel, currentMillis);
        _peakLevelOutput = pos;
      }
      return _peakDetected;
    }

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakDetector peakDetector;

  private:
    void calculatePeak(float vuLevel,
                       uint32_t currentMillis)
    {
      // VU level has overtaken current peak?
      if (vuLevel > pos)
      {
        // abort and await next peak
        peakDetector.arm();
      }

      _peakDetected = false;
      const float peakVuLevel = peakDetector.process(vuLevel);
      // detected a new peak?
      if (peakVuLevel)
      {
        _peakDetected = true;
        acc = a0;
        vel = v0;
        pos = peakVuLevel;
      }

      // awaiting next peak?
      if (peakDetector.isArmed())
      {
        pos = vuLevel;
      }
      // peak processing is active
      else
      {
        const float delta_t = (currentMillis - lastMillis) / 1000.0;
        vel += acc * delta_t;
        pos += vel * delta_t;

        // floating peak floated off the top?
        if (acc > 0.0 && pos > 1.0)
        {
          // await next peak
          peakDetector.arm();
        }
      }

      lastMillis = currentMillis;
    }

  private:
    float acc = 0.0; // current acceleration
    float vel = 0.0; // current velocity
    float pos = 0.0; // current position

    uint32_t lastMillis = 0;
    float _peakLevelOutput = 0.0;
    bool _peakDetected = false;
  };

}
