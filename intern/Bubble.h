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

#include "FastLedStrip.h"
#include "MathUtils.h"

//------------------------------------------------------------------------------

namespace EC
{
  /// One Bubble for the Bubbles Animation.
  class Bubble
  {
  public:
    void show(FastLedStrip &strip)
    {
#ifdef BUBBLES_DEBUG
      // if (_debugPos_vMax > 0.0)
      // {
      //   strip.normPixel(_debugPos_vMax * animation.ledCount) = CRGB(64, 0, 0);
      // }
#endif

      uint8_t paletteIndex = uint8_t(3 * 255 * _pos); // DRAFT
      uint8_t pixelVolume = _volumeMax;               // DRAFT

      if (pixelVolume)
      {
        strip.n_pixel(_pos) = ColorFromPalette(OceanColors_p, paletteIndex, pixelVolume);
      }

#ifdef BUBBLES_DEBUG
      Serial.print(F(" paletteIndex:"));
      Serial.print(paletteIndex);
      Serial.println();
#endif
    }

    void update(uint16_t delta_ms)
    {
      float deltaT = delta_ms * 0.001;
      _vel += deltaT * _acc;
      _pos += deltaT * _vel;

      if (_acc > 0.0)
      {
        // max. rising speed reached?
        if (_vel > _vMax)
        {
          _acc = 0.0;
#ifdef BUBBLES_DEBUG
          _debugPos_vMax = _pos;
#endif
        }
      }

      // reached the top?
      if (_pos > 1.0)
      {
        _acc = randomF(0.1, 0.3);
        _vel = 0.0;
        _pos = randomF(0.0, 0.25);

        _vMax = randomF(0.2, 0.4);
        _volumeMax = 64 + random8(256 - 64);
      }
    }

  private:
    // acceleration
    float _acc = 0.0;

    // velocity: 1.0 = 100% of strip per second
    float _vel = 0.5;

    // position: 1.0 = top end of strip
    float _pos = randomF(0.0, 1.0);

    uint8_t _volumeMax = 0;
    float _vMax = 0.0;

#ifdef BUBBLES_DEBUG
    float _debugPos_vMax = 0.0;
#endif
  };

} // namespace EC
