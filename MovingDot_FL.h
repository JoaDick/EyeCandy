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

  /** A dot moving forth and back again.
   * Example for an Animation that can be used as Pattern and as Overlay.
   */
  class MovingDot_FL
      : public AnimationBase_FL
  {
    int16_t _position = 0;
    bool _rising = false;

  public:
    /** Draw the dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB foregroundColor = foregroundColor_default();
    static CRGB foregroundColor_default() { return CRGB(255, 0, 0); }

    /** Fill LED strip with this color.
     * This setting can be adjusted at runtime.
     * It is ignored in Overlay mode.
     */
    CRGB backgroundColor = backgroundColor_default();
    static CRGB backgroundColor_default() { return CRGB(0, 10, 0); }

    /** Delay between moving the dot by 1 pixel (in ms).
     * 0 makes the dot disappear.
     * This setting can be adjusted at runtime.
     * @note This delay influences the "Animation speed", but not the LED
     * refresh rate.
     */
    uint16_t animationDelay = animationDelay_default();
    static uint16_t animationDelay_default() { return 20; }

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param foregroundColor  Draw the dot with this color.
     * @param backgroundColor  Fill LED strip with this color.
     */
    MovingDot_FL(CRGB *ledStrip,
                 uint16_t ledCount,
                 bool overlayMode,
                 const CRGB &foregroundColor = foregroundColor_default(),
                 const CRGB &backgroundColor = backgroundColor_default())
        : AnimationBase_FL(overlayMode ? TYPE_OVERLAY : TYPE_SOLID_PATTERN, ledStrip, ledCount), foregroundColor(foregroundColor), backgroundColor(backgroundColor)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      fill_solid(ledStrip, ledCount, backgroundColor);
      showOverlay(currentMillis);
      return 0;
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (animationDelay)
      {
        pixel(_position) = foregroundColor;
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      if (_rising)
      {
        if (_position < ledCount - 1)
          ++_position;
        else
          _rising = false;
      }
      else
      {
        if (_position > 0)
          --_position;
        else
          _rising = true;
      }
    }

    /// @see AnimationBase::getAnimationDelay()
    uint16_t getAnimationDelay() override
    {
      return animationDelay;
    }
  };

} // namespace EC
