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
#include "Lightbulbs.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Emulation of a retro-style VU with 8 Lightbulbs.
  class LightbulbVU
      : public Animation
  {
  public:
    LightbulbArray lightbulbArray;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU.
     */
    LightbulbVU(FastLedStrip ledStrip,
                VuSource &vuSource)
        : lightbulbArray(ledStrip, 8, CRGB::Green), _vuSource(vuSource)
    {
      lightbulbArray.setColor(7, CRGB(255, 0, 0));
      lightbulbArray.setColor(6, CRGB(255, 255, 0));
      lightbulbArray.setColor(5, CRGB(255, 255, 0));
    }

    /// Get the VuSource that is used as input.
    VuSource &getInputVuSource() { return _vuSource; }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (!wasModified)
        return;

      float vuLevel = _vuSource.getVU();
      vuLevel *= 1.1; // to make the red bulb flash a bit more often
      uint16_t bulbPattern = 1 << uint8_t(vuLevel * 9);
      --bulbPattern;
      lightbulbArray.show(bulbPattern);
    }

  private:
    VuSource &_vuSource;
  };

} // namespace EC
