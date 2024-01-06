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
        : m_stripData(ledStrip), m_sizeNrev(ledCount < 0 ? -ledCount : ledCount)
    {
      if (ledCount < 0)
      {
        reversed ^= true;
      }
      else if (ledCount == 0)
      {
        m_sizeNrev = 1;
      }
      if (reversed)
      {
        m_sizeNrev |= 0x8000;
      }
    }

    /** Access the pixel at position \a index.
     * If \a index is not within the LED strip, a dummy-pixel is returned, and
     * all of this strip's LEDs will turn red as error feedback.
     * Use pixel() when such "off-strip-pixel-access" is intended by the Animation.
     */
    CRGB &operator[](int16_t index)
    {
      const auto size = getSize();
      convertToStripIndex(index);
      if (index < 0 || index >= size)
      {
        drawLine(CRGB(64, 0, 0), 0, size - 1);
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
      const auto size = getSize();
      convertToStripIndex(index);
      if (index < 0 || index >= size)
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
      return pixel(pos * (getSize() - 1));
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
      const auto size = getSize();
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
      if (lastStripIndex >= size)
      {
        // start AND end off the strip?
        if (firstStripIndex >= size)
        {
          return;
        }
        lastStripIndex = size - 1;
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
      drawLine(color, 0, getSize() - 1);
    }

    /// Same as FastLed's fadeToBlackBy()
    void fadeToBlackBy(uint8_t fadeBy)
    {
      // copied implementation of FastLed's fadeToBlackBy()
      nscale8(m_stripData, getSize(), 255 - fadeBy);
    }

    /// Same as FastLed's fadeLightBy()
    void fadeLightBy(uint8_t fadeBy)
    {
      // copied implementation of FastLed's fadeLightBy()
      nscale8_video(m_stripData, getSize(), 255 - fadeBy);
    }

    /// Get a new strip with the same underlying LED pixel array, but reversed drawing direction.
    FastLedStrip getReversedStrip()
    {
      return FastLedStrip(m_stripData, getSize(), getReversed() ^ true);
    }

    /** Get a new strip consisting of the first half of the underlying LED pixel array.
     * Useful e.g. for mirroring that half into the full strip after drawing its Animation.
     * @param reversed  Draw the new strip's content in reverse direction.
     * @see copyUp()
     */
    FastLedStrip getHalfStrip(bool reversed = false)
    {
      const auto size = getSize();
      const bool revd = getReversed();

      // current strip is normal
      if (!revd)
      {
        //      pixels: | even |      |  odd  |
        //       index: |0--->5|      |0---->6|
        // this size 6: [012345]   7: [0123456]
        //  new size 3:  |||       4:  ||||
        //   new strip: [012]         [0123]
        //   new index: |0>2|         |0->3|     reversed = false
        //   new index: |2<0|         |3<-0|     reversed = true
        return FastLedStrip(&m_stripData[0], (size + 1) / 2, revd ^ reversed);
      }
      // current strip is reversed
      else
      {
        //      pixels: | even |      |  odd  |
        //       index: |5<---0|      |6<----0|
        // this size 6: [012345]   7: [0123456]
        //  new size 3:     |||    4:     ||||
        //   new strip:    [012]         [0123]
        //   new index:    |2<0|         |3<-0|     reversed = false
        //   new index:    |0>2|         |0->3|     reversed = true
        return FastLedStrip(&m_stripData[size / 2], (size + 1) / 2, revd ^ reversed);
      }
    }

    /** Get a new strip that contains only a part of stis strip's underlying LED pixel array.
     * The resulting strip's boundaries are checked, and if necessary restricted to the dimensions of this strip.
     * Even when restricted, the returned sub-strip contains at least one visible pixel.
     * @note When \a size is negative, \a offset is interpreted as the end of the strip,
     * and its start will be \a size pixels before. Also the new strip's direction is reversed.
     * @param offset  New strip starts at this LED.
     * @param newSize  Number of LEDs in the new strip.
     * @param reversed  Draw the new strip's content in reverse direction.
     */
    FastLedStrip getSubStrip(int16_t offset, int16_t newSize, bool reversed = false)
    {
      const auto size = getSize();
      const bool revd = getReversed();

      if (newSize == 0)
      {
        return FastLedStrip(m_stripData, 1);
      }
      if (convertToStripIndex(offset))
      {
        newSize = -newSize;
      }
      if (newSize < 0)
      {
        reversed ^= true;
        newSize = -newSize;
        offset -= newSize;
      }
      if (offset < 0)
      {
        newSize -= -offset;
        offset = 0;
      }
      if (offset >= size)
      {
        return FastLedStrip(&m_stripData[size - 1], 1);
      }
      if (offset + newSize >= size)
      {
        newSize = size - offset - 1;
      }
      return FastLedStrip(&m_stripData[offset], newSize, revd ^ reversed);
    }

    /** Copy (and optionally mirror) the lower half of this strip into its upper half.
     * Any existing content in the upper half is overwritten.
     * @param mirrored  false = straight copying, true = mirror the content
     * @see getHalfStrip()
     */
    void copyUp(bool mirrored)
    {
      const auto size = getSize();
      const bool revd = getReversed();

      // copying and mirroring
      if (mirrored)
      {
        // copy & mirror lower half up
        if (!revd)
        {
          // pixels: | even |      |  odd  |
          //  index: |0--->5|      |0---->6|
          // size 6: [012xxx]   7: [0123xxx]
          //            |d->e         | d->e
          //          <-s           <-s
          // result: [012210]      [0123210]
          CRGB *dst = &m_stripData[(size + 1) / 2];
          CRGB *end = &m_stripData[size];
          CRGB *src = &m_stripData[(size / 2) - 1];
          while (dst < end)
          {
            *(dst++) = *(src--);
          }
        }
        // copy & mirror upper half down
        else
        {
          // pixels: | even |      |  odd  |
          //  index: |5<---0|      |6<----0|
          // size 6: [xxx345]   7: [xxx3456]
          //          d->e |        d->e  |
          //             <-s            <-s
          // result: [543345]      [6543456]
          CRGB *dst = &m_stripData[0];
          CRGB *end = &m_stripData[size / 2];
          CRGB *src = &m_stripData[size - 1];
          while (dst < end)
          {
            *(dst++) = *(src--);
          }
        }
      }
      // only copying without mirroring
      else
      {
        // copy lower half up
        if (!revd)
        {
          // pixels: | even |      |  odd  |
          //  index: |0--->5|      |0---->6|
          // size 6: [012xxx]   7: [0123xxx]
          //          |  d->e       |   d->e
          //          s->           s->
          // result: [012012]      [0123012]
          CRGB *dst = &m_stripData[(size + 1) / 2];
          CRGB *end = &m_stripData[size];
          CRGB *src = &m_stripData[0];
          while (dst < end)
          {
            *(dst++) = *(src++);
          }
        }
        // copy upper half down
        else
        {
          // pixels: | even |      |  odd  |
          //  index: |5<---0|      |6<----0|
          // size 6: [xxx345]   7: [xxx3456]
          //          d->e          d->e|
          //             s->            s->
          // result: [345345]      [4563456]
          CRGB *dst = &m_stripData[0];
          CRGB *end = &m_stripData[size / 2];
          CRGB *src = &m_stripData[(size + 1) / 2];
          while (dst < end)
          {
            *(dst++) = *(src++);
          }
        }
      }
    }

    /// This strip's number of LEDs.
    int16_t ledCount()
    {
      return getSize();
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
    int16_t getSize()
    {
      return m_sizeNrev & 0x7FFF;
    }

    bool getReversed()
    {
      return m_sizeNrev & 0x8000;
    }

    int16_t toStripIndex(int16_t index)
    {
      return getReversed() ? getSize() - 1 - index : index;
    }

    bool convertToStripIndex(int16_t &index)
    {
      index = toStripIndex(index);
      return getReversed();
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
    uint16_t m_sizeNrev;
  };

  /// Put more emphasis on the red'ish colors.
  inline uint8_t redShift(uint8_t hue)
  {
    return cos8(128 + hue / 2);
  }

} // namespace EC
