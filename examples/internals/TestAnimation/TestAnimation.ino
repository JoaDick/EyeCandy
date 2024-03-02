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

// ---------- Animations ----------

void makeBalls(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::BouncingBalls<>(strip, false));
}

void makeBlur(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Blur(strip));
    // autoMode = false;
}

void makeBubbles(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pacifica(strip));
    scene.append(new EC::BgFadeToBlack(strip, true, 150));
    scene.append(new EC::Bubbles(strip, true));
}

void makeFire(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = scene.append(new EC::Fire2012<NUM_LEDS>(strip));
    scene.append(new EC::Fire2012Changer<NUM_LEDS>(*fire));
}

void makeFireworks(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Firework<>(strip, false, 1500));
    scene.append(new EC::Firework<>(strip, true, 3100));
    scene.append(new EC::Firework<>(strip, true, 4700));
    scene.append(new EC::Firework<>(strip, true, 6300));
    scene.append(new EC::Firework<>(strip, true, 7900));
}

void makeFlare(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    const uint16_t fireLedCount = NUM_LEDS / 2 + NUM_LEDS / 10;
    EC::FastLedStrip fireStrip = strip.getSubStrip(0, fireLedCount, true);

    auto fire = scene.append(new EC::Fire2012<NUM_LEDS>(fireStrip));
    fire->SPARKING = 75;
    fire->setModelUpdatePeriod(10);

    scene.append(new EC::Kaleidoscope(strip));
}

void makeFloatingBlobs(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::FloatingBlobs(strip));
    // autoMode = false;
}

void makeGlitterDot(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    const uint8_t hue = random8();
    scene.append(new EC::BgFillColor(strip, CHSV(hue, 255, 32)));
    scene.append(new EC::Glitter(strip, true));
    scene.append(new EC::MovingDot(strip, true, CHSV(hue + 128, 255, 255)));
}

void makeLavalamp(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Lavalamp(strip));
    // autoMode = false;
}

void makePacifica(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pacifica(strip));
}

void makePride(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pride2015(strip));
}

void makeRainbow(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Rainbow(strip));
}

void makeRainbowBuiltin(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::RainbowBuiltin(strip));
}

void makeRainbowDrips(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::RainbowDrips(strip));
    // autoMode = false;
}

void makeRainbowTwinkle(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto rainbow = scene.append(new EC::RainbowTwinkle(strip));
    rainbow->setModelUpdatePeriod(25);
}

void makeRgbBlocks(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::RgbBlocks(strip));
}

void makeTwinkles(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Twinkles(strip, false));
}

void makeWaterfall(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Waterfall(strip));
}

//------------------------------------------------------------------------------

EC::AnimationSceneBuilderFct allAnimations[] = {
    // &makeRgbBlocks,

    &makeTwinkles,
    &makeRainbowBuiltin,
    &makeRainbow,
    &makeFloatingBlobs,
    &makeGlitterDot,
    &makeRainbowTwinkle,
    &makeBlur,
    &makePride,
    &makeRainbowDrips,
    &makePacifica,
    &makeBubbles,
    &makeWaterfall,
    &makeLavalamp,
    &makeBalls,
    &makeFire,
    &makeFlare,
    &makeFireworks,
    nullptr};

#if (0)
EC::AnimationChanger animationChanger(allAnimations);
#else
EC::AnimationChangerSoft animationChanger(allAnimations);
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

    Serial.print(F("Blur = "));
    Serial.println((int)sizeof(EC::Blur));

    Serial.print(F("BouncingBalls<> = "));
    Serial.println((int)sizeof(EC::BouncingBalls<>));

    Serial.print(F("Bubbles = "));
    Serial.println((int)sizeof(EC::Bubbles));

    Serial.print(F("BgFadeToBlack = "));
    Serial.println((int)sizeof(EC::BgFadeToBlack));

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

    Serial.print(F("Lavalamp = "));
    Serial.println((int)sizeof(EC::Lavalamp));

    Serial.print(F("Kaleidoscope = "));
    Serial.println((int)sizeof(EC::Kaleidoscope));

    Serial.print(F("MovingDot = "));
    Serial.println((int)sizeof(EC::MovingDot));

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

    Serial.print(F("RgbBlocks = "));
    Serial.println((int)sizeof(EC::RgbBlocks));

    Serial.print(F("BgFillColor = "));
    Serial.println((int)sizeof(EC::BgFillColor));

    Serial.print(F("Twinkles = "));
    Serial.println((int)sizeof(EC::Twinkles));

    Serial.print(F("Waterfall = "));
    Serial.println((int)sizeof(EC::Waterfall));
    Serial.print(F("WaterfallDroplet = "));
    Serial.println((int)sizeof(EC::WaterfallDroplet));
}
#endif

//------------------------------------------------------------------------------
