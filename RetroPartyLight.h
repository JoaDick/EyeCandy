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

  /** Emulation of a retro-style running light with colorful Lightbulbs.
   */
  class RetroPartyLight
      : public AnimationModelBase
  {
  public:
    /** Pattern sequence for the Lightbulbs.
     * Adjustable e.g. via presetXXX() functions.
     */
    BitPatternSequencer sequencer;

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param shiftPeriod  Delay between shifting the dot (in ms).
     * @param bulbs  Number of Lightbulbs (max 8).
     */
    explicit RetroPartyLight(FastLedStrip ledStrip,
                             uint16_t shiftPeriod = 500,
                             uint8_t bulbs = 6)
        : AnimationModelBase(shiftPeriod, ledStrip, false),
          lightbulbArray(ledStrip, bulbs)
    {
      presetRunningLight(sequencer, lightbulbArray.numBulbs);
      // presetScanner(sequencer, lightbulbArray.numBulbs);
      // presetToggle(sequencer);
      // presetToggle2(sequencer);
      // presetMarquee12(sequencer);
      // presetMarquee13(sequencer);
      // presetMarquee21(sequencer);
      // presetMarquee22(sequencer);
      // presetMarquee23(sequencer);
      // presetMarquee24(sequencer);
      // presetAlternating6(sequencer);
      // presetAlternating8(sequencer);
      // presetExpand6(sequencer);
      // presetExpand6a(sequencer);
      // presetExpand8(sequencer);
      // presetExpand8a(sequencer);
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
