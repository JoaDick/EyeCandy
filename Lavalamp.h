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
#include "MathUtils.h"

//------------------------------------------------------------------------------

#ifndef EC_LAVALAMP_DEBUG
#define EC_LAVALAMP_DEBUG 0
#endif

//------------------------------------------------------------------------------

namespace EC
{

  /** Animation of a relaxing Lava-Lamp.
   * With Bubbles moving calmingly up and down the LED strip.
   */
  class Lavalamp
      : public AnimationBase
  {
  public:
    /// Determines how fast the color changes.
    uint8_t colorBPM;
    static uint8_t colorBPM_default() { return 1; }

    /** Brightness of the Lavalamp.
     * This setting can be adjusted at runtime.
     */
    uint8_t volume = 192;

    // /// Animation speed.
    // float speed = 4.0;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param colorBPM  How fast the color changes.
     * @param startHue  Initial color.
     */
    explicit Lavalamp(FastLedStrip ledStrip,
                      uint8_t colorBPM = colorBPM_default(),
                      uint8_t startHue = random8())
        : AnimationBase(ledStrip), colorBPM(colorBPM), _startHue(startHue)
    {
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
#if (EC_LAVALAMP_DEBUG)
      strip.fill(CRGB::Black);
#else
      strip.fadeToBlack(50);
#endif
      _ceiling.process();
      _floor.process();

      const uint8_t hue = _startHue + beat8(colorBPM);
      const CRGB color = CHSV(hue, 255, volume);
      for (auto i = 0; i < _numBlobs; ++i)
      {
        auto &theBlob = _blobs[i];
        theBlob.process(_ceiling, _floor, i);
        if (theBlob.isActive())
        {
          strip.normLineAbs(theBlob.posMin(), theBlob.posMax(), color);
        }
      }

#if (EC_LAVALAMP_DEBUG)
      // strip.normLineAbs(0, _floor.pos(), color);
      // strip.normLineAbs(1.0, _ceiling.pos(), color);
      strip.normPixel(_ceiling.posMax()) = CRGB(64, 0, 0);
      strip.normPixel(_ceiling.pos()) = CRGB(128, 0, 0);
      strip.normPixel(_ceiling.posMin()) = CRGB(64, 0, 0);
      strip.normPixel(_floor.posMax()) = CRGB(0, 0, 64);
      strip.normPixel(_floor.pos()) = CRGB(0, 0, 128);
      strip.normPixel(_floor.posMin()) = CRGB(0, 0, 64);
      for (uint16_t i = 0; i < _numBlobs; ++i)
      {
        auto &theBlob = _blobs[i];
        const uint8_t hue = i * 256 / _numBlobs;

        CRGB color = CRGB(255, 0, 0);
        switch (theBlob.mode())
        {
        case LavaBlob::ready:
          color = CHSV(hue, 255, 128);
          break;

        case LavaBlob::active:
          color = CHSV(hue, 255, 96);
          break;

        case LavaBlob::armedTop:
          color = CRGB(64, 255, 0);
          break;

        case LavaBlob::armedBottom:
          color = CRGB(0, 255, 64);
          break;

        case LavaBlob::blockedTop:
          color = CRGB(255, 64, 0);
          break;

        case LavaBlob::blockedBottom:
          color = CRGB(255, 0, 64);
          break;

        default:
          break;
        }
        if (theBlob.stateUpdated)
        {
          color = CRGB(255, 255, 255);
        }
        strip.normPixel(theBlob.pos()) = color;
      }
#else
      strip.normLineAbs(0, _floor.pos(), color);
      strip.normLineAbs(1.0, _ceiling.pos(), color);
      strip.blur(75);
#endif
    }

  private:
    class LavaReservoir
    {
    public:
      const float offset;
      const float maxSize;

      void process()
      {
        const float balanceAmount = 0.00001;
        if (fillLevel() <= 0.5)
        {
          _sizeSetpoint += balanceAmount;
        }
        else
        {
          _sizeSetpoint -= balanceAmount;
        }
        _sizeSetpoint = constrain(_sizeSetpoint, 0.0, maxSize);
        _size.process(_sizeSetpoint);
      }

      void consume(float blobSize)
      {
        _sizeSetpoint += blobSize;
      }

      bool release(float blobSize)
      {
        if (random8() & 0x01 || fillLevel() > 0.667)
        {
          _sizeSetpoint -= blobSize;
          return true;
        }
        return false;
      }

      float fillLevel()
      {
        return size() / maxSize;
      }

    protected:
      LavaReservoir(float posMin,
                    const float posMax)
          : offset(posMin), maxSize(posMax - posMin)
      {
      }

      float size() { return _size.get(); }

    private:
      float _sizeSetpoint = offset;
      MovingAverage _size{150};
    };

    class LavaCeiling : public LavaReservoir
    {
    public:
      LavaCeiling() : LavaReservoir(0.01, 0.15) {}
      float posMax() { return 1.0 - offset; }
      float pos() { return 1.0 - offset - size(); }
      float posMin() { return 1.0 - offset - maxSize; }
    };

    class LavaFloor : public LavaReservoir
    {
    public:
      LavaFloor() : LavaReservoir(0.05, 0.2) {}
      float posMax() { return offset + maxSize; }
      float pos() { return offset + size(); }
      float posMin() { return offset; }
    };

    class LavaBlob
    {
    public:
      enum State
      {
        ready,
        armedTop,
        armedBottom,
        active,
        blockedTop,
        blockedBottom
      };

      /// Animation speed.
      float speed = 0.8;

      float pos() { return _pos; }
      float posMin() { return _pos - _radius; }
      float posMax() { return _pos + _radius; }
      bool isActive() { return _state == active; }
      State mode() { return _state; }

      LavaBlob()
      {
#if (EC_LAVALAMP_DEBUG)
        speed = 3.0;
#endif
      }

      void process(LavaCeiling &ceiling, LavaFloor &floor, uint8_t blobNr)
      {
        const float blobMin = 0.025;
        const float blobWobble = 0.125;
        const auto s1 = beatsinAmp(11.0 - blobNr / 4.0, blobMin, blobWobble);
        const auto s2 = beatsinAmp(13.0 + blobNr / 2.0, blobMin, blobWobble);
        _radius = (s1 + s2) / 4.0;

        const float maxPos = ceiling.pos() + _radius + 0.075;
        const float minPos = floor.pos() - _radius - 0.05;
        const float posAmp = beatsinRng(3.45 + blobNr / 5.0, minPos, maxPos);
        const float posMod = beatsinRng(2.34 - blobNr / 7.0, 0.5, 1.0);
        _pos = posAmp * posMod;

#if (EC_LAVALAMP_DEBUG)
        // _radius = 0.025;
        // _pos = posAmp;

        stateUpdated = false;
#endif

        switch (_state)
        {
        case ready:
          if (posMin() > ceiling.pos())
          {
            setState(armedTop);
          }
          else if (posMax() < floor.pos())
          {
            setState(armedBottom);
          }
          break;

        case armedTop:
          if (posMin() <= ceiling.pos())
          {
            if (ceiling.release(_radius * _transferFactor))
            {
              setState(active);
            }
            else
            {
              setState(ready);
            }
          }
          break;

        case armedBottom:
          if (posMax() >= floor.pos())
          {
            if (floor.release(_radius * _transferFactor))
            {
              setState(active);
            }
            else
            {
              setState(ready);
            }
          }
          break;

        case active:
          if (posMin() >= ceiling.pos())
          {
            ceiling.consume(_radius * _transferFactor);
            setState(blockedTop);
          }
          if (posMax() <= floor.pos())
          {
            floor.consume(_radius * _transferFactor);
            setState(blockedBottom);
          }
          break;

        case blockedTop:
          if (posMax() < ceiling.pos())
          {
            setState(ready);
          }
          break;

        case blockedBottom:
          if (posMin() > floor.pos())
          {
            setState(ready);
          }
          break;

        default:
          // should never happen
          setState(ready);
          break;
        }
      }

      void setState(State mode)
      {
        _state = mode;
#if (EC_LAVALAMP_DEBUG)
        stateUpdated = true;
#endif
      }

#if (EC_LAVALAMP_DEBUG)
      bool stateUpdated = false;
#endif

    private:
      float beatsinAmp(float bpm, float lowest, float amplitute)
      {
        return beatsinRng(bpm, lowest, lowest + amplitute);
      }

      float beatsinRng(float bpm, float lowest, float highest)
      {
        return beatsinF(bpm * speed, lowest, highest, 0, 0.67);
      }

      State _state = ready;
      float _pos = 0.0;
      float _radius = 0.0;
      static const float _transferFactor = 0.67;
    };

  private:
#if (EC_LAVALAMP_DEBUG)
    static const uint8_t _numBlobs = 5;
#else
    static const uint8_t _numBlobs = 5;
#endif
    const uint8_t _startHue;
    LavaBlob _blobs[_numBlobs];
    LavaCeiling _ceiling;
    LavaFloor _floor;
  };

} // namespace EC
