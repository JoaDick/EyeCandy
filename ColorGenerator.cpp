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

#include "ColorGenerator.h"
#include <Arduino.h>
#include "AnimationBaseFL.h"

//------------------------------------------------------------------------------

namespace
{

  class RainbowColorGenerator
      : public EC::ColorGenerator
  {
    CRGB generateColor(uint8_t index, uint8_t brightness)
    {
      // this will result in 3 rainbow cycles per minute
      uint32_t hue = (millis() * 3 * 255) / (1000ul * 60);
      hue += index;
      hue %= 255;
      // put more emphasis on the red'ish colors
      hue = EC::AnimationBaseFL::redShift(hue);
      return CHSV(hue, 255, brightness);
    }
  };

} // namespace

namespace EC
{

  ColorGenerator &DefaultRainbowCG()
  {
    static RainbowColorGenerator inst;
    return inst;
  }

} // namespace EC
