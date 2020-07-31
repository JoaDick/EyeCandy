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

// #define ESSENTIAL_VU_DEBUG

//------------------------------------------------------------------------------

namespace EC
{

  /** Basic VU meter Animation & Overlay.
   * Although being called "basic", it already offers a lot of features and
   * configuration options:
   * - show VU level bargraph yes/no
   * - show peak dot yes/no
   * - solid background or fading background, incl. fading rate
   * - VU bargraph color
   * - peak dot color
   * - background color
   * - peak hold duration
   * - peak dot falling speed
   * - range extender to stretch the "interesting" part of the VU meter over
   *   the entire LED strip
   */
  class EssentialVU
      : public VuBaseFL
  {
  public:
    /** Draw the VU bar with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB vuBarColor = CRGB(0, 64, 0);

    /** Draw the peak dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB peakDotColor = CRGB(255, 0, 0);

    /** Fill LED strip with this color.
     * This setting can be adjusted at runtime.
     * Not relevant in Overlay mode or when #fadeRate is non-zero.
     */
    CRGB backgroundColor = CRGB(0, 0, 0);

    /** Fading speed.
     * Lower value = longer glowing.
     * 0 means no fading, but solid background using #backgroundColor.
     * This setting can be adjusted at runtime.
     * Not relevant in Overlay mode.
     */
    uint8_t fadeRate = 50;

    /** Render the VU bar.
     * This setting can be adjusted at runtime.
     */
    bool enableVuBar = true;

    /** Render the peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enablePeakDot = true;

    /** Stretch the "interesting" part of the VU meter over the entire LED strip.
     * This setting can be adjusted at runtime.
     */
    bool enableRangeExtender = true;

    /** Configure the following properties according to your needs:
     * - VuPeakHandler::peakHold
     * - VuPeakHandler::peakDecay
     * - Don't call any of its methods!
     */
    VuPeakHandler vuPeakHandler;

    /// Usually there's nothing to configure here; only for debugging.
    VuRangeExtender vuRangeExtender;

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param audioSource  Read the audio samples from there.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    EssentialVU(CRGB *ledStrip,
                uint16_t ledCount,
                float &audioSource,
                bool overlayMode = false)
        : VuBaseFL(overlayMode ? TYPE_OVERLAY : TYPE_FADING_PATTERN, ledStrip, ledCount, audioSource)
    {
      animationDelay = 10;
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      if (fadeRate)
      {
        fadeToBlackBy(ledStrip, ledCount, fadeRate);
      }
      else
      {
        fill_solid(ledStrip, ledCount, backgroundColor);
      }
      showOverlay(currentMillis);
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (enableVuBar)
      {
        lineRel(0, _vuLevel * (ledCount - 1), vuBarColor);
      }

      const float peakLevel = vuPeakHandler.peakLevel();
      if (enablePeakDot &&
          peakLevel > 0.0)
      {
        safePixel(peakLevel * (ledCount - 1)) = peakDotColor;
      }

#ifdef ESSENTIAL_VU_DEBUG
      Serial.print(" -:");
      Serial.print(0.0);
      Serial.print(" +:");
      Serial.print(10.0);
      Serial.print(" VU:");
      Serial.print(10.0 * _vuLevel);
      Serial.print(" peak:");
      Serial.print(10.0 * peakLevel);
      Serial.println();
#endif
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      _vuLevel = vuLevelHandler.capture();
      if (enableRangeExtender)
      {
        _vuLevel = vuRangeExtender.process(_vuLevel);
      }
      vuPeakHandler.process(_vuLevel, currentMillis);
    }

  private:
    float _vuLevel = 0.0;
  };

} // namespace EC
