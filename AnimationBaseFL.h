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
  class AnimationBaseFL
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
    bool mirrored = mirrored_default();
    static bool mirrored_default() { return false; }

    /** Resize the Animation and move it on the LED strip (optional).
     * \a newLedCount + \a offset must not be higher than #maxLedCount.
     * This setting can be adjusted at runtime.
     */
    void resizeStrip(uint16_t newLedCount,
                     uint16_t offset = 0)
    {
      if (newLedCount + offset > maxLedCount)
      {
        alarmFlash(2);
      }
      else
      {
        ledStrip = _ledStrip + offset;
        ledCount = newLedCount;
      }
    }

    /** Physical number of LEDs of the LED strip.
     * Use this one only if you really know what you are doing!
     * Usually, #ledCount is the better choice to use.
     */
    const uint16_t maxLedCount;

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

    /** Access the pixel at position \a index (with error feeback).
     * If \a index is not within the LED strip, all LEDs will flash red as
     * error feedback. Use safePixel() when such "off-strip-pixels" are intended
     * by the Animation.
     */
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

    /** Access the pixel at position \a index ("off-strip-pixels" are allowed).
     * If \a index is not within the LED strip, the pixel's setting is ignored.
     * May be useful for Animations where something "drops off" the strip.
     * @note Don't abuse this feature to sweep programming errors under the rug!
     */
    CRGB &safePixel(int16_t index)
    {
      static CRGB trash;
      if (index < 0 || index >= ledCount)
      {
        return trash;
      }
      const uint16_t transformedIndex = (mirrored ^ _defaultMirrored) ? ledCount - 1 - index : index;
      return ledStrip[transformedIndex];
    }

  protected:
    /** Constructor.
     * @param animationType  Type of Animation.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param mirrored  The "regular" Animation shall be shown in reverse direction.
     */
    AnimationBaseFL(Type animationType,
                    CRGB *ledStrip,
                    uint16_t ledCount,
                    bool mirrored = false)
        : AnimationBase(animationType), ledStrip(ledStrip), ledCount(ledCount), maxLedCount(ledCount), _defaultMirrored(mirrored), _ledStrip(ledStrip)
    {
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