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

//------------------------------------------------------------------------------

namespace EC
{

  CRGB FastLedStrip::s_trashPixel;

  CRGB &FastLedStrip::operator[](int16_t index)
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

  CRGB &FastLedStrip::pixel(int16_t index)
  {
    const auto size = getSize();
    const auto ledIndex = toLedIndex(index);
    if (ledIndex < 0 || ledIndex >= size)
    {
      return s_trashPixel;
    }
    return m_ledArray[ledIndex];
  }

  void FastLedStrip::lineAbs(int16_t firstIndex, int16_t lastIndex, CRGB color)
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

  void FastLedStrip::shift(int16_t distance, CRGB color)
  {
    while (distance > 0)
    {
      shiftUp(color);
      --distance;
    }
    while (distance < 0)
    {
      shiftDown(color);
      ++distance;
    }
  }

  void FastLedStrip::rotate(int16_t distance)
  {
    while (distance > 0)
    {
      rotateUp();
      --distance;
    }
    while (distance < 0)
    {
      rotateDown();
      ++distance;
    }
  }

  FastLedStrip FastLedStrip::getSubStrip(int16_t offset, int16_t newSize, bool reversed) const
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

  void FastLedStrip::copyUp(bool mirrored)
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

  void FastLedStrip::fillLedBlock(int16_t firstLedIndex, int16_t lastLedIndex, CRGB color)
  {
    CRGB *firstLed = &m_ledArray[firstLedIndex];
    CRGB *lastLed = &m_ledArray[lastLedIndex];
    while (firstLed <= lastLed)
    {
      *(firstLed++) = color;
    }
  }

  void FastLedStrip::shiftLeds(int16_t firstLedIndex, int16_t lastLedIndex, CRGB newColor)
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

} // namespace EC
