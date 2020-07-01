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

    /** Show the Animation in reverse direction.
     * This setting can be adjusted at runtime.
     */
    bool mirrored = false;

    /** Make the Animation smaller.
     * \a newLedCount must not be higher than #maxLedCount.
     * This setting can be adjusted at runtime.
     */
    void setLedCount(uint16_t newLedCount)
    {
      if (newLedCount > maxLedCount)
      {
        alarmFlash(2);
      }
      else
      {
        ledCount = newLedCount;
      }
    }

    /** Render the Animation somewhere in the middle of the LED strip.
     * \a offset + \a newLedCount must not be higher than #maxLedCount.
     * This setting can be adjusted at runtime.
     */
    void setLedOffset(uint16_t offset,
                      uint16_t newLedCount)
    {
      if (offset + newLedCount > maxLedCount)
      {
        alarmFlash(3);
      }
      else
      {
        ledStrip = &_ledStrip[offset];
        ledCount = newLedCount;
      }
    }

    /** Physical number of LEDs of the LED strip.
     * Use this one only if you really know what you are doing!
     * Usually, #ledCount is the better choice to use.
     */
    const uint16_t maxLedCount;

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
     * @param mirrored  The "regular" Animation shall be shown in reverse direction.
     */
    AnimationBase_FL(Type animationType,
                     CRGB *ledStrip,
                     uint16_t ledCount,
                     bool mirrored = false)
        : AnimationBase(animationType), ledStrip(ledStrip), ledCount(ledCount), maxLedCount(ledCount), _defaultMirrored(mirrored), _ledStrip(ledStrip)
    {
    }

    /// Access the pixel at position \a index.
    CRGB &pixel(uint16_t index)
    {
      if (index >= ledCount)
      {
        alarmFlash(1);
        return ledStrip[0];
      }
      const uint16_t transformedIndex = (mirrored ^ _defaultMirrored) ? ledCount - 1 - index : index;
      return ledStrip[transformedIndex];
    }

  private:
    void alarmFlash(uint8_t count)
    {
      while (count--)
      {
        fill_solid(ledStrip, ledCount, CRGB(64, 0, 0));
        FastLED.show();
        delay(250);
        fill_solid(ledStrip, ledCount, CRGB::Black);
        FastLED.show();
        delay(250);
      }
      delay(500);
    }

  private:
    const bool _defaultMirrored;
    CRGB *_ledStrip;
  };

} // namespace EC
