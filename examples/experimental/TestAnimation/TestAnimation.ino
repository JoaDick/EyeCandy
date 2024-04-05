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

/// A sketch for Animation testing during EyeCandy development.

//------------------------------------------------------------------------------

#define PRINT_MEMORY_USAGE 1
#define PRINT_PATTERN_RATE 0

//------------------------------------------------------------------------------

// #define EC_DEFAULT_UPDATE_PERIOD 20

#include <EyeCandy.h>
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// #define LED_COLOR_ORDER RGB
// #define NUM_LEDS 50
#include <Animation_IO_config.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

ButtonHandler selectButton;

bool autoMode = true;

//------------------------------------------------------------------------------

void setup()
{
    // random16_set_seed(analogRead(A3));
    // randomSeed(random16_get_seed());

    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
#if (PRINT_MEMORY_USAGE)
    printMemoryUsage();
#endif
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
uint16_t animationDuration = defaultAnimationDuration;

//------------------------------------------------------------------------------

void make_VisualizeRainbow(EC::SetupEnv &env)
{
    auto &animation = env.add(new EC::VisualizeRGB(env.strip(), false));
    auto &changer = env.add(new EC::ColorChangerRainbow(animation.color, 6.0));
    changer.colorSource.moreRed = false;
    // autoMode = false;
}

//------------------------------------------------------------------------------

void make_AnimationTemplate(EC::SetupEnv &env)
{
    auto &animation = env.add(new EC::AnimationTemplate(env.strip(), false));
    // autoMode = false;
}

//------------------------------------------------------------------------------

void make_TestAnimation(EC::SetupEnv &env)
{
    auto &animation = env.add(new EC::RetroPartyLight(env.strip()));
    // autoMode = false;
}

//------------------------------------------------------------------------------

EC::AnimationSceneMakerFct allAnimations[] = {
    // &make_VisualizeRainbow,
    // &make_AnimationTemplate,
    // &make_TestAnimation,

    // &make_Twinkles,
    // &make_RainbowBuiltin,
    // &make_Rainbow,
    &make_ColorClouds,
    &make_FloatingBlobs,
    &make_KITT,
    &make_MeteorGlitter,
    &make_RetroPartyLight,
    &make_BallLightning,
    &make_UnicornMeteor,
    // &make_RainbowTwinkle,
    // &make_Blur,
    // &make_Pride,
    &make_RainbowDrips,
    // &make_Pacifica,
    // &make_Bubbles,
    &make_Waterfall,
    &make_Lavalamp,
    // &make_BouncingBalls,
    &make_Fire<NUM_LEDS>,
    &make_Flare<NUM_LEDS>,
    &make_Fireworks,
    // &make_RgbBlocks,
    nullptr};

//------------------------------------------------------------------------------

EC::AnimationScene mainScene;
EC::SetupEnv animationSetupEnv({leds, NUM_LEDS}, mainScene);

#if (0)
EC::AnimationChanger animationChanger(animationSetupEnv, allAnimations);
#else
EC::AnimationChangerSoft animationChanger(animationSetupEnv, allAnimations);
#endif

//------------------------------------------------------------------------------

void handleAnimationChange(uint32_t currentMillis = millis())
{
    static uint32_t lastChangeTime = 0;

    bool mustChange = false;
    if (autoMode)
    {
        if (currentMillis > lastChangeTime + animationDuration * 1000)
        {
            mustChange = true;
        }
    }

    // skip to next animation with this button
    // long press = enable auto mode
    switch (selectButton.process(!digitalRead(PIN_SELECT_BTN)))
    {
    case 1:
        if (!autoMode)
        {
            mustChange = true;
        }
        autoMode = false;
        break;
    case 2:
        mustChange = true;
        autoMode = true;
        break;
    case 3:
        autoMode = false;
        mustChange = true;
        break;
    }

#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    digitalWrite(LED_BUILTIN, autoMode);
#endif
    if (mustChange)
    {
        animationDuration = defaultAnimationDuration;
        animationChanger.selectNext();
        lastChangeTime = currentMillis;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    const uint32_t currentMillis = millis();

    handleAnimationChange(currentMillis);

    if (animationChanger.process(currentMillis))
    {
#if (0)
        static bool toggleFlag = false;
        toggleFlag ^= true;
        leds[0] = toggleFlag ? CRGB(0, 10, 0) : CRGB::Black;
#endif
        FastLED.show();
#if (PRINT_PATTERN_RATE)
        printPatternRate(currentMillis);
#endif
        updateColor();
        updateSpeed();
        updateFlip();
    }

    // // this avoids nasty flickering with ESP8266 - don't know why...?!?
    // #ifdef ARDUINO_ARCH_ESP8266
    //     delay(2);
    // #endif
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

void updateColor()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_COLOR_POT), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t hue = analogValue;

        static uint16_t lastHue = 0;
        if (hue != lastHue)
        {
            // Serial.print("hue: ");
            // Serial.println(hue);
            lastHue = analogValue;
        }

        // animationChanger.maxBrightness = analogValue;
    }
}

//------------------------------------------------------------------------------

void updateSpeed()
{
    const uint16_t analogValue = constrainAndMap(analogRead(PIN_SPEED_POT), 50, 900, 0, 256);

    if (analogValue < 256)
    {
        const uint8_t animationSpeed = analogValue;
        const uint8_t modelUpdatePeriod = animationSpeed ? 256 - animationSpeed : 0;

        static uint16_t lastSpeed = 0;
        if (animationSpeed != lastSpeed)
        {
            // Serial.print("speed: ");
            // Serial.print(animationSpeed);
            // Serial.print(" delay: ");
            // Serial.println(modelUpdatePeriod);
            lastSpeed = analogValue;
        }
    }
}

//------------------------------------------------------------------------------

void updateFlip()
{
    const bool flipped = !digitalRead(PIN_FLIP_BTN);

    if (flipped)
    {
    }
    else
    {
    }
}

//------------------------------------------------------------------------------

#if (PRINT_PATTERN_RATE)
void printPatternRate(uint32_t currentMillis)
{
    static uint32_t nextPrint = 1000;
    static uint32_t counter = 0;

    ++counter;
    if (currentMillis >= nextPrint)
    {
        const uint32_t patternRate = counter;
        counter = 0;
        nextPrint = currentMillis + 1000;

        Serial.print(patternRate);
        Serial.println(F(" Hz pattern rate"));
    }
}
#endif

//------------------------------------------------------------------------------

#if (PRINT_MEMORY_USAGE)
void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

    Serial.print(F("BgFadeToBlack = "));
    Serial.println((int)sizeof(EC::BgFadeToBlack));

    Serial.print(F("BgFillColor = "));
    Serial.println((int)sizeof(EC::BgFillColor));

    Serial.print(F("BgMeteorFadeToBlack = "));
    Serial.println((int)sizeof(EC::BgMeteorFadeToBlack));

    Serial.print(F("BgRotate = "));
    Serial.println((int)sizeof(EC::BgRotate));

    Serial.print(F("BallLightning = "));
    Serial.println((int)sizeof(EC::BallLightning));

    Serial.print(F("Blur = "));
    Serial.println((int)sizeof(EC::Blur));

    Serial.print(F("BouncingBalls<> = "));
    Serial.println((int)sizeof(EC::BouncingBalls<>));

    Serial.print(F("Bubbles = "));
    Serial.println((int)sizeof(EC::Bubbles));

    Serial.print(F("ColorClouds = "));
    Serial.println((int)sizeof(EC::ColorClouds));

    Serial.print(F("Fire2012<*> = "));
    Serial.println((int)sizeof(EC::Fire2012<NUM_LEDS>));
    Serial.print(F("Fire2012Changer = "));
    Serial.println((int)sizeof(EC::Fire2012Changer<NUM_LEDS>));

    Serial.print(F("Firework<> = "));
    Serial.println((int)sizeof(EC::Firework<>));
    Serial.print(F("FireworkParticle = "));
    Serial.println((int)sizeof(EC::FireworkParticle));

    Serial.print(F("FloatingBlobs = "));
    Serial.println((int)sizeof(EC::FloatingBlobs));

    Serial.print(F("Glitter = "));
    Serial.println((int)sizeof(EC::Glitter));

    Serial.print(F("Kaleidoscope = "));
    Serial.println((int)sizeof(EC::Kaleidoscope));

    Serial.print(F("KITT = "));
    Serial.println((int)sizeof(EC::KITT));

    Serial.print(F("Lavalamp = "));
    Serial.println((int)sizeof(EC::Lavalamp));

    Serial.print(F("Meteor = "));
    Serial.println((int)sizeof(EC::Meteor));

    Serial.print(F("Pacifica = "));
    Serial.println((int)sizeof(EC::Pacifica));

    Serial.print(F("Pride2015 = "));
    Serial.println((int)sizeof(EC::Pride2015));

    Serial.print(F("Rainbow = "));
    Serial.println((int)sizeof(EC::Rainbow));

    Serial.print(F("RainbowBuiltin = "));
    Serial.println((int)sizeof(EC::RainbowBuiltin));

    Serial.print(F("RainbowDrips = "));
    Serial.println((int)sizeof(EC::RainbowDrips));

    Serial.print(F("RainbowTwinkle = "));
    Serial.println((int)sizeof(EC::RainbowTwinkle));

    Serial.print(F("RetroPartyLight = "));
    Serial.println((int)sizeof(EC::RetroPartyLight));

    Serial.print(F("RgbBlocks = "));
    Serial.println((int)sizeof(EC::RgbBlocks));

    Serial.print(F("Twinkles = "));
    Serial.println((int)sizeof(EC::Twinkles));

    Serial.print(F("Waterfall = "));
    Serial.println((int)sizeof(EC::Waterfall));
    Serial.print(F("WaterfallDroplet = "));
    Serial.println((int)sizeof(EC::WaterfallDroplet));
}
#endif

//------------------------------------------------------------------------------
