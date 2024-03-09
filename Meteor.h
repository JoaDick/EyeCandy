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
    /// Color source of the meteor.
    ColorWheel color;

    /// Beats per minute of the meteor oscillation.
    float bpm;

    /// How far the meteor shoots off the LED strip (as fraction of the strip).
    float overshoot;

    /// Fading speed: Lower value = longer glowing.
    uint8_t fadeBy = 96;

    /// Chance of fading: 0 = never, 255 = always.
    uint8_t fadeChance = 24;

    /** Size of the meteor (as fraction of the entire strip).
     * This setting can be adjusted at runtime.
     * Choose small values, like e.g. 0.03 for 3% of the strip. \n
     * 0.0 means exactly 1 pixel.
     */
    float size = 0.05;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param bpm  Beats per minute of the meteor oscillation.
     * @param overshoot  How far the meteor shoots off the LED strip (as fraction of the strip).
     */
    explicit Meteor(FastLedStrip ledStrip,
                    bool overlayMode,
                    float bpm = 8.0,
                    float overshoot = 0.2)
        : AnimationBase(ledStrip, overlayMode, 0),
          bpm(bpm), overshoot(overshoot)
    {
      color.moreRed = true;
      // Unicorn-Meteor :-)
      // color.bpm = 90;
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
      color.update();
      const float pos = beatsinF(bpm, 0.0 - overshoot, 1.0 + overshoot);
      if (size > 0.0)
      {
        strip.n_lineCentered(pos, size, color);
      }
      else
      {
        strip.n_pixel(pos) = color;
      }
    }
  };

} // namespace EC
