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
#include "randomF.h"

//------------------------------------------------------------------------------

class Blob
{
public:
    Blob *nextBlob = nullptr;

    uint8_t nr = 0;

    uint8_t hue = 0;
    bool isBlack = false;

    uint16_t speed = 0;
    float maxWeight = 0.0;

    uint32_t age = 0;
    float weight = 0.0;

    void reset(float maxWeight,
               uint16_t speed,
               uint32_t age = 0)
    {
        this->isBlack = true;
        this->age = age;
        this->speed = speed;
        this->maxWeight = maxWeight;
        updateWeight();
    }

    void reset(uint8_t hue,
               float maxWeight,
               uint16_t speed,
               uint32_t age = 0)
    {
        reset(maxWeight, speed, age);
        this->isBlack = false;
        this->hue = hue;
    }

    bool prepareNext()
    {
        age += speed;
        if (age > 0xFFFF)
        {
            speed = 0;
            maxWeight = 0.0;
        }
        updateWeight();
        return speed != 0;
    }

private:
    void updateWeight()
    {
        weight = cos16(0x8000 + age);
        weight += 0x8000;
        weight *= maxWeight;
        weight /= 0xFFFF;
    }
};

#if (1)
void initColor(Blob &blob,
               float maxWeight = 0.0,
               uint16_t speed = 0)
{
    if (maxWeight <= 0.0)
    {
        maxWeight = randomF(0.5) + 0.5f;
    }
    if (speed == 0)
    {
        speed = random16(0x100) + 0x40;
    }
    blob.reset(random8(), maxWeight, speed);
}

void initBlack(Blob &blob,
               float maxWeight = 0.0,
               uint16_t speed = 0)
{
    if (maxWeight == 0.0)
    {
        maxWeight = randomF(0.25) + 0.25f;
    }
    if (speed == 0)
    {
        speed = random16(0x200) + 0x80;
    }
    blob.reset(maxWeight, speed);
}

void initBeat(Blob &blob,
              float maxWeight = 1.0,
              uint16_t speed = 0)
{
    if (speed == 0)
    {
        speed = 0x800;
    }
    blob.reset(random8(), maxWeight, speed, 0x8000);
}
#endif
