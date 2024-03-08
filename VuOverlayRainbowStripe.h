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

  /** Similar to VuOverlayStripe, but the color is also derived from a VuSource.
   * A VU Overlay that draws a line from the given VU position to the last VU position.
   * @note The actual VU value is obtained from the given VuSource; a VU value of 0.0 represents
   * the beginning of the LED strip, 1.0 represents the end.
   */
  class VuOverlayRainbowStripe
      : public AnimationBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 50; }

    /// How fast the initial hue changes over time.
    float baseHueStep = -0.05;

    /** How much the hue varies depending on the VU level.
     * 1.0 means one full color wheel cycle between 0.0 ... 1.0 VU level.
     */
    float vuHueRange = 0.33;

    /** Brightness of the VU.
     * This setting can be adjusted at runtime.
     */
    uint8_t volume = 128;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU level and color.
     */
    VuOverlayRainbowStripe(FastLedStrip ledStrip, VuSource &vuSource)
        : VuOverlayRainbowStripe(ledStrip, vuSource, vuSource)
    {
    }

    /** Constructor with different input for calculating the VU color.
     * @param ledStrip  The LED strip.
     * @param vuLevelSource  Input for calculating the VU level.
     * @param vuColorSource  Input for calculating the VU color.
     */
    VuOverlayRainbowStripe(FastLedStrip ledStrip, VuSource &vuLevelSource, VuSource &vuColorSource)
        : AnimationBase(ledStrip, true), _vuLevelSource(vuLevelSource), _vuCcolorSource(vuColorSource)
    {
      setModelUpdatePeriod(10);
    }

    /// Set the initial color of this VU.
    void setStartHue(uint8_t startHue)
    {
      _startHue = startHue;
    }

    /// Get the VuSource that is used for calculating the VU level.
    VuSource &getLevelVuSource() { return _vuLevelSource; }

    /// Get the VuSource that is used for calculating the VU color.
    VuSource &getColorVuSource() { return _vuCcolorSource; }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      const uint8_t hue = _startHue + _vuCcolorSource.getVU() * vuHueRange * 255;
      const CRGB color = CHSV(redShift(hue), 255, volume);
      const float vuLevel = _vuLevelSource.getVU();
      if (_lastVuLevel <= 0.0 || _lastVuLevel >= 1.0)
      {
        _lastVuLevel = vuLevel;
      }
      strip.n_lineRel(vuLevel, _lastVuLevel - vuLevel, color);
      _lastVuLevel = vuLevel;
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      _startHue += baseHueStep;
      while (_startHue > 255.0)
      {
        _startHue -= 255.0;
      }
      while (_startHue < 0.0)
      {
        _startHue += 255.0;
      }
    }

  private:
    VuSource &_vuLevelSource;
    VuSource &_vuCcolorSource;
    float _startHue = millis() & 0xFF;
    float _lastVuLevel = 0.0;
  };

} // namespace EC
