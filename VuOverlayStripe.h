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

  /** A VU Overlay that draws a line from the given VU position to the last VU position.
   * @note The actual VU value is obtained from the given VuSource; a VU value of 0.0 represents
   * the beginning of the LED strip, 1.0 represents the end.
   */
  class VuOverlayStripe
      : public Animation
  {
  public:
    /** Draw the VU Overlay with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU Overlay.
     * @param color  Draw the strip with this color.
     */
    VuOverlayStripe(FastLedStrip ledStrip,
                    VuSource &vuSource,
                    CRGB color = CRGB(0, 64, 0))
        : color(color), _strip(ledStrip),
          _vuSource(vuSource)
    {
    }

    /// Get the VuSource that is used as input.
    VuSource &getInputVuSource() { return _vuSource; }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (!wasModified)
        return;

      const float vuLevel = _vuSource.getVU();
      if (_lastVuLevel <= 0.0 || _lastVuLevel >= 1.0)
      {
        _lastVuLevel = vuLevel;
      }
      _strip.n_lineRel(vuLevel, _lastVuLevel - vuLevel, color);
      _lastVuLevel = vuLevel;
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuSource;
    float _lastVuLevel = 0.0;
  };

} // namespace EC
