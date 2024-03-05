#pragma once
/*******************************************************************************

MIT License

Copyright (c) 2024 Joachim Dick

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

  //------------------------------------------------------------------------------

  /** A Pattern that renders all LEDs with the same color.
   * Mainly intended as example, but can also be used in combination with Overlays.
   */
  class BgFillColor
      : public AnimationBase
  {
  public:
    /// Fill LED strip with this color.
    CRGB color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param color  Fill LED strip with this color.
     */
    BgFillColor(FastLedStrip ledStrip,
                CRGB color)
        : AnimationBase(ledStrip, false), color(color)
    {
    }

    /** Constructor with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_UPDATE_PERIOD.
     * @param patternUpdatePeriod  Period (in ms) for calling showPattern().
     * @param ledStrip  The LED strip.
     * @param color  Fill LED strip with this color.
     */
    BgFillColor(uint8_t patternUpdatePeriod,
                FastLedStrip ledStrip,
                CRGB color)
        : AnimationBase(patternUpdatePeriod, ledStrip), color(color)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      strip.fill(color);
    }
  };

  //------------------------------------------------------------------------------

  /** An Effect that fades out the current content of the LED strip.
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   * Useful e.g. for dimming the underlying Pattern in an AnimationScene.
   */
  class BgFadeToBlack
      : public AnimationBase
  {
  public:
    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    BgFadeToBlack(FastLedStrip ledStrip,
                  bool overlayMode,
                  uint8_t fadeRate)
        : AnimationBase(ledStrip, overlayMode, fadeRate)
    {
    }

    /** Constructor for Pattern with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_UPDATE_PERIOD.
     * @param patternUpdatePeriod  Period (in ms) for calling showPattern().
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    BgFadeToBlack(uint8_t patternUpdatePeriod,
                  FastLedStrip ledStrip,
                  uint8_t fadeRate)
        : AnimationBase(patternUpdatePeriod, ledStrip, fadeRate)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      // must be empty
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      showDefaultPattern(strip, fadeRate);
    }
  };

  //------------------------------------------------------------------------------

  // TODO: BgBlur

  //------------------------------------------------------------------------------

  /** An Effect that fades out the individual LEDs of the strip randomly (instead of all at once).
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   */
  class BgMeteorFadeToBlack
      : public AnimationBase
  {
  public:
    /// Fading speed: Lower value = longer glowing.
    uint8_t fadeBy;

    /// Chance of fading a LED (0 = never, 255 = always).
    uint8_t fadeChance;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param fadeBy  Fading speed: Lower value = longer glowing.
     * @param fadeChance  Chance of fading a LED (0 = never, 255 = always).
     */
    BgMeteorFadeToBlack(FastLedStrip ledStrip,
                        bool overlayMode,
                        uint8_t fadeBy = 96,
                        uint8_t fadeChance = 32)
        : AnimationBase(ledStrip, overlayMode, 0), fadeBy(fadeBy), fadeChance(fadeChance)
    {
    }

    /** Constructor for Pattern with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_UPDATE_PERIOD.
     * @param patternUpdatePeriod  Period (in ms) for calling showPattern().
     * @param ledStrip  The LED strip.
     * @param fadeBy  Fading speed: Lower value = longer glowing.
     * @param fadeChance  Chance of fading a LED (0 = never, 255 = always).
     */
    BgMeteorFadeToBlack(uint8_t patternUpdatePeriod,
                        FastLedStrip ledStrip,
                        uint8_t fadeBy = 96,
                        uint8_t fadeChance = 32)
        : AnimationBase(patternUpdatePeriod, ledStrip), fadeBy(fadeBy), fadeChance(fadeChance)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      // must be empty
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      meteorFadeToBlack(strip, fadeBy, fadeChance);
    }
  };

  //------------------------------------------------------------------------------

  /** An Effect that rotates the content of the LED strip up or down by one pixel.
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   */
  class BgRotate
      : public AnimationBase
  {
  public:
    /** Rotating direction.
     * - \c true From begin to end
     * - \c false From end to begin
     */
    bool rotateUp;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param rotateUp  Rotating direction:
     *                  - \c true From begin to end
     *                  - \c false From end to begin
     */
    BgRotate(FastLedStrip ledStrip,
             bool overlayMode,
             bool rotateUp = false)
        : AnimationBase(ledStrip, overlayMode), rotateUp(rotateUp)
    {
    }

    /** Constructor for Pattern with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_UPDATE_PERIOD.
     * @param patternUpdatePeriod  Period (in ms) for calling showPattern().
     * @param ledStrip  The LED strip.
     * @param rotateUp  Rotating direction:
     *                  - \c true From begin to end
     *                  - \c false From end to begin
     */
    BgRotate(uint8_t patternUpdatePeriod,
             FastLedStrip ledStrip,
             bool rotateUp = false)
        : AnimationBase(patternUpdatePeriod, ledStrip), rotateUp(rotateUp)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      // must be empty
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      strip.shift(rotateUp);
    }
  };

  //------------------------------------------------------------------------------

  /** A pseudo-Pattern that just triggers subsequent Overlays.
   * It does _not_ manipulate the LED strip.
   */
  class TriggerPattern
      : public Animation
  {
  public:
    /** Constructor.
     * @param patternUpdatePeriod  Period (in ms) for triggering the Overlays.
     * Change this only if you have an urgent reason for a different update rate.
     */
    explicit TriggerPattern(uint8_t patternUpdatePeriod = EC_DEFAULT_UPDATE_PERIOD)
        : _patternUpdateTimer(patternUpdatePeriod)
    {
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      wasModified = _patternUpdateTimer.process(currentMillis);
    }

    AnimationTimer _patternUpdateTimer;
  };

  //------------------------------------------------------------------------------

} // namespace EC
