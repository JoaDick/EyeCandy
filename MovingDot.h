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

#include "AnimationBase.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A dot moving forth and back again.
   * Example for an Animation that can be used as Pattern and as Overlay.
   */
  class MovingDot
      : public AnimationBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 0; }

    /** Draw the dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color;

    /** Delay between moving the dot by 1 pixel (in ms).
     * 0 makes the dot disappear.
     * This setting can be adjusted at runtime.
     * @note This delay influences the "Animation speed", but not the LED
     * refresh rate.
     */
    static uint16_t modelUpdatePeriod_default() { return 20; }

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param color  Draw the dot with this color.
     */
    MovingDot(FastLedStrip ledStrip,
              bool overlayMode,
              CRGB color = CRGB::Red)
        : AnimationBase(ledStrip, overlayMode, fadeRate_default()), color(color)
    {
      setModelUpdatePeriod(modelUpdatePeriod_default());
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (getModelUpdatePeriod())
      {
        strip[_position] = color;
      }
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      if (_rising)
      {
        if (_position < strip.ledCount() - 1)
          ++_position;
        else
          _rising = false;
      }
      else
      {
        if (_position > 0)
          --_position;
        else
          _rising = true;
      }
    }

  private:
    int16_t _position = 0;
    bool _rising = false;
  };

} // namespace EC
