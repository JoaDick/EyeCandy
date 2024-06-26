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

  /** Some moving red / green / blue blocks.
   * Mainly intended for testing LED strips.
   */
  class RgbBlocks
      : public AnimationModelBase
  {
    static const uint8_t _blockCount = 6;
    uint16_t _animationCounter = 0;

  public:
    /** Number of LEDs per block.
     * This setting can be adjusted at runtime.
     */
    uint8_t blockSize = blockSize_default();
    static uint8_t blockSize_default() { return 5; }

    /** Delay between updating the Animation (in ms).
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     * @note This delay influences the "Animation speed", but not the LED
     * refresh rate.
     */
    static uint16_t modelUpdatePeriod_default() { return 100; }

    /** Constructor
     * @param ledStrip  The LED strip.
     */
    explicit RgbBlocks(FastLedStrip ledStrip)
        : AnimationModelBase(modelUpdatePeriod_default(), ledStrip.getReversedStrip(), false)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
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
        for (uint16_t i = 0; i < strip.ledCount(); ++i)
        {
          const uint16_t colorIndex = ((i + _animationCounter) / blockSize) % _blockCount;
          strip[i] = colorTable[colorIndex];
        }
      }
      else
      {
        strip.fill(CRGB::Black);
      }
    }

    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      if (++_animationCounter >= _blockCount * blockSize)
      {
        _animationCounter = 0;
      }
    }
  };

} // namespace EC
