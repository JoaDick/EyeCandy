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

#ifndef _MSC_VER
#error "This file should not have been included!"
#endif

//------------------------------------------------------------------------------

#include "Arduino.h"

//------------------------------------------------------------------------------

// hide content of original headerfile
#define __INC_LIB8TION_TRIG_H

int16_t sin16(uint16_t x);
inline int16_t cos16(uint16_t x)
{
	return sin16(x + 16384);
}

int8_t sin8(uint8_t x);
inline int8_t cos8(uint8_t x)
{
	return sin8(x + 64);
}

//------------------------------------------------------------------------------

// hide content of original headerfiles
#define __INC_FASTSPI_LED2_H
#define __INC_LED_SYSDEFS_H

// some other stuff to disable
#define FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_END
#define FASTLED_USING_NAMESPACE
#define __attribute__(x)

#include "../../../FastLED/pixeltypes.h"

//------------------------------------------------------------------------------
