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

  //------------------------------------------------------------------------------

  /** A Pattern that renders all LEDs with the same color.
   * Mainly intended as example, but can also be used in combination with Overlays.
   */
  class BgFillColor
      : public AnimationBase
  {
  public:
    /** Fill LED strip with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param color  Fill LED strip with this color.
     */
    BgFillColor(FastLedStrip ledStrip,
                CRGB color)
        : AnimationBase(ledStrip, false), color(color)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      strip.fill(color);
    }
  };

  //------------------------------------------------------------------------------

  /** An Effect that fades out the current content of the LED strip.
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   * Useful e.g. for dimming the underlying Pattern in an AnimationScene.
   */
  class BgFadeToBlack
      : public AnimationBase
  {
  public:
    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    BgFadeToBlack(FastLedStrip ledStrip,
                  bool overlayMode,
                  uint8_t fadeRate)
        : AnimationBase(ledStrip, overlayMode, fadeRate)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      // must be empty
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      showDefaultPattern(strip, fadeRate);
    }
  };

  //------------------------------------------------------------------------------

  // TODO: BgBlur

} // namespace EC
