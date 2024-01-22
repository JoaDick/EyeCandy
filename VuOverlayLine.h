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

  /** A VU Overlay that draws a line from the first LED to the given VU position.
   * @note The actual VU value is obtained from the given VuSource; a VU value of 0.0 represents
   * the beginning of the LED strip, 1.0 represents the end.
   */
  class VuOverlayLine
      : public Animation
  {
  public:
    /** Draw the VU Overlay with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color = CRGB(0, 64, 8);

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU Overlay.
     */
    VuOverlayLine(FastLedStrip ledStrip, VuSource &vuSource)
        : _strip(ledStrip), _vuSource(vuSource)
    {
    }

    /// Get the VuSource that is used as input.
    VuSource &getVuSource()
    {
      return _vuSource;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        _strip.normLineRel(0.0, _vuSource.getVU(), color);
      }
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuSource;
  };

} // namespace EC
