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

  /** Many many Rainbow variations.
   */
  class Rainbow
      : public AnimationBaseFL
  {
    uint8_t _hue = 0;

  public:
    /** "Stretch" of the rainbow pattern.
     * 0 means all LEDs have the same color.
     * The higher the value, the more of the rainbow(s) are shown.
     * Values up to ~25 look fine.
     * This setting can be adjusted at runtime.
     */
    uint8_t deltahue = deltahue_default();
    static uint8_t deltahue_default() { return 4; }

    /** Delay between updating the Animation (in ms).
     * 0 means freeze (don't update the animation).
     * This setting can be adjusted at runtime.
     * @note This delay influences the "Animation speed", but not the LED
     * refresh rate.
     */
    static uint16_t modelUpdatePeriod_default() { return 35; }

    /** Brightness of the rainbow.
     * This setting can be adjusted at runtime.
     */
    uint8_t volume = volume_default();
    static uint8_t volume_default() { return 255; }

    /** Put more emphasis on the red'ish colors when true.
     * This setting can be adjusted at runtime.
     */
    bool moreRed = moreRed_default();
    static bool moreRed_default() { return true; }

    /** Constructor.
     * @param ledStrip  The LED strip.
     */
    explicit Rainbow(FastLedStrip ledStrip)
        : AnimationBaseFL(ledStrip.getReversedStrip(), false)
    {
      modelUpdatePeriod = modelUpdatePeriod_default();
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      for (auto i = 0; i < strip.ledCount(); i++)
      {
        uint8_t pixelHue = _hue + i * deltahue;
        if (moreRed)
        {
          pixelHue = redShift(pixelHue);
        }
        strip[i] = CHSV(pixelHue, 255, volume);
      }
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      ++_hue;
    }
  };

} // namespace EC
