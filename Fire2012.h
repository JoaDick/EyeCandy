#pragma once
/*******************************************************************************

MIT License

Copyright (c) Mark Kriegsman
Copyright (c) 2020 Joachim Dick - Ported to EyeCandy Animation

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

//------------------------------------------------------------------------------

namespace EC
{

  /// This first palette is the basic 'black body radiation' colors,
  /// which run from black to red to bright yellow to white.
  inline CRGBPalette16 Fire2012_gPal_default() { return HeatColors_p; }

  /// These are other ways to set up the color palette for the 'fire'.
  /// First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  inline CRGBPalette16 Fire2012_gPal_BlackRedYellowWhite() { return CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White); }

  /// Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  inline CRGBPalette16 Fire2012_gPal_BlackBlueAquaWhite() { return CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White); }

  /// Third, here's a simpler, three-step gradient, from black to red to white
  inline CRGBPalette16 Fire2012_gPal_BlackRedWhite() { return CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::White); }

  /// Default value for #Fire2012::COOLING
  inline uint8_t Fire2012_COOLING_default() { return 75 /* 55 */; }

  /// Default value for #Fire2012::SPARKING
  inline fract8 Fire2012_SPARKING_default() { return 120; }

  /// Fourth, the most sophisticated: this one sets up a new palette every
  /// time through the loop, based on a hue that changes every time.
  /// The palette is a gradient from black, to a dark color based on the hue,
  /// to a light color based on the hue, to white.
  ///
  /// 0 means no rainbow effect; always using #gPal
  /// Other means delay (in ms) between updating the hue (i.e. "rainbow speed").
  /// @note This delay influences the "Animation speed", but not the LED
  /// refresh rate.
  inline uint16_t Fire2012_animationDelay_default() { return 0; }

  /** Fire2012 with programmable Color Palette
  This code is the same fire simulation as the original "Fire2012",
  but each heat cell's temperature is translated to color through a FastLED
  programmable color palette, instead of through the "HeatColor(...)" function.

  Four different static color palettes are provided here, plus one dynamic one.

  The three static ones are:
    1. the FastLED built-in HeatColors_p -- this is the default, and it looks
      pretty much exactly like the original Fire2012.

  To use any of the other palettes below, just "uncomment" the corresponding code.

    2. a gradient from black to red to yellow to white, which is
      visually similar to the HeatColors_p, and helps to illustrate
      what the 'heat colors' palette is actually doing,
    3. a similar gradient, but in blue colors rather than red ones,
      i.e. from black to blue to aqua to white, which results in
      an "icy blue" fire effect,
    4. a simplified three-step gradient, from black to red to white, just to show
      that these gradients need not have four components; two or
      three are possible, too, even if they don't look quite as nice for fire.

  The dynamic palette shows how you can change the basic 'hue' of the
  color palette every time through the loop, producing "rainbow fire".

  https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino

  There are two main parameters you can play with to control the look and
  feel of your fire: COOLING (used in step 1 above), and SPARKING (used
  in step 3 above).
  */
  template <uint16_t NUM_LEDS>
  class Fire2012 : public AnimationBaseFL2
  {
    static const uint16_t FRAMES_PER_SECOND = 60;

    // Array of temperature readings at each simulation cell
    byte heat[NUM_LEDS];
    uint8_t hue = 0;

  public:
    /// This first palette is the basic 'black body radiation' colors,
    /// which run from black to red to bright yellow to white.
    CRGBPalette16 gPal = Fire2012_gPal_default();

    /// COOLING: How much does the air cool as it rises?
    /// Less cooling = taller flames.  More cooling = shorter flames.
    /// suggested range 20-100
    uint8_t COOLING = Fire2012_COOLING_default();

    /// SPARKING: What chance (out of 255) is there that a new spark will be lit?
    /// Higher chance = more roaring fire.  Lower chance = more flickery fire.
    /// suggested range 50-200.
    fract8 SPARKING = Fire2012_SPARKING_default();

    /// Deprecated; only for legacy compatibility.
    Fire2012(CRGB *ledStrip,
             uint16_t ledCount)
        : Fire2012(FastLedStrip(ledStrip, ledCount))
    {
    }

    /** Constructor
     * @param ledStrip  The LED strip.
     */
    explicit Fire2012(FastLedStrip ledStrip)
        : AnimationBaseFL2(ledStrip, false)
    {
      patternDelay = 1000 / FRAMES_PER_SECOND;
      animationDelay = Fire2012_animationDelay_default();
    }

  private:
    /// @see AnimationBase::showPattern()
    void showPattern(uint32_t currentMillis) override
    {
      // Add entropy to random number generator; we use a lot of it.
      random16_add_entropy(random());
      Fire2012WithPalette(); // run simulation frame, using palette colors
    }

    // Fire2012 by Mark Kriegsman, July 2012
    // as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
    ////
    // This basic one-dimensional 'fire' simulation works roughly as follows:
    // There's a underlying array of 'heat' cells, that model the temperature
    // at each point along the line.  Every cycle through the simulation,
    // four steps are performed:
    //  1) All cells cool down a little bit, losing heat to the air
    //  2) The heat from each cell drifts 'up' and diffuses a little
    //  3) Sometimes randomly new 'sparks' of heat are added at the bottom
    //  4) The heat from each cell is rendered as a color into the leds array
    //     The heat-to-color mapping uses a black-body radiation approximation.
    //
    // Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
    //
    // This simulation scales it self a bit depending on NUM_LEDS; it should look
    // "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
    //
    // I recommend running this simulation at anywhere from 30-100 frames per second,
    // meaning an interframe delay of about 10-35 milliseconds.
    //
    // Looks best on a high-density LED setup (60+ pixels/meter).
    void Fire2012WithPalette()
    {
      const auto ledCount = strip.ledCount();

      // Step 1.  Cool down every cell a little
      for (int i = 0; i < ledCount; i++)
      {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / ledCount) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for (int k = ledCount - 1; k >= 2; k--)
      {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if (random8() < SPARKING)
      {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
      }

      // Step 4.  Map from heat cells to LED colors
      for (int j = 0; j < ledCount; j++)
      {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        byte colorindex = scale8(heat[j], 240);
        CRGB color = ColorFromPalette(gPal, colorindex);

#if (0) // mirroring can be done through class FastLedStrip
        int pixelnumber;
        if (gReverseDirection)
        {
          pixelnumber = (ledCount - 1) - j;
        }
        else
        {
          pixelnumber = j;
        }
        leds[pixelnumber] = color;
#else
        strip[j] = color;
#endif
      }
    }

    /// @see AnimationBase::updateAnimation()
    void updateAnimation(uint32_t currentMillis) override
    {
      if (animationDelay)
      {
        // Fourth, the most sophisticated: this one sets up a new palette every
        // time through the loop, based on a hue that changes every time.
        // The palette is a gradient from black, to a dark color based on the hue,
        // to a light color based on the hue, to white.
        hue++;
        CRGB darkcolor = CHSV(hue, 255, 192);  // pure hue, three-quarters brightness
        CRGB lightcolor = CHSV(hue, 128, 255); // half 'whitened', full brightness
        gPal = CRGBPalette16(CRGB::Black, darkcolor, lightcolor, CRGB::White);
      }
    }
  };

} // namespace EC
