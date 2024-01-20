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

#include "PseudoAnimationBase.h"
#include "Fire2012.h"

//------------------------------------------------------------------------------

namespace EC
{

  /// Adjust Fire2012's settings automatically at runtime.
  template <uint16_t NUM_LEDS>
  class Fire2012Changer
      : public PseudoAnimationBase
  {
    Fire2012<NUM_LEDS> &_fire;

  public:
    accum88 bpm_COOLING = 5;
    accum88 bpm_SPARKING = 9;

    /** Constructor.
     * @param fire  Original #Fire2012 animation to manipulate.
     */
    explicit Fire2012Changer(Fire2012<NUM_LEDS> &fire)
        : PseudoAnimationBase(), _fire(fire)
    {
      modelUpdatePeriod = 10;
    }

  private:
    /// @see PseudoAnimationBase::updateModel()
    void updateModel(uint32_t currentMillis) override
    {
      /// COOLING: How much does the air cool as it rises?
      /// Less cooling = taller flames.  More cooling = shorter flames.
      /// suggested range 20-100
      _fire.COOLING = beatsin8(bpm_COOLING, 55, 90);

      /// SPARKING: What chance (out of 255) is there that a new spark will be lit?
      /// Higher chance = more roaring fire.  Lower chance = more flickery fire.
      /// suggested range 50-200.
      _fire.SPARKING = beatsin8(bpm_SPARKING, 50, 150);
    }
  };

} // namespace EC
