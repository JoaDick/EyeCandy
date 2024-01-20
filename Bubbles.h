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

#include "AnimationBase.h"
#include "intern/Bubble.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Rising Bubbles.
  class Bubbles
      : public AnimationBase
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
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    Bubbles(FastLedStrip ledStrip,
            bool overlayMode)
        : AnimationBase(ledStrip, overlayMode, fadeRate_default())
    {
      modelUpdatePeriod = 10;
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < BUBBLES_COUNT; ++i)
      {
        _bubbles[i].show(strip);
      }
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < BUBBLES_COUNT; ++i)
      {
        _bubbles[i].update(modelUpdatePeriod);
      }
    }

  private:
    Bubble _bubbles[BUBBLES_COUNT];
  };

} // namespace EC
