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

#include "Animation.h"
#include "FastLedStrip.h"
#include "VuPeakHandler.h"
#include "VuSource.h"

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
     */
    CRGB color = CRGB::White;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuRangeExtender vuRangeExtender;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

#if (1)
    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    PeakGlitterVU(float &audioSource,
                  FastLedStrip ledStrip,
                  bool overlayMode)
        : VuBaseFL(ledStrip, overlayMode, fadeRate_default(), audioSource)
    {
      modelUpdatePeriod = 10;
      vuPeakHandler.peakHold = 20;
      vuPeakHandler.peakDecay = 0;
      vuRangeExtender.smoothingFactor = 3;
    }

#else // DRAFT
    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param color  Draw the glitter with this color.
     */
    PeakGlitterVU(float &audioSource,
                  FastLedStrip ledStrip,
                  bool overlayMode,
                  CRGB color = CRGB::White)
        : VuBaseFL(ledStrip, overlayMode, fadeRate_default(), audioSource), color(color)
    {
      modelUpdatePeriod = 10;
      vuPeakHandler.peakHold = 20;
      vuPeakHandler.peakDecay = 0;
      vuRangeExtender.smoothingFactor = 3;
    }
#endif

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
        strip.normPixel(_peakLevel) = color;
        _peakLevel = 0.0;
      }
    }

    /// @see AnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      const float vuLevel = vuRangeExtender.process(vuLevelHandler.capture());
      if (vuPeakHandler.process(vuLevel, currentMillis))
      {
        _peakLevel = vuPeakHandler.getVU();
      }
    }

  private:
    float _peakLevel = 0.0;
  };

  //------------------------------------------------------------------------------

  /** A VU Overlay that produces some glitter around the VU peaks.
   * @note The actual VU value is obtained from the given VuSource.
   */
  class VuOverlayPeakGlitter
      : public Animation
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 50; }

    /** Draw the glitter with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB color = CRGB::White;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    // VuSourcePeakHold vuPeakSource;

#if (0)
    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     */
    VuOverlayPeakGlitter(FastLedStrip ledStrip, VuSource &vuSource)
        : _strip(ledStrip), _vuSource(vuSource)
    {
      vuPeakHandler.peakHold = 20;
      vuPeakHandler.peakDecay = 0;
    }
#else // DRAFT
    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Read the VU value from there.
     * @param color  Draw the glitter with this color.
     */
    VuOverlayPeakGlitter(FastLedStrip ledStrip, VuSource &vuSource,
                         CRGB color = CRGB(255, 255, 0) /*CRGB::White*/)
        : color(color), _strip(ledStrip), _vuSource(vuSource)
    {
      vuPeakHandler.peakHold = 20;
      vuPeakHandler.peakDecay = 0;
    }
#endif

    VuSource &getVuSource()
    {
      return _vuSource;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        if (vuPeakHandler.process(_vuSource.getVU(), currentMillis))
        {
          // TODO: let it overshoot a little bit
          _strip.normPixel(vuPeakHandler.getVU()) = color;
        }

#ifdef PEAK_GLITTER_VU_DEBUG
        Serial.print(" -:");
        Serial.print(0.0);
        Serial.print(" +:");
        Serial.print(10.0);
        Serial.print(" VU:");
        Serial.print(10.0 * _vuSource.getVU());
        Serial.print(" peak:");
        Serial.print(10.0 * vuPeakHandler.getVU());
        Serial.print(" peakDet:");
        Serial.print((vuPeakHandler.getVU() > 0.0) ? 0.5 : 0.0);
        Serial.println();
#endif
      }
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuSource;
  };

} // namespace EC
