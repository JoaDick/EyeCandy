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

#include "BlobList.hpp"

//------------------------------------------------------------------------------

template <uint8_t BLOB_COUNT>
class FloatingBlobController
{
    BlobList<2 * BLOB_COUNT> _blobList;

    const uint8_t _colorWheelDelay = 5;
    uint8_t _colorWheelCounter = 0;
    uint8_t _colorWheelPos = random8();

    float _totalWeight = 0.0;

public:
    int8_t floatingMode = 0;

    FloatingBlobController()
    {
        reset();
    }

    void reset()
    {
        _blobList.reset();
        insertColorBlobAfter(nullptr, _colorWheelPos);
        while (Blob *blob = _blobList.insertBlobRandomly())
        {
            initBlackBlob(*blob);
            blob->age = random16(0x8000);
        }
    }

    void process(CRGB *ledStrip,
                 uint16_t ledCount)
    {
        prepareNext();
        show(ledStrip, ledCount);
    }

    void makeBeat()
    {
        uint8_t hue = _colorWheelPos; //random8();

        _blobList._firstBlob->age = 0x8000;
        _blobList._firstBlob->maxWeight = 2 * _totalWeight;
        _blobList._firstBlob->speed = 0x600;
        _blobList._firstBlob->isBlack = false;
        _blobList._firstBlob->hue = hue;

        _blobList._lastBlob->age = 0x8000;
        _blobList._lastBlob->maxWeight = 2 * _totalWeight;
        _blobList._lastBlob->speed = 0x600;
        _blobList._lastBlob->isBlack = false;
        _blobList._lastBlob->hue = hue;
    }

private:
    void show(CRGB *ledStrip,
              uint16_t ledCount)
    {
        // Serial.print("totalWeight: ");
        // Serial.println(_totalWeight);

        if (_totalWeight > 0.0)
        {
            Blob *currentBlob = _blobList._firstBlob;
            float currentWeight = currentBlob->weight;
            uint16_t nextBlobThreshold = uint16_t((currentWeight * ledCount) / _totalWeight);

            for (uint16_t i = 0; i < ledCount; ++i)
            {
                if (i > nextBlobThreshold)
                {
                    currentBlob = currentBlob->nextBlob;
                    if (!currentBlob)
                    {
                        // just a fallback - must never happen!!!
                        currentBlob = _blobList._firstBlob;
#if (FLOATING_BLOBS_SIMU)
                        std::cout << "WTF?!?" << std::endl;
#else
                        Serial.println("WTF?!?");
#endif
                    }
                    currentWeight += currentBlob->weight;
                    //uint16_t lastBlobThreshold = nextBlobThreshold;
                    nextBlobThreshold = uint16_t((currentWeight * ledCount) / _totalWeight);
#if (0)
                    // to avoid some unpleasant flickering,
                    // we want at least one LED to be on
                    while (nextBlobThreshold - lastBlobThreshold < 2 &&
                           !currentBlob->isBlack)
                    {
                        ++nextBlobThreshold;
                    }
#endif
                }

                if (currentBlob->isBlack)
                {
                    ledStrip[i] = CRGB::Black;
                }
                else
                {
                    ledStrip[i] = CHSV(redShift(currentBlob->hue), 255, 255);
                }
            }
        }
    }

    void prepareNext()
    {
        Blob *currentBlob = _blobList._firstBlob;
        Blob *previousBlob = nullptr;
        while (currentBlob)
        {
            if (!currentBlob->prepareNext())
            {
                currentBlob = currentBlob->nextBlob;
                _blobList.removeBlobAfter(previousBlob);
            }
            else
            {
                previousBlob = currentBlob;
                currentBlob = currentBlob->nextBlob;
            }
        }

        insertRecommendedBlob();

        if (++_colorWheelCounter > _colorWheelDelay)
        {
            _colorWheelCounter = 0;
            ++_colorWheelPos;
        }

#if (1)
        Blob *borderBlob = nullptr;
        if (floatingMode > 0)
        {
            borderBlob = _blobList._lastBlob;
        }
        else if (floatingMode < 0)
        {
            borderBlob = _blobList._firstBlob;
        }
        if (borderBlob)
        {
            if (borderBlob->age < 0x8000)
            {
                borderBlob->age = 0x8000;
                borderBlob->maxWeight = borderBlob->weight;
            }
            if (borderBlob->isBlack)
            {
                if (borderBlob->speed < 0x200)
                {
                    borderBlob->speed = random16(0x80) + 0x200;
                }
            }
            else
            {
                if (borderBlob->speed < 0x100)
                {
                    borderBlob->speed = random16(0x40) + 0x100;
                }
            }
        }
#endif

        _totalWeight = 0.0;
        for (Blob *blob = _blobList._firstBlob; blob != nullptr; blob = blob->nextBlob)
        {
            _totalWeight += blob->weight;
        }
    }

    void insertRecommendedBlob()
    {
        if (!_blobList._firstBlob)
        {
            reset();
            return;
        }

        if (floatingMode >= 0)
        {
            if (!_blobList._firstBlob->isBlack)
            {
                insertBlackBlobAfter(nullptr);
            }
        }

        if (floatingMode <= 0)
        {
            if (!_blobList._lastBlob->isBlack)
            {
                insertBlackBlobAfter(_blobList._lastBlob);
            }
        }

        uint8_t lastHue = _colorWheelPos;
        for (Blob *blob = _blobList._firstBlob;
             blob && blob->nextBlob;
             blob = blob->nextBlob)
        {
            Blob *nextBlob = blob->nextBlob;
            if (blob->isBlack)
            {
                if (nextBlob->isBlack)
                {
                    Blob *nextColorBlob = nextBlob->nextBlob;
                    if (nextColorBlob)
                    {
                        uint8_t nextHue = nextColorBlob->isBlack ? _colorWheelPos : nextColorBlob->hue;
                        if (floatingMode == 0)
                        {
                            // insertColorBlobAfter(blob, random8());
                            // insertColorBlobAfter(blob, _colorWheelPos);
                            insertColorBlobAfter(blob, getHueAcross(lastHue, nextHue));
                            // insertColorBlobAfter(blob, getHueBetween(lastHue, nextHue));
                        }
                        else
                        {
                            insertColorBlobAfter(blob, getHueAcross(lastHue, nextHue));
                            // insertColorBlobAfter(blob, getHueBetween(lastHue, nextHue));
                        }
                    }
                }
            }
            else
            {
                lastHue = blob->hue;
                if (!nextBlob->isBlack)
                {
                    insertBlackBlobAfter(blob);
                }
            }
        }

        if (floatingMode > 0)
        {
            insertColorBlobAfter(nullptr, _colorWheelPos);
        }
        else if (floatingMode < 0)
        {
            insertColorBlobAfter(_blobList._lastBlob, _colorWheelPos);
        }
        else
        {
            // insertRandomColorBlob(random8());
            insertRandomColorBlob(_colorWheelPos);
        }
    }

    void insertRandomColorBlob(uint8_t hue)
    {
        Blob *newBlob = _blobList.insertBlobRandomly();
        if (newBlob)
        {
            initColorBlob(*newBlob, hue);
        }
    }

    void insertColorBlobAfter(Blob *blob,
                              uint8_t hue)
    {
        Blob *newBlob = _blobList.insertBlobAfter(blob);
        if (newBlob)
        {
            initColorBlob(*newBlob, hue);
        }
    }

    void insertBlackBlobAfter(Blob *blob)
    {
        Blob *newBlob = _blobList.insertBlobAfter(blob);
        if (newBlob)
        {
            initBlackBlob(*newBlob);
        }
    }

    void initColorBlob(Blob &blob,
                       uint8_t hue)
    {
        blob.reset(hue, randomF(0.5) + 0.5f, random16(0x100) + 0x40);
    }

    void initBlackBlob(Blob &blob)
    {
        // blob.reset(randomF(0.1f) + 0.1f, random16(0x200) + 0x80);
        blob.reset(randomF(0.2f) + 0.2f, random16(0x200) + 0x80);
        // blob.reset(randomF(0.5) + 0.5f, random16(0x100) + 0x40);
    }

    static uint8_t getHueBetween(uint8_t hue1,
                                 uint8_t hue2)
    {
        if (hue1 > hue2)
        {
            return getHueBetween(hue2, hue1);
        }

        uint16_t hue = hue1;
        hue += hue2;
        hue /= 2;

        uint8_t delta = hue2 - hue1;
        if (delta > 128)
        {
            hue -= 128;
        }

// #if (!FLOATING_BLOBS_SIMU)
//         Serial.print(hue1);
//         Serial.print(" | ");
//         Serial.print(hue2);
//         Serial.print(" -> ");
//         Serial.println(uint8_t(hue));
// #endif

        return uint8_t(hue);
    }

    static uint8_t getHueAcross(uint8_t hue1,
                                uint8_t hue2)
    {
        return getHueBetween(hue1, hue2) + 128;
    }

    /// Put more emphasis on the red'ish colors.
    static uint8_t redShift(uint8_t hue)
    {
#if (FLOATING_BLOBS_SIMU)
        return hue;
#else
        return cos8(128 + hue / 2);
#endif
    }
};
