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
#include "VuPeakHandler.h"
#include "VuRangeExtender.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// VU meter with its color depending on the current VU level.
  class RainbowLevelVU
      : public AnimationBaseFL
  {
    // -1.0 ... +1.0
    float &_audioSample;

    float _vuLevel = 0.0;
    float _peakLevel = 0.0;

    float _startHue = random8();

  public:
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

    /** Configure the following properties according to your needs:
     * - VuLevelHandler::smoothingFactor
     * - Don't call any of its methods!
     */
    VuLevelHandler vuLevelHandler;

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
     * @param audioSample  Read the audio data from there.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    RainbowLevelVU(CRGB *ledStrip,
                   uint16_t ledCount,
                   float &audioSample,
                   bool overlayMode = false)
        : AnimationBaseFL(overlayMode ? TYPE_OVERLAY : TYPE_FADING_PATTERN, ledStrip, ledCount), _audioSample(audioSample)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
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
      return 0;
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      uint8_t hue = redShift(_startHue + _vuLevel * vuHueRange * 255);

      if (enableVuBar)
      {
        const CRGB barColor = CHSV(hue, 255, volume);
        lineRel(0, _vuLevel * ledCount, barColor);
      }

      if (enablePeakDot &&
          _peakLevel > 0.0)
      {
        const CRGB dotColor = CHSV(hue + 128, 255, volume);
        safePixel(_peakLevel * ledCount) = dotColor;
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      _vuLevel = vuLevelHandler.capture();
      _vuLevel = vuRangeExtender.process(_vuLevel);
      _peakLevel = vuPeakHandler.process(_vuLevel, currentMillis);

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

    /// @see AnimationBase::processAnimationBackground()
    void processAnimationBackground(uint32_t currentMillis) override
    {
      vuLevelHandler.addSample(_audioSample);
    }
  };

} // namespace EC
