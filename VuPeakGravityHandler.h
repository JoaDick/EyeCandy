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

/** Peak dot of a VU meter, influenced by gravity.
 * Set #a0 < 0.0 for the behaviour of a ball, that's bumped up by the VU bar.
 * Set #a0 > 0.0 for a bubble, floating off the peak.
 * Just provide the output of a #VuLevelHandler instance, and get the position
 * of the corresponding peak dot.
 */
class VuPeakGravityHandler
{
public:
    /** VU levels below that level will set the dot's position to 0.
     * Render the dot only when its position is greater than 0.0, so it will
     * disappear during silence.
     */
    float peakThreshold = 0.01;

#if (1)              // "Punched Ball"
    float a0 = -3.0; /// Takeoff acceleration.
    float v0 = 0.5;  /// Takeoff velocity.
#else                // "Floating Bubble"
    float a0 = 0.5; /// Takeoff acceleration.
    float v0 = 0.1; /// Takeoff velocity.
#endif

    /** Curent position of the peak dot.
     * @return A value between 0.0 and (typically) 1.0, representing the dot's position.
     * @note Be aware that the retured value may also be greater than 1.0!
     */
    float peakLevel()
    {
        return pos;
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
        // VU rising?
        if (vuLevel >= lastVuLevel)
        {
            // falling "peak ball"?
            if (a0 < 0.0)
            {
                if (acc == 0)
                {
                    pos = vuLevel;
                }
            }
            // rising "peak bubble"
            else
            {
                if (pos == 0.0)
                {
                    acc = 0.0;
                }
            }
        }
        // VU falling
        else
        {
            if (vuLevel > peakThreshold)
            {
                // ready for trigger?
                if (acc == 0)
                {
                    peakDetected = true;
                    acc = a0;
                    vel = v0;
                    pos = lastVuLevel;
                }
            }
        }

        const float delta_t = (currentMillis - lastMillis) / 1000.0;
        vel += acc * delta_t;
        pos += vel * delta_t;

        // falling "peak ball"?
        if (a0 < 0.0)
        {
            if (pos <= vuLevel)
            {
                acc = 0.0;
                pos = vuLevel;
            }
            else if (pos < 0.0)
            {
                acc = 0.0;
                vel = 0.0;
                pos = 0.0;
            }
        }
        // rising "peak bubble"?
        else
        {
            if (pos > 0.0 &&
                pos <= vuLevel)
            {
                vel = v0;
                pos = vuLevel;
            }
            else if (pos < 0.0)
            {
                vel = 0.0;
                pos = 0.0;
            }
            else if (pos > 1.05) // let it float a bit off the top
            {
                acc = -1.0;
                vel = 0.0;
                pos = 0.0;
            }
        }

        lastVuLevel = vuLevel;
        lastMillis = currentMillis;

        return peakDetected;
    }

private:
    float acc = 0.0; // current acceleration
    float vel = 0.0; // current velocity
    float pos = 0.0; // current position

    float lastVuLevel = 0.0;
    uint32_t lastMillis = 0;
};
