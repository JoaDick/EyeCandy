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

#include <FastLED.h>
#include "MathUtils.h"

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  /// Put more emphasis on the red'ish colors.
  inline uint8_t redShift(uint8_t hue)
  {
    return cos8(128 + hue / 2);
  }

  //------------------------------------------------------------------------------

  /// Helper class for generating rainbow colors.
  class ColorWheel
  {
  public:
    /** Beats per minute of the color cycle.
     * Negative values mean "wheel turning in the other direction".
     * 0 means fixed at \a hueOffset (i.e. no color cycle).
     */
    float bpm;

    /** How much the hue can vary depending on getColor()'s \a offset parameter.
     * Setting this e.g. to 0.5 means half a color wheel cycle when \a offset is 1.0
     */
    float hueRange;

    /// Initial hue value.
    uint8_t hueOffset = 0;

    /// Brightness.
    uint8_t volume = 255;

    /// Color saturation.
    uint8_t saturation = 255;

    /// When \c true put more emphasis on the red'ish colors.
    bool moreRed = true;

    /** Constructor.
     * @param bpm  Beats per minute of the color cycle.
     * @param hueRange  How much the hue can vary depending on getColor()'s \a offset parameter.
     */
    explicit ColorWheel(float bpm = 1.0,
                        float hueRange = 0.33)
        : bpm(bpm), hueRange(hueRange)
    {
    }

    /// Call this method once before updating the LED strip.
    void update()
    {
      if (bpm > 0.0)
      {
        _startHueF = 1.0 - beatF(bpm, 0.0, 1.0);
      }
      else if (bpm < 0.0)
      {
        _startHueF = beatF(-bpm, 0.0, 1.0);
      }
    }

    /** Get the desired color.
     * @param offset  Offset of the color wheel
     *                1.0 means one full color cycle, scaled by \a hueRange
     * @see hueRange
     */
    CRGB getColor(float offset = 0.0)
    {
      const float hueF = _startHueF + (offset * hueRange);
      uint8_t hue = hueF * 256;
      hue += hueOffset;
      if (moreRed)
      {
        hue = redShift(hue);
      }
      return CHSV(hue, saturation, volume);
    }

    /// Implicit conversion to CRGB, so it can be used directly as pixel color.
    operator CRGB() { return getColor(); }

    /// Obtain offset color also via index operator.
    CRGB operator[](float offset) { return getColor(offset); }

  private:
    float _startHueF = 0;
  };

  //------------------------------------------------------------------------------

}
