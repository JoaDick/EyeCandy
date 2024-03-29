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

#include "AnimationBase.h"
#include "Lightbulbs.h"
#include "VuPeakGravityHandler.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

#ifndef EC_RETRO_PARTY_VU_DEBUG
#define EC_RETRO_PARTY_VU_DEBUG 0
#endif

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class that pretends to be a beat detector.
   * Although it is implemented completely different than a "regular" beat detector, it is working
   * surprisingly well for these kind of fun VU Animations.
   */
  class BeatDetectorFake
  {
  public:
    /// Constructor.
    BeatDetectorFake()
    {
      vuPeakGravityHandler.v0 = 1.0;
      vuPeakGravityHandler.a0 = -6.25;
    }

    /** Calculate the beat for the given \a vuLevel.
     * @param vuLevel  The current VU level.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @retval 0.0    No beat detected.
     * @retval > 0.0  A new beat has been detected with that level.
     */
    float process(float vuLevel,
                  uint32_t currentMillis)
    {
      // make the VU level a bit smoother before counterfeiting a detected peak as beat
      if (vuPeakGravityHandler.process(smoothedVuLevel.process(vuLevel), currentMillis))
      {
        return vuPeakGravityHandler.getVU();
      }
      return 0.0;
    }

    /// Usually there's nothing to configure here; mainly for debugging.
    VuPeakGravityHandler vuPeakGravityHandler;

    /// Usually there's nothing to configure here; mainly for debugging.
    MovingAverage smoothedVuLevel{7};
  };

  //------------------------------------------------------------------------------

  /** Emulation of a retro-style sound reactive running light with colorful Lightbulbs.
   * Shifting the Lightbulb pattern is triggered by the beat of the music.
   */
  class RetroPartyVU
      : public Animation
  {
  public:
    /** Pattern sequence for the Lightbulbs.
     * Adjustable e.g. via presetXXX() functions.
     */
    BitPatternSequencer sequencer;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param vuSource  Input for calculating the VU.
     * @param bulbs  Number of Lightbulbs (max 8).
     */
    RetroPartyVU(FastLedStrip ledStrip,
                 VuSource &vuSource,
                 uint8_t bulbs = 6)
#if (EC_RETRO_PARTY_VU_DEBUG)
        : lightbulbArray(ledStrip.getSubStrip(0, ledStrip.ledCount() / 2), bulbs),
          _debugStrip(ledStrip.getSubStrip(ledStrip.ledCount() / 2, 0)),
#else
        : lightbulbArray(ledStrip, bulbs),
#endif
          _vuSource(vuSource)
    {
      // presetRunningLight(sequencer, lightbulbArray.numBulbs);
      // presetScanner(sequencer, lightbulbArray.numBulbs);
      // presetMarquee12(sequencer);
      presetMarquee24(sequencer);
    }

    /// Get the VuSource that is used as input.
    VuSource &getInputVuSource() { return _vuSource; }

    /** Helper for drawing the Lightbulb patterns.
     * Usually there's nothing to configure here; mainly for debugging.
     */
    LightbulbArray lightbulbArray;

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (!wasModified)
        return;

      const float vuLevelRaw = _vuSource.getVU();
      const float vuLevelBeat = _beatDetector.process(vuLevelRaw, currentMillis);
      if (vuLevelBeat > 0.0)
      {
        sequencer.update();
      }
      lightbulbArray.show(sequencer.get());

#if (EC_RETRO_PARTY_VU_DEBUG)
      _debugStrip.clear();
      // _debugStrip.n_pixel(vuLevelRaw) = CRGB(0, 0, 64);
      // _debugStrip.n_pixel(_levelAvg.process(vuLevelRaw)) = CRGB(0, 66, 64);
      _debugStrip.n_pixel(_beatDetector.smoothedVuLevel) = CRGB(0, 64, 0);
      _debugStrip.n_pixel(_beatDetector.vuPeakGravityHandler.getVU()) = CRGB(255, 0, 0);
      if (vuLevelBeat > 0.0)
      {
        _debugStrip.n_lineRel(0.0, 0.05, CRGB::Yellow);
      }

#endif
    }

  private:
#if (EC_RETRO_PARTY_VU_DEBUG)
    FastLedStrip _debugStrip;
    // MovingAverage _levelAvg{100};
#endif
    VuSource &_vuSource;
    BeatDetectorFake _beatDetector;
  };

} // namespace EC
