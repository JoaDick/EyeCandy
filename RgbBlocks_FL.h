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

  /** Some moving red / green / blue blocks.
   * Mainly intended for testing LED strips.
   */
  class RgbBlocks_FL
      : public AnimationBase_FL
  {
    static const uint8_t _blockCount = 6;
    uint16_t _animationCounter = 0;

  public:
    /** Number of LEDs per block.
     * This setting can be adjusted at runtime.
     */
    uint8_t blockSize = 5;

    /** Delay between updating the Animation (in ms).
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     * @note This delay influences the "Animation speed", but not the LED
     * refresh rate.
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
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      static const CRGB colorTable[_blockCount] =
          {
              CRGB{0x10, 0x00, 0x00}, // red
              CRGB{0x00, 0x00, 0x00}, // black
              CRGB{0x00, 0x10, 0x00}, // green
              CRGB{0x00, 0x00, 0x00}, // black
              CRGB{0x00, 0x00, 0x10}, // blue
              CRGB{0x00, 0x00, 0x00}  // black
          };

      if (blockSize > 0)
      {
        for (uint16_t i = 0; i < ledCount; ++i)
        {
          const uint16_t colorIndex = ((i + _animationCounter) / blockSize) % _blockCount;
          ledStrip[ledCount - 1 - i] = colorTable[colorIndex];
        }
      }
      else
      {
        fill_solid(ledStrip, ledCount, CRGB::Black);
      }

      return 0;
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      if (++_animationCounter >= _blockCount * blockSize)
      {
        _animationCounter = 0;
      }
    }

    /// @see AnimationBase::getAnimationDelay()
    uint16_t getAnimationDelay() override
    {
      return animationDelay;
    }
  };

} // namespace EC
