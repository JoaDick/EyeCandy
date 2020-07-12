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

// #define WATERFALL_DEBUG

#include "AnimationBaseFL.h"
#include "intern/WaterfallDroplet.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// A Waterfall Animation.
  class Waterfall
      : public AnimationBaseFL
  {
#ifdef WATERFALL_DEBUG
    static const uint8_t DROPLET_COUNT = 1;
#else
    static const uint8_t DROPLET_COUNT = 15;
#endif

  public:
    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    Waterfall(CRGB *ledStrip,
              uint16_t ledCount)
        : AnimationBaseFL(TYPE_FADING_PATTERN, ledStrip, ledCount)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      fadeLightBy(ledStrip, ledCount, 25);
      showOverlay(currentMillis);
      return 0;
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < DROPLET_COUNT; ++i)
      {
        _droplets[i].show(*this);
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      const uint16_t animationDelay = getAnimationDelay();
      for (uint8_t i = 0; i < DROPLET_COUNT; ++i)
      {
        _droplets[i].update(animationDelay);
      }
    }

  private:
    WaterfallDroplet _droplets[DROPLET_COUNT];
  };

} // namespace EC
