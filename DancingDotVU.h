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
#include "VuLevelHandler.h"
#include "VuPeakForceHandler.h"
#include "VuRangeExtender.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// VU dot only, dancing around the current VU level.
  class DancingDotVU
      : public AnimationBaseFL
  {
  public:
    /** Draw the peak dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB peakDotColor = CRGB(255, 0, 0);

    /** Fill LED strip with this color.
     * This setting can be adjusted at runtime.
     * Not relevant in Overlay mode.
     */
    CRGB backgroundColor = CRGB(0, 0, 0);

    /// Usually there's nothing to configure here; only for debugging.
    VuLevelHandler vuLevelHandler;

    /// Usually there's nothing to configure here; only for debugging.
    VuPeakForceHandler vuPeakHandler;

    /// Usually there's nothing to configure here; only for debugging.
    VuRangeExtender vuRangeExtender;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param audioSource  Read the audio samples from there.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    DancingDotVU(CRGB *ledStrip,
                 uint16_t ledCount,
                 float &audioSource,
                 bool overlayMode = false)
        : AnimationBaseFL(overlayMode ? TYPE_OVERLAY : TYPE_SOLID_PATTERN, ledStrip, ledCount), _audioSource(audioSource)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      fill_solid(ledStrip, ledCount, backgroundColor);
      showOverlay(currentMillis);
      return 0;
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (vuPeakHandler.peakLevel() > 0.0)
      {
        const uint16_t pixelPos = vuPeakHandler.peakLevel() * (ledCount - 1);
        safePixel(pixelPos) = peakDotColor;
        safePixel(pixelPos - 1) = peakDotColor;
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      float vuLevel = vuLevelHandler.capture();
      vuLevel = vuRangeExtender.process(vuLevel);
      vuPeakHandler.process(vuLevel, currentMillis);
    }

    /// @see AnimationBase::processAnimationBackground()
    void processAnimationBackground(uint32_t currentMillis) override
    {
      vuLevelHandler.addSample(_audioSource);
    }

  private:
    float &_audioSource;
  };

} // namespace EC
