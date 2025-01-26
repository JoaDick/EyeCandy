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
   * Can be used standalone, or as base Pattern in combination with other Overlays.
   */
  class BgFillColor
      : public PatternBase
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
        : PatternBase(ledStrip), color(color), _frametime(0)
    {
    }

    /** Constructor with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_FRAMETIME.
     * @param frametime  Delay (in ms) between updates of the LED strip.
     * @param ledStrip  The LED strip.
     * @param color  Fill LED strip with this color.
     */
    BgFillColor(uint8_t frametime,
                FastLedStrip ledStrip,
                CRGB color)
        : PatternBase(ledStrip), color(color), _frametime(frametime)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      strip.fill(color);
      return _frametime;
    }

  private:
    const uint8_t _frametime;
  };

  //------------------------------------------------------------------------------

  /** An Effect that fades out the current content of the LED strip.
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   * Useful e.g. for dimming the underlying Pattern in an AnimationScene.
   */
  class BgFadeToBlack
      : public PatternBase
  {
  public:
    /** Fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    uint8_t fadeRate;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    BgFadeToBlack(FastLedStrip ledStrip,
                  uint8_t fadeRate)
        : PatternBase(ledStrip), fadeRate(fadeRate), _frametime(0)
    {
    }

    /** Constructor for Pattern with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_FRAMETIME.
     * @param frametime  Delay (in ms) between updates of the LED strip.
     * @param ledStrip  The LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    BgFadeToBlack(uint8_t frametime,
                  FastLedStrip ledStrip,
                  uint8_t fadeRate)
        : PatternBase(ledStrip), fadeRate(fadeRate), _frametime(frametime)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      showDefaultPattern(strip, fadeRate);
      return _frametime;
    }

  private:
    const uint8_t _frametime;
  };

  //------------------------------------------------------------------------------

  // TODO: BgBlur

  //------------------------------------------------------------------------------

  /** An Effect that fades out the individual LEDs of the strip randomly (instead of all at once).
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   */
  class BgMeteorFadeToBlack
      : public PatternBase
  {
  public:
    /// Chance of fading a LED (0 = never, 255 = always).
    uint8_t fadeChance;

    /// Fading speed: Lower value = longer glowing.
    uint8_t fadeBy;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param fadeChance  Chance of fading a LED (0 = never, 255 = always).
     * @param fadeBy  Fading speed: Lower value = longer glowing.
     */
    BgMeteorFadeToBlack(FastLedStrip ledStrip,
                        uint8_t fadeChance = 32,
                        uint8_t fadeBy = 96)
        : PatternBase(ledStrip),
          fadeChance(fadeChance),
          fadeBy(fadeBy),
          _frametime(0)
    {
    }

    /** Constructor for Pattern with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_FRAMETIME.
     * @param frametime  Delay (in ms) between updates of the LED strip.
     * @param ledStrip  The LED strip.
     * @param fadeChance  Chance of fading a LED (0 = never, 255 = always).
     * @param fadeBy  Fading speed: Lower value = longer glowing.
     */
    BgMeteorFadeToBlack(uint8_t frametime,
                        FastLedStrip ledStrip,
                        uint8_t fadeChance = 32,
                        uint8_t fadeBy = 96)
        : PatternBase(ledStrip),
          fadeChance(fadeChance),
          fadeBy(fadeBy),
          _frametime(frametime)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      meteorFadeToBlack(strip, fadeChance, fadeBy);
      return _frametime;
    }

  private:
    const uint8_t _frametime;
  };

  //------------------------------------------------------------------------------

  /** An Effect that rotates the content of the LED strip up or down by one pixel.
   * Can be used as Overlay, or as base Pattern in combination with other Overlays.
   */
  class BgRotate
      : public PatternBase
  {
  public:
    /** Rotating distance & direction.
     * Posive values of \a distance rotate upward (from begin to end), negative values rotate
     * downward (from end to begin).
     */
    int16_t distance;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param distance  Rotating distance & direction: posive values rotate upward (from begin to
     *                  end), negative values rotate downward (from end to begin).
     */
    explicit BgRotate(FastLedStrip ledStrip,
                      int16_t distance = -1)
        : PatternBase(ledStrip),
          distance(distance),
          _frametime(0)
    {
    }

    /** Constructor for Pattern with a custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_FRAMETIME.
     * @param frametime  Delay (in ms) between updates of the LED strip.
     * @param ledStrip  The LED strip.
     * @param distance  Rotating distance & direction: posive values rotate upward (from begin to
     *                  end), negative values rotate downward (from end to begin).
     */
    BgRotate(uint8_t frametime,
             FastLedStrip ledStrip,
             int16_t distance = -1)
        : PatternBase(ledStrip),
          distance(distance),
          _frametime(frametime)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      strip.shift(distance);
      return _frametime;
    }

  private:
    const uint8_t _frametime;
  };

  //------------------------------------------------------------------------------

  /** A pseudo-Animation for determining a specific frametime.
   * Use this only if you have an urgent reason for a specific update rate other
   * than EC_DEFAULT_FRAMETIME.
   * It does _not_ manipulate the LED strip.
   */
  class TriggerPattern
      : public Animation
  {
  public:
    /** Constructor.
     * @param frametime  Delay (in ms) between updates of the LED strip.
     */
    explicit TriggerPattern(uint8_t frametime)
        : _frametime(frametime)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      return _frametime;
    }

  private:
    const uint8_t _frametime;
  };

  //------------------------------------------------------------------------------

} // namespace EC
