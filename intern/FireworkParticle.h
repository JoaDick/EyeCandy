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

#include "../AnimationBaseFL.h"
#include "../randomF.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Particle for Firework Animation.
  class FireworkParticle
  {
  public:
    /// Possible Glitter variations.
    enum GlitterType
    {
      GLITTER_TWINKLE = 0, //< Twinkling instead of a fading trail.
      GLITTER_SAME,        //< Glitter with particle's color.
      GLITTER_COMP,        //< Glitter with complementary color.
      GLITTER_WHITE,
      GLITTER_RED,
      GLITTER_GREEN,
      GLITTER_BLUE,
      GLITTER_YELLOW,
      GLITTER_CYAN,
      GLITTER_PURPLE,
      GLITTER_RANDOM,
      GLITTER_MAX
    };

    /// Particle configuration.
    struct Config
    {
      /// Effect color.
      uint8_t colorHue = AnimationBaseFL::redShift(random8());

      /// Effect brightness.
      uint8_t colorVolume = 127 + random8(128);

      /** Height where the particle shall explode.
       * Range 0.0 ... 1.0
       */
      float blowingPos = randomF(0.5, 0.7);

      /** Height where the particle lifecycle is over.
       * Range 0.0 ... 1.0
       */
      float fadingEndPos = randomF(0.1, 0.5);

      /** Duration of fading out towards lifecycle end.
       * Range 0.0 ... 1.0
       */
      float fadingDuration = randomF(0.05, 0.5);

      /** Add glitter towards the end?
       * Range 0.0 ... 1.0
       * 0.0 means no glitter.
       */
      float glitterDuration = (random8(100) < 25) ? randomF(0.4, 0.9) : 0.0;

      /// Type of Glitter.
      GlitterType glitterType = GlitterType(random8(GLITTER_MAX - 1));
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
    void dump()
    {
      Serial.print(F("pos:"));
      Serial.print(_pos);
      Serial.print(F(" vel:"));
      Serial.print(_vel);
      Serial.print(F(" acc:"));
      Serial.print(_acc);
      Serial.println();
    }
#endif

    State getState()
    {
      return _state;
    }

    void launch(const Config &cfg)
    {
      _config = &cfg;

      _acc = randomF(3.5, 5.0);
      _vel = 0.1; //randomF(0.1, 0.25);
      _pos = 0.0;

      _apexPos = 0.0;
      _lastPixelPos = -1;

      _state = STATE_LAUNCHING;

#ifdef FIREWORK_DEBUG
      _debugPos_blow = 0.0;
      _debugPos_gliding = 0.0;
#endif
    }

    void show(AnimationBaseFL &animation)
    {
#ifdef FIREWORK_DEBUG
      if (_debugPos_blow > 0.0)
      {
        animation.safePixel(_debugPos_blow * animation.ledCount) = CRGB(128, 64, 0);
      }
      if (_apexPos > 0.0)
      {
        animation.safePixel(_apexPos * animation.ledCount) = CRGB(255, 0, 0);
      }
      if (_debugPos_gliding > 0.0)
      {
        animation.safePixel(_debugPos_gliding * animation.ledCount) = CRGB(64, 0, 32);
      }
#endif

      const int16_t pixelPos = _pos * animation.ledCount;

      switch (_state)
      {
      case STATE_IDLE:
        // nothing to do
        break;

      case STATE_LAUNCHING:
        process_LAUNCHING(animation, pixelPos);
        break;

      case STATE_BLOW:
        process_BLOW(animation, pixelPos);
        break;

      case STATE_RISING:
        process_RISING(animation, pixelPos);

        break;

      case STATE_FALLING:
        process_FALLING(animation, pixelPos);
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
        if (_vel < 0.0)
        {
          _apexPos = _pos;
          _acc /= 50.0;
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
            _debugPos_blow = 0.0;
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
    void process_LAUNCHING(AnimationBaseFL &animation, int16_t pixelPos)
    {
      if (pixelPos != _lastPixelPos)
      {
        animation.safePixel(_lastPixelPos) = CRGB::Black;
      }
      animation.safePixel(pixelPos) = CRGB(32, 16, 0);
    }

    void process_BLOW(AnimationBaseFL &animation, int16_t pixelPos)
    {
      animation.safePixel(pixelPos + 1) = CRGB::White;
      animation.safePixel(pixelPos) = CRGB::Yellow;
      _acc = randomF(-4.5, -2.8);
      _state = STATE_RISING;

#ifdef FIREWORK_DEBUG
      _debugPos_blow = _pos;
#endif
    }

    void process_RISING(AnimationBaseFL &animation, int16_t pixelPos)
    {
      animation.safePixel(pixelPos) = addGlitter(animation, CHSV(_config->colorHue, 255, _config->colorVolume));
    }

    void process_FALLING(AnimationBaseFL &animation, int16_t pixelPos)
    {
      const float fadingEndPos = _apexPos * _config->fadingEndPos;
      const float fadingBeginPos = fadingEndPos + (_apexPos - fadingEndPos) * _config->fadingDuration;

#ifdef FIREWORK_DEBUG
      if (fadingBeginPos > 0.0)
      {
        animation.safePixel(fadingBeginPos * animation.ledCount) = CRGB(0, 0, 16);
      }
      if (fadingEndPos > 0.0)
      {
        animation.safePixel(fadingEndPos * animation.ledCount) = CRGB(0, 0, 16);
      }
#endif

      CRGB pixelColor = CRGB::Black;
      // no fading necessary?
      if (_pos > fadingBeginPos)
      {
        pixelColor = CHSV(_config->colorHue, 255, _config->colorVolume);
      }
      // need fading?
      else if (_pos >= fadingEndPos)
      {
        const float fadingRange = fadingBeginPos - fadingEndPos;
        const float pixelVolume = _config->colorVolume * (_pos - fadingEndPos) / fadingRange;
        pixelColor = CHSV(_config->colorHue, 255, pixelVolume);
      }
      // else: particle off

      pixelColor = addGlitter(animation, pixelColor, fadingBeginPos);
      if (pixelPos != _lastPixelPos)
      {
        animation.safePixel(_lastPixelPos) = pixelColor;
      }
      animation.safePixel(pixelPos) = pixelColor;
    }

    CRGB addGlitter(AnimationBaseFL &animation, CRGB pixelColor, float fadingBeginPos = 0.0)
    {
      // Twinkling instead of a fading trail?
      if (_config->glitterType == GLITTER_TWINKLE)
      {
        return (random8(100) < 80) ? CRGB::Black : pixelColor;
      }

      // no other glitter needed?
      if (_config->glitterDuration <= 0.0)
      {
        return pixelColor;
      }

      // Glitter with complementary color?
      if (_config->glitterType == GLITTER_COMP)
      {
        const float glitterEndPos = _apexPos * (1.0 - _config->glitterDuration);

#ifdef FIREWORK_DEBUG
        if (glitterEndPos > 0.0)
        {
          animation.safePixel(glitterEndPos * animation.ledCount) = CRGB(0, 16, 0);
        }
#endif

        if (_pos >= glitterEndPos)
        {
          if (random8(100) < 12)
          {
            return CHSV(_config->colorHue + 128, 255, 255);
          }
        }
        return pixelColor;
      }

      // the following glitter types are not applied before fading has begun
      if (fadingBeginPos <= 0.0)
      {
        return pixelColor;
      }

      const float glitterBeginPos = fadingBeginPos + _config->glitterDuration / 10;
      const float glitterEndPos = glitterBeginPos - (glitterBeginPos * _config->glitterDuration);

#ifdef FIREWORK_DEBUG
      if (glitterBeginPos > 0.0)
      {
        animation.safePixel(glitterBeginPos * animation.ledCount) = CRGB(0, 16, 0);
      }
      if (glitterEndPos > 0.0)
      {
        animation.safePixel(glitterEndPos * animation.ledCount) = CRGB(0, 16, 0);
      }
#endif

      if (_pos <= glitterBeginPos &&
          _pos >= glitterEndPos)
      {
        if (random(100) < 20)
        {
          switch (_config->glitterType)
          {
          case GLITTER_SAME:
            pixelColor = CHSV(_config->colorHue, 255, 255);
            break;

          case GLITTER_WHITE:
            pixelColor = CRGB(255, 255, 255);
            break;

          case GLITTER_RED:
            pixelColor = CRGB(255, 0, 0);
            break;

          case GLITTER_GREEN:
            pixelColor = CRGB(0, 255, 0);
            break;

          case GLITTER_BLUE:
            pixelColor = CRGB(0, 0, 255);
            break;

          case GLITTER_YELLOW:
            pixelColor = CRGB(255, 255, 0);
            break;

          case GLITTER_CYAN:
            pixelColor = CRGB(0, 255, 255);
            break;

          case GLITTER_PURPLE:
            pixelColor = CRGB(255, 0, 255);
            break;

          default:
            break;
          }
        }
      }

      return pixelColor;
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
