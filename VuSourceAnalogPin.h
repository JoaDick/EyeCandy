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
#include "FastLedStrip.h"
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
      : public Animation,
        public VuSource
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

    /** Stretch the "interesting" part of the VU meter over the entire LED strip.
     * This setting can be adjusted at runtime.
     */
    bool enableRangeExtender = true;

    /** Usually there's nothing to configure here.
     * Publicly accessible mainly for debugging.
     */
    VuRangeExtender vuRangeExtender;

    /** Get the current VU level.
     * @see VuSource::getVU()
     */
    float getVU() override
    {
      return _vuLevel;
    }

    /** Constructor for Pattern mode.
     * @param audioSource  Read the audio samples from there.
     * @param strip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     *                  If the following VU Overlay has a \c fadeRate_default() method, use that.
     *                  Not relevant in Overlay mode.
     */
    VuSourceAnalogPin(float &audioSource,
                      FastLedStrip ledStrip,
                      uint8_t fadeRate)
        : fadeRate(fadeRate), patternUpdatePeriod(AnimationBase::patternUpdatePeriod_default), _audioSource(audioSource), _strip(ledStrip)
    {
    }

    /** Constructor for Overlay mode.
     * @param audioSource  Read the audio samples from there.
     */
    explicit VuSourceAnalogPin(float &audioSource)
        : fadeRate(0), patternUpdatePeriod(0), _audioSource(audioSource), _strip(FastLedStrip::GetNULL())
    {
    }

    /** Period (in ms) for updating the LED strip's Pattern.
     * It reflects how frequent showPattern() is getting called. \n
     * 0 means Overlay mode; Pattern updates are \e not triggered.
     * @see patternUpdatePeriod_default
     */
    const uint8_t patternUpdatePeriod;

    /** Default value for patternUpdatePeriod (in ms).
     * @see AnimationBase::patternUpdatePeriod_default
     */
    static constexpr uint8_t patternUpdatePeriod_default = AnimationBase::patternUpdatePeriod_default;

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      vuLevelHandler.addSample(_audioSource);

      if (patternUpdatePeriod)
      {
        if (currentMillis >= _nextShowPattern)
        {
          if (fadeRate)
          {
            _strip.fadeToBlackBy(fadeRate);
          }
          else
          {
            _strip.fillSolid(CRGB::Black);
          }
          _nextShowPattern = currentMillis + patternUpdatePeriod;
          wasModified = true;
        }
      }

      if (wasModified)
      {
        _vuLevel = vuLevelHandler.capture();
        if (enableRangeExtender)
        {
          _vuLevel = vuRangeExtender.process(_vuLevel);
        }
      }
    }

  private:
    float &_audioSource;
    FastLedStrip _strip;
    float _vuLevel = 0.0;
    uint32_t _nextShowPattern = 0;
  };

} // namespace EC
