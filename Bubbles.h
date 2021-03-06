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

// #define BUBBLES_DEBUG

#include "AnimationBaseFL.h"
#include "intern/Bubble.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Rising Bubbles.
  class Bubbles
      : public AnimationBaseFL
  {
#ifdef BUBBLES_DEBUG
    static const uint8_t BUBBLES_COUNT = 1;
#else
    static const uint8_t BUBBLES_COUNT = 10;
#endif

  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 0; }

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    Bubbles(CRGB *ledStrip,
            uint16_t ledCount,
            bool overlayMode = false)
        : AnimationBaseFL(overlayMode, ledStrip, ledCount, fadeRate_default())
    {
      animationDelay = 10;
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < BUBBLES_COUNT; ++i)
      {
        _bubbles[i].show(*this);
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < BUBBLES_COUNT; ++i)
      {
        _bubbles[i].update(animationDelay);
      }
    }

  private:
    Bubble _bubbles[BUBBLES_COUNT];
  };

} // namespace EC
