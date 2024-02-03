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

#include "VuOverlayRainbowDot.h"
#include "VuOverlayRainbowLine.h"
#include "VuSourcePeakGravity.h"

//------------------------------------------------------------------------------

namespace EC
{

#if (0)
  /// VU meter with its color depending on the current VU level.
  class RainbowLevelVU

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

    /** Access to VuPeakGravityHandler configuration.
     * Adjust the following properties according to your needs:
     * - Set VuSourcePeakGravity::VuPeakGravityHandler::a0 < 0.0 for the behaviour of a falling ball, that's bumped up by the VU bar.
     * - Set VuSourcePeakGravity::VuPeakGravityHandler::a0 > 0.0 for a bubble, floating off the peak.
     * - Or use a preset like VuPeakGravityHandler::presetPunchedBall() or
     *   VuSourcePeakGravity::VuPeakGravityHandler::presetFloatingBubble()
     * - VuSourcePeakGravity::VuPeakGravityHandler::enableDipMode
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
    RainbowLevelVU(FastLedStrip ledStrip, VuSource &vuSource)
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
#endif

} // namespace EC
