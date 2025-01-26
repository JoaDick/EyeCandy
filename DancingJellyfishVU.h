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

#ifndef EC_JELLYFISH_VU_DEBUG
#define EC_JELLYFISH_VU_DEBUG 0
#endif

//------------------------------------------------------------------------------

namespace EC
{

  /** A squishy colorful thingy that is dancing to the music.
   * Best used as standalone VU without other overlays.
   */
  class DancingJellyfishVU
      : public Animation
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static constexpr uint8_t fadeRate_default = 20;

    /// Color source of the VU.
    ColorWheel color{1.0, -0.67};

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     */
    DancingJellyfishVU(FastLedStrip ledStrip,
                       VuSource &vuSource)
#if (EC_JELLYFISH_VU_DEBUG)
        : _strip(ledStrip.getSubStrip(ledStrip.ledCount() / 2, 0)),
          _debugStrip(ledStrip.getSubStrip(0, ledStrip.ledCount() / 2)),
#else
        : _strip(ledStrip),
#endif
          _vuSource(vuSource)
    {
      // color.volume = 192;
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
      const float colorOffset = vuLevelAvg - vuDeltaAvg;

      _strip.n_lineCentered(vuLevelAvg, 1.25 * vuDeltaAvg, color[colorOffset]);

#if (EC_JELLYFISH_VU_DEBUG)
      _debugStrip.clear();

      _debugStrip.n_pixel(vuLevel) = CRGB(0, 0, 128);
      _debugStrip.n_pixel(vuLevelAvg) = CRGB(0, 128, 0);
      // _debugStrip.n_pixel(colorVolume) = CRGB(32, 32, 32);
      _debugStrip.n_pixel(colorOffset) = CRGB(64, 64, 0);
#endif

      return 0;
    }

  private:
#if (EC_JELLYFISH_VU_DEBUG)
    FastLedStrip _debugStrip;
#endif
    FastLedStrip _strip;
    VuSource &_vuSource;
    MovingAverage _vuLevelAvg{25};
    MovingAverage _vuDeltaAvg{3};
  };

} // namespace EC
