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

#include "VuOverlayRainbowDot.h"
#include "VuOverlayRainbowLine.h"
#include "VuSourcePeakGravity.h"

//------------------------------------------------------------------------------

namespace EC
{

#if (1)
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
      vuPeakHandler.presetPunchedBall();
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
#endif

  //------------------------------------------------------------------------------

  class RainbowLevelVU2
      : public Animation
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 50; }

    /** Enable rendering the VU bar.
     * This setting can be adjusted at runtime.
     */
    bool enableVuLevelBar = true;

    /** Enable rendering the VU peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enableVuPeakDot = true;

    /** Access to VuPeakHandler configuration.
     * Adjust the following properties according to your needs:
     * - VuSourcePeakHold::VuPeakHandler::peakHold
     * - VuSourcePeakHold::VuPeakHandler::peakDecay
     */
    VuSourcePeakGravity vuPeakSource;

    /** Access to VuOverlayLine configuration.
     * Adjust the following properties according to your needs:
     * - VuOverlayLine::color
     */
    VuOverlayRainbowLine vuLevelBar;

    /** Access to VuOverlayDot configuration.
     * Adjust the following properties according to your needs:
     * - VuOverlayDot::color
     */
    VuOverlayRainbowDot vuPeakDot;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU Overlay.
     */
    RainbowLevelVU2(FastLedStrip ledStrip, VuSource &vuSource)
        : vuPeakSource(vuSource), vuLevelBar(ledStrip, vuSource), vuPeakDot(ledStrip, vuPeakSource, vuSource)
    {
      vuPeakSource.vuPeakHandler.presetPunchedBall();
    }

    /// Get the VuSource that is used for the VU level bar.
    VuSource &getLevelVuSource()
    {
      return vuLevelBar.getLevelVuSource();
    }

    /// Get the VuSource that is used for the VU peak dot.
    VuSource &getPeakVuSource()
    {
      return vuPeakDot.getLevelVuSource();
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (enableVuLevelBar)
      {
        vuLevelBar.process(currentMillis, wasModified);
      }
      if (enableVuPeakDot)
      {
        vuPeakSource.process(currentMillis, wasModified);
        vuPeakDot.process(currentMillis, wasModified);
      }

#ifdef ESSENTIAL_VU_DEBUG
      Serial.print(" -:");
      Serial.print(0.0);
      Serial.print(" +:");
      Serial.print(10.0);
      Serial.print(" VU:");
      Serial.print(10.0 * vuLevelBar.getVuSource().getVU());
      Serial.print(" peak:");
      Serial.print(10.0 * vuPeakDot.getVuSource().getVU());
      Serial.println();
#endif
    }
  };

} // namespace EC
