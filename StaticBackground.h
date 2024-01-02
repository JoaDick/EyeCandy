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

#include "AnimationBaseFL.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A Pattern that renders all LEDs with the same color.
   * Mainly intended as example, but can also be used in combination with Overlays.
   */
  class StaticBackground
      : public AnimationBaseFL
  {
  public:
    /** Fill LED strip with this color.
     * This setting can be adjusted at runtime.
     * @note It is overwritten when a #colorGenerator is provided.
     */
    CRGB color;

    /// Get LED strip color from there (optional).
    ColorGenerator *colorGenerator = nullptr;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param color  Fill LED strip with this color.
     */
    StaticBackground(CRGB *ledStrip,
                     uint16_t ledCount,
                     const CRGB &color)
        : AnimationBaseFL(false, ledStrip, ledCount), color(color)
    {
    }

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param colorGenerator  Get color for the LED strip from there.
     */
    StaticBackground(CRGB *ledStrip,
                     uint16_t ledCount,
                     ColorGenerator &colorGenerator)
        : AnimationBaseFL(false, ledStrip, ledCount), colorGenerator(&colorGenerator)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      if (colorGenerator)
      {
        color = colorGenerator->generateColor();
      }
      fill_solid(ledStrip, ledCount, color);
    }
  };

} // namespace EC
