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

#include "AnimationBase.h"
#include "Lightbulbs.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** A completely overengineered Larson scanner.
   * But with a lot of love for the details <3
   */
  class KITT
      : public AnimationModelBase
  {
  public:
    /** Pattern sequence for the scanner's running light.
     * Adjustable e.g. via presetXXX() functions.
     */
    BitPatternSequencer sequencer;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param shiftPeriod  Delay between shifting the dot (in ms).
     * @param color  Draw K.I.T.T.'s scanner with this color.
     */
    explicit KITT(FastLedStrip ledStrip,
                  uint16_t shiftPeriod = 250,
                  CRGB color = CRGB(255, 8, 0))
        : AnimationModelBase(shiftPeriod, ledStrip, false),
          lightbulbArray(ledStrip, 8, color)
    {
      presetScanner(sequencer, lightbulbArray.numBulbs);
    }

    /** Helper for drawing the Lightbulb patterns.
     * Usually there's nothing to configure here; mainly for debugging.
     */
    LightbulbArray lightbulbArray;

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      lightbulbArray.show(sequencer.get());
    }

    /// @see AnimationModelBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      sequencer.update();
    }
  };

} // namespace EC
