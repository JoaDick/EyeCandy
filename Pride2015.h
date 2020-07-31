#pragma once
/*******************************************************************************

MIT License

Copyright (c) Mark Kriegsman
Copyright (c) 2020 Joachim Dick - Ported to EyeCandy Animation

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

#include "AnimationBaseFL.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Pride2015
   * Animated, ever-changing rainbows.
   * by Mark Kriegsman
   * This Animation draws rainbows with an ever-changing,
   * widely-varying set of parameters.
   * https://github.com/FastLED/FastLED/blob/master/examples/Pride2015/Pride2015.ino
   */
  class Pride2015
      : public AnimationBaseFL
  {
    uint16_t sPseudotime = 0;
    uint16_t sLastMillis = 0;
    uint16_t sHue16 = 0;

  public:
    /** Put more emphasis on the red'ish colors when true.
     * This setting can be adjusted at runtime.
     */
    bool moreRed = true;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    Pride2015(CRGB *ledStrip,
              uint16_t ledCount)
        : AnimationBaseFL(TYPE_SOLID_PATTERN, ledStrip, ledCount)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      uint8_t sat8 = beatsin88(87, 220, 250);
      uint8_t brightdepth = beatsin88(341, 96, 224);
      uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
      uint8_t msmultiplier = beatsin88(147, 23, 60);

      uint16_t hue16 = sHue16; //gHue * 256;
      uint16_t hueinc16 = beatsin88(113, 1, 3000);

      uint16_t ms = millis();
      uint16_t deltams = ms - sLastMillis;
      sLastMillis = ms;
      sPseudotime += deltams * msmultiplier;
      sHue16 += deltams * beatsin88(400, 5, 9);
      uint16_t brightnesstheta16 = sPseudotime;

      for (uint16_t i = 0; i < ledCount; i++)
      {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;
        if (moreRed)
        {
          hue8 = redShift(hue8);
        }

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        CRGB newcolor = CHSV(hue8, sat8, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (ledCount - 1) - pixelnumber;

        nblend(pixel(pixelnumber), newcolor, 64);
      }
    }
  };

} // namespace EC
