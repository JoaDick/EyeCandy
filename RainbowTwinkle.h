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

#include "AnimationBase.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A twinkling rainbow animation.
   */
  class RainbowTwinkle
      : public PatternBase
  {
  public:
    /** Fading speed.
     * Lower value = longer glowing; 0 = not working.
     */
    uint8_t fadeRate;
    static uint8_t fadeRate_default() { return 5; }

    /// Color source of the Animation.
    ColorWheel color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = not working.
     */
    explicit RainbowTwinkle(FastLedStrip ledStrip,
                            uint8_t fadeRate = fadeRate_default())
        : PatternBase(ledStrip),
          fadeRate(fadeRate), color(4.0)
    {
      color.update();
      for (auto &pixel : strip)
      {
        color.volume = random(0xEF) + 0x10;
        // color.saturation = random(0x2F) + 0xD0;
        pixel = color;
      }
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      color.update();
      strip.fadeToBlack(fadeRate);
      for (auto &pixel : strip)
      {
        if (pixel.getLuma() <= 6)
        {
          color.volume = random(0x30) + 0xCF;
          // color.saturation = random(0x2F) + 0xD0;
          pixel = color;
        }
      }

      return 0;
    }
  };

} // namespace EC
