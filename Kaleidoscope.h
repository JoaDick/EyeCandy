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

#include "Animation.h"
#include "FastLedStrip.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** An Overlay that duplicates the lower part of the LED strip like a Kaleidoscope.
   * Because the content is duplicated, the main Animation needs to draw only
   * in the lower half of the LED strip.
   * @see FastLedStrip::getHalfStrip()
   */
  class Kaleidoscope
      : public Animation
  {
  public:
    /** Determine how the lower part of the strip is duplicated into the upper part.
     * false = straight copying, true = mirror the content \n
     * This setting can be adjusted at runtime.
     */
    bool mirrored;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param mirrored  false = straight copying, true = mirror the content
     */
    explicit Kaleidoscope(FastLedStrip ledStrip,
                          bool mirrored = true)
        : mirrored(mirrored), _strip(ledStrip)
    {
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        _strip.copyUp(mirrored);
      }
    }

  private:
    FastLedStrip _strip;
  };

} // namespace EC
