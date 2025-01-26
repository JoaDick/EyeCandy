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

//------------------------------------------------------------------------------

namespace EC
{

  /** A Pattern of colorful splashes, spreading over the entire LED strip.
   * Best to be used standalone; probably not suitable together with overlays.
   */
  class RainbowDrips
      : public AnimationModelBase
  {
  public:
    /// Color source of the Animation.
    ColorWheel color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     */
    explicit RainbowDrips(FastLedStrip ledStrip)
        : AnimationModelBase(100, ledStrip),
          color(1.0)
    {
      color.moreRed = false;
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      color.update();
      strip.fadeToBlack(1);
      for (auto i = 0; i < _numDrips; ++i)
      {
        const int8_t jitter = beatsin8(5, 0, 16, 0, _dripPos[i]) - 8;
        const int16_t ledPos = _dripPos[i] + jitter;
        strip.pixel(ledPos) = color /*[float(ledPos) / strip.ledCount()]*/;
      }
      strip.blur(beatsin8(11, 100, 172));

      return 40;
    }

    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      for (auto i = 0; i < _numDrips; ++i)
      {
        if (random8() < _effectRate)
        {
          const auto p1 = beatsin16(13, 0, strip.ledCount() - 1);
          const auto p2 = beatsin16(19, 0, strip.ledCount() - 1);
          _dripPos[i] = (p1 + p2) / 2;
        }
      }
    }

  private:
    static const uint8_t _effectRate = 10;
    static const uint8_t _numDrips = 4;
    uint16_t _dripPos[_numDrips]{};
  };

} // namespace EC
