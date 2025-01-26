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
#include "ColorUtils.h"

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  /** Update another Animation's color in a rainbow sequence.
   * This helper Animation manipulates another Animation's (or any other class') public member
   * variable of type CRGB.
   * @note Uses the ColorWheel helper class internally; see there for more details.
   */
  class ColorChangerRainbow
      : public Animation
  {
  public:
    /// Color source for the other Animation.
    ColorWheel colorSource;

    /** Constructor.
     * @param targetColor  Other Animation's color property to manipulate.
     * @param bpm  Beats per minute of the color cycle.
     * @param hueOffset  Initial hue value.
     * @param saturation  Color saturation.
     * @param volume  Brightness.
     * @param moreRed  When \c true put more emphasis on the red'ish colors.
     */
    explicit ColorChangerRainbow(CRGB &targetColor,
                                 float bpm = 1.0,
                                 uint8_t hueOffset = 0,
                                 uint8_t saturation = 255,
                                 uint8_t volume = 255,
                                 bool moreRed = true)
        : colorSource(bpm), _targetColor(targetColor)
    {
      colorSource.hueOffset = hueOffset;
      colorSource.saturation = saturation;
      colorSource.volume = volume;
      colorSource.moreRed = moreRed;
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      colorSource.update();
      _targetColor = colorSource.getColor();
      return 0;
    }

  private:
    CRGB &_targetColor;
  };

  //------------------------------------------------------------------------------

  /** Update another Animation's color based on different sine waves for red, green and blue.
   * This helper Animation manipulates another Animation's (or any other class') public member
   * variable of type CRGB.
   * @note Uses the ColorSineRGB helper class internally; see there for more details.
   */
  class ColorChangerSineRGB
      : public Animation
  {
  public:
    /// Color source for the other Animation.
    ColorSineRGB colorSource;

    /** Constructor.
     * @param targetColor  Other Animation's color property to manipulate.
     * @param syncMode  \c true = Preset with same BPM for all colors at 120° phase offset
     *                  \c false = Preset with individual BPM for every color
     */
    explicit ColorChangerSineRGB(CRGB &targetColor,
                                 bool syncMode = true)
        : colorSource(syncMode), _targetColor(targetColor)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      _targetColor = colorSource.getColor();
      return 0;
    }

  private:
    CRGB &_targetColor;
  };

  //------------------------------------------------------------------------------

  /** Update another Animation's color based on fluently changing random RGB colors.
   * This helper Animation manipulates another Animation's (or any other class') public member
   * variable of type CRGB.
   * @note Uses the ColorNoiseRGB helper class internally; see there for more details.
   * Rule of thumb for a nice effect: "Only change \a noiseDivider and \a bpmBase together, while
   * their sum should be ~16."
   */
  class ColorChangerNoiseRGB
      : public Animation
  {
  public:
    /// Color source for the other Animation.
    ColorNoiseRGB colorSource;

    /** Constructor.
     * @param targetColor  Other Animation's color property to manipulate.
     */
    ColorChangerNoiseRGB(CRGB &targetColor)
        : colorSource(), _targetColor(targetColor)
    {
    }

    /** Constructor with custom color change settings.
     * @param targetColor  Other Animation's color property to manipulate.
     * @param noiseDivider  Determines the dynamic changing speed (1 ... ~15).
     * - 0 = not allowed
     * - 1 = very fast
     * - 9+ = slow
     * @param bpmBase  Determines the underlying color wheel cycle (in beats per minute).
     */
    explicit ColorChangerNoiseRGB(CRGB &targetColor,
                                  uint8_t noiseDivider,
                                  uint8_t bpmBase)
        : colorSource(noiseDivider, bpmBase), _targetColor(targetColor)
    {
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      _targetColor = colorSource.getColor();
      return 0;
    }

  private:
    CRGB &_targetColor;
  };

  //------------------------------------------------------------------------------

} // namespace EC
