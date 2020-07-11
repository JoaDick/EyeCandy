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

  /** Helper base class for periodic custom processing.
   * Benefit of this helper is that it can be added to an #AnimationRunner, thus
   * the child's updateAnimation() method is getting called frequently.
   * May e.g. be used for manipulating another Animation's configuration setting
   * automatically during runtime.
   */
  class PseudoAnimationBase
      : public Animation
  {
    uint32_t _nextUpdateAnimation = 0;

  protected:
    /// Constructor.
    PseudoAnimationBase()
        : Animation(TYPE_OVERLAY)
    {
    }

    /** Process the Pseudo-Animation's duties.
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @return Delay (in ms) until calling this method again.
     */
    virtual uint16_t updateAnimation(uint32_t currentMillis) = 0;

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
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      processAnimationBackground(currentMillis);

      if (currentMillis >= _nextUpdateAnimation)
      {
        const uint16_t updateDelay = updateAnimation(currentMillis);
        _nextUpdateAnimation = currentMillis + updateDelay;
      }
    }
  };

} // namespace EC
