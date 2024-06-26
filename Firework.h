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

#include "AnimationBase.h"
#include "intern/FireworkParticle.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Default value for #Firework::fadeRate
  inline uint8_t Firework_fadeRate_default() { return 50; }

  /// Default value for #Firework::launchDelay
  inline uint16_t Firework_launchDelay_default() { return 900; }

  /** A Firework Animation.
   * @tparam PARTICLE_COUNT Number of effect particles.
   */
  template <uint8_t PARTICLE_COUNT = 5>
  class Firework
      : public AnimationModelBase
  {
  public:
    /// Delay (in ms) before relaunching the Particles.
    uint16_t launchDelay = Firework_launchDelay_default();

    /** Constructor
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param launchDelay  Multiples of 1500 give a nice effect.
     */
    Firework(FastLedStrip ledStrip,
             bool overlayMode,
             uint16_t launchDelay = Firework_launchDelay_default())
        : AnimationModelBase(10, ledStrip, overlayMode, Firework_fadeRate_default()),
          launchDelay(launchDelay)
    {
#ifdef FIREWORK_DEBUG
      // setPatternUpdatePeriod(20);
#endif
    }

  private:
#ifdef FIREWORK_DEBUG
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      AnimationBase::showDefaultPattern();
      _particles[0].dump();
    }
#endif

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < PARTICLE_COUNT; ++i)
      {
        _particles[i].show(strip);
      }
    }

    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      bool mustLaunch = true;
      for (uint8_t i = 0; i < PARTICLE_COUNT; ++i)
      {
        _particles[i].update(getModelUpdatePeriod());
        if (_particles[i].getState() != FireworkParticle::STATE_IDLE)
        {
          mustLaunch = false;
        }
      }

      if (mustLaunch)
      {
        if (_launchTime == 0)
        {
          _launchTime = currentMillis + launchDelay;
        }
        if (_launchTime <= currentMillis)
        {
          _particleConfig = FireworkParticle::Config();
          for (uint8_t i = 0; i < PARTICLE_COUNT; ++i)
          {
            _particles[i].launch(_particleConfig);
            _launchTime = 0;
          }
        }
      }
    }

  private:
    FireworkParticle::Config _particleConfig;
    FireworkParticle _particles[PARTICLE_COUNT];
    uint32_t _launchTime = 0;
  };

} // namespace EC
