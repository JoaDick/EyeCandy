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

  /** Some randomly twinkling pixels.
   */
  class Twinkles_FL
      : public AnimationBase_FL
  {
  public:
    /** Effect occurrence rate.
     * Higher value = more twinkles.
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     */
    uint8_t effectRate = 50;

    /** Fading speed.
     * Lower value = longer glowing.
     * This setting can be adjusted at runtime.
     */
    uint8_t fadeRate = 5;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    Twinkles_FL(CRGB *ledStrip,
                uint16_t ledCount)
        : AnimationBase_FL(TYPE_FADING_PATTERN, ledStrip, ledCount)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      fadeToBlackBy(ledStrip, ledCount, fadeRate);

      if (random8() < effectRate)
      {
        uint16_t i = random(ledCount);
        if (ledStrip[i].getLuma() < 3)
        {
          ledStrip[i] = CHSV(redShift(random(256)), 255, random(64) + 192);
        }
      }

      return 0;
    }
  };

} // namespace EC
