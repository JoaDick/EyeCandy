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

  /** Randomly glittering pixels.
   * Can be used as Pattern or as Overlay.
   */
  class Glitter_FL
      : public AnimationBase_FL
  {
    bool _mustAddGlitter = false;

  public:
    /** Draw the Glitter with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB foregroundColor = foregroundColor_default();
    static CRGB foregroundColor_default() { return CRGB::White; }

    /** Effect occurrence rate.
     * Higher value = more glitter.
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     */
    uint8_t effectRate = effectRate_default();
    static uint8_t effectRate_default() { return 20; }

    /** Fading speed.
     * Lower value = longer glowing.
     * This setting can be adjusted at runtime.
     * It is ignored in Overlay mode.
     */
    uint8_t fadeRate = fadeRate_default();
    static uint8_t fadeRate_default() { return 100; }

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    Glitter_FL(CRGB *ledStrip,
               uint16_t ledCount,
               bool overlayMode)
        : AnimationBase_FL(overlayMode ? TYPE_OVERLAY_FADING : TYPE_FADING_PATTERN, ledStrip, ledCount)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      fadeToBlackBy(ledStrip, ledCount, fadeRate);
      showOverlay(currentMillis);
      return 0;
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      if (random8() < effectRate)
      {
        _mustAddGlitter = true;
      }
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (_mustAddGlitter)
      {
        pixel(random16(ledCount)) = foregroundColor;
        _mustAddGlitter = false;
      }
    }
  };

} // namespace EC
