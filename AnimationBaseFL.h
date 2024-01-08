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

#include <FastLED.h>
#include "AnimationBase.h"
#include "FastLedStrip.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Simple base class suitable for most Animations which are using FastLED.
   * Child classes must fulfill the same requirements as for #AnimationBase.
   */
  class AnimationBaseFL
      : public AnimationBase
  {
  public:
    /** Fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     * This setting can be adjusted at runtime.
     * Only relevant if the child class explicitly uses this as configuration
     * option, i.e. when it also offers a static \c fadeRate_default() method.
     * Ignored in Overlay mode.
     */
    uint8_t fadeRate;

    /// Only for debugging.
    FastLedStrip getStrip()
    {
      return strip;
    }

  protected:
    /** Constructor.
     * @param strip        The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param fadeRate     Fading speed:
     *                     Lower value = longer glowing; 0 = black background.
     *                     Only relevant when default implementation of
     *                     showPattern() is used.
     */
    AnimationBaseFL(FastLedStrip strip,
                    bool overlayMode,
                    uint8_t fadeRate = 0)
        : AnimationBase(overlayMode), fadeRate(fadeRate), strip(strip)
    {
    }

    /** Default implementation of AnimationBase::showPattern()
     * Provides the oftentimes used fading background or black background.
     */
    void showPattern(uint32_t currentMillis) override
    {
      if (fadeRate)
      {
        strip.fadeToBlackBy(fadeRate);
      }
      else
      {
        strip.fillSolid(CRGB::Black);
      }
      showOverlay(currentMillis);
    }

    /// LED strip representation to be used by child classes for drawing the LED pixels.
    FastLedStrip strip;
  };

} // namespace EC
