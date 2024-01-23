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
#include "VuPeakGravityHandler.h"
#include "VuSource.h"

#include "VuPeakHandler.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** TBD
   *
   * An Animation-Worker for calculating a VU peak level.
   * It reads the current VU level from a given VuSource, and calculates the corresponding peak VU
   * level - including peak hold functionality.
   * @note This Worker is only doing the math, thus it provides the result of the calculation
   * through the VuSource interface that it implements. This means that the VU value for rendering
   * LEDs can be obtained - e.g. by an VU Overlay - via the getVU() method.
   */
  class VuSourcePeakGravity
      : public Animation,
        public VuSource
  {
  public:
    /** TBD
     *
     * Access to VuPeakHandler configuration.
     * Adjust the following properties according to your needs:
     * - VuPeakHandler::peakHold
     * - VuPeakHandler::peakDecay
     */
    VuPeakGravityHandler vuPeakHandler;

    /** Constructor.
     * @param vuSource  Input for calculating the VU peak level.
     */
    explicit VuSourcePeakGravity(VuSource &vuSource)
        : _vuSource(vuSource)
    {
    }

    /** Get the peak VU level.
     * @see VuSource::getVU()
     */
    float getVU() override
    {
      return _vuLevel;
    }

    /// Get the VuSource that is used as input.
    VuSource &getInputVuSource()
    {
      return _vuSource;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        vuPeakHandler.process(_vuSource.getVU(), currentMillis);
        _vuLevel = vuPeakHandler.getVU();
      }
    }

  private:
    VuSource &_vuSource;
    float _vuLevel = 0.0;
  };

} // namespace EC
