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
#include "VuPeakHandler.h"
#include "VuRangeExtender.h"

//------------------------------------------------------------------------------

// #define PEAK_GLITTER_VU_DEBUG

//------------------------------------------------------------------------------

namespace EC
{

  /** VU meter that makes glitter at the peaks.
   * Recommended to use as Overlay together with another VU meter.
   */
  class PeakGlitterVU
      : public VuBaseFL
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 50; }

    /** Draw the glitter with this color.
     * This setting can be adjusted at runtime.
     * @note It is overwritten when a #colorGenerator is provided.
     */
    CRGB color = CRGB(255, 255, 255);

    /// Get glitter color from there (optional).
    ColorGenerator *colorGenerator = nullptr;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuRangeExtender vuRangeExtender;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param audioSource  Read the audio samples from there.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    PeakGlitterVU(CRGB *ledStrip,
                  uint16_t ledCount,
                  float &audioSource,
                  bool overlayMode = false)
        : VuBaseFL(overlayMode, ledStrip, ledCount, audioSource, fadeRate_default())
    {
      animationDelay = 10;
      vuPeakHandler.peakHold = 20;
      vuPeakHandler.peakDecay = 0;
      vuRangeExtender.smoothingFactor = 3;
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
#ifdef PEAK_GLITTER_VU_DEBUG
      Serial.print(" -:");
      Serial.print(0.0);
      Serial.print(" +:");
      Serial.print(10.0);
      Serial.print(" VU:");
      Serial.print(10.0 * vuRangeExtender.vuLevel());
      Serial.print(" peak:");
      Serial.print(10.0 * _peakLevel);
      Serial.print(" peakDet:");
      Serial.print((_peakLevel > 0.0) ? 0.5 : 0.0);
      Serial.println();
#endif

      if (_peakLevel > 0.0)
      {
        if (colorGenerator)
        {
          color = colorGenerator->generateColor(_peakLevel * 255);
        }
        safePixel(_peakLevel * (ledCount - 1)) = color;
        _peakLevel = 0.0;
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      float vuLevel = vuLevelHandler.capture();
      vuLevel = vuRangeExtender.process(vuLevel);
      if (vuPeakHandler.process(vuLevel, currentMillis))
      {
        _peakLevel = vuPeakHandler.peakLevel();
      }
    }

  private:
    float _peakLevel = 0.0;
  };

} // namespace EC
