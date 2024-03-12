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

  /// A pattern with softly floating colorful clouds.
  class ColorClouds
      : public PatternBase
  {
  public:
    /// Higher values make the color change slower.
    uint8_t hueSlowdown;

    /// Higher values "squeeze" more color gradients on the LED strip.
    uint8_t hueSqueeze;

    /// Higher values make the clouds change slower.
    uint8_t volSlowdown;

    /// Higher values make more clouds (but smaller ones).
    uint8_t volSqueeze;

    /// Minimum brightness.
    uint8_t volMin = 0;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param slowdown  Higher values make the animation slower.
     * @param squeeze  Higher values increase the effect density; recommended: 1 - ~10.
     */
    explicit ColorClouds(FastLedStrip ledStrip,
                         uint8_t slowdown = 32,
                         uint8_t squeeze = 4)
        : PatternBase(ledStrip),
          hueSlowdown(slowdown), hueSqueeze(squeeze),
          volSlowdown(slowdown / 2), volSqueeze(squeeze * 4)
    {
    }

  private:
    /// @see PatternBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      const auto ledCount = strip.ledCount();
      for (auto i = 0; i < ledCount; i++)
      {
        const uint16_t hueSD = currentMillis / (hueSlowdown + 1);
        uint8_t hue = (3 * inoise8(i * hueSqueeze, hueSD)) / 2;
        hue += beatsin8(2);

        const uint16_t volSD = currentMillis / (volSlowdown + 1);
        int16_t vol = 2 * inoise8(i * volSqueeze + 10000, volSD);
        vol -= 128;
        vol = constrain(vol, volMin, 255);

        strip[i] = CHSV(hue, 255, vol);

#if (0) // debugging only
        strip[i] = CHSV(hue, 255, vol / 2);
        if (i == ledCount - 1)
        {
          strip.n_pixel(hue / 255.0) = CRGB(255, 0, 0); // hue of last pixel = red dot
          strip.n_pixel(vol / 255.0) = CRGB(0, 255, 0); // vol of last pixel = green dot
        }
#endif
      }
    }

  private:
  };

} // namespace EC
