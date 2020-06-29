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

#include "AnimationBase_FL.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** An Animation that uses FastLED's builtin fill_rainbow() function.
   * In case you observe a random red pixel in the pattern, that is a known bug:
   * // https://github.com/FastLED/FastLED/issues/668
   */
  class RainbowBuiltin_FL
      : public AnimationBase_FL
  {
  public:
    /** "Stretch" of the rainbow pattern.
     * 0 means all LEDs have the same color.
     * The higher the value, the more of the rainbow(s) are shown.
     * Values up to ~25 look fine.
     * This value can be adjusted at runtime.
     */
    uint8_t deltahue;

    /** Delay between each Animation update (in ms).
     * This value can be adjusted at runtime.
     */
    uint16_t animationDelay = 10;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param deltahue  "Stretch" of the rainbow pattern.
     */
    RainbowBuiltin_FL(CRGB *ledStrip,
                      uint16_t ledCount,
                      uint8_t deltahue = 5)
        : AnimationBase_FL(TYPE_SOLID_PATTERN, ledStrip, ledCount), deltahue(deltahue)
    {
    }

  private:
    /// @see AnimationBase::getAnimationDelay()
    uint16_t getAnimationDelay() override
    {
      return animationDelay;
    }

    /// @see AnimationBase::showPattern()
    bool showPattern(uint32_t currentMillis) override
    {
      ++_hue;
      fill_rainbow(ledStrip, ledCount, _hue, deltahue);
      return true;
    }

  private:
    uint8_t _hue = 0;
  };

} // namespace EC
