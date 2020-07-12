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
      : public AnimationBaseFL
  {
    uint16_t _remainLedCount;

  public:
    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    Kaleidoscope(CRGB *ledStrip,
                 uint16_t ledCount)
        : AnimationBaseFL(TYPE_OVERLAY, ledStrip, ledCount), _remainLedCount((ledCount + 1) / 2)
    {
      // we want the mirror effect by default
      AnimationBaseFL::mirrored = true;
    }

    /** Get the number of LEDs that may be used by the underlying Animation.
     * Use other's #setLedCount() method to store this value.
     */
    uint16_t remainLedCount()
    {
      return _remainLedCount;
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      copyPixel(ledStrip, ledStrip + ledCount / 2, _remainLedCount, mirrored);
    }

    void copyPixel(CRGB *from, CRGB *to, uint16_t count, bool flip)
    {
      if (flip)
      {
        CRGB *src = from;
        CRGB *dst = to + count - 1;
        while (count--)
        {
          *(dst--) = *(src++);
        }
      }
      else
      {
        CRGB *src = from;
        CRGB *dst = to;
        while (count--)
        {
          *(dst++) = *(src++);
        }
      }
    }
  };

} // namespace EC
