#pragma once
/*******************************************************************************

MIT License

Copyright (c) 2024 Joachim Dick

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

#include "VuBlueprints.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A playground for VU testing during EyeCandy development.
   */
  class TestVU1
      : public AnimationBase
  {
  public:
    /// Signature of the function for rendering the VU on the LED strip.
    using DrawingFct = void (*)(FastLedStrip &strip, TestVU1 &vu);

    DrawingFct drawingFct;

    VuLevelHandler vuLevelHandler;

    VuRangeExtender vuRangeExtender;

    VuPeakHandler vuPeakHandler;
    VuPeakHandler vuDipHandler;

    VuPeakGravityHandler vuPeakGravityHandler;
    VuPeakGravityHandler vuDipGravityHandler;

    VuPeakForceHandler vuPeakForceHandler;

    float vuLevel = 0.0;
    float lastVuLevel = 0.0;

    /** Constructor
     * @param audioSource  Read the audio samples from there.
     * @param ledStrip  The LED strip.
     * @param drawingFct  Pointer to a function for rendering the VU on the LED strip.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     */
    TestVU1(float &audioSource,
            FastLedStrip ledStrip,
            DrawingFct drawingFct,
            uint8_t fadeRate = 0)
        : AnimationBase(ledStrip, false, fadeRate), _audioSource(audioSource), drawingFct(drawingFct)
    {
      vuPeakHandler.peakHold = 600;
      vuDipHandler.peakHold = vuPeakHandler.peakHold;

      vuDipHandler.enableDipMode = true;
      vuDipGravityHandler.enableDipMode = true;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      vuLevelHandler.addSample(_audioSource);
      AnimationBase::processAnimation(currentMillis, wasModified);
    }

    /// @see AnimationBase::showOverlay()
    void showOverlay(uint32_t currentMillis) override
    {
      vuLevel = vuRangeExtender.process(vuLevelHandler.capture());
      vuPeakHandler.process(vuLevel, currentMillis);
      vuDipHandler.process(vuLevel, currentMillis);
      vuPeakGravityHandler.process(vuLevel, currentMillis);
      vuDipGravityHandler.process(vuLevel, currentMillis);
      vuPeakForceHandler.process(vuLevel, currentMillis);

      if (drawingFct)
      {
        drawingFct(strip, *this);
      }

      lastVuLevel = vuLevel;
    }

  private:
    float &_audioSource;
  };

} // namespace EC
