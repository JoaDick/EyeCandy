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
     */
    CRGB backgroundColor = backgroundColor_default();
    static CRGB backgroundColor_default() { return CRGB::Black; }

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param backgroundColor  Fill LED strip with this color.
     */
    StaticBackground(CRGB *ledStrip,
                     uint16_t ledCount,
                     const CRGB &backgroundColor = backgroundColor_default())
        : AnimationBaseFL(TYPE_SOLID_PATTERN, ledStrip, ledCount), backgroundColor(backgroundColor)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      fill_solid(ledStrip, ledCount, backgroundColor);
    }
  };

} // namespace EC
