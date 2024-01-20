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

#include "VuBaseFL.h"
#include "VuPeakGravityHandler.h"
#include "VuRangeExtender.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// VU meter with its color depending on the current VU level.
  class RainbowLevelVU
      : public VuBaseFL
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

    /** Render the VU bar.
     * This setting can be adjusted at runtime.
     */
    bool enableVuBar = true;

    /** Render the peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enablePeakDot = true;

    /** Configure the following properties according to your needs:
     * - VuPeakGravityHandler::a0
     * - VuPeakGravityHandler::v0
     * - Don't call any of its methods!
     */
    VuPeakGravityHandler vuPeakHandler;

    /// Usually there's nothing to configure here; only for debugging.
    VuRangeExtender vuRangeExtender;

    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    RainbowLevelVU(float &audioSource,
                   FastLedStrip ledStrip,
                   bool overlayMode)
        : VuBaseFL(ledStrip, overlayMode, fadeRate_default(), audioSource)
    {
      modelUpdatePeriod = 10;
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      const float vuLevel = vuRangeExtender.getVU();
      const float peakLevel = vuPeakHandler.getVU();
      const uint8_t hue = _startHue + vuLevel * vuHueRange * 255;

      if (enableVuBar)
      {
        const CRGB vuBarColor = CHSV(redShift(hue), 255, volume);
        strip.normLineRel(0.0, vuLevel, vuBarColor);
      }
      if (enablePeakDot)
      {
        const CRGB peakDotColor = CHSV(redShift(hue + 128), 255, volume);
        strip.optPixel(peakLevel) = peakDotColor;
      }
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      const float vuLevel = vuRangeExtender.process(vuLevelHandler.capture());
      vuPeakHandler.process(vuLevel, currentMillis);

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
    float _startHue = random8();
  };

} // namespace EC
