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

namespace EC
{

  //------------------------------------------------------------------------------

  /** Peak dot of a VU meter, "loosely tied to the VU level".
   * Behaves as if the VU level and the peak dot are connected with a rubber band.
   */
  class VuPeakForceHandler
  {
  public:
    float inertia = 0.47;

    float friction = 0.125;

    /** Get the current VU level.
     * This means the curent position of the peak dot.
     * @return A normalized VU value between (typically) 0.0 ... 1.0, representing the dot's position.
     * @note Be aware that the retured value will sometimes exceed these limits!
     */
    float getVU()
    {
      return pos;
    }

    /** Calculate the peak dot's position for the given \a vuLevel.
     * @param vuLevel  The current VU level.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * Use getVU() to get the position of the peak dot.
     */
    void process(float vuLevel,
                 uint32_t currentMillis)
    {
      const float delta_t = (currentMillis - lastMillis) / 1000.0;
      const float force = (vuLevel - pos) * delta_t;
      const float drag = vel * friction * delta_t;
      const float acc = (force - drag) / (inertia / 1000.0);
      vel += acc * delta_t;
      pos += vel * delta_t;
      lastMillis = currentMillis;
    }

  private:
    float vel = 0.0; // current velocity
    float pos = 0.0; // current position
    uint32_t lastMillis = 0;
  };

}
