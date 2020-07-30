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
    uint32_t _lastUpdateAnimation = 0;

  public:
    /** Delay (in ms) before calling updateAnimation() the next time.
     * 0 means don't call updateAnimation()
     * This value is generally assigned by the Animation implementation.
     * Change it only if the child class explicitly uses this as configuration
     * option, i.e. when it also offers a static \c animationDelay_default()
     * method.
     */
    uint16_t animationDelay = 0;

  protected:
    /// Constructor.
    PseudoAnimationBase()
        : Animation(TYPE_OVERLAY)
    {
    }

    /** Process the Pseudo-Animation's duties.
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     */
    virtual void updateAnimation(uint32_t currentMillis) = 0;

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
    }
  };

} // namespace EC
