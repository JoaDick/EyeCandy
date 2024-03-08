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

// #define FIRE2012VU_DEBUG

#include "AnimationBase.h"
#include "Fire2012.h"
#include "VuPeakHandler.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Adjust Fire2012's "height" according to the current VU level.
  template <uint16_t NUM_LEDS>
  class Fire2012VU
#ifndef FIRE2012VU_DEBUG
      : public Animation
#else
      : public AnimationBase
#endif
  {
  public:
    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    /** Constructor.
     * @param fire  Original #Fire2012 animation to manipulate.
     * @param vuSource  Read the VU value from there.
     */
    Fire2012VU(Fire2012<NUM_LEDS> &fire, VuSource &vuSource)
#ifndef FIRE2012VU_DEBUG
        : _fire(fire), _vuSource(vuSource)
#else
        : AnimationBase(fire.getStrip(), true), _fire(fire), _vuSource(vuSource)
#endif
    {
      vuPeakHandler.peakHold = 150;
      vuPeakHandler.peakDecay = 500;
    }

  private:
#ifndef FIRE2012VU_DEBUG
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (wasModified)
      {
        updateFireConfig(currentMillis);
      }
    }
#else
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      updateFireConfig(currentMillis);
      strip.n_pixel(vuPeakHandler.getVU()) = CRGB(0, 0, 255);
    }
#endif

    void updateFireConfig(uint32_t currentMillis)
    {
      float vuLevel = vuPeakHandler.process(_vuSource.getVU(), currentMillis);
      vuLevel = constrainVU(vuPeakHandler.getVU());

      /// COOLING: How much does the air cool as it rises?
      /// Less cooling = taller flames.  More cooling = shorter flames.
      /// suggested range 20-100
      _fire.COOLING = 255 - vuLevel * 255;

      /// SPARKING: What chance (out of 255) is there that a new spark will be lit?
      /// Higher chance = more roaring fire.  Lower chance = more flickery fire.
      /// suggested range 50-200.
      _fire.SPARKING = 75 + vuLevel * 115;
    }

  private:
    Fire2012<NUM_LEDS> &_fire;
    VuSource &_vuSource;
  };

} // namespace EC
