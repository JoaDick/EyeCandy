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
#include "VuPeakForceHandler.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** An Animation-Worker for calculating a VU peak level; as if "loosely tied to the VU level"
   * Behaves as if the VU level and the peak dot are connected with a rubber band.
   * @note This Worker is only doing the math. It reads the current VU level from a given VuSource,
   * and calculates the corresponding peak VU level. The result thereof (for rendering the pixels)
   * can be obtained through the VuSource interface that is provided.
   * @see VuPeakForceHandler for the math details.
   */
  class VuSourcePeakForce
      : public Animation
  {
  public:
    /** Access to VuPeakForceHandler configuration.
     * Adjust the following properties according to your needs:
     * - VuPeakForceHandler::inertia
     * - VuPeakForceHandler::friction
     */
    VuPeakForceHandler vuPeakHandler;

    /// Make this class usable as a VuSource (to circumvent multiple inheritance).
    operator VuSource &() { return asVuSource(); }
    VuSource &asVuSource() { return vuPeakHandler; }

    /** Constructor.
     * @param vuSource  Input for calculating the VU peak level.
     */
    explicit VuSourcePeakForce(VuSource &vuSource)
        : _vuSource(vuSource)
    {
    }

    /// Get the VuSource that is used as input.
    VuSource &getInputVuSource() { return _vuSource; }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      vuPeakHandler.process(_vuSource.getVU(), currentMillis);
      return 0;
    }

  private:
    VuSource &_vuSource;
  };

} // namespace EC
