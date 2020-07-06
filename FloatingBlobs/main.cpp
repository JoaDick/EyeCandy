
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

#define FLOATING_BLOBS_SIMU 1

#include "Arduino.h"
#include "FloatingBlobs.hpp"

//------------------------------------------------------------------------------

const uint16_t LED_COUNT = 90;

CRGB leds[LED_COUNT];

FloatingBlobs<5> blobs;

//------------------------------------------------------------------------------

void setup()
{
	std::srand(999);
}

//------------------------------------------------------------------------------

void loop()
{
    blobs.process(leds, LED_COUNT);
    delay(10);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include <conio.h>
#include <ctime>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#undef WIN32_LEAN_AND_MEAN
#undef max

//------------------------------------------------------------------------------

int main()
{
	randomSeed(uint32_t(std::time(nullptr)));

	std::srand(999);

    HANDLE m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    setup();

    bool run = true;

    while (run)
    {
        loop();

        // foreground light grey.
        SetConsoleTextAttribute(m_hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        std::cout << '|';

        for (uint16_t i = 0; i < LED_COUNT; ++i)
        {
#if (0)
            if (leds[i].color == 0)
            {
                SetConsoleTextAttribute(m_hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
            }
            else
            {
                SetConsoleTextAttribute(m_hOut, leds[i].color << 4);
            }
            std::cout << char('0' + leds[i].blobNr);
#else
            // ToDo: RGB color - https://github.com/Microsoft/WSL/issues/1118
            SetConsoleTextAttribute(m_hOut, leds[i].color << 4);
            std::cout << ' ';
#endif
        }

        // foreground light grey.
        SetConsoleTextAttribute(m_hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        std::cout << '|' << std::endl;

#if (0)
        static uint32_t count = 0;
        if (++count % 60 == 0)
        {
            blobs.makeBeat();
        }
#endif

        if (_kbhit())
        {
            char c = _getch();
            switch (c)
            {
            case '\r':
                run = false;
                break;

            case 'f':
                if (++blobs.floatingMode > 1)
                {
                    blobs.floatingMode = -1;
                }
                std::cout << "floatingMode = " << int(blobs.floatingMode) << std::endl;
                break;

            default:
                blobs.makeBeat();
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
