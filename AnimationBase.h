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
    const bool _overlayMode;
    uint32_t _lastUpdateAnimation = 0;
    uint32_t _nextShowPattern = 0;

  public:
    /** Default delay (in ms) between updating the LED strip.
     * This value is used when showPattern() returns 0.
     * Default of 10ms will result in a refresh rate of 100Hz.
     */
    uint8_t defaultPatternDelay = 10;

  protected:
    /** Constructor.
     * @param animationType  Type of Animation.
     */
    explicit AnimationBase(Type animationType)
        : Animation(animationType), _overlayMode((animationType & 0x30) == 0x10)
    {
    }

    /** Render the Animation's Pattern content.
     * This method must be implemented by all child classes that provide a
     * Pattern type Animation (#TYPE_SOLID_PATTERN or #TYPE_FADING_PATTERN).
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @return Delay (in ms) until calling this method again.
     *         Returning 0 means use default delay (recommended).
     * @note This method is \e not called in Overlay mode.
     */
    virtual uint8_t showPattern(uint32_t currentMillis)
    {
      return 0;
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

    /** Get the delay (in ms) before calling updateAnimation() the next time.
     * Defaults to 10ms. Child classes can override this method if they want a
     * different timing. 0 means don't call updateAnimation()
     */
    virtual uint16_t getAnimationDelay()
    {
      return 10;
    }

    /** Hook for custom background jobs.
     * This method may be optionally implemented by child classes for doing some
     * background work. It is called every time the Animation's process() method
     * gets called, just before the other processing.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note Child's implementation shall be as short as possible and must not
     * block or use any delays!
     */
    virtual void processAnimationBackground(uint32_t currentMillis)
    {
    }

  private:
    /// @see Animation::processAnimation()
    bool processAnimation(uint32_t currentMillis, bool wasModified) override
    {
      processAnimationBackground(currentMillis);

      const uint16_t animationDelay = getAnimationDelay();
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
          uint8_t patternDelay = showPattern(currentMillis);
          if (patternDelay == 0)
          {
            patternDelay = defaultPatternDelay;
          }
          _nextShowPattern = currentMillis + patternDelay;
          wasModified = true;
        }
      }

      return wasModified;
    }
  };

} // namespace EC
