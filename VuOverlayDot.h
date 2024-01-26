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

#include "Animation.h"
#include "FastLedStrip.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A VU Overlay that draws a dot at the given VU position.
   * A VU value of 0.0 will hide the dot.
   * @note The actual VU value is obtained from the given VuSource; a VU value of 0.0 represents
   * the beginning of the LED strip, 1.0 represents the end.
   */
  class VuOverlayDot
      : public Animation
  {
  public:
    /** Draw the VU Overlay with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color;

    /** Size of the dot (as fraction of the entire strip).
     * This setting can be adjusted at runtime.
     * Choose small values, like e.g. 0.03 for 3% of the strip. \n
     * 0.0 means exactly 1 pixel.
     */
    float size;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU Overlay.
     * @param color  Draw the dot with this color.
     * @param size  Size of the dot (as fraction of the entire strip). \n
     *              Choose small values, like e.g. 0.03 for 3% of the strip. \n
     *              0.0 means exactly 1 pixel.
     */
    VuOverlayDot(FastLedStrip ledStrip, VuSource &vuSource, CRGB color = CRGB::Red, float size = 0.0)
        : color(color), size(size), _strip(ledStrip), _vuSource(vuSource)
    {
    }

    /// Get the VuSource that is used as input.
    VuSource &getInputVuSource()
    {
      return _vuSource;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        const float vuLevel = _vuSource.getVU();
        if (vuLevel > 0.0)
        {
          if (size > 0.0)
          {
            _strip.normLineRel(vuLevel, -size, color);
          }
          else
          {
            _strip.normPixel(vuLevel) = color;
          }
        }
      }
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuSource;
  };

} // namespace EC
