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

  /** Emulation of a classic Lightbulb with run-up and afterglow effect.
   * Normally don't use this class directly - see LightbulbArray.
   */
  class Lightbulb
  {
  public:
    /// Color of the Lightbulb.
    CRGB color;

    /// One-off fading immediately after switching off.
    uint8_t fadeBy1 = 175;

    /// Continuous fading after switching off.
    uint8_t fadeBy2 = 23;

    /// Configure the Lightbulb with (its fraction of) the LED strip and color.
    void init(FastLedStrip ledStrip, CRGB col)
    {
      _strip = ledStrip;
      color = col;
    }

    /// Render the Ligtbulb in its given state on the LED strip.
    void draw(bool isOn)
    {
      if (isOn)
      {
        if (!_lastIsOn)
        {
          _strip.n_lineCentered(0.5, 0.33, color);
          _strip.blur(75, 4);
        }
        else
        {
          _strip.blur(125, 2);
          _strip.n_lineCentered(0.5, 0.25, color);
        }
      }
      else
      {
        if (_lastIsOn)
        {
          _strip.fadeToBlack(fadeBy1);
          _strip.n_lineCentered(0.5, 0.1, color);
        }
        else
        {
          _strip.fadeToBlack(fadeBy2);
        }
      }
      _lastIsOn = isOn;
    }

  private:
    FastLedStrip _strip = FastLedStrip::getNull();
    bool _lastIsOn = false;
  };

  //------------------------------------------------------------------------------

  /// Array of (at most) 8 Lightbulbs.
  class LightbulbArray
  {
  public:
    /// Number of Lightbulbs in this array.
    const uint8_t numBulbs = 8;

    /** Constructor.
     * Using a rainbow color sequence for the individual Lightbulbs.
     * @param ledStrip  The LED strip.
     * @param bulbs  Number of Lightbulbs (max 8).
     */
    explicit LightbulbArray(FastLedStrip ledStrip,
                            uint8_t bulbs = 6)
        : numBulbs(min(bulbs, _maxBulbs))
    {
      ledStrip.clear();
      const int16_t bulbSize = ledStrip.ledCount() / numBulbs;
      for (uint8_t i = 0; i < numBulbs; ++i)
      {
        _bulbs[i].init(ledStrip.getSubStrip(i * bulbSize, bulbSize), CHSV(i * (256 / numBulbs), 255, 255));
      }
    }

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param bulbs  Number of Lightbulbs (max 8).
     * @param color  Color of all Lightbulbs.
     */
    LightbulbArray(FastLedStrip ledStrip,
                   uint8_t bulbs,
                   CRGB color)
        : numBulbs(min(bulbs, _maxBulbs))
    {
      ledStrip.clear();
      int16_t bulbSize = ledStrip.ledCount() / _maxBulbs;
      for (uint8_t i = 0; i < numBulbs; ++i)
      {
        _bulbs[i].init(ledStrip.getSubStrip(i * bulbSize, bulbSize), color);
      }
    }

    /** Override the color of a specific Lightbulb.
     * @param bulbIndex  Which Lightbulb (starting at index 0; max 7).
     * @param color  New color for that Lightbulb.
     */
    void setColor(uint8_t bulbIndex, CRGB color)
    {
      if (bulbIndex < numBulbs)
      {
        _bulbs[bulbIndex].color = color;
      }
    }

    /** Render the Lightbulbs on the LED strip according to the given \a bitPattern.
     * Call this method frequently when the content of the LED strip is updated.
     * Bit set means the corresponding Lightbulb is on.
     */
    void show(uint8_t bitPattern)
    {
      uint8_t bitMask = 0x01;
      for (uint8_t i = 0; i < numBulbs; ++i)
      {
        const bool isOn = bitPattern & bitMask;
        _bulbs[i].draw(isOn);
        bitMask <<= 1;
      }
    }

  private:
    static const uint8_t _maxBulbs = 8;
    Lightbulb _bulbs[_maxBulbs];
  };

  //------------------------------------------------------------------------------

  /** Helper class for generating bit pattern sequences.
   * An empty sequence (after calling reset() or directly after initialization) will produce
   * random bit patterns.
   */
  class BitPatternSequencer
  {
  public:
    /// Maximum number of bit patterns that can be stored.
    static const uint8_t capacity = 20;

    /// Get the currently selected bit pattern.
    uint8_t get()
    {
      return _patterns[_index];
    }

    /// Select the next bit pattern of the sequence.
    void update()
    {
      if (_size == 0)
      {
        _patterns[0] = random8();
      }
      else if (++_index >= _size)
      {
        _index = 0;
      }
    }

    /// Add \a bitPattern to the sequence.
    void add(uint8_t bitPattern)
    {
      if (_size < capacity)
      {
        _patterns[_size++] = bitPattern;
      }
    }

    /// Remove all bit patterns from the sequence.
    void reset()
    {
      _index = 0;
      _size = 0;
    }

    /// Get number of bit patterns in the sequence.
    uint8_t size()
    {
      return _size;
    }

  private:
    uint8_t _patterns[capacity];
    uint8_t _size = 0;
    uint8_t _index = 0;
  };

  /// Produce random bit patterns.
  inline void presetRandom(BitPatternSequencer &seq)
  {
    seq.reset();
  }

  /// One single bit "running up".
  inline void presetRunningLight(BitPatternSequencer &seq, uint8_t numBits)
  {
    seq.reset();
    uint8_t pattern = 1;
    while (numBits-- > 0)
    {
      seq.add(pattern);
      pattern <<= 1;
    }
  }

  /// One single bit "running up and down".
  inline void presetScanner(BitPatternSequencer &seq, uint8_t numBits)
  {
    seq.reset();
    uint8_t pattern = 0x01;
    for (uint8_t i = 0; i < numBits; ++i)
    {
      seq.add(pattern);
      pattern <<= 1;
    }
    pattern = 1 << (numBits - 1);
    for (uint8_t i = 2; i < numBits; ++i)
    {
      pattern >>= 1;
      seq.add(pattern);
    }
  }

  /// Toggle pattern: alternating on and off.
  inline void presetToggle(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b01010101);
    seq.add(0b10101010);
  }

  /// Toggle pattern: 2 on, 2 off.
  inline void presetToggle2(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00110011);
    seq.add(0b11001100);
  }

  /// Maquee pattern: 1 on, 2 off.
  inline void presetMarquee12(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b01001001);
    seq.add(0b10010010);
    seq.add(0b00100100);
  }

  /// Maquee pattern: 1 on, 3 off.
  inline void presetMarquee13(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00010001);
    seq.add(0b00100010);
    seq.add(0b01000100);
    seq.add(0b10001000);
  }

  /// Maquee pattern: 2 on, 1 off.
  inline void presetMarquee21(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b11011011);
    seq.add(0b10110110);
    seq.add(0b01101101);
  }

  /// Maquee pattern: 2 on, 2 off.
  inline void presetMarquee22(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00110011);
    seq.add(0b01100110);
    seq.add(0b11001100);
    seq.add(0b10011001);
  }

  /// Maquee pattern: 2 on, 3 off.
  inline void presetMarquee23(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b01100011);
    seq.add(0b11000110);
    seq.add(0b10001100);
    seq.add(0b00011000);
    seq.add(0b00110001);
  }

  /// Maquee pattern: 2 on, 4 off.
  inline void presetMarquee24(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b11000011);
    seq.add(0b10000110);
    seq.add(0b00001100);
    seq.add(0b00011000);
    seq.add(0b00110000);
    seq.add(0b01100001);
  }

  inline void presetAlternating6(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00000001);
    seq.add(0b00001000);
    seq.add(0b00000010);
    seq.add(0b00010000);
    seq.add(0b00000100);
    seq.add(0b00100000);
  }

  inline void presetAlternating8(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00000001);
    seq.add(0b00010000);
    seq.add(0b00000010);
    seq.add(0b00100000);
    seq.add(0b00000100);
    seq.add(0b01000000);
    seq.add(0b00001000);
    seq.add(0b10000000);
  }

  inline void presetExpand6(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00001100);
    seq.add(0b00010010);
    seq.add(0b00100001);
    seq.add(0b00000000);
  }

  inline void presetExpand6a(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00001100);
    seq.add(0b00011110);
    seq.add(0b00110011);
    seq.add(0b00100001);
    seq.add(0b00000000);
  }

  inline void presetExpand8(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00011000);
    seq.add(0b00100100);
    seq.add(0b01000010);
    seq.add(0b10000001);
    seq.add(0b00000000);
  }

  inline void presetExpand8a(BitPatternSequencer &seq)
  {
    seq.reset();
    seq.add(0b00011000);
    seq.add(0b00111100);
    seq.add(0b01100110);
    seq.add(0b11000011);
    seq.add(0b10000001);
    seq.add(0b00000000);
  }

} // namespace EC
