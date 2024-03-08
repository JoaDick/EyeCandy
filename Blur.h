#pragma once
/*******************************************************************************

MIT License

Copyright (c) original ???
2015 Andrew Tuline
2024 Joachim Dick

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

  /** Another variation of the "Blur" pattern, which is featured in many Youtube videos.
   * Unfortunately I wasn't able to find out the original author of that animation.
   * However, this implementation is based on Andrew Tuline's FastLED-Demo:
   * https://github.com/atuline/FastLED-Demos/blob/master/blur/blur.ino
   */
  class Blur
      : public AnimationBase
  {
  public:
    /** Constructor
     * @param ledStrip  The LED strip.
     */
    Blur(FastLedStrip ledStrip)
        : AnimationBase(ledStrip),
          NUM_LEDS(strip.ledCount())
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t ms) override
    {
      uint8_t blurAmount = dim8_raw(beatsin8(3, 64, 192)); // A sinewave at 3 BPS with values ranging from 64 to 192.
      strip.blur(blurAmount);                              // Apply some blurring to whatever's already on the strip, which will eventually go black.

      uint8_t i = beatsin8(9, 0, NUM_LEDS - 1);
      uint8_t j = beatsin8(7, 0, NUM_LEDS - 1);
      uint8_t k = beatsin8(5, 0, NUM_LEDS - 1);

      // The color of each point shifts over time, each at a different speed.
      strip[(i + j) / 2] = CHSV(ms / 29, 200, 255);
      strip[(j + k) / 2] = CHSV(ms / 41, 200, 255);
      strip[(k + i) / 2] = CHSV(ms / 73, 200, 255);
      strip[(k + i + j) / 3] = CHSV(ms / 53, 200, 255);
    }

    const uint16_t NUM_LEDS;
  };

} // namespace EC
