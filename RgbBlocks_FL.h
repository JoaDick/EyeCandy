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

#ifndef ARRAYLEN
#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
#endif

//------------------------------------------------------------------------------

namespace EC
{

  /** Some moving red / green / blue blocks.
   * Mainly intended for testing LED strips.
   */
  class RgbBlocks_FL
      : public AnimationBase_FL
  {
    uint16_t animationCounter = 0;

  public:
    /** Delay between each Animation update (in ms).
     * This value can be adjusted at runtime.
     */
    uint16_t animationDelay = 100;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    RgbBlocks_FL(CRGB *ledStrip,
                 uint16_t ledCount)
        : AnimationBase_FL(TYPE_SOLID_PATTERN, ledStrip, ledCount)
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
      static const CRGB colorTable[] =
          {
              CRGB{0x10, 0x00, 0x00}, // red
              CRGB{0x00, 0x00, 0x00}, // black
              CRGB{0x00, 0x10, 0x00}, // green
              CRGB{0x00, 0x00, 0x00}, // black
              CRGB{0x00, 0x00, 0x10}, // blue
              CRGB{0x00, 0x00, 0x00}  // black
          };

      for (uint16_t i = 0; i < ledCount; ++i)
      {
        const uint16_t colorIndex = (i / 5) % ARRAYLEN(colorTable);
        const CRGB color = colorTable[colorIndex];
        const uint16_t ledIndex = (i + animationCounter) % ledCount;
        ledStrip[ledIndex] = color;
      }

      if (++animationCounter >= ledCount)
      {
        animationCounter = 0;
      }

      return true;
    }
  };

} // namespace EC
