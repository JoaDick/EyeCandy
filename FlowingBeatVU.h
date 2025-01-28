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
#include "VuSource.h"

//------------------------------------------------------------------------------

#ifndef EC_FLOWING_BEAT_VU_DEBUG
#define EC_FLOWING_BEAT_VU_DEBUG 0
#endif

//------------------------------------------------------------------------------

namespace EC
{

  /** Visualization of the music's beats flowing along the LED strip.
   * Best used as standalone VU; doesn't work well with overlays.
   */
  class FlowingBeatVU
      : public Animation
  {
  public:
    /// Color source of the VU.
    ColorWheel color{1.0, -0.75};

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     */
    FlowingBeatVU(FastLedStrip ledStrip,
                  VuSource &vuSource)
#if (EC_FLOWING_BEAT_VU_DEBUG)
        : _strip(ledStrip.getSubStrip(ledStrip.ledCount() / 2, 0)),
          _debugStrip(ledStrip.getSubStrip(0, ledStrip.ledCount() / 2)),
#else
        : _strip(ledStrip),
#endif
          _vuSource(vuSource)
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

      const float colorVolume = constrainVU(0.4 + 2.5 * vuDelta);
      color.volume = colorVolume * 255;
      const float colorOffset = vuLevelAvg - vuDeltaAvg;

      _strip.shiftDown(color[colorOffset]);
      // _strip.shiftUp(color[colorOffset]);

#if (EC_FLOWING_BEAT_VU_DEBUG)
      _debugStrip.clear();

      _debugStrip.n_pixel(vuLevel) = CRGB(0, 0, 128);
      _debugStrip.n_pixel(vuLevelAvg) = CRGB(0, 128, 0);
      // _debugStrip.n_pixel(colorVolume) = CRGB(32, 32, 32);
      _debugStrip.n_pixel(colorOffset) = CRGB(64, 64, 0);
#endif

      return 20;
    }

  private:
#if (EC_FLOWING_BEAT_VU_DEBUG)
    FastLedStrip _debugStrip;
#endif
    FastLedStrip _strip;
    VuSource &_vuSource;
    MovingAverage _vuLevelAvg{30};
    MovingAverage _vuDeltaAvg{5};
  };

} // namespace EC
