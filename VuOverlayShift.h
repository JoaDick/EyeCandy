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
      : public AnimationModelBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 5; }

    /// Color source of the VU.
    ColorWheel color;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     */
    VuOverlayShift(FastLedStrip ledStrip,
                   VuSource &vuSource)
        : AnimationModelBase(10, ledStrip, true),
          color(1.0, 0.67), _vuSource(vuSource)
    {
      vuPeakHandler.peakHold = 500;
      vuPeakHandler.peakDecay = 500;
      color.volume = 128;
    }

    VuSource &getVuSource() { return _vuSource; }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      color.update();

      const float vuLevel = constrainVU(_vuSource.getVU());

      const bool isPeak = vuPeakHandler.process(vuLevel, currentMillis);
      const float vuPeakLevel = constrainVU(vuPeakHandler.getVU());

      color.saturation = 255;
      // color.saturation = vuLevel * 255;

      color.volume = 128;
      // color.volume = vuLevel * 255;
      // color.volume = vuPeakLevel * 255;

      CRGB col = color;
      if (isPeak)
      {
        col = CRGB(96, 96, 96);
      }
      else
      {
        // col = CRGB::Black;
      }
      // strip.shiftUp(col);
      // strip.shiftUp(CRGB::Black);
      strip.shiftDown(CRGB::Black);
      strip.n_pixel(vuLevel) = col;
      // strip.n_lineAbs(0.0, vuLevel, col);
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
    }

  private:
    VuSource &_vuSource;
    // float _lastVuLevel = 0.0;
  };

} // namespace EC
