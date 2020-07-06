
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

// https://stackoverflow.com/questions/9509278/rgb-specific-console-text-color-c
WORD color2fg(const CRGB &color)
{
    WORD retval = 0;
    if (color.red > 64)
    {
        retval |= FOREGROUND_RED;
    }
    if (color.green > 64)
    {
        retval |= FOREGROUND_GREEN;
    }
    if (color.blue > 64)
    {
        retval |= FOREGROUND_BLUE;
    }
    if (color.red + color.green + color.blue > 3 * 64)
    {
        retval |= FOREGROUND_INTENSITY;
    }
    return retval;
}

WORD color2bg(const CRGB &color)
{
    WORD retval = 0;
    if (color.red > 64)
    {
        retval |= BACKGROUND_RED;
    }
    if (color.green > 64)
    {
        retval |= BACKGROUND_GREEN;
    }
    if (color.blue > 64)
    {
        retval |= BACKGROUND_BLUE;
    }
    if (color.red + color.green + color.blue > 3 * 64)
    {
        retval |= BACKGROUND_INTENSITY;
    }
    return retval;
}

//------------------------------------------------------------------------------

int main()
{
    //random16_set_seed(uint16_t(std::time(nullptr) & 0xFFFF));

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
            // ToDo: RGB color - https://github.com/Microsoft/WSL/issues/1118
            SetConsoleTextAttribute(m_hOut, color2bg(leds[i]));
            std::cout << ' ';
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
