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
#include "MathUtils.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A dot moving forth and back again, leaving a sparkling trail.
   * When used as Overlay, it is just a moving dot without a trail.
   * \par Credits:
   * - The original meteor animation seems to be from there:
   *   https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectMeteorRain
   * - However, this implementation was developed based on another great tutorial:
   *   https://www.youtube.com/watch?v=yM5dY7K2KHM
   *   - https://www.youtube.com/watch?v=s3vHUdwVbVQ
   *   - https://pastebin.com/zUdcZpE9
   */
  class Meteor
      : public AnimationBase
  {
  public:
    /// Draw the meteor with this color.
    CRGB color;

    /// Beats per minute of the meteor oscillation.
    float bpm;

    /// How far the meteor shoots off the LED strip (as fraction of the strip).
    float overshoot;

    /// Fading speed: Lower value = longer glowing.
    uint8_t fadeBy = 96;

    /// Chance of fading: 0 = never, 255 = always.
    uint8_t fadeChance = 32;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param color  Draw the meteor with this color.
     * @param bpm  Beats per minute of the meteor oscillation.
     * @param overshoot  How far the meteor shoots off the LED strip (as fraction of the strip).
     */
    explicit Meteor(FastLedStrip ledStrip,
                    bool overlayMode,
                    CRGB color = CRGB::OrangeRed,
                    float bpm = 10.0,
                    float overshoot = 0.15)
        : AnimationBase(ledStrip, overlayMode, 0),
          color(color), bpm(bpm), overshoot(overshoot)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      meteorFadeToBlack(strip, fadeBy, fadeChance);
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      const float pos = beatsinF(bpm, 0.0 - overshoot, 1.0 + overshoot);
      strip.n_pixel(pos) = color;
    }
  };

} // namespace EC
