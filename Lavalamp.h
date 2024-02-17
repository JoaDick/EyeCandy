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

//------------------------------------------------------------------------------

namespace EC
{

  /** Animation of a relaxing Lava-Lamp.
   * With Bubbles moving calmingly up and down the LED strip.
   */
  class Lavalamp
      : public AnimationBase
  {
  public:
    /// Determines how fast the color changes.
    uint8_t hueSpeed;
    static uint8_t hueSpeed_default() { return 20; }

    /// Animation speed.
    float speed = 1.0;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param hueSpeed  How fast the color changes.
     * @param startHue  Initial color.
     */
    explicit Lavalamp(FastLedStrip ledStrip,
                      uint8_t hueSpeed = hueSpeed_default(),
                      uint8_t startHue = random8())
        : AnimationBase(ledStrip), hueSpeed(hueSpeed), _baseHue(startHue)
    {
      _baseHue *= _hueScaleFactor;
      // to avoid a boring startup
      _blobs[0].isActive = true;
      _blobs[1].isActive = true;
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      updateBlobs();
      strip.fill(CRGB::Black);

      const CRGB color = CHSV(_baseHue / _hueScaleFactor, 255, 255);
      strip.normLineAbs(0, _floor, color);
      strip.normLineAbs(_ceiling, 1.0, color);

      for (auto i = 0; i < _numBlobs; ++i)
      {
        if (_blobs[i].isActive)
        {
          strip.normLineRel(_blobs[i].pos, -_blobs[i].size, color);
        }
        else
        {
          // strip.normPixel(_blobs[i].pos) = color / 16;
        }
      }

      strip.blur(75);
      strip.blur(150);
    }

    void updateBlobs()
    {
      _ceiling = 1.0 - (beatsin(1.37, 0.05) + 0.01);
      _floor = beatsin(1.93, 0.1) + 0.05;

      for (auto i = 0; i < _numBlobs; ++i)
      {
        const auto size = beatsin(0.97 + i * 2.43, 0.4 / _numBlobs) + 0.02 + i / 100.0;
        const auto posBase = beatsin(2.71 - i / 15.7, 1.0 + size, i / float(_numBlobs));
        const auto posStretch = beatsin(3.69 + i / 11.1, 0.8) + 0.4;
        const auto pos = posBase * posStretch;

        _blobs[i].size = size;
        _blobs[i].pos = pos;
        if (pos - size > _ceiling)
        {
          _blobs[i].isActive = random8() < 150;
        }
        if (pos < _floor)
        {
          _blobs[i].isActive = random8() < 150;
        }
      }

      _baseHue += hueSpeed;
    }

  private:
    struct Blob
    {
      float pos = 0.0;
      float size = 0.0;
      bool isActive = false;
    };

    float beatsin(float bpm, float maxVal, float phaseOffset = 0.0)
    {
      float x = beatsin88(bpm * 256 * speed, 0, 0xFFFF, 0, phaseOffset * 0xFFFF);
      x /= 0xFFFF;
      x *= maxVal;
      return x;
    }

  private:
    static const uint8_t _numBlobs = 5;
    static const uint8_t _hueScaleFactor = 128;

    Blob _blobs[_numBlobs];
    float _ceiling;
    float _floor;
    uint16_t _baseHue;
  };

} // namespace EC
