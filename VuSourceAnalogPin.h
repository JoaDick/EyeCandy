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

#include "AnimationBase.h"
#include "VuLevelHandler.h"
#include "VuRangeExtender.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** An Animation-Worker for calculating the current VU level from an analog input pin.
   * For convenience, it also provides the oftentimes used default Patterns fading background and
   * black background. Thus it will determine the update rate of the following VU Overlays. \n
   * When a different Pattern shall be used for the VU, this worker can be configured to Overlay
   * mode. Then it only provides the VU value based on the other Pattern's update rate - but
   * it doesn't manipulate the LED strip.
   */
  class VuSourceAnalogPin
      : public Animation
  {
  public:
    /** Fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     * Change it only when the following VU Overlay explicitly uses this as configuration option,
     * i.e. usually when it offers a static \c fadeRate_default() method. \n
     * Not relevant in Overlay mode.
     */
    uint8_t fadeRate;

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

    /** Constructor for Pattern mode.
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background. \n
     *                  If the following VU Overlay has a \c fadeRate_default() method, use that. \n
     *                  Not relevant in Overlay mode.
     * @param sampleCount  Number of audio samples to integrate for calculating the VU level.
     */
    VuSourceAnalogPin(float &audioSource,
                      FastLedStrip ledStrip,
                      uint8_t fadeRate,
                      uint16_t sampleCount = 100)
        : VuSourceAnalogPin(EC_DEFAULT_UPDATE_PERIOD, audioSource, ledStrip, fadeRate, sampleCount)
    {
    }

    /** Constructor for Overlay mode.
     * @param audioSource  Read the audio samples from there.
     * @param sampleCount  Number of audio samples to integrate for calculating the VU level.
     */
    explicit VuSourceAnalogPin(float &audioSource,
                               uint16_t sampleCount = 100)
        : VuSourceAnalogPin(0, audioSource, FastLedStrip::getNull(), 0, sampleCount)
    {
    }

    /** Constructor for a default Pattern with custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_UPDATE_PERIOD.
     * @param patternUpdatePeriod  Period (in ms) for updating the LED strip's Pattern.
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background. \n
     *                  If the following VU Overlay has a \c fadeRate_default() method, use that. \n
     *                  Not relevant in Overlay mode.
     * @param sampleCount  Number of audio samples to integrate for calculating the VU level.
     */
    VuSourceAnalogPin(uint8_t patternUpdatePeriod,
                      float &audioSource,
                      FastLedStrip ledStrip,
                      uint8_t fadeRate,
                      uint16_t sampleCount = 100)
        : fadeRate(fadeRate),
          vuLevelHandler(sampleCount),
          _strip(ledStrip),
          _audioSource(audioSource),
          _patternUpdateTimer(patternUpdatePeriod)
    {
    }

    /** Get the pause (in ms) between updates of the LED strip's Pattern.
     * It reflects how frequent showPattern() is getting called. \n
     * 0 means Overlay mode; Pattern updates are \e not triggered. \n
     * @note By default, EC_DEFAULT_UPDATE_PERIOD is used - unless the Animation implementation
     * configures a differnt value.
     */
    uint16_t getPatternUpdatePeriod() { return _patternUpdateTimer.updatePeriod; }

    /// Only for debugging.
    FastLedStrip getStrip() { return _strip; }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      vuLevelHandler.addSample(_audioSource);

      if (_patternUpdateTimer.process(currentMillis))
      {
        wasModified = true;
        showDefaultPattern(_strip, fadeRate);
      }

      if (wasModified)
      {
        vuRangeExtender.process(vuLevelHandler.capture());
      }
    }

  private:
    FastLedStrip _strip;
    float &_audioSource;
    AnimationTimer _patternUpdateTimer;
  };

} // namespace EC
