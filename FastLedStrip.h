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

  /** A thin wrapper for manipulating FastLed LED arrays.
   * Accessing the individual LED through this wrapper - e.g. pixel() or the
   * index operator [] - are always boundary-checked to prevent memory corruption.
   * Additionally, it offers various helper functions for manipulating the entire strip.
   */
  class FastLedStrip
  {
  public:
    /** Constructor.
     * @param ledArray  The strip's underlying LED pixel array.
     * @param ledCount  Number of LEDs.
     * @param reversed  Draw the strip's content in reverse direction.
     * @note A negative value for \a ledCount has the same effect as if \a reversed was true.
     */
    FastLedStrip(CRGB *ledArray, int16_t ledCount, bool reversed = false)
        : m_ledArray(ledArray), m_sizeNrev(ledCount < 0 ? -ledCount : ledCount)
    {
      if (ledCount < 0)
      {
        reversed ^= true;
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
      const auto ledIndex = toLedIndex(index);
      if (ledIndex < 0 || ledIndex >= size)
      {
        fillLedBlock(0, size - 1, CRGB(64, 0, 0));
        return s_trashPixel;
      }
      return m_ledArray[ledIndex];
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
      const auto ledIndex = toLedIndex(index);
      if (ledIndex < 0 || ledIndex >= size)
      {
        return s_trashPixel;
      }
      return m_ledArray[ledIndex];
    }

    /** Draw a line in the given \a color from \a firstIndex to \a lastIndex.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * Both \a firstIndex and \a lastIndex are included in the line.
     */
    void lineAbs(int16_t firstIndex, int16_t lastIndex, CRGB color)
    {
      const auto size = getSize();
      auto firstLedIndex = toLedIndex(firstIndex);
      auto lastLedIndex = toLedIndex(lastIndex);

      if (firstLedIndex > lastLedIndex)
      {
        // std::swap(firstLedIndex, lastLedIndex);
        auto temp = firstLedIndex;
        firstLedIndex = lastLedIndex;
        lastLedIndex = temp;
      }
      if (firstLedIndex < 0)
      {
        // start AND end off the strip?
        if (lastLedIndex < 0)
        {
          return;
        }
        firstLedIndex = 0;
      }
      if (lastLedIndex >= size)
      {
        // start AND end off the strip?
        if (firstLedIndex >= size)
        {
          return;
        }
        lastLedIndex = size - 1;
      }
      fillLedBlock(firstLedIndex, lastLedIndex, color);
    }

    /** Draw a line in the given \a color with the given \a length, starting at \a startIndex.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * @note A negative value for \a length will draw in the opposite direction.
     */
    void lineRel(int16_t startIndex, int16_t length, CRGB color)
    {
      if (length)
      {
        const auto delta = length > 0 ? (length - 1) : (length + 1);
        lineAbs(startIndex, startIndex + delta, color);
      }
    }

    /** Convert the given normalized position \a pos to its corresponding pixel index.
     * \a pos = 0.0 corresponds to the first pixel; 1.0 corresponds to the last pixel.
     * @note \a pos < 0.0 or \a pos > 1.0 result in off-strip indices (plus giving half a pixel
     * safety margin at both ends).
     * However, all methods dealing with normalized positions can tolerate these :-)
     */
    int16_t toPixelIndex(float pos) const
    {
      // example with 5 pixel:
      // pos:    ... -0.25|-0.24  0.24|0.25   0.49|0.50   0.74|0.75   0.99|1.00   1.24|1.25 ...
      // index:  ... -0.50|-0.49  0.49|0.50   1.49|1.50   2.49|2.50   3.49|3.50   4.49|4.50 ...
      // LED:   off-strip |     0     |     1     |     2     |     3     |     4     | off-strip
      return round(pos * (getSize() - 1));
    }

    /** Access the optional pixel at the normalized position \a pos.
     * Same as normPixel(), but only positive values of \a pos are valid.
     * This means that the (optional) pixel at exactly \a pos == 0.0 will \e not be drawn. \n
     * This may be useful when the Animation wants to implement something like a simple
     * "invalid" or "muted" state of a pixel algorithm.
     * @param pos  Normalized pixel position.
     * @see normPixel()
     */
    CRGB &optPixel(float pos)
    {
      return pos <= 0.0 ? s_trashPixel : normPixel(pos);
    }

    /** Access the pixel at the normalized position \a pos ("off-strip-pixels" are allowed).
     * If \a pos is not within the LED strip, a dummy-pixel is returned
     * (i.e. the pixel's setting will be ignored). \n
     * This may be useful for Animations where something "drops off" the strip.
     * @param pos  Normalized pixel position.
     * @note \a pos = 0.0 corresponds to the first pixel; 1.0 corresponds to the last pixel.
     * @see toPixelIndex()
     */
    CRGB &normPixel(float pos)
    {
      return pixel(toPixelIndex(pos));
    }

    /** Draw a line in the given \a color from the normalized position \a firstPos to \a lastPos.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * @note \a firstPos or \a lastPos = 0.0 corresponds to the first pixel; 1.0 corresponds to the last pixel.
     * @see toPixelIndex()
     */
    void normLineAbs(float firstPos, float lastPos, CRGB color)
    {
      lineAbs(toPixelIndex(firstPos), toPixelIndex(lastPos), color);
    }

    /** Draw a line in the given \a color with the given \a length, starting at the normalized position \a startPos.
     * Strip boundaries are checked, so parts of the line may even be off the strip.
     * @note \a startPos = 0.0 corresponds to the first pixel; 1.0 corresponds to the last pixel. \n
     * \a length = 1.0 corresponds to the whole strip. \n
     * A negative value for \a length will draw in the opposite direction.
     * @see toPixelIndex()
     */
    void normLineRel(float startPos, float length, CRGB color)
    {
      if (length)
      {
        const auto firstPixel = toPixelIndex(startPos);
        const auto lastPixel = toPixelIndex(startPos + length);
        lineAbs(firstPixel, lastPixel, color);
      }
    }

    /// Fill the entire strip with the given \a color.
    void fill(CRGB color)
    {
      fillLedBlock(0, getSize() - 1, color);
    }

    /** Wrapper for FastLed's fadeToBlackBy()
     * Reduce the brightness of all pixels at once.
     * This function will eventually fade all the way to black.
     */
    void fadeToBlack(uint8_t fadeBy)
    {
      fadeToBlackBy(m_ledArray, getSize(), fadeBy);
    }

    /** Wrapper for FastLed's blur1d()
     * Total light is NOT entirely conserved, so many repeated calls to 'blur' will also result in
     * the light fading, eventually all the way to black; this is by design so that it can be used
     * to (slowly) clear the LEDs to black.
     * @param blurAmount  0 = no spread at all
     *                   64 = moderate spreading
     *                  172 = maximum smooth, even spreading
     *             173..255 = wider spreading, but increasing flicker
     */
    // TODO: parameter uint8_t blurCycles = 1
    void blur(uint8_t blurAmount)
    {
      blur1d(m_ledArray, getSize(), blurAmount);
    }

    /// Shift the LED strip in the given direction, and set the remaining pixel to \a color.
    void shift(bool upward, CRGB color = CRGB::Black) { upward ? shiftUp(color) : shiftDown(color); }

    /// Shift the LED strip from begin to end, and set the first pixel to \a color.
    void shiftUp(CRGB color = CRGB::Black) { shiftLeds(getFirstLedIndex(), getLastLedIndex(), color); }

    /// Shift the LED strip from end to begin, and set the last pixel to \a color.
    void shiftDown(CRGB color = CRGB::Black) { shiftLeds(getLastLedIndex(), getFirstLedIndex(), color); }

    /// Rotate the LED strip in the given direction.
    void rotate(bool upward) { upward ? rotateUp() : rotateDown(); }

    /// Rotate the LED strip from begin to end (set the first pixel to last pixel's old content).
    void rotateUp() { shiftUp(pixel(getLastLedIndex())); }

    /// Rotate the LED strip from end to begin (set the last pixel to first pixel's old content).
    void rotateDown() { shiftDown(pixel(getFirstLedIndex())); }

    /// Get a new strip with the same underlying LED pixel array, but reversed drawing direction.
    FastLedStrip getReversedStrip() const
    {
      return FastLedStrip(m_ledArray, getSize(), getReversed() ^ true);
    }

    /** Get a new strip consisting of the first half of the underlying LED pixel array.
     * Useful e.g. for mirroring that half into the full strip after drawing its Animation.
     * @param reversed  Draw the new strip's content in reverse direction.
     * @see copyUp()
     */
    FastLedStrip getHalfStrip(bool reversed = false) const
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
        return FastLedStrip(&m_ledArray[0], (size + 1) / 2, revd ^ reversed);
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
        return FastLedStrip(&m_ledArray[size / 2], (size + 1) / 2, revd ^ reversed);
      }
    }

    /** Get a new strip that contains only a part of stis strip's underlying LED pixel array.
     * The resulting strip's boundaries are checked, and if necessary restricted to the dimensions of this strip.
     * Even when restricted, the returned sub-strip contains at least one visible pixel.
     * @note When \a size is negative, \a offset is interpreted as the end of the strip,
     * and its start will be \a size pixels before. Also the new strip's direction is reversed.
     * @param offset  New strip starts at this LED.
     * @param newSize  Number of LEDs in the new strip.
     *                 0 means all from \a offset up to the end of the strip.
     * @param reversed  Draw the new strip's content in reverse direction.
     */
    FastLedStrip getSubStrip(int16_t offset, int16_t newSize, bool reversed = false) const
    {
      const auto size = getSize();
      const bool revd = getReversed();

      if (newSize == 0)
      {
        newSize = size; // will be limited below
      }
      if (convertToLedIndex(offset))
      {
        newSize = -newSize;
      }
      if (newSize < 0)
      {
        reversed ^= true;
        newSize = -newSize;
        offset -= newSize;
      }
      if (offset >= size)
      {
        newSize = 1;
        offset = size - 1;
      }
      if (offset < 0)
      {
        newSize -= -offset;
        offset = 0;
      }
      if (offset + newSize >= size)
      {
        newSize = size - offset;
      }
      return FastLedStrip(&m_ledArray[offset], newSize, revd ^ reversed);
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
          CRGB *dst = &m_ledArray[(size + 1) / 2];
          CRGB *end = &m_ledArray[size];
          CRGB *src = &m_ledArray[(size / 2) - 1];
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
          CRGB *dst = &m_ledArray[0];
          CRGB *end = &m_ledArray[size / 2];
          CRGB *src = &m_ledArray[size - 1];
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
          CRGB *dst = &m_ledArray[(size + 1) / 2];
          CRGB *end = &m_ledArray[size];
          CRGB *src = &m_ledArray[0];
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
          CRGB *dst = &m_ledArray[0];
          CRGB *end = &m_ledArray[size / 2];
          CRGB *src = &m_ledArray[(size + 1) / 2];
          while (dst < end)
          {
            *(dst++) = *(src++);
          }
        }
      }
    }

    /// This strip's number of LEDs.
    int16_t ledCount() const
    {
      return getSize();
    }

    /** Get the strip's underlying LED array.
     * Use this method only when calling FastLed functions directly; mostly together with ledCount().
     * @note The strip's "reversed" property has no effect when this method is used!
     */
    CRGB *ledArray()
    {
      return m_ledArray;
    }

    // Enable iterating over all pixels of the strip via range-based for loop.
    // Use that only when the strip's direction (i.e. the "reversed" property) is not relevant.
    using iterator = CRGB *;
    iterator begin() { return &m_ledArray[0]; }
    iterator end() { return &m_ledArray[getSize()]; }

    static FastLedStrip GetNULL() { return FastLedStrip(); }

  private:
    FastLedStrip()
        : m_ledArray(&s_trashPixel), m_sizeNrev(0)
    {
    }

    int16_t getSize() const
    {
      return m_sizeNrev & 0x7FFF;
    }

    bool getReversed() const
    {
      return m_sizeNrev & 0x8000;
    }

    int16_t getFirstLedIndex() const
    {
      return toLedIndex(0);
    }

    int16_t getLastLedIndex() const
    {
      return toLedIndex(getSize() - 1);
    }

    int16_t toLedIndex(int16_t index) const
    {
      return getReversed() ? getSize() - 1 - index : index;
    }

    bool convertToLedIndex(int16_t &index) const
    {
      index = toLedIndex(index);
      return getReversed();
    }

    void fillLedBlock(int16_t firstLedIndex, int16_t lastLedIndex, CRGB color)
    {
      CRGB *firstLed = &m_ledArray[firstLedIndex];
      CRGB *lastLed = &m_ledArray[lastLedIndex];
      while (firstLed <= lastLed)
      {
        *(firstLed++) = color;
      }
    }

    /** Shift all LED pixels by one from \a firstLedIndex to \a lastLedIndex.
     * After this operation, the old content of the LED at \a lastLedIndex will be gone,
     * and the content of the LED at \a firstLedIndex will be \a newColor.
     */
    void shiftLeds(int16_t firstLedIndex, int16_t lastLedIndex, CRGB newColor)
    {
      CRGB *firstLed = &m_ledArray[firstLedIndex];
      CRGB *lastLed = &m_ledArray[lastLedIndex];

      while (firstLed < lastLed)
      {
        CRGB *previousLed = lastLed--;
        *previousLed = *lastLed;
      }
      while (firstLed > lastLed)
      {
        CRGB *previousLed = lastLed++;
        *previousLed = *lastLed;
      }
      *firstLed = newColor;
    }

  private:
    static CRGB s_trashPixel;
    CRGB *m_ledArray;
    uint16_t m_sizeNrev;
  };

  //------------------------------------------------------------------------------

  /** Fading function for Meteor and BgMeteorFadeToBlack Animation.
   * The individual LEDs of the strip are faded randomly (instead of all at once).
   * @param ledStrip  The LED strip.
   * @param fadeBy  Fading speed: Lower value = longer glowing.
   * @param chance  Chance of fading a LED (0 = never, 255 = always).
   */
  inline void meteorFadeToBlack(FastLedStrip &strip, uint8_t fadeBy = 96, uint8_t chance = 32)
  {
    for (auto &pixel : strip)
    {
      if (random8(1, 255) <= chance)
      {
        pixel.nscale8(255 - fadeBy);
      }
    }
  }

  //------------------------------------------------------------------------------

  /** Helper function for rendering an oftentimes used Pattern background.
   * Depending on the setting of \a fadeBy, it makes either a fading background, or a solid
   * black background.
   * @param fadeBy  Fading speed: Lower value = longer glowing; 0 = black background.
   */
  inline void showDefaultPattern(FastLedStrip &strip, uint8_t fadeBy)
  {
    if (fadeBy)
    {
      strip.fadeToBlack(fadeBy);
    }
    else
    {
      strip.fill(CRGB::Black);
    }
  }

  /// Put more emphasis on the red'ish colors.
  inline uint8_t redShift(uint8_t hue)
  {
    return cos8(128 + hue / 2);
  }

} // namespace EC
