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

#include "Animation.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Simple base class suitable for most Animations.
   * Provides support for Patterns and Overlays.
   */
  class AnimationBase
      : public Animation
  {
    // When this is commented out, code size of DemoReelVU.ino increases by
    // almost 400 byte !?!?!
    const bool dummy = 0;

    const bool _overlayMode;
    uint32_t _lastUpdateAnimation = 0;
    uint32_t _nextShowPattern = 0;

  public:
    /** Delay (in ms) before calling updateAnimation() the next time.
     * 0 means don't call updateAnimation()
     * This value is generally assigned by the Animation implementation.
     * Change it only if the child class explicitly uses this as configuration
     * option, i.e. when it also offers a static \c animationDelay_default()
     * method.
     */
    uint16_t animationDelay = 0;

    /** Delay (in ms) between updating the LED strip.
     * It determines how frequent showPattern() is getting called (not relevant
     * in overlay mode).
     * Default of 10ms will result in a refresh rate of 100Hz.
     * This value is generally assigned by the Animation implementation, and
     * there's usually no need to change it.
     */
    uint8_t patternDelay = 10;

  protected:
    /** Constructor.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    explicit AnimationBase(bool overlayMode)
        : _overlayMode(overlayMode)
    {
    }

    /** Render the Animation's Pattern content.
     * This method must be implemented by all child classes that provide a
     * Pattern type Animation (#TYPE_SOLID_PATTERN or #TYPE_FADING_PATTERN).
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method is \e not called in Overlay mode.
     */
    virtual void showPattern(uint32_t currentMillis)
    {
    }

    /** Render the Animation's Overlay content.
     * This method must be implemented by all child classes that provide an
     * Overlay type Animation.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method is \e only called in Overlay mode.
     */
    virtual void showOverlay(uint32_t currentMillis)
    {
    }

    /** Update the Animation's internal state (but don't show it yet).
     * This method should be implemented by all child classes that provide an
     * Overlay type Animation.
     * It may also be implemented by Pattern type Animations, where the
     * animation algorithm is separated from showPattern().
     * Timing is determined via getAnimationDelay().
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     */
    virtual void updateAnimation(uint32_t currentMillis)
    {
    }

  protected:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (animationDelay > 0)
      {
        if (currentMillis >= _lastUpdateAnimation + animationDelay)
        {
          updateAnimation(currentMillis);
          _lastUpdateAnimation = currentMillis;
        }
      }

      if (_overlayMode)
      {
        if (wasModified)
        {
          showOverlay(currentMillis);
        }
      }
      else
      {
        if (currentMillis >= _nextShowPattern)
        {
          showPattern(currentMillis);
          _nextShowPattern = currentMillis + patternDelay;
          wasModified = true;
        }
      }
    }
  };

} // namespace EC
