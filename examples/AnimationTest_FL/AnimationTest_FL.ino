/*******************************************************************************

A sketch for testing during EyeCandy development.

********************************************************************************

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

#include <EyeCandy.h>

//#define LED_COLOR_ORDER     RGB
//#define LED_COLOR_ORDER     RBG
#define LED_COLOR_ORDER GRB
//#define LED_COLOR_ORDER     GBR
//#define LED_COLOR_ORDER     BRG
//#define LED_COLOR_ORDER     BGR

#define LED_PIN 6

#define PIN_BUTTON_FLIP 2
#define PIN_COLOR A0
#define PIN_SPEED A1

//------------------------------------------------------------------------------

#define LED_TYPE WS2812B
#define NUM_LEDS 90

CRGB leds[NUM_LEDS];

//------------------------------------------------------------------------------

// Patterns
EC::FadeOut_FL fadeOut_FL(leds, NUM_LEDS);
EC::Fire2012_FL<NUM_LEDS> fire2012_FL(leds, NUM_LEDS);
EC::Glitter_FL glitter_FL(leds, NUM_LEDS, false);
EC::MovingDot_FL movingDot_FL(leds, NUM_LEDS, false);
EC::Pride2015_FL pride2015_FL(leds, NUM_LEDS);
EC::Rainbow_FL rainbow_FL(leds, NUM_LEDS);
EC::RainbowBuiltin_FL rainbowBuiltin_FL(leds, NUM_LEDS);
EC::RainbowTwinkle_FL rainbowTwinkle_FL(leds, NUM_LEDS);
EC::RgbBlocks_FL rgbBlocks_FL(leds, NUM_LEDS);
EC::StaticBackground_FL staticBackground_FL(leds, NUM_LEDS, CRGB(0, 10, 0));
EC::Twinkles_FL twinkles_FL(leds, NUM_LEDS, false);

// Overlays
EC::Glitter_FL glitterOverlay_FL(leds, NUM_LEDS, true);
EC::MovingDot_FL movingDotOverlay_FL(leds, NUM_LEDS, true);
EC::Twinkles_FL twinklesOverlay_FL(leds, NUM_LEDS, true);

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_BUTTON_FLIP, INPUT_PULLUP);
    pinMode(PIN_COLOR, INPUT_PULLUP);
    pinMode(PIN_SPEED, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));
    printMemoryUsage();
}

//------------------------------------------------------------------------------

void loop()
{
    updateColor();
    updateSpeed();
    updateFlip();

    bool mustShow = false;

    // Base Animation (enable one)
    //mustShow = fadeOut_FL.process(mustShow);
    mustShow = fire2012_FL.process(mustShow);
    //mustShow = glitter_FL.process(mustShow);
    //mustShow = movingDot_FL.process(mustShow);
    //mustShow = pride2015_FL.process(mustShow);
    //mustShow = rainbow_FL.process(mustShow);
    //mustShow = rainbowBuiltin_FL.process(mustShow);
    //mustShow = rainbowTwinkle_FL.process(mustShow);
    //mustShow = rgbBlocks_FL.process(mustShow);
    //mustShow = staticBackground_FL.process(mustShow);
    //mustShow = twinkles_FL.process(mustShow);

    // Overlays
    //mustShow = glitterOverlay_FL.process(mustShow);
    //mustShow = movingDotOverlay_FL.process(mustShow);
    //mustShow = twinklesOverlay_FL.process(mustShow);

    if (mustShow)
    {
#if (0)
        static bool toggleFlag = false;
        toggleFlag ^= true;
        leds[0] = toggleFlag ? CRGB(0, 10, 0) : CRGB::Black;
#endif
        FastLED.show();
    }
}

//------------------------------------------------------------------------------

template <typename IN_TYPE, typename OUT_TYPE>
OUT_TYPE constrainAndMap(const IN_TYPE &x,
                         const IN_TYPE &minThreshold, const IN_TYPE &maxThreshold,
                         const OUT_TYPE &outMin, const OUT_TYPE &outMax)
{
    return map(constrain(x, minThreshold, maxThreshold), minThreshold, maxThreshold, outMin, outMax);
}

//------------------------------------------------------------------------------

uint8_t lastHue = 0;
void updateColor()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_COLOR), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t hue = analogValue;

        if (hue != lastHue)
        {
            Serial.print("hue: ");
            Serial.println(hue);
            lastHue = hue;
        }

        fire2012_FL.SPARKING = hue;
        glitter_FL.effectRate = hue;
        movingDot_FL.foregroundColor = CHSV(hue, 255, 255);
        movingDot_FL.backgroundColor = CHSV(hue + 128, 255, 64);
        rainbow_FL.volume = hue;
        rainbowBuiltin_FL.deltahue = hue / 10;
        rgbBlocks_FL.blockSize = hue / 10;
        staticBackground_FL.backgroundColor = CHSV(hue + 128, 255, 128);
        twinkles_FL.effectRate = hue;

        glitterOverlay_FL.effectRate = hue;
        movingDotOverlay_FL.foregroundColor = CHSV(hue + 64, 255, 255);
        twinklesOverlay_FL.effectRate = hue;
    }
}

//------------------------------------------------------------------------------

uint8_t lastSpeed = 0;
void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t animationSpeed = analogValue;
        const uint8_t animationDelay = animationSpeed ? 256 - animationSpeed : 0;

        if (animationSpeed != lastSpeed)
        {
            Serial.print("speed: ");
            Serial.print(animationSpeed);
            Serial.print(" delay: ");
            Serial.println(animationDelay);
            lastSpeed = animationSpeed;
        }

        fire2012_FL.COOLING = 255 - animationSpeed;
        movingDot_FL.animationDelay = animationDelay;
        rainbow_FL.animationDelay = animationDelay;
        rainbowBuiltin_FL.animationDelay = animationDelay;
        rainbowTwinkle_FL.animationDelay = animationDelay;
        rgbBlocks_FL.animationDelay = 8 * animationDelay;
        twinkles_FL.fadeRate = animationSpeed;

        movingDotOverlay_FL.animationDelay = 2 * animationDelay;
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool mirrored = !digitalRead(PIN_BUTTON_FLIP);

    rainbow_FL.mirrored = mirrored;
    rgbBlocks_FL.mirrored = mirrored;

    if (mirrored)
    {
        //fire2012_FL.gPal = EC::Fire2012_gPal_BlackBlueAquaWhite();
        fire2012_FL.gPal = CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::Blue, CRGB::White);
    }
    else
    {
        fire2012_FL.gPal = EC::Fire2012_gPal_default();
    }
}

//------------------------------------------------------------------------------

void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("(*) is dependant on NUM_LEDS"));

    Serial.print(F("FadeOut_FL = "));
    Serial.println(sizeof(EC::FadeOut_FL));

    Serial.print(F("Fire2012_FL (*) = "));
    Serial.println(sizeof(EC::Fire2012_FL<NUM_LEDS>));

    Serial.print(F("Glitter_FL = "));
    Serial.println(sizeof(EC::Glitter_FL));

    Serial.print(F("Kaleidoscope_FL = "));
    Serial.println(sizeof(EC::Kaleidoscope_FL));

    Serial.print(F("MovingDot_FL = "));
    Serial.println(sizeof(EC::MovingDot_FL));

    Serial.print(F("Pride2015_FL = "));
    Serial.println(sizeof(EC::Pride2015_FL));

    Serial.print(F("Rainbow_FL = "));
    Serial.println(sizeof(EC::Rainbow_FL));

    Serial.print(F("RainbowBuiltin_FL = "));
    Serial.println(sizeof(EC::RainbowBuiltin_FL));

    Serial.print(F("RainbowTwinkle_FL = "));
    Serial.println(sizeof(EC::RainbowTwinkle_FL));

    Serial.print(F("RgbBlocks_FL = "));
    Serial.println(sizeof(EC::RgbBlocks_FL));

    Serial.print(F("StaticBackground_FL = "));
    Serial.println(sizeof(EC::StaticBackground_FL));

    Serial.print(F("Twinkles_FL = "));
    Serial.println(sizeof(EC::Twinkles_FL));
}

//------------------------------------------------------------------------------
