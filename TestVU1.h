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

#include "VuBaseFL.h"
#include "VuPeakHandler.h"
#include "VuRangeExtender.h"

//------------------------------------------------------------------------------

// #define TESTVU1_DEBUG

//------------------------------------------------------------------------------

namespace EC
{

  /** A playground for VU testing during EyeCandy development.
   * Initial revision is a clone of EssentialVU.
   */
  class TestVU1
      : public VuBaseFL
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static uint8_t fadeRate_default() { return 20; }

    /** Draw the VU bar with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB vuBarColor = CRGB(0, 64, 0);

    /** Draw the VU dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB vuDotColor = CRGB(0, 255, 0);

    /** Draw the peak dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB peakDotColor = CRGB(255, 0, 0);

    /** Draw the inverse peak dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB peakInvDotColor = CRGB(0, 0, 255);

    /** Render the VU level as bar.
     * This setting can be adjusted at runtime.
     */
    bool enableVuBar = false;

    /** Render the VU level as dot.
     * This setting can be adjusted at runtime.
     */
    bool enableVuDot = true;

    /** Render the peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enablePeakDot = true;

    /** Render the inverse peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enablePeakInvDot = true;

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

    /** Configure the following properties according to your needs:
     * - VuPeakHandlerInv::peakHold
     * - VuPeakHandlerInv::peakDecay
     * - Don't call any of its methods!
     */
    VuPeakHandlerInv vuPeakHandlerInv;

    /// Usually there's nothing to configure here; only for debugging.
    VuRangeExtender vuRangeExtender;

    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     */
    TestVU1(float &audioSource,
            FastLedStrip ledStrip,
            bool overlayMode)
        : VuBaseFL(ledStrip, overlayMode, fadeRate_default(), audioSource)
    {
      animationDelay = 10;
      vuPeakHandler.peakHold = 500;
      vuPeakHandler.peakDecay = 3000;
      vuPeakHandlerInv.peakHold = vuPeakHandler.peakHold;
      vuPeakHandlerInv.peakDecay = vuPeakHandler.peakDecay;
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (enableVuBar)
      {
        strip.normLineRel(0.0, _vuLevel, vuBarColor);
      }
      if (enableVuDot)
      {
        strip.normLineAbs(_vuLevel, _lastVuLevel, vuDotColor);
        // strip.normPixel(_vuLevel) = vuDotColor;
      }
      if (enablePeakDot)
      {
        strip.optPixel(vuPeakHandler.peakLevel()) = peakDotColor;
      }
      if (enablePeakInvDot)
      {
        strip.optPixel(vuPeakHandlerInv.peakLevel()) = peakInvDotColor;
      }
      _lastVuLevel = _vuLevel;

#ifdef TESTVU1_DEBUG
      // Teleplot: VU levels
      Serial.print(">-:");
      Serial.println(0.0);
      Serial.print(">+:");
      Serial.println(1.0);
      Serial.print(">VU:");
      Serial.println(_vuLevel);
      Serial.print(">peak:");
      Serial.println(vuPeakHandler.peakLevel());
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
      vuPeakHandlerInv.process(_vuLevel, currentMillis);
    }

  private:
    float _vuLevel = 0.0;
    float _lastVuLevel = 0.0;
  };

} // namespace EC