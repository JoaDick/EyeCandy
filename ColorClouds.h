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
    /// Higher values make the color change faster.
    uint8_t hueSpeed;

    /// Higher values "squeeze" more color gradients on the LED strip.
    uint8_t hueSqueeze;

    /// Higher values make the clouds change faster.
    uint8_t volSpeed;

    /// Higher values make more clouds (but smaller ones).
    uint8_t volSqueeze;

    /// Put more emphasis on the red'ish colors when true.
    bool moreRed = false;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param speed  Higher values make the animation faster.
     * @param squeeze  Higher values increase the effect density.
     */
    explicit ColorClouds(FastLedStrip ledStrip,
                         uint8_t speed = 64,
                         uint8_t squeeze = 64)
        : PatternBase(ledStrip),
          hueSpeed(speed), hueSqueeze(squeeze),
          volSpeed(speed), volSqueeze(squeeze)
    {
      // hueSpeed = 255;
      // hueSpeed = 128;
      // hueSpeed = 3;
      // hueSpeed = 0;

      // hueSqueeze = 255;
      // hueSqueeze = 128;
      // hueSqueeze = 25;
      // hueSqueeze = 0;

      // volSpeed = 255;
      // volSpeed = 128;
      // volSpeed = 25;
      // volSpeed = 0;

      // volSqueeze = 255;
      // volSqueeze = 128;
      // volSqueeze = 45;
      // volSqueeze = 0;
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      const auto ledCount = strip.ledCount();
      const uint8_t hueOffset = beat88(64) >> 8;

      for (uint32_t i = 0; i < ledCount; i++)
      {
        const uint32_t hueX = i * hueSqueeze * 16;
        const uint32_t hueT = currentMillis * (1 + hueSpeed) / 4;
        uint8_t hue = inoise16(hueX, hueT) >> 7;
        hue += hueOffset;
        if (moreRed)
        {
          hue = redShift(hue);
        }

        const uint32_t volX = i * volSqueeze * 64;
        const uint32_t volT = currentMillis * (1 + volSpeed) / 8;
        long vol = inoise16(volX, volT);
        vol = map(vol, 25000, 47500, 0, 255);
        vol = constrain(vol, 0, 255);

        auto &pixel = strip[i];
        pixel = CHSV(hue, 255, vol);
        if (int(pixel.r) + pixel.g + pixel.b <= 1)
        {
          pixel = CRGB::Black;
        }

#if (0) // debugging only
        strip[i] = CHSV(hue, 255, vol / 2);
        if (i == ledCount - 1)
        {
          strip.n_pixel(hue / 255.0) = CRGB(255, 0, 0); // hue of last pixel = red dot
          strip.n_pixel(vol / 255.0) = CRGB(0, 255, 0); // vol of last pixel = green dot
        }
#endif
      }

      return 0;
    }

  private:
  };

} // namespace EC
