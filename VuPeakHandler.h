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

    /** Curent position of the peak dot.
     * @return A value between 0.0 ... 1.0, representing the dot's position.
     * @note Be aware that an overloaded / clipped / too loud audio signal may
     * return values greater than 1.0!
     */
    float peakLevel()
    {
        return _peakLevel;
    }

    /** Calculate the peak dot's position.
     * @param vuLevel  Returnvalue of VuLevelHandler::capture()
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @retval true   A new peak has been detected.
     * @retval false  No peak detected.
     * Use peakLevel() to get the position of the peak dot.
     * A new peak is reported, when the current \a vuLevel value is below the
     * previous one.
     */
    bool process(float vuLevel,
                 uint32_t currentMillis)
    {
        bool peakDetected = false;
        // rising?
        if (vuLevel >= _peakLevel)
        {
            _lastPeakTime = 0;
            _maxPeakValue = vuLevel;
            _peakLevel = vuLevel;
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
                    _peakLevel = _maxPeakValue * decayFactor;
                }
                else
                {
                    _peakLevel = 0.0;
                }
            }
        }

        // peak too low?
        if (_peakLevel <= peakThreshold)
        {
            _peakLevel = 0.0;
            // peakDetected = false;
            _lastPeakTime = 0;
        }

        return peakDetected;
    }

private:
    uint32_t _lastPeakTime = 0;
    float _maxPeakValue = 0.0;
    float _peakLevel = 0.0;
};
