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
      : public AnimationBase
  {
  public:
    /// Determines how fast the color changes.
    uint8_t hueSpeed;
    static uint8_t hueSpeed_default() { return 35; }

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param hueSpeed  How fast the color changes.
     * @param startHue  Initial color.
     */
    explicit RainbowDrips(FastLedStrip ledStrip,
                          uint8_t hueSpeed = hueSpeed_default(),
                          uint8_t startHue = random8())
        : AnimationBase(40, ledStrip), hueSpeed(hueSpeed), _baseHue(startHue)
    {
      setModelUpdatePeriod(100);
      _baseHue *= _hueScaleFactor;
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      strip.fadeToBlack(1);
      for (auto i = 0; i < _numDrips; ++i)
      {
        const int8_t jitter = beatsin8(5, 0, 16, 0, _dripPos[i]) - 8;
        const int16_t ledPos = _dripPos[i] + jitter;
        strip.pixel(ledPos) = CHSV(_baseHue / _hueScaleFactor, 255, 255);
      }
      strip.blur(beatsin8(11, 100, 172));
    }

    /// @see AnimationBase::updateModel()
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
      _baseHue += hueSpeed;
    }

  private:
    static const uint8_t _effectRate = 10;
    static const uint8_t _numDrips = 4;
    static const uint8_t _hueScaleFactor = 16;
    uint16_t _baseHue = 0;
    uint16_t _dripPos[_numDrips]{};
  };

} // namespace EC
