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
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Similar to VuOverlayLine, but the color is also derived from a VuSource.
   * A VU Overlay that draws a line from the first LED to the given VU position.
   * @note The actual VU value is obtained from the given VuSource; a VU value of 0.0 represents
   * the beginning of the LED strip, 1.0 represents the end.
   */
  class VuOverlayRainbowLine
      : public Animation
  {
  public:
    /// Color source of the VU.
    ColorWheel color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU level and color.
     */
    VuOverlayRainbowLine(FastLedStrip ledStrip, VuSource &vuSource)
        : VuOverlayRainbowLine(ledStrip, vuSource, vuSource)
    {
    }

    /** Constructor with different input for calculating the VU color.
     * @param ledStrip  The LED strip.
     * @param vuLevelSource  Input for calculating the VU level.
     * @param vuColorSource  Input for calculating the VU color.
     */
    VuOverlayRainbowLine(FastLedStrip ledStrip,
                         VuSource &vuLevelSource,
                         VuSource &vuColorSource)
        : color(1.0, 0.33), _strip(ledStrip),
          _vuLevelSource(vuLevelSource), _vuCcolorSource(vuColorSource)
    {
      color.volume = 128;
    }

    /// Get the VuSource that is used for calculating the VU level.
    VuSource &getLevelVuSource() { return _vuLevelSource; }

    /// Get the VuSource that is used for calculating the VU color.
    VuSource &getColorVuSource() { return _vuCcolorSource; }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (!wasModified)
        return;

      color.update();
      const float vuLevel = _vuLevelSource.getVU();
      const float colorVuLevel = _vuCcolorSource.getVU();

      _strip.n_lineRel(0.0, vuLevel, color[colorVuLevel]);
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuLevelSource;
    VuSource &_vuCcolorSource;
  };

} // namespace EC
