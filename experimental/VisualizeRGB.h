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

  /// A test Pattern that visualizes the RGB portions of its \a color property.
  class VisualizeRGB
      : public AnimationModelBase
  {
  public:
    /** Color to visualize.
     * To be manipulated by a ColorChanger or inside updateModel()
     */
    CRGB color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param color  Initial color.
     */
    explicit VisualizeRGB(FastLedStrip ledStrip,
                          CRGB color = CRGB(64, 64, 64))
        : AnimationModelBase(10, ledStrip, false),
          color(color)
    {
    }

  private:
    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      // update color property here (when not done through a ColorChanger)
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      const uint8_t minCol = 6;

      visualizeColor(1 / 8.0, 255, CRGB(minCol, 0, 0));
      visualizeColor(3 / 8.0, 255, CRGB(0, minCol, 0));
      visualizeColor(5 / 8.0, 255, CRGB(0, 0, minCol));

      visualizeColor(1 / 8.0, color.r, CRGB(255, 0, 0));
      visualizeColor(3 / 8.0, color.g, CRGB(0, 255, 0));
      visualizeColor(5 / 8.0, color.b, CRGB(0, 0, 255));

      visualizeColor(7 / 8.0, 255, color);
    }

    void visualizeColor(float pos, uint8_t value, CRGB col, float size = 0.2)
    {
      strip.n_lineCentered(pos, size * value / 255.0, col);
    }
  };

} // namespace EC
