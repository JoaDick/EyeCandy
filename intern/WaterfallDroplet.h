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
  static uint8_t _gHue = 130;

  /// A Droplet for the Waterfall Animation.
  class WaterfallDroplet
  {
  public:
    void show(AnimationBaseFL &animation)
    {
#ifdef WATERFALL_DEBUG
      if (_debugPos_vMax > 0.0)
      {
        animation.safePixel(_debugPos_vMax * animation.ledCount) = CRGB(64, 0, 0);
      }
      animation.safePixel(_fadingBeginPos * animation.ledCount) = CRGB(0, 64, 32);
      animation.safePixel(_fadingEndPos * animation.ledCount) = CRGB(0, 64, 32);
#endif

      const int16_t pixelPos = _pos * animation.ledCount;
      if (_pos > _fadingBeginPos)
      {
        animation.safePixel(pixelPos) = CHSV(_colorHue, 255, _colorVolume);
      }
      else if (_pos >= _fadingEndPos)
      {
        const float fadingRange = _fadingBeginPos - _fadingEndPos;
        const float pixelVolume = _colorVolume * (_pos - _fadingEndPos) / fadingRange;
        animation.safePixel(pixelPos) = CHSV(_colorHue, 255, pixelVolume);
      }
    }

    void update(uint16_t delta_ms)
    {
      float deltaT = delta_ms * 0.001;
      _vel += deltaT * _acc;
      _pos += deltaT * _vel;

      if (_acc < 0.0)
      {
        // max. falling speed reached?
        if (_vel < _vMax)
        {
          _acc = 0.0;
#ifdef WATERFALL_DEBUG
          _debugPos_vMax = _pos;
#endif
        }
      }

      // reached the ground?
      if (_pos <= 0.0)
      {
        _acc = randomF(-0.75, -0.5);
        _vel = randomF(0.05, 0.1);
        // _vel = 0.0;
        _pos = randomF(0.95, 1.0);

        _vMax = randomF(-0.75, -0.5);

        ++_gHue;
        // _gHue += 128;
        _colorHue = _gHue; //random8();
        _colorVolume = 127 + random8(128);

        _fadingBeginPos = randomF(0.2, 0.9);
        _fadingEndPos = randomF(-0.2, 0.1);
      }
    }

  private:
    // acceleration
    float _acc = 0.0;

    // velocity: 1.0 = 100% of strip per second
    float _vel = -0.5;

    // position: 1.0 = top end of strip
    float _pos = randomF(0.0, 1.0);

    float _vMax = 0.0;

    uint8_t _colorHue = 0;
    uint8_t _colorVolume = 0;

    float _fadingBeginPos = 0.0;
    float _fadingEndPos = 0.0;

#ifdef WATERFALL_DEBUG
    float _debugPos_vMax = 0.0;
#endif
  };

} // namespace EC