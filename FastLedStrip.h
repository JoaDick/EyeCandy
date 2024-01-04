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
     * @param reversed  Draw the strip's content in reverse direction.
     * @note A negative value for \a ledCount has the same effect as if \a reversed was true.
     */
    FastLedStrip(CRGB *ledStrip, int16_t ledCount, bool reversed = false)
        : m_stripData(ledStrip), m_size(ledCount), m_reversed(reversed)
    {
      if (m_size < 0)
      {
        m_size = -m_size;
        m_reversed ^= true;
      }
      else if (m_size == 0)
      {
        m_size = 1;
      }
    }

    /** Access the pixel at position \a index.
     * If \a index is not within the LED strip, a dummy-pixel is returned, and
     * all of this strip's LEDs will turn red as error feedback.
     * Use pixel() when such "off-strip-pixel-access" is intended by the Animation.
     */
    CRGB &operator[](int16_t index)
    {
      convertToStripIndex(index);
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
      convertToStripIndex(index);
      if (index < 0 || index >= m_size)
      {
        return s_trashPixel;
      }
      return m_stripData[index];
    }

    /** Access the pixel at the normalized position \a pos ("off-strip-pixels" are allowed).
     * If \a index is not within the LED strip, a dummy-pixel is returned
     * (i.e. the pixel's setting will be ignored). \n
     * This may be useful for Animations where something "drops off" the strip.
     * @param pos  Normalized pixel position.
     *             The valid range for drawing is 0.0 <= \a pos < 1.0
     */
    CRGB &normPixel(float pos)
    {
      // TODO: Should pos == 1.0 also be a valid pixel (i.e. the last one)?
      return pixel(pos * (m_size - 1));
    }

    /** Access the optional pixel at the normalized position \a pos.
     * Same as normPixel(), but only positive values of \a pos are valid.
     * This means that the (optional) pixel at exactly \a pos == 0.0 will \e not be drawn. \n
     * This may be useful when the Animation wants to implement something like a simple
     * "invalid" or "muted" state of a pixel algorithm.
     */
    CRGB &optPixel(float pos)
    {
      return pos <= 0.0 ? s_trashPixel : normPixel(pos);
    }

    /** Draw a line in the given \a color from \a firstIndex to \a lastIndex.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * Both \a firstIndex and \a lastIndex are included in the line.
     */
    void lineAbs(int16_t firstIndex, int16_t lastIndex, const CRGB &color)
    {
      int16_t firstStripIndex = toStripIndex(firstIndex);
      int16_t lastStripIndex = toStripIndex(lastIndex);

      if (firstStripIndex > lastStripIndex)
      {
        // std::swap(firstStripIndex, lastStripIndex);
        auto temp = firstStripIndex;
        firstStripIndex = lastStripIndex;
        lastStripIndex = temp;
      }
      if (firstStripIndex < 0)
      {
        // start AND end off the strip?
        if (lastStripIndex < 0)
        {
          return;
        }
        firstStripIndex = 0;
      }
      if (lastStripIndex >= m_size)
      {
        // start AND end off the strip?
        if (firstStripIndex >= m_size)
        {
          return;
        }
        lastStripIndex = m_size - 1;
      }
      drawLine(color, firstStripIndex, lastStripIndex);
    }

#if (0)
    /** Draw a line in the given \a color with the given \a length, starting at \a startIndex.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * @note A negative value for \a length will draw in the opposite direction.
     */
    void lineRel(int16_t startIndex, int16_t length, const CRGB &color)
    {
      if (length)
      {
        lineAbs(startIndex, startIndex + length, color);
      }
    }
#endif

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

    /// Same as FastLed's fadeLightBy()
    void fadeLightBy(uint8_t fadeBy)
    {
      // copied implementation of FastLed's fadeLightBy()
      nscale8_video(m_stripData, m_size, 255 - fadeBy);
    }

    /// Get a new strip with the same underlying LED pixel array, but reversed drawing direction.
    FastLedStrip getReversedStrip()
    {
      return FastLedStrip(m_stripData, m_size, m_reversed ^ true);
    }

#if (0)
    /** Get a new strip that contains only a part of stis strip's underlying LED pixel array.
     * The resulting strip's boundaries are checked, and if necessary restricted to the dimensions of this strip.
     * Even when restricted, the returned sub-strip contains at least one visible pixel.
     * @note When \a size is negative, \a offset is interpreted as the end of the strip,
     * and its start will be \a size pixels before. Also the new strip's direction is reversed.
     * @param offset    New strip starts at this LED.
     * @param size      Number of LEDs in the new strip.
     * @param reversed  Draw the new strip's content in reverse direction.
     */
    FastLedStrip getSubStrip(int16_t offset, int16_t size, bool reversed = false)
    {
      if (size == 0)
      {
        return FastLedStrip(m_stripData, 1);
      }
      if (convertToStripIndex(offset))
      {
        size = -size;
      }
      if (size < 0)
      {
        reversed ^= true;
        size = -size;
        offset -= size;
      }
      if (offset < 0)
      {
        size -= -offset;
        offset = 0;
      }
      if (offset >= m_size)
      {
        return FastLedStrip(&m_stripData[m_size - 1], 1);
      }
      if (offset + size >= m_size)
      {
        size = m_size - offset - 1;
      }
      return FastLedStrip(&m_stripData[offset], size, m_reversed ^ reversed);
    }
#endif

    /// This strip's number of LEDs.
    int16_t ledCount()
    {
      return m_size;
    }

    /** Get the strip's underlying LED pixel array.
     * Use this method only when calling FastLed functions directly; mostly together with ledCount().
     * @note The strip's "reversed" property has no effect when this method is used!
     */
    CRGB *rawStripData()
    {
      return m_stripData;
    }

  private:
    int16_t toStripIndex(int16_t index)
    {
      return m_reversed ? m_size - 1 - index : index;
    }

    bool convertToStripIndex(int16_t &index)
    {
      index = toStripIndex(index);
      return m_reversed;
    }

    void drawLine(const CRGB &color, int16_t firstStripIndex, int16_t lastStripIndex)
    {
      CRGB *firstPixel = &m_stripData[firstStripIndex];
      CRGB *lastPixel = &m_stripData[lastStripIndex];
      while (firstPixel <= lastPixel)
      {
        *(firstPixel++) = color;
      }
    }

  private:
    static CRGB s_trashPixel;
    CRGB *m_stripData;
    int16_t m_size;
    bool m_reversed;
  };

} // namespace EC
