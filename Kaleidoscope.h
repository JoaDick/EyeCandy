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

  /** An Overlay that multiplies a part of the LED strip like a Kaleidoscope.
   */
  class Kaleidoscope
      : public AnimationBaseFL2
  {
  public:
    /// Deprecated; only for legacy compatibility.
    Kaleidoscope(CRGB *ledStrip,
                  uint16_t ledCount,
                  bool mirrored = true)
        : Kaleidoscope(FastLedStrip(ledStrip, ledCount), mirrored)
    {
    }

    /** Constructor
     * @param ledStrip  The LED strip.
     */
    explicit Kaleidoscope(FastLedStrip ledStrip,
                           bool mirrored = true)
        : AnimationBaseFL2(ledStrip, true), m_mirrored(mirrored)
    {
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      strip.copyUp(m_mirrored);
    }

    bool m_mirrored;
  };

} // namespace EC
