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

#include <FastLED.h>

//------------------------------------------------------------------------------

namespace EC
{

  /** Interface of a Color Generator.
   * Color Generators provide an easy way to add different color variations to
   * an Animation algorithm. So to say, the algoritm defines the shape of the
   * Animation, whereas the Color Generator provides the coloring scheme. \n
   * This is achieved via dependency injection, so any Color Generator capable
   * Animation can be equipped with whatever coloring scheme you desire. \n
   * A simple example of a Color Generator is a rainbow, where the generated
   * color varies through the rainbow colors over time. \n
   * The generateColor() method also mimics FastLED's color palettes interface,
   * where you can provide the desired brightness of the color, and a custom
   * index. In the rainbow example, the index can be used as an offset to the
   * rainbow color sequence, while the base rainbow color is still constantly
   * changing over time. \n
   * For VU meters, you can e.g. use the volume level as index to the Color
   * Generator - a very nice effect.
   */
  class ColorGenerator
  {
  public:
    /// Destructor.
    virtual ~ColorGenerator() = default;

    /** Get the generated color.
     * This method must be implemented by all child classes.
     * @param index  Index of the color (if supported).
     * @param brightness  Brightness of the color.
     */
    virtual CRGB generateColor(uint8_t index = 0, uint8_t brightness = 255) = 0;
  };

  /// A Color Generator with 3 rainbow cycles per minute.
  ColorGenerator &DefaultRainbowCG();

} // namespace EC
