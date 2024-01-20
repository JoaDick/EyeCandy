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

#include "AnimationBase.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A Pattern that just fades out the current content of the LED strip.
   * Useful as trigger Pattern in combination with other Overlays.
   * However, since most Overlays already have this feature built-in through their own base class
   * AnimationBase, there's likely not a lot of demand for this Pattern...
   */
  class FadeOutPattern
      : public AnimationBase
  {
  public:
    /** Constructor
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed; Lower value = longer glowing; 0 = black background.
     */
    FadeOutPattern(FastLedStrip ledStrip,
                   uint8_t fadeRate)
        : AnimationBase(ledStrip, false, fadeRate)
    {
    }

    // nothing to do here; everything is handled by base class AnimationBase
  };

  //------------------------------------------------------------------------------

  /** An Overlay that just fades out the current content of the LED strip.
   * Sometimes useful, e.g. for dimming the underlying Pattern in an AnimationScene.
   */
  class FadeOutOverlay
      : public Animation
  {
  public:
    /** Fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     * Change it only if the child class explicitly uses this as configuration option,
     * i.e. when it also offers a static \c fadeRate_default() method. \n
     * Not relevant in Overlay mode.
     */
    uint8_t fadeRate;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed; Lower value = longer glowing.
     */
    FadeOutOverlay(FastLedStrip ledStrip,
                   uint8_t fadeRate)
        : fadeRate(fadeRate), strip(ledStrip)
    {
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        strip.fadeToBlackBy(fadeRate);
      }
    }

  private:
    FastLedStrip strip;
  };

} // namespace EC
