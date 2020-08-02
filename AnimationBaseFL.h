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
     * Not implemented by all Animations.
     */
    bool mirrored = false;

    /** Fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     * This setting can be adjusted at runtime.
     * Only relevant if the child class explicitly uses this as configuration
     * option, i.e. when it also offers a static \c fadeRate_default() method.
     * Ignored in Overlay mode.
     */
    uint8_t fadeRate;

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

    /** Draw a line in the given \a color from \a firstPixel to \a lastPixel.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * Both \a firstPixel and \a lastPixel are included.
     */
    void lineAbs(int16_t firstPixel, int16_t lastPixel, const CRGB &color)
    {
      if (firstPixel > lastPixel)
      {
        lineAbs(lastPixel, firstPixel, color);
        return;
      }
      if (firstPixel < 0)
      {
        // start AND end off the strip?
        if (lastPixel < 0)
        {
          return;
        }
        firstPixel = 0;
      }
      if (lastPixel >= ledCount)
      {
        // start AND end off the strip?
        if (firstPixel >= ledCount)
        {
          return;
        }
        lastPixel = ledCount - 1;
      }
      if (mirrored)
      {
        int16_t temp = lastPixel;
        lastPixel = ledCount - 1 - firstPixel;
        firstPixel = ledCount - 1 - temp;
      }
      while (firstPixel <= lastPixel)
      {
        ledStrip[firstPixel++] = color;
      }
    }

    /** Draw a line in the given \a color with the given \a length, starting at \a firstPixel.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     */
    void lineRel(int16_t firstPixel, int16_t length, const CRGB &color)
    {
      if (length)
      {
        lineAbs(firstPixel, firstPixel + length, color);
      }
    }

  protected:
    /** Constructor.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param fadeRate  Fading speed:
     *                  Lower value = longer glowing; 0 = black background.
     *                  Only relevant when default implementation of
     *                  showPattern() is used.
     */
    AnimationBaseFL(bool overlayMode,
                    CRGB *ledStrip,
                    uint16_t ledCount,
                    uint8_t fadeRate = 0)
        : AnimationBase(overlayMode), ledStrip(ledStrip), fadeRate(fadeRate), ledCount(ledCount), maxLedCount(ledCount), _ledStrip(ledStrip)
    {
    }

    /** Animation shall be mirrored by default.
     * Call this method from child class' constructor when the Animation shall
     * be rendered from top to bottom.
     */
    void mirroredByDefault()
    {
      _defaultMirrored = true;
    }

    /** Default implementation of AnimationBase::showPattern()
     * Provides the oftentimes used fading background or black background.
     */
    void showPattern(uint32_t currentMillis) override
    {
      if (fadeRate)
      {
        fadeToBlackBy(ledStrip, ledCount, fadeRate);
      }
      else
      {
        fill_solid(ledStrip, ledCount, CRGB::Black);
      }
      showOverlay(currentMillis);
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
    bool _defaultMirrored = false;
    CRGB *_ledStrip;
  };

} // namespace EC
