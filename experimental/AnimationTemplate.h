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

  /** Template for creating new Animations.
   * ...
   */
  class AnimationTemplate
      : public AnimationModelBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static constexpr uint8_t fadeRate_default = 50;

    /// Color of the Animation.
    CRGB color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param color  Draw the glitter with this color.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    AnimationTemplate(FastLedStrip ledStrip,
                      bool overlayMode,
                      CRGB color = CRGB(64, 64, 64),
                      uint8_t fadeRate = fadeRate_default)
        : AnimationModelBase(3000, ledStrip, overlayMode, fadeRate),
          color(color)
    {
    }

  private:
// implement this method only if needed (e.g. for Patterns)
// --> but then also consider changing the base class to PatternBase
#if (0)
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      return 0;
    }
#endif

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      strip.n_pixel(beatsinF(30.0, 0.05, 0.95)) = color;
      if (_enableExtras)
      {
        strip.n_pixel(randomF()) = CHSV(random8(), 255, 160);
      }
    }

// delete this method if not needed
// --> then also change the base class to AnimationBase
#if (1)
    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      _enableExtras ^= true;
    }
#endif

  private:
    bool _enableExtras = false;
  };

} // namespace EC
