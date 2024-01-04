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

#include "AnimationBaseFL.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Some randomly twinkling pixels.
   * Can be used as Pattern or as Overlay.
   */
  class Twinkles
      : public AnimationBaseFL2
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background (not
     * recommended).
     */
    static uint8_t fadeRate_default() { return 5; }

    /** Effect occurrence rate.
     * Higher value = more twinkles.
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     */
    uint8_t effectRate = effectRate_default();
    static uint8_t effectRate_default() { return 50; }

    /// Deprecated; only for legacy compatibility.
    Twinkles(CRGB *ledStrip,
             uint16_t ledCount,
             bool overlayMode = false)
        : Twinkles(FastLedStrip(ledStrip, ledCount), overlayMode)
    {
    }

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    explicit Twinkles(FastLedStrip ledStrip,
                      bool overlayMode = false)
        : AnimationBaseFL2(ledStrip, overlayMode, fadeRate_default())
    {
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (random8() < effectRate)
      {
        uint16_t i = random(strip.ledCount());
        auto& pixel = strip[i];
        if (pixel.getLuma() < 3)
        {
          pixel = CHSV(redShift(random(256)), 255, random(64) + 192);
        }
      }
    }
  };

} // namespace EC
