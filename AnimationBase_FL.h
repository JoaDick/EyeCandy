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
    /// Put more emphasis on the red'ish colors.
    static uint8_t redShift(uint8_t hue)
    {
      return cos8(128 + hue / 2);
    }

  protected:
    /** The LED strip.
     * Whenever feasible, child classes shall use the pixel() method for
     * rendering the Animation instead of manipulating the LED strip directly.
     * By following this recommendation, Animations can be mirrored without
     * any effort.
     */
    CRGB *ledStrip;

    /** Number of LEDs.
     * Child classes shall always use this variable instead of storing that
     * value in their own context.
     */
    uint16_t ledCount;

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

    /// Access the pixel at position \a index.
    CRGB &pixel(uint16_t index)
    {
      return ledStrip[index];
    }

    /** Access the pixel at position \a index.
     * When the Animation is rendered in reverse direction, \a mirrored must be true.
     */
    CRGB &pixel(uint16_t index, bool mirrored)
    {
      const uint16_t transformedIndex = mirrored ? ledCount - 1 - index : index;
      return ledStrip[transformedIndex];
    }
  };

} // namespace EC
