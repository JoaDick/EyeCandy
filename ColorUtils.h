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

  /// Helper class for generating a rainbow color sequence.
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
    float _startHueF = 0.0;
  };

  //------------------------------------------------------------------------------

  /** Helper class for generating colors based on different sine waves for red, green and blue.
   * Supports 2 presets for convenience:
   * - Sync: Same BPM for all colors at 120° phase offset.
   * - Async: Individual BPM for every color.
   * @note All BPM and phase parameters must be provided in Q8.8 format with fractions of 1/256;
   * see http://fastled.io/docs/group___beat_generators.html#gaf917825ff26c8e1174c5b674a2f3c631
   */
  class ColorSineRGB
  {
  public:
    accum88 bpm_red;
    accum88 bpm_green;
    accum88 bpm_blue;

    accum88 phase_red;
    accum88 phase_green;
    accum88 phase_blue;

    /** Constructor.
     * @param syncMode  \c true = Preset with same BPM for all colors at 120° phase offset
     *                  \c false = Preset with individual BPM for every color
     */
    explicit ColorSineRGB(bool syncMode = true)
    {
      syncMode ? presetSync() : presetAsync();
    }

    /// Configure with the given \a bpm for all colors at 120° phase offset (sync mode).
    void presetSync(accum88 bpm = 4 * 256)
    {
      bpm_red = bpm;
      bpm_green = bpm;
      bpm_blue = bpm;
      phase_red = 0;
      phase_green = 85 * 256;
      phase_blue = 170 * 256;
    }

    /// Configure with the given individual bpm for every colors (async mode).
    void presetAsync(accum88 bpmRed = 15 * 256,
                     accum88 bpmGreen = 13 * 256,
                     accum88 bpmBlue = 11 * 256)
    {
      bpm_red = bpmRed;
      bpm_green = bpmGreen;
      bpm_blue = bpmBlue;
      phase_red = 0;
      phase_green = 0;
      phase_blue = 0;
    }

    /** Get the desired color.
     * @param offset  Dummy parameter for API compatibility with other color generators.
     */
    CRGB getColor(float offset = 0.0)
    {
      const uint8_t red = beatsin88(bpm_red, 0, 255, 0, phase_red);
      const uint8_t green = beatsin88(bpm_green, 0, 255, 0, phase_green);
      const uint8_t blue = beatsin88(bpm_blue, 0, 255, 0, phase_blue);
      return CRGB(red, green, blue);
    }

    /// Implicit conversion to CRGB, so it can be used directly as pixel color.
    operator CRGB() { return getColor(); }

    /// Obtain offset color also via index operator.
    CRGB operator[](float offset) { return getColor(offset); }

    /// Dummy method for API compatibility with other color generators.
    void update() {}
  };

  //------------------------------------------------------------------------------

  /** Helper class for generating fluently changing RGB colors based on random noise.
   * Rule of thumb for a nice effect: "Only change \a noiseDivider and \a bpmBase together, while
   * their sum should be ~16."
   */
  class ColorNoiseRGB
  {
  public:
    /** Determines the dynamic changing speed.
     * - 0 = not allowed
     * - 1 = very fast
     * - 9+ = slow
     */
    uint8_t noiseDivider;

    /// Determines the underlying color wheel cycle (in beats per minute).
    uint8_t bpmBase;

    /** Constructor.
     * @param noiseDivider  Determines the dynamic changing speed (1 ... ~15).
     * - 0 = not allowed
     * - 1 = very fast
     * - 9+ = slow
     * @param bpmBase  Determines the underlying color wheel cycle (in beats per minute).
     */
    explicit ColorNoiseRGB(uint8_t noiseDivider = 8,
                           uint8_t bpmBase = 8)
        : noiseDivider(noiseDivider), bpmBase(bpmBase)
    {
    }

    /// Configure very smooth color changes.
    void presetSmooth()
    {
      noiseDivider = 14;
      bpmBase = 2;
    }

    /// Configure twitchy color changes.
    void presetTwitchy()
    {
      noiseDivider = 2;
      bpmBase = 14;
    }

    /** Get the desired color.
     * @param offset  Dummy parameter for API compatibility with other color generators.
     */
    CRGB getColor(float offset = 0.0)
    {
      const uint32_t noiseX = millis() / noiseDivider;

      const uint8_t red = beatsin8(bpmBase, 0, 255, 0, inoise8(noiseX));
      const uint8_t green = beatsin8(bpmBase, 0, 255, 0, 85 + inoise8(noiseX + 10000));
      const uint8_t blue = beatsin8(bpmBase, 0, 255, 0, 170 + inoise8(noiseX + 20000));

      return CRGB(red, green, blue);
    }

    /// Implicit conversion to CRGB, so it can be used directly as pixel color.
    operator CRGB() { return getColor(); }

    /// Obtain offset color also via index operator.
    CRGB operator[](float offset) { return getColor(offset); }

    /// Dummy method for API compatibility with other color generators.
    void update() {}
  };

  //------------------------------------------------------------------------------

}
