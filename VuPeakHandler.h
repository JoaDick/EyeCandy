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

//------------------------------------------------------------------------------

/** Helper class for calculating the peak dot position of a VU meter.
 * Just provide the output of a #VuLevelHandler instance, and get the position
 * of the corresponding peak dot.
 */
class VuPeakHandler
{
    uint32_t _lastPeakTime = 0;
    float _maxPeakValue = 0.0;

public:
    /// Delay (in ms) before the dot starts to fall down.
    uint16_t peakHold = 200;

    /** Falling speed equivalent.
     * Time (in ms) the dot would need to fall down the entire strip.
     */
    uint16_t peakDecay = 1500;

    /** VU levels below that level will set the dot's position to 0.
     * Render the dot only when its position is greater than 0.0, so it will
     * disappear during silence.
     */
    float peakThreshold = 0.005;

    /// This is set to true by process() only when a new peak was detected.
    bool peakDetected = false;

    /// Dot's position of last call to process(). Only for debugging; don't modify!
    float peakLevel = 0.0;

    /** Calculate the peak dot's position.
     * @param volume  Returnvalue of VuLevelHandler::capture()
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @return A value between 0.0 ... 1.0, representing the dot's position.
     * Whenever a new peak is detected, #peakDetected is also set to true.
     * This is the case, when the current \a volume value is below the previous
     * one.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0!
     */
    float process(float volume,
                  uint32_t currentMillis)
    {
        peakDetected = false;
        // rising?
        if (volume >= peakLevel)
        {
            _lastPeakTime = 0;
            _maxPeakValue = volume;
            peakLevel = volume;
        }
        // falling
        else
        {
            // new peak detected?
            if (_lastPeakTime == 0)
            {
                peakDetected = true;
                _lastPeakTime = currentMillis;
            }
            // peak hold?
            else if (currentMillis <= _lastPeakTime + peakHold)
            {
                // nothing to do
            }
            // peak decay
            else
            {
                const uint16_t decayTime = currentMillis - (_lastPeakTime + peakHold);
                if (decayTime < peakDecay)
                {
                    const float decayFactor = float(peakDecay - decayTime) / peakDecay;
                    peakLevel = _maxPeakValue * decayFactor;
                }
                else
                {
                    peakLevel = 0.0;
                }
            }
        }

        // peak too low?
        if (peakLevel <= peakThreshold)
        {
            peakLevel = 0.0;
            peakDetected = false;
            _lastPeakTime = 0;
        }

        return peakLevel;
    }
};
