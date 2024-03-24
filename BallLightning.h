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
#include "VuPeakForceHandler.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A ball that is emitting sparkling flares while moving randomly up and down the strip.
   * Best used as standalone Pattern; doesn't work well with overlays.
   *
   * ToDo:
   * - make this also a VU?
   * - extract class: BallLightningEngine{strip} -> showAnimation(millis, color)
   */
  class BallLightning
      : public PatternBase
  {
  public:
    /// Draw the lightning ball with this color.
    CRGB color;

    /** Size of the lightning ball (as fraction of the entire strip), without the flares.
     * This setting can be adjusted at runtime.
     * Choose small values, like e.g. 0.03 for 3% of the strip. \n
     * 0.0 means exactly 1 pixel.
     */
    float size = 0.05;

    /// Beats per minute of the lightning ball's essential oscillation.
    float bpm;

    /// Chance of fading: 0 = never, 255 = always.
    uint8_t fadeChance = 96;

    /// Flare's fading speed: Lower value = longer glowing.
    uint8_t fadeBy = 108;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param color  Draw the lightning ball with this color.
     * @param bpm  Beats per minute of the lightning ball's essential oscillation.
     */
    explicit BallLightning(FastLedStrip ledStrip,
                           CRGB color = CRGB::MediumVioletRed,
                           float bpm = 12.0)
        : PatternBase(ledStrip), color(color), bpm(bpm)
    {
    }

  private:
    /// @see PatternBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      // randomly dim current strip content
      meteorFadeToBlack(strip, fadeChance, fadeBy);

      // let the ball overshoot a bit to compensate for inoise8()'s limited output range
      const float overshoot = 0.1;
      float pos = inoise8(currentMillis / 4) / 255.0;
      pos += beatsinF(bpm, 0.0 - overshoot, 1.0 + overshoot);
      pos /= 2.0;

      // smoothing the ball's position
      _vuPeakHandler.process(pos, currentMillis);
      const float smoothedPos = _vuPeakHandler.getVU();
      const int16_t pixelPos = strip.n_pixelIndex(smoothedPos);

      // shift the whole strip content to the new position
      strip.shift(pixelPos - _lastPixelPos);
      _lastPixelPos = pixelPos;

      // emit the flares every 2nd time (i.e. every 20ms)
      if (_shiftFlares)
      {
        strip.getSubStrip(0, pixelPos).shiftDown(color);
        strip.getSubStrip(pixelPos, 0).shiftUp(color);
      }
      _shiftFlares ^= true;

      // draw the ball itself
      if (size > 0.0)
      {
        strip.n_lineCentered(pos, size, color);
      }
      else
      {
        strip.n_pixel(pos) = color;
      }
    }

  private:
    VuPeakForceHandler _vuPeakHandler;
    bool _shiftFlares = true;
    int16_t _lastPixelPos = 0;
  };

} // namespace EC
