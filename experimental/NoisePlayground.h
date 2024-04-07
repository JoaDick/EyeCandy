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

  /// Scratchpad for experimenting with noise functions.
  class NoisePlayground
      : public AnimationBase
  // : public AnimationModelBase
  {
  public:
    /** Default fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     */
    static constexpr uint8_t fadeRate_default = 3;

    /// Color of the Animation.
    CRGB color;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param color  Draw the glitter with this color.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    NoisePlayground(FastLedStrip ledStrip,
                    bool overlayMode,
                    CRGB color = CRGB(64, 64, 64),
                    uint8_t fadeRate = fadeRate_default)
        : AnimationBase(ledStrip, overlayMode, fadeRate),
          // : AnimationModelBase(3000, ledStrip, overlayMode, fadeRate),
          color(color)
    {
    }

  private:
// implement this method only if needed (e.g. for Patterns)
// --> but then also consider changing the base class to PatternBase
#if (0)
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
    }
#endif

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      showNoise_ColorCloudsVol(currentMillis);
      // showNoise_uint16(currentMillis);
      // showNoise_float(currentMillis);
    }

    void showNoise_ColorCloudsVol(uint32_t currentMillis)
    {
      const uint16_t rawSample = inoise16(currentMillis * 256, currentMillis * 256);

      long sample = map(rawSample, 25000, 47500, 0, 255);

      if (sample > _maxSample_uint8)
      {
        _maxSample_uint8 = sample;
      }
      if (sample < _minSample_uint8)
      {
        _minSample_uint8 = sample;
      }

      // sample = constrain(sample, 0, 255);

      strip.n_pixel(_maxSample_uint8 / float(0xFF)) = CRGB(128, 0, 0);
      strip.n_pixel(_minSample_uint8 / float(0xFF)) = CRGB(0, 0, 128);
      strip.n_pixel(_lastSample_uint8 / float(0xFF)) = CRGB(0, 128, 0);
      strip.n_pixel(sample / float(0xFF)) = CRGB(255, 255, 0);

      if (1)
      {
        Serial.print(F(">-:"));
        Serial.println(0xFF);
        Serial.print(F(">+:"));
        Serial.println(0x00);
        Serial.print(F(">noise:"));
        Serial.println(sample);
        Serial.print(F(">max:"));
        Serial.println(_maxSample_uint8);
        Serial.print(F(">min:"));
        Serial.println(_minSample_uint8);
      }

      _lastSample_uint8 = sample;
    }

    void showNoise_uint16(uint32_t currentMillis)
    {
      const uint16_t sample = inoise16(currentMillis * 128, currentMillis * 128);

      if (sample > _maxSample_uint16)
      {
        _maxSample_uint16 = sample;
      }
      if (sample < _minSample_uint16)
      {
        _minSample_uint16 = sample;
      }

      strip.n_pixel(_maxSample_uint16 / float(0xFFFF)) = CRGB(128, 0, 0);
      strip.n_pixel(_minSample_uint16 / float(0xFFFF)) = CRGB(0, 0, 128);
      strip.n_pixel(_lastSample_uint16 / float(0xFFFF)) = CRGB(0, 128, 0);
      strip.n_pixel(sample / float(0xFFFF)) = CRGB(255, 255, 0);

      if (1)
      {
        Serial.print(F(">-:"));
        Serial.println(0xFFFF);
        Serial.print(F(">+:"));
        Serial.println(0x0000);
        Serial.print(F(">noise:"));
        Serial.println(sample);
        Serial.print(F(">max:"));
        Serial.println(_maxSample_uint16);
        Serial.print(F(">min:"));
        Serial.println(_minSample_uint16);
      }

      _lastSample_uint16 = sample;
    }

    void showNoise_float(uint32_t currentMillis)
    {
      const uint16_t rawSample = inoise16(currentMillis * 128, currentMillis * 128);
      const float sample = float(rawSample) / 0xFFFF;

      if (sample > _maxSample_float)
      {
        _maxSample_float = sample;
      }
      if (sample < _minSample_float)
      {
        _minSample_float = sample;
      }

      strip.n_pixel(_maxSample_float) = CRGB(128, 0, 0);
      strip.n_pixel(_minSample_float) = CRGB(0, 0, 128);
      strip.n_pixel(_lastSample_float) = CRGB(0, 128, 0);
      strip.n_pixel(sample) = CRGB(255, 255, 0);

      if (1)
      {
        const float scale = 100.0;
        Serial.print(F(">-:"));
        Serial.println(0.0 * scale);
        Serial.print(F(">+:"));
        Serial.println(1.0 * scale);
        Serial.print(F(">noise:"));
        Serial.println(sample * scale);
        Serial.print(F(">max:"));
        Serial.println(_maxSample_float * scale);
        Serial.print(F(">min:"));
        Serial.println(_minSample_float * scale);
      }

      _lastSample_float = sample;
    }

    // // delete this method if not needed
    // // --> then also change the base class to AnimationBase
    // #if (1)
    //     /// @see AnimationModelBase::updateModel()
    //     void updateModel(uint32_t currentMillis) override
    //     {
    //       _enableExtras ^= true;
    //     }
    // #endif

  private:
    int16_t _lastSample_uint8 = 0x80;
    int16_t _maxSample_uint8 = 0x80;
    int16_t _minSample_uint8 = 0x80;

    uint16_t _lastSample_uint16 = 0x8000;
    uint16_t _maxSample_uint16 = 0x8000;
    uint16_t _minSample_uint16 = 0x8000;

    float _lastSample_float = 0.5;
    float _maxSample_float = 0.5;
    float _minSample_float = 0.5;
  };

} // namespace EC
