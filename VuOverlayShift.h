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
#include "VuPeakHandler.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** TBD - WIP
   *
   */
  class VuOverlayShift
      : public AnimationBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 5; }

    /** How fast the initial hue changes over time.
     * This setting can be adjusted at runtime.
     */
    float baseHueStep = -0.05;

    /** How much the hue varies depending on the VU level.
     * This setting can be adjusted at runtime. \n
     * 1.0 means one full color wheel cycle between 0.0 ... 1.0 VU level.
     */
    float vuHueRange = 0.67;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     */
    VuOverlayShift(FastLedStrip ledStrip, VuSource &vuSource)
        : AnimationBase(ledStrip, true), _vuSource(vuSource)
    {
      setModelUpdatePeriod(10);
      vuPeakHandler.peakHold = 500;
      vuPeakHandler.peakDecay = 500;
    }

    VuSource &getVuSource() { return _vuSource; }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      // if (wasModified)
      {
        const float vuLevel = constrainVU(_vuSource.getVU());

        const bool isPeak = vuPeakHandler.process(vuLevel, currentMillis);
        const float vuPeakLevel = constrainVU(vuPeakHandler.getVU());

        uint8_t hue = _startHue;
        hue = _startHue + vuLevel * vuHueRange * 255;
        // hue = _startHue + vuPeakLevel * vuHueRange * 255;

        uint8_t sat = 255;
        // sat = vuLevel * 255;

        uint8_t vol = 128;
        // vol = vuLevel * 255;
        // vol = vuPeakLevel * 255;

        CRGB color = CHSV(hue, sat, vol);
        if (isPeak)
        {
          color = CRGB(96, 96, 96);
        }
        else
        {
          // color = CRGB::Black;
        }
        // strip.shiftUp(color);
        // strip.shiftUp(CRGB::Black);
        strip.shiftDown(CRGB::Black);
        strip.normPixel(vuLevel) = color;
        // strip.normLineAbs(0.0, vuLevel, color);
      }
    }
    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      _startHue += baseHueStep;
      while (_startHue > 255.0)
      {
        _startHue -= 255.0;
      }
      while (_startHue < 0.0)
      {
        _startHue += 255.0;
      }
    }

  private:
    VuSource &_vuSource;
    // float _lastVuLevel = 0.0;
    float _startHue = (millis() + 128) & 0xFF;
  };

} // namespace EC
