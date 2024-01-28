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

#include "Animation.h"
#include "VuSource.h"
#include "VuPeakHandler.h"
#include "VuPeakGravityHandler.h"

//------------------------------------------------------------------------------

// #define TESTVU1_DEBUG

//------------------------------------------------------------------------------

namespace EC
{

  /** A playground for VU testing during EyeCandy development.
   */
  class TestVU1
      : public Animation
  {
  public:
    /** Render the VU level as bar.
     * This setting can be adjusted at runtime.
     */
    bool enableVuBar = true;

    /** Render the VU level as stripe.
     * This setting can be adjusted at runtime.
     */
    bool enableVuStripe = false;

    /** Render the VU level as dot.
     * This setting can be adjusted at runtime.
     */
    bool enableVuDot = false;

    /** Render the peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enablePeakDot = true;

    /** Render the minimum peak dot.
     * This setting can be adjusted at runtime.
     */
    bool enablePeakDotMin = false;

    /** Draw the VU bar with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB vuBarColor = CRGB(0, 64, 0);

    /** Draw the VU stripe with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB vuStripeColor = CRGB(16, 128, 0);

    /** Draw the VU dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB vuDotColor = CRGB(0, 255, 32);

    /** Draw the peak dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB peakDotColor = CRGB(255, 0, 0);

    /** Draw the minimum peak dot with this color.
     * This setting can be adjusted at runtime.
     */
    CRGB peakDotMinColor = CRGB(0, 0, 255);

#if (0)
    /** Adjust the following properties according to your needs:
     * - VuPeakHandler::peakHold
     * - VuPeakHandler::peakDecay
     * - VuPeakHandler::enableDipMode
     */
    VuPeakHandler vuPeakHandler;

    /** Adjust the following properties according to your needs:
     * - VuPeakHandlerInv::peakHold
     * - VuPeakHandlerInv::peakDecay
     * - VuPeakHandler::enableDipMode
     */
    VuPeakHandler vuDipHandler;
#else
    /** Adjust the following properties according to your needs:
     * - Set VuPeakGravityHandler::a0 < 0.0 for the behaviour of a falling ball, that's bumped up by the VU bar.
     * - Set VuPeakGravityHandler::a0 > 0.0 for a bubble, floating off the peak.
     * - Or use a preset like VuPeakGravityHandler::presetPunchedBall() or
     *   VuPeakGravityHandler::presetFloatingBubble()
     * - VuPeakGravityHandler::enableDipMode
     */
    VuPeakGravityHandler vuPeakHandler;

    /** Adjust the following properties according to your needs:
     * - Set VuPeakGravityHandler::a0 < 0.0 for the behaviour of a falling ball, that's bumped up by the VU bar.
     * - Set VuPeakGravityHandler::a0 > 0.0 for a bubble, floating off the peak.
     * - Or use a preset like VuPeakGravityHandler::presetPunchedBall() or
     *   VuPeakGravityHandler::presetFloatingBubble()
     * - VuPeakGravityHandler::enableDipMode
     */
    VuPeakGravityHandler vuDipHandler;
#endif

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU Overlay.
     */
    TestVU1(FastLedStrip ledStrip,
            VuSource &vuSource)
        : _strip(ledStrip), _vuSource(vuSource)
    {
#if (0)
      vuPeakHandler.peakHold = 500;
      vuPeakHandler.peakDecay = 3000;
      vuDipHandler.enableDipMode = true;
      vuDipHandler.peakHold = vuPeakHandler.peakHold;
      vuDipHandler.peakDecay = vuPeakHandler.peakDecay;
#else
      vuPeakHandler.a0 = -0.8;
      vuPeakHandler.v0 = 0.3;
      // vuPeakHandler.presetPunchedBall();
      // vuPeakHandler.presetFloatingBubble();
      vuDipHandler.enableDipMode = true;
      vuDipHandler.a0 = vuPeakHandler.a0;
      vuDipHandler.v0 = vuPeakHandler.v0;
#endif
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        const float vuLevel = _vuSource.getVU();
        vuPeakHandler.process(vuLevel, currentMillis);
        vuDipHandler.process(vuLevel, currentMillis);

        if (enableVuBar)
        {
          _strip.normLineRel(0.0, vuLevel, vuBarColor);
        }
        if (enableVuStripe)
        {
          _strip.normLineAbs(vuLevel, _lastVuLevel, vuStripeColor);
        }
        if (enableVuDot)
        {
          _strip.normPixel(vuLevel) = vuDotColor;
        }
        if (enablePeakDot)
        {
          _strip.optPixel(vuPeakHandler.getVU()) = peakDotColor;
        }
        if (enablePeakDotMin)
        {
          _strip.optPixel(vuDipHandler.getVU()) = peakDotMinColor;
        }
        _lastVuLevel = vuLevel;

#ifdef TESTVU1_DEBUG
        // Teleplot: VU levels
        Serial.print(">-:");
        Serial.println(0.0);
        Serial.print(">+:");
        Serial.println(1.0);
        Serial.print(">VU:");
        Serial.println(vuLevel);
        Serial.print(">peak:");
        Serial.println(vuPeakHandler.getVU());
#endif
      }
    }

  private:
    FastLedStrip _strip;
    VuSource &_vuSource;
    float _lastVuLevel = 0.0;
  };

} // namespace EC
