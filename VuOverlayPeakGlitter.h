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

#include "Animation.h"
#include "FastLedStrip.h"
#include "VuPeakHandler.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A VU Overlay that produces some glitter around the VU peaks.
   * @note The actual VU value is obtained from the given VuSource.
   */
  class VuOverlayPeakGlitter
      : public Animation
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 50; }

    /** Draw the glitter with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     * @param color  Draw the glitter with this color.
     */
    VuOverlayPeakGlitter(FastLedStrip ledStrip, VuSource &vuSource,
                         CRGB color = CRGB::White)
        : color(color), _strip(ledStrip), _vuSource(vuSource)
    {
      vuPeakHandler.peakHold = 20;
      vuPeakHandler.peakDecay = 0;
    }

    VuSource &getVuSource() { return _vuSource; }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        if (vuPeakHandler.process(_vuSource.getVU(), currentMillis))
        {
          const float vuLevel = vuPeakHandler.getVU();
          // let it overshoot a little bit so that it doesn't mess up with a peak dot
          const float delta = (vuLevel - _lastVuLevel);
          const float vuOvershoot = delta / 8.0;
          _strip.normPixel(vuLevel + vuOvershoot) = color;
          _lastVuLevel = vuLevel;
        }
      }
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuSource;
    float _lastVuLevel = 0.0;
  };

} // namespace EC
