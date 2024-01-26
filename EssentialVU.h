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

#include "VuOverlayDot.h"
#include "VuOverlayLine.h"
#include "VuSourcePeakHold.h"

//------------------------------------------------------------------------------

// #define ESSENTIAL_VU_DEBUG

//------------------------------------------------------------------------------

namespace EC
{

  /** Basic VU meter Overlay.
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
    VuSourcePeakHold vuPeakSource;

    /** Access to VuOverlayLine configuration.
     * Adjust the following properties according to your needs:
     * - VuOverlayLine::color
     */
    VuOverlayLine vuLevelBar;

    /** Access to VuOverlayDot configuration.
     * Adjust the following properties according to your needs:
     * - VuOverlayDot::color
     */
    VuOverlayDot vuPeakDot;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU Overlay.
     */
    EssentialVU(FastLedStrip ledStrip, VuSource &vuSource)
        : vuPeakSource(vuSource), vuLevelBar(ledStrip, vuSource), vuPeakDot(ledStrip, vuPeakSource)
    {
    }

    /// Get the VuSource that is used for the VU level bar.
    VuSource &getLevelVuSource()
    {
      return vuLevelBar.getInputVuSource();
    }

    /// Get the VuSource that is used for the VU peak dot.
    VuSource &getPeakVuSource()
    {
      return vuPeakDot.getInputVuSource();
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
