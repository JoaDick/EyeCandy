#pragma once
/*******************************************************************************

MIT License

Copyright (c) 2025 Joachim Dick

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

  /** A utility pattern for identifying the strip's color order and number of LEDs.
   * First, it draws a (small) red, (medium) green and (large) blue block at the start of the strip.
   * Then it turns on selected LEDs every second, with a specific color, making it easy to
   * determine how many LEDs the whole strip contains.
   *
   * The turn-on sequence is as follows:
   * - every 100th LED purple
   * - every 50th LED cyan
   * - every 10th LED yellow
   * - every 5th LED blue
   * - every odd LED green
   * - every even LED red
   *
   * Somehow inspired by
   * https://github.com/FastLED/FastLED/blob/master/examples/RGBCalibrate/RGBCalibrate.ino
   */
  class StripYardstick
      : public AnimationModelBase
  {
  public:
    /** Constructor.
     * @param ledStrip  The LED strip.
     */
    explicit StripYardstick(FastLedStrip ledStrip)
        : AnimationModelBase(1000, ledStrip)
    {
    }

  private:
    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      if (mustShowRGB)
        showRGB();
      else
        showRuler();
    }

    void showRGB()
    {
      dumpPixelColorOrder(strip, 5);
      if (counter >= 3)
      {
        counter = 0;
        mustShowRGB = false;
      }
    }

    void showRuler()
    {
      // every 100th LED
      if (counter > 0)
        showLed(100);
      // every 50th LED
      if (counter > 1)
        showLed(50);
      // every 10th / 60th LED
      if (counter > 2)
        showLed(10, 50);
      // every 20th / 70th LED
      if (counter > 3)
        showLed(20, 50);
      // every 30th / 80th LED
      if (counter > 4)
        showLed(30, 50);
      // every 40th / 90th LED
      if (counter > 5)
        showLed(40, 50);
      // every 5th LED
      if (counter > 6)
        showLed(5);
      // every 1st / 6th LED
      if (counter > 7)
        showLed(1, 5);
      // every 2nd / 7th LED
      if (counter > 8)
        showLed(2, 5);
      // every 3rd / 8th LED
      if (counter > 9)
        showLed(3, 5);
      // every 4th / 9th LED
      if (counter > 10)
        showLed(4, 5);
    }

    void showLed(uint8_t first, uint8_t step = 0)
    {
      if (step == 0)
      {
        step = first;
      }
      for (uint16_t i = first; i <= strip.ledCount(); i += step)
      {
        strip[i - 1] = getColor(i);
      }
    }

    CRGB getColor(int16_t pos)
    {
      // every 100th LED purple
      if ((pos % 100) == 0)
      {
        return CRGB(192, 0, 192);
      }
      // every 50th LED cyan
      else if ((pos % 50) == 0)
      {
        return CRGB(0, 160, 64);
      }
      // every 10th LED yellow
      else if ((pos % 10) == 0)
      {
        return CRGB(64, 64, 0);
      }
      // every 5th LED blue
      else if ((pos % 5) == 0)
      {
        return CRGB(0, 0, 64);
      }
      // every odd LED green
      else if (pos & 1)
      {
        return CRGB(0, 16, 0);
      }
      // every even LED red
      return CRGB(16, 0, 0);
    }

    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      if (++counter > 21)
      {
        counter = 0;
        mustShowRGB = true;
      }
    }

  private:
    uint8_t counter = 0;
    bool mustShowRGB = true;
  };

} // namespace EC
