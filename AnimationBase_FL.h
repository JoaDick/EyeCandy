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

//------------------------------------------------------------------------------

namespace EC
{

  /** Simple base class suitable for most Animations which are using FastLED.
   * Child classes must fulfill the same requirements as for #AnimationBase.
   */
  class AnimationBase_FL
      : public AnimationBase
  {
  public:
    /// The LED strip.
    CRGB *ledStrip;

    /// Number of LEDs.
    uint16_t ledCount;

    /// Put more emphasis on the red'ish colors.
    static uint8_t redShift(uint8_t hue)
    {
      return cos8(128 + hue / 2);
    }

  protected:
    /** Constructor.
     * @param animationType  Type of Animation.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    AnimationBase_FL(Type animationType,
                     CRGB *ledStrip,
                     uint16_t ledCount)
        : AnimationBase(animationType), ledStrip(ledStrip), ledCount(ledCount)
    {
    }
  };

} // namespace EC
