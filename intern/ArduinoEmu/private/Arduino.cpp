
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

#pragma message("Using Arduino Emulation on PC")
#include "../Arduino.h"
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduinoArduino.h

#include <chrono>
#include <cstdlib>
#include <thread>

//------------------------------------------------------------------------------

void delay(uint32_t ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//------------------------------------------------------------------------------

int32_t random(int32_t max)
{
	return std::rand() % max;
}

//------------------------------------------------------------------------------

int32_t random(int32_t min, int32_t max)
{
	return random(max - min) + min;
}

//------------------------------------------------------------------------------

void randomSeed(uint32_t seed)
{
	std::srand(seed);
}

//------------------------------------------------------------------------------
