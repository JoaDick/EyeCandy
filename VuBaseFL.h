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

#include "AnimationBaseFL.h"
#include "VuLevelHandler.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Base class suitable for most VUs which are using FastLED.
   * Child classes must fulfill the same requirements as for #AnimationBase.
   */
  class VuBaseFL
      : public AnimationBaseFL
  {
  public:
    /** Usually there's nothing to configure here.
     * Publicly accessible mainly for debugging.
     * Child classes shall get the current VU level from here.
     */
    VuLevelHandler vuLevelHandler;

  protected:
    /** Constructor.
     * @param overlayMode  Set to true when Animation shall be an Overlay.
     * @param ledStrip  The LED strip.
     * @param ledCount  Number of LEDs.
     * @param audioSource  Read the audio samples from there.
     */
    VuBaseFL(bool overlayMode,
             CRGB *ledStrip,
             uint16_t ledCount,
             float &audioSource)
        : AnimationBaseFL(overlayMode, ledStrip, ledCount), _audioSource(audioSource)
    {
    }

    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      vuLevelHandler.addSample(_audioSource);
      AnimationBaseFL::processAnimation(currentMillis, wasModified);
    }

  protected:
    /// Most child classes won't need access to this.
    float &_audioSource;
  };

} // namespace EC
