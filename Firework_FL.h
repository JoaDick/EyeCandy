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

// #define FIREWORK_DEBUG

#include "AnimationBase_FL.h"
#include "intern/FireworkParticle.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** AFirework Animation.
   */
  class Firework_FL
      : public AnimationBase_FL
  {
  public:
    /** Constructor
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     */
    Firework_FL(CRGB *ledStrip,
                uint16_t ledCount)
        : AnimationBase_FL(TYPE_FADING_PATTERN, ledStrip, ledCount)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    uint8_t showPattern(uint32_t currentMillis) override
    {
      fadeToBlackBy(ledStrip, ledCount, _fadeRate);
      showOverlay(currentMillis);
#ifdef FIREWORK_DEBUG
      return _particles[0].dump();
#endif
      return 0;
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      for (uint8_t i = 0; i < _particleCount; ++i)
      {
        _particles[i].show(*this);
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      const uint16_t animationDelay = getAnimationDelay();

      bool mustLaunch = true;
      for (uint8_t i = 0; i < _particleCount; ++i)
      {
        _particles[i].update(animationDelay);
        if (_particles[i].getState() != FireworkParticle::STATE_IDLE)
        {
          mustLaunch = false;
        }
      }

      if (mustLaunch)
      {
        _particleConfig = FireworkParticle::Config();
        for (uint8_t i = 0; i < _particleCount; ++i)
        {
          _particles[i].launch(_particleConfig);
        }
      }
    }

  private:
    static const uint8_t _fadeRate = 50;
#ifdef FIREWORK_DEBUG
    static const uint8_t _particleCount = 1;
#else
    static const uint8_t _particleCount = 6;
#endif
    FireworkParticle::Config _particleConfig;
    FireworkParticle _particles[_particleCount];
  };

} // namespace EC
