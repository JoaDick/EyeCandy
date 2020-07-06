
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

#pragma message("Using FastLED Emulation on PC")
#include "../FastLED.h"

#define _USE_MATH_DEFINES

#include <cmath>
#include <limits>

//------------------------------------------------------------------------------

int16_t sin16(uint16_t x)
{
	double x_d = (x * 2.0 * M_PI) / std::numeric_limits<uint16_t>::max();
	double tmp = std::sin(x_d);
	return int16_t(tmp * std::numeric_limits<int16_t>::max());
}

//------------------------------------------------------------------------------

int8_t sin8(uint8_t x)
{
	double x_d = (x * 2.0 * M_PI) / std::numeric_limits<uint8_t>::max();
	double tmp = std::sin(x_d);
	return int8_t(tmp * std::numeric_limits<int8_t>::max());
}

//------------------------------------------------------------------------------

#define RAND16_SEED 1337
uint16_t rand16seed = RAND16_SEED;

//------------------------------------------------------------------------------

#define asm
#define volatile(x)

#include "../../../../FastLED/hsv2rgb.cpp"

//------------------------------------------------------------------------------
