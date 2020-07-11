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

#include <Arduino.h>

/** Helper class for reading the state of an input button.
 * With debouncing.
 * Can distinguish between a short and long button press.
 * Reports continous long press.
 */
class ButtonHandler
{
    uint32_t _triggerTime = 0;
    bool _longPressed = false;

public:
    /// Debouncing time (in ms).
    uint8_t debounceTime = 50;

    /** Call this method periodically.
     *  @param state  Current state of the input pin (true = button pressed).
     *  @param longPress  Threshold (in ms) for detecting a long button press.
     *  @param longPressRepeat  Period (in ms) for reporting repeated long button press.
     *  @retval 0 = Button not pressed.
     *  @retval 1 = Short button press.
     *  @retval 2 = Long button press.
     *  @retval 3 = Still pressed (after long button press).
     */
    uint8_t process(bool state,
                    uint16_t longPress = 1000,
                    uint16_t longPressRepeat = 500)
    {
        const uint32_t now = millis();
        const uint16_t duration = now - _triggerTime;

        // button pressed
        if (state)
        {
            if (_triggerTime == 0)
            {
                _triggerTime = now;
            }
            else
            {
                if (_longPressed == false)
                {
                    if (duration >= longPress)
                    {
                        _longPressed = true;
                        _triggerTime = now;
                        return 2;
                    }
                }
                else if (longPressRepeat > 0 &&
                         duration >= longPressRepeat)
                {
                    _triggerTime = now;
                    return 3;
                }
            }
        }
        // button released
        else
        {
            if (_triggerTime)
            {
                _triggerTime = 0;
                if (duration > debounceTime &&
                    _longPressed == false)
                {
                    return 1;
                }
            }
            _longPressed = false;
        }
        return 0;
    }
};
