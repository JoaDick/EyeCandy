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
    enum State
    {
      STATE_IDLE = 0,
      STATE_RISING,
      STATE_BLOWING,
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

    void show(AnimationBase_FL &animation)
    {
      int16_t pixelPos = _pos * animation.ledCount;

#ifdef FIREWORK_DEBUG
      if (_debugPos_blow > 0.0)
      {
        animation.safePixel(_debugPos_blow * animation.ledCount) = CRGB(64, 64, 64);
      }
      if (_debugPos_apex > 0.0)
      {
        animation.safePixel(_debugPos_apex * animation.ledCount) = CRGB(128, 0, 0);
      }
      if (_debugPos_gliding > 0.0)
      {
        animation.safePixel(_debugPos_gliding * animation.ledCount) = CRGB(0, 0, 128);
      }
#endif

      switch (_state)
      {
      case STATE_IDLE:
        // nothing to do
        break;

      case STATE_RISING:
        if (pixelPos != _lastPixelPos)
        {
          animation.safePixel(_lastPixelPos) = CRGB::Black;
        }
        animation.safePixel(pixelPos) = CRGB(32, 16, 0);
        break;

      case STATE_BLOWING:
        animation.safePixel(pixelPos - 1) = CRGB::White;
        animation.safePixel(pixelPos) = CRGB::White;
#if (1)
        _vel = randomF(0.5, _vel + 0.5);
        _acc = -5.0;
#else
        {
          float maxVel = _vel + 0.5;
          float deltaV = randomF(0.01, maxVel);
          deltaV = maxVel - maxVel / deltaV;
          deltaV -= _vel;
          _vel += deltaV;
          _acc = -5.0;
        }
#endif
        _state = STATE_FALLING;
        break;

      case STATE_FALLING:
        animation.safePixel(pixelPos) = CHSV(_pixelHue, 255, _pixelVolume);
        break;

      default:
        _state = STATE_IDLE;
        break;
      }

      if (pixelPos != _lastPixelPos)
      {
        _lastPixelPos = pixelPos;
      }
    }

    State update(uint16_t delta_ms)
    {
#ifdef FIREWORK_DEBUG
      _debugPos_last = _pos;
#endif

      float deltaT = delta_ms * 0.001;
      _vel += deltaT * _acc;
      _pos += deltaT * _vel;

      switch (_state)
      {
      case STATE_IDLE:
        // nothing to do
        break;

      case STATE_RISING:
        if (_vel > 1.5)
        {
          _acc = 0.0;
        }
        if (_pos >= _blowingPos)
        {
          _state = STATE_BLOWING;
#ifdef FIREWORK_DEBUG
          if (_debugPos_blow <= 0.0)
          {
            _debugPos_blow = _pos;
          }
#endif
        }
        break;

      case STATE_BLOWING:
        // waiting for show()
        break;

      case STATE_FALLING:
#ifdef FIREWORK_DEBUG
        if (_debugPos_apex <= 0.0)
        {
          if (_debugPos_last > _pos)
          {
            _debugPos_apex = _debugPos_last;
          }
        }
#endif
        if (_pixelVolume)
        {
          --_pixelVolume;
        }
        if (_vel < -0.3)
        {
          _acc = 0.0;
#ifdef FIREWORK_DEBUG
          if (_debugPos_gliding <= 0.0)
          {
            _debugPos_gliding = _pos;
          }
#endif
        }
        if (_pos < 0.0)
        {
          _state = STATE_IDLE;
        }
        break;

      default:
        _state = STATE_IDLE;
        break;
      }

      return _state;
    }

    void launch()
    {
      _acc = randomF(3.5, 5.0);
      _vel = randomF(0.25);
      _pos = 0.0;

      _pixelVolume = 255;
      _pixelHue = random8();

      _blowingPos = 0.6; // randomF(0.5, 0.75);
      _lastPixelPos = -1;

      _state = STATE_RISING;

#ifdef FIREWORK_DEBUG
      _debugPos_last = 0.0;
      _debugPos_blow = 0.0;
      _debugPos_apex = 0.0;
      _debugPos_gliding = 0.0;
#endif
    }

  private:
    State _state = STATE_IDLE;

    uint8_t _pixelVolume;
    uint8_t _pixelHue;
    float _blowingPos;

    // acceleration
    float _acc;

    // velocity: 1.0 = 100% of strip per second
    float _vel;

    // position: 1.0 = top end of strip
    float _pos;

    int16_t _lastPixelPos;

#ifdef FIREWORK_DEBUG
    float _debugPos_last;
    float _debugPos_blow;
    float _debugPos_apex;
    float _debugPos_gliding;
#endif
  };

} // namespace EC
