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

  /** Helper Animation for manipulating any other Animation's color property.
   * If an Animation has a public color property of type CRGB, that can be manipuled in a rainbow
   * cycle through this helper.
   */
  class ColorChangerRainbow
      : public Animation
  {
  public:
    /// Color source for other Animation.
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
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        colorSource.update();
        _targetColor = colorSource;
      }
    }

  private:
    CRGB &_targetColor;
  };

} // namespace EC
