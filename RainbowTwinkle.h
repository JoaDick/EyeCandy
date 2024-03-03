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
      : public AnimationBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background (not
     * recommended).
     */
    static uint8_t fadeRate_default() { return 5; }

    /** Delay between updating the Animation (in ms).
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     * @note This delay influences the "Animation speed", but not the LED
     * refresh rate.
     */
    static uint16_t modelUpdatePeriod_default() { return 100; }

    /** Constructor.
     * @param ledStrip  The LED strip.
     */
    explicit RainbowTwinkle(FastLedStrip ledStrip)
        : AnimationBase(ledStrip, false, fadeRate_default())
    {
      setModelUpdatePeriod(modelUpdatePeriod_default());
      for (auto &pixel : strip)
      {
        pixel = CHSV(redShift(_hue), random(0x2F) + 0xD0, random(0xEF) + 0x10);
      }
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      strip.fadeToBlack(fadeRate);

      for (auto &pixel : strip)
      {
        if (pixel.getLuma() <= 6)
        {
          pixel = CHSV(redShift(_hue), 0xFF, random(0x30) + 0xCF);
        }
      }
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      ++_hue;
    }

    uint8_t _hue = random(0xFF);
  };

} // namespace EC
