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

#ifndef FIRE2012VU_DEBUG
#include "PseudoAnimationBase.h"
#else
#include "AnimationBaseFL.h"
#endif
#include "Fire2012.h"
#include "VuLevelHandler.h"
#include "VuPeakHandler.h"
#include "VuRangeExtender.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Adjust Fire2012's "height" according to the current VU level.
  template <uint16_t NUM_LEDS>
  class Fire2012VU
#ifndef FIRE2012VU_DEBUG
      : public PseudoAnimationBase
#else
      : public AnimationBaseFL2
#endif
  {
  public:
    /// Usually there's nothing to configure here; only for debugging.
    VuLevelHandler vuLevelHandler;

    /// Usually there's nothing to configure here; only for debugging.
    VuRangeExtender vuRangeExtender;

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakHandler vuPeakHandler;

    /** Constructor.
     * @param audioSource  Read the audio samples from there.
     * @param fire  Original #Fire2012 animation to manipulate.
     */
    Fire2012VU(float &audioSource,
               Fire2012<NUM_LEDS> &fire)
#ifndef FIRE2012VU_DEBUG
        : _audioSource(audioSource), _fire(fire)
#else
        : AnimationBaseFL2(fire.getStrip(), true), _audioSource(audioSource), _fire(fire)
#endif
    {
      animationDelay = 10;
      vuPeakHandler.peakHold = 150;
      vuPeakHandler.peakDecay = 500;
    }

  private:
#ifdef FIRE2012VU_DEBUG
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      strip.normPixel(vuPeakHandler.peakLevel()) = CRGB(0, 0, 255);
    }
#endif

    /// @see PseudoAnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      float vuLevel = vuLevelHandler.capture();
      vuLevel = vuRangeExtender.process(vuLevel);
      vuPeakHandler.process(vuLevel, currentMillis);
      vuLevel = constrainF(vuPeakHandler.peakLevel());

      /// COOLING: How much does the air cool as it rises?
      /// Less cooling = taller flames.  More cooling = shorter flames.
      /// suggested range 20-100
      _fire.COOLING = 255 - vuLevel * 255;

      /// SPARKING: What chance (out of 255) is there that a new spark will be lit?
      /// Higher chance = more roaring fire.  Lower chance = more flickery fire.
      /// suggested range 50-200.
      _fire.SPARKING = 75 + vuLevel * 115;
    }

    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      vuLevelHandler.addSample(_audioSource);
#ifdef FIRE2012VU_DEBUG
      AnimationBaseFL2::processAnimation(currentMillis, wasModified);
#else
      PseudoAnimationBase::processAnimation(currentMillis, wasModified);
#endif
    }

  private:
    float &_audioSource;
    Fire2012<NUM_LEDS> &_fire;
  };

} // namespace EC
