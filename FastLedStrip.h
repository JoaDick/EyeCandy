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

#include <FastLED.h>

//------------------------------------------------------------------------------

namespace EC
{

  /** A thin wrapper for manipulating FastLed LED pixel arrays.
   * Accessing the individual LED pixels through this wrapper - e.g. pixel() or
   * the index operator [] - are always boundary-checked to prevent memory corruption.
   * Additionally, it offers various helper functions for manipulating the entire strip.
   */
  class FastLedStrip
  {
  public:
    /** Constructor.
     * @param ledStrip  The strip's underlying LED pixel array.
     * @param ledCount  Number of LEDs.
     */
    FastLedStrip(CRGB *ledStrip, int16_t ledCount)
        : m_stripData(ledStrip), m_size(ledCount > 0 ? ledCount : 1)
    {
    }

    /** Access the pixel at position \a index.
     * If \a index is not within the LED strip, a dummy-pixel is returned, and
     * all of this strip's LEDs will turn red as error feedback.
     * Use pixel() when such "off-strip-pixel-access" is intended by the Animation.
     */
    CRGB &operator[](int16_t index)
    {
      if (index < 0 || index >= m_size)
      {
        drawLine(CRGB(64, 0, 0), 0, m_size - 1);
        return s_trashPixel;
      }
      return m_stripData[index];
    }

    /** Access the pixel at position \a index ("off-strip-pixels" are allowed).
     * If \a index is not within the LED strip, a dummy-pixel is returned
     * (i.e. the pixel's setting will be ignored). \n
     * This may be useful for Animations where something "drops off" the strip.
     * @note Don't abuse this feature to sweep programming errors under the rug!
     */
    CRGB &pixel(int16_t index)
    {
      if (index < 0 || index >= m_size)
      {
        return s_trashPixel;
      }
      return m_stripData[index];
    }

    /** Draw a line in the given \a color from \a firstIndex to \a lastIndex.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * Both \a firstIndex and \a lastIndex are included in the line.
     */
    void lineAbs(int16_t firstIndex, int16_t lastIndex, const CRGB &color)
    {
      if (firstIndex > lastIndex)
      {
        lineAbs(lastIndex, firstIndex, color);
        return;
      }
      if (firstIndex < 0)
      {
        // start AND end off the strip?
        if (lastIndex < 0)
        {
          return;
        }
        firstIndex = 0;
      }
      if (lastIndex >= m_size)
      {
        // start AND end off the strip?
        if (firstIndex >= m_size)
        {
          return;
        }
        lastIndex = m_size - 1;
      }
      drawLine(color, firstIndex, lastIndex);
    }

    /** Draw a line in the given \a color with the given \a length, starting at \a startIndex.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     */
    void lineRel(int16_t startIndex, int16_t length, const CRGB &color)
    {
      if (length)
      {
        lineAbs(startIndex, startIndex + length, color);
      }
    }

    /// Fill the entire strip with the given \a color.
    void fillSolid(const CRGB &color)
    {
      drawLine(color, 0, m_size - 1);
    }

    /// Same as FastLed's fadeToBlackBy()
    void fadeToBlackBy(uint8_t fadeBy)
    {
      // copied implementation of FastLed's fadeToBlackBy()
      nscale8(m_stripData, m_size, 255 - fadeBy);
    }

    /** Get a new strip that contains only a part of stis strip's underlying LED pixel array.
     * @param offset  New strip starts at this LED.
     * @param size    Number of LEDs in the new strip.
     * The resulting strip's boundaries are checked, and if necessary restricted to the dimensions of this strip.
     * Even when restricted, the returned sub-strip contains at least one visible pixel.
     */
    FastLedStrip getSubStrip(int16_t offset, int16_t size)
    {
      if (offset < 0)
      {
        size -= -offset;
        offset = 0;
      }
      if (size <= 0)
      {
        return FastLedStrip(m_stripData, 1);
      }
      if (offset >= m_size)
      {
        return FastLedStrip(&m_stripData[m_size - 1], 1);
      }
      if (offset + size >= m_size)
      {
        size = m_size - offset - 1;
      }
      return FastLedStrip(&m_stripData[offset], size);
    }

    /// This strip's number of LEDs.
    int16_t ledCount()
    {
      return m_size;
    }

    /** Get the strip's underlying LED pixel array.
     * Use this method only when calling FastLed functions directly; mostly together with ledCount().
     */
    CRGB *rawStripData()
    {
      return m_stripData;
    }

  private:
    void drawLine(const CRGB &color, int16_t firstPixelIndex, int16_t lastPixelIndex)
    {
      CRGB *first = &m_stripData[firstPixelIndex];
      CRGB *last = &m_stripData[lastPixelIndex];
      while (first <= last)
      {
        *(first++) = color;
      }
    }

  private:
    static CRGB s_trashPixel;
    CRGB *m_stripData;
    const int16_t m_size;
  };

} // namespace EC
