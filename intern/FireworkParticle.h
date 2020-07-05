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

#include "../AnimationBase_FL.h"
#include "../randomF.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Particle for Firework Animation.
  class FireworkParticle
  {
  public:
    /// Particle configuration.
    struct Config
    {
      /// Effect color.
      uint8_t colorHue = random8();

      /// Effect brightness.
      uint8_t colorVolume = 255;

      /** Height where the particle shall explode.
       * Range 0.0 ... 1.0
       */
      float blowingPos = 0.6; // randomF(0.5, 0.75);

      /** Height where the particle lifecycle is over.
       * Range 0.0 ... 1.0
       */
      float fadingEndPos = randomF(0.1, 0.5);

      /** Duration of fading out towards lifecycle end.
       * Range 0.0 ... 1.0
       */
      float fadingDuration = randomF(0.05, 0.5);

      /// Starting speed offset.
      float launchVel = randomF(0.1, 0.25);

      /// Rising acceleration.
      float launchAcc = randomF(3.5, 5.0);
    };

    enum State
    {
      STATE_IDLE = 0,
      STATE_LAUNCHING,
      STATE_BLOW,
      STATE_RISING,
      STATE_FALLING
    };

#ifdef FIREWORK_DEBUG
    uint8_t dump()
    {
      Serial.print(F("pos:"));
      Serial.print(_pos);
      Serial.print(F(" vel:"));
      Serial.print(_vel);
      Serial.print(F(" acc:"));
      Serial.print(_acc);
      Serial.println();

      return 20;
    }
#endif

    State getState()
    {
      return _state;
    }

    void launch(const Config &cfg)
    {
      _config = &cfg;

      _acc = _config->launchAcc;
      _vel = _config->launchVel;
      _pos = 0.0;

      _apexPos = 0.0;
      _lastPixelPos = -1;

      _state = STATE_LAUNCHING;

#ifdef FIREWORK_DEBUG
      _debugPos_blow = 0.0;
      _debugPos_gliding = 0.0;
#endif
    }

    void show(AnimationBase_FL &animation)
    {
#ifdef FIREWORK_DEBUG
      if (_apexPos > 0.0)
      {
        animation.safePixel(_apexPos * animation.ledCount) = CRGB(128, 0, 0);
      }
      if (_debugPos_blow > 0.0)
      {
        animation.safePixel(_debugPos_blow * animation.ledCount) = CRGB(64, 64, 64);
      }
      if (_debugPos_gliding > 0.0)
      {
        // animation.safePixel(_debugPos_gliding * animation.ledCount) = CRGB(0, 0, 128);
      }
#endif

      const int16_t pixelPos = _pos * animation.ledCount;

      switch (_state)
      {
      case STATE_IDLE:
        // nothing to do
        break;

      case STATE_LAUNCHING:
        if (pixelPos != _lastPixelPos)
        {
          animation.safePixel(_lastPixelPos) = CRGB::Black;
        }
        animation.safePixel(pixelPos) = CRGB(32, 16, 0);
        break;

      case STATE_BLOW:
        animation.safePixel(pixelPos - 1) = CRGB::White;
        animation.safePixel(pixelPos) = CRGB::White;
        _acc = randomF(-4.0 * _vel, -1.75 * _vel);
        _state = STATE_RISING;

#ifdef FIREWORK_DEBUG
        _debugPos_blow = _pos;
#endif
        break;

      case STATE_RISING:
        animation.safePixel(pixelPos) = CHSV(_config->colorHue, 255, _config->colorVolume);
        break;

      case STATE_FALLING:
      {
        const float fadingEndPos = _apexPos * _config->fadingEndPos;
        const float fadingBeginPos = fadingEndPos + (_apexPos - fadingEndPos) * _config->fadingDuration;

#ifdef FIREWORK_DEBUG
        if (fadingBeginPos > 0.0)
        {
          animation.safePixel(fadingBeginPos * animation.ledCount) = CRGB(0, 16, 0);
        }
        if (fadingEndPos > 0.0)
        {
          animation.safePixel(fadingEndPos * animation.ledCount) = CRGB(0, 0, 16);
        }
#endif

        CRGB pixelColor = CRGB::Black;
        if (_pos > fadingBeginPos)
        {
          pixelColor = CHSV(_config->colorHue, 255, 255);
        }
        else if (_pos >= fadingEndPos)
        {
          const float fadingRange = fadingBeginPos - fadingEndPos;
          const float pixelVolume = _config->colorVolume * (_pos - fadingEndPos) / fadingRange;
          pixelColor = CHSV(_config->colorHue, 255, pixelVolume);
        }
        else
        {
          // particle off
        }

        if (1)
        {
          animation.safePixel(pixelPos) = pixelColor;
        }
        break;
      }

      default:
        _state = STATE_IDLE;
        break;
      }

      if (pixelPos != _lastPixelPos)
      {
        _lastPixelPos = pixelPos;
      }
    }

    void update(uint16_t delta_ms)
    {
      float deltaT = delta_ms * 0.001;
      _vel += deltaT * _acc;
      _pos += deltaT * _vel;

      switch (_state)
      {
      case STATE_IDLE:
        // nothing to do
        break;

      case STATE_LAUNCHING:
        // max. rising speed reached?
        if (_vel > 1.5)
        {
          _acc = 0.0;
        }
        // blowing position reached?
        if (_pos >= _config->blowingPos)
        {
          _state = STATE_BLOW;
        }
        break;

      case STATE_BLOW:
        // waiting for show()
        break;

      case STATE_RISING:
        // reached the apex?
        if (_vel <= 0.0)
        {
          _apexPos = _pos;
          _state = STATE_FALLING;
        }
        break;

      case STATE_FALLING:
        // not gliding down yet?
        if (_acc < 0.0)
        {
          // max. falling speed reached?
          if (_vel < randomF(-0.5, -0.2))
          {
            // continue gliding down at constant velocity
            _acc = 0.0;

#ifdef FIREWORK_DEBUG
            _debugPos_gliding = _pos;
#endif
          }
        }
        // reached the ground?
        if (_pos <= 0.0)
        {
          _acc = 0.0;
          _vel = 0.0;
          _pos = 0.0;
          _state = STATE_IDLE;
        }
        break;

      default:
        _state = STATE_IDLE;
        break;
      }
    }

  private:
    const Config *_config;
    State _state = STATE_IDLE;

    // acceleration
    float _acc;

    // velocity: 1.0 = 100% of strip per second
    float _vel;

    // position: 1.0 = top end of strip
    float _pos;

    float _apexPos;
    int16_t _lastPixelPos;

#ifdef FIREWORK_DEBUG
    float _debugPos_blow;
    float _debugPos_gliding;
#endif
  };

} // namespace EC
