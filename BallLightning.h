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
#include "ColorUtils.h"
#include "MathUtils.h"
#include "VuPeakForceHandler.h"

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  /// Rendering algorithm for the BallLightning Animation and VU.
  class LightningBallEngine
  {
  public:
    /// Chance of fading: 0 = never, 255 = always.
    uint8_t fadeChance = 96;

    /// Flare's fading speed: Lower value = longer glowing.
    uint8_t fadeBy = 108;

    /** Render the lightning ball.
     * @param strip  The LED strip.
     * @param pos  Normalized position of the ball.
     * @param size  Size of the ball's core (as fraction of the strip).
     * @param color  Draw the ball with this color.
     */
    void show(FastLedStrip &strip, float pos, float size, const CRGB &color)
    {
      // randomly dim current strip content
      meteorFadeToBlack(strip, fadeChance, fadeBy);

      // shift the whole strip content to the new position
      const int16_t pixelPos = strip.n_pixelIndex(pos);
      strip.shift(pixelPos - _lastPixelPos);
      _lastPixelPos = pixelPos;

      // shift emitted flares every 2nd time (i.e. every 20ms)
      if (_shiftFlares)
      {
        strip.getSubStrip(0, pixelPos).shiftDown();
        strip.getSubStrip(pixelPos, 0).shiftUp();
      }
      _shiftFlares ^= true;

      // draw the ball itself
      strip.n_lineCentered(pos, size, color);
    }

  private:
    int16_t _lastPixelPos = 0;
    bool _shiftFlares = true;
  };

  //------------------------------------------------------------------------------

  /** A ball that is emitting sparkling flares while moving randomly up and down the strip.
   * Best used as standalone Pattern; doesn't work well with overlays.
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
     * 0.0 means hiding the ball.
     */
    float size = 0.05;

    /// Beats per minute of the lightning ball's essential oscillation.
    float bpm;

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
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      // let the ball overshoot a bit to compensate for inoise8()'s limited output range
      const float overshoot = 0.1;
      float pos = inoise8(currentMillis / 4) / 255.0;
      pos += beatsinF(bpm, 0.0 - overshoot, 1.0 + overshoot);
      pos /= 2.0;

      // smoothing the ball's position
      _vuPeakHandler.process(pos, currentMillis);
      const float smoothedPos = _vuPeakHandler.getVU();

      // render the ball
      _ball.show(strip, smoothedPos, size, color);
      return 0;
    }

  private:
    LightningBallEngine _ball;
    VuPeakForceHandler _vuPeakHandler;
  };

  //------------------------------------------------------------------------------

  /** A ball that is emitting sparkling flares while dancing to the music.
   * Best used as standalone VU; doesn't work well with overlays.
   */
  class BallLightningVU
      : public PatternBase
  {
  public:
    /// Color source of the VU.
    ColorWheel color{-2.5, -0.33};

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     */
    BallLightningVU(FastLedStrip ledStrip,
                    VuSource &vuSource)
        : PatternBase(ledStrip), _vuSource(vuSource)
    {
    }

    VuSource &getVuSource() { return _vuSource; }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      color.update();

      const float vuLevel = _vuSource.getVU();
      const float vuLevelAvg = _vuLevelAvg.process(vuLevel);

      const float vuDelta = vuLevel - vuLevelAvg;
      const float vuDeltaAvg = _vuDeltaAvg.process(abs(vuDelta));

      const float colorVolume = constrainVU(0.45 + 1.8 * vuDelta);
      color.volume = colorVolume * 255;
      const float colorOffset = vuLevelAvg - vuDeltaAvg;

      _ball.show(strip, vuLevelAvg, 0.9 * vuDeltaAvg, color[colorOffset]);
      return 0;
    }

  private:
    VuSource &_vuSource;
    LightningBallEngine _ball;
    MovingAverage _vuLevelAvg{12};
    MovingAverage _vuDeltaAvg{4};
  };

  //------------------------------------------------------------------------------

} // namespace EC
