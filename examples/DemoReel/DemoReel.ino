/*******************************************************************************

Many many Animations!

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
// #define NUM_LEDS 50
#include <Animation_IO_config.h>
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

ButtonHandler selectButton;

bool autoMode = true;

#define PRINT_MEMORY_USAGE 1

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
    Serial.println();
    Serial.println(F("Welcome to EyeCandy"));
#if (PRINT_MEMORY_USAGE)
    printMemoryUsage();
#endif
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
const uint16_t animationDuration = defaultAnimationDuration;

// ---------- Animations ----------

void makeBalls(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::BouncingBalls<>(strip, false));
}

void makeFloatingBlobs(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::FloatingBlobs(strip));
}

void makeBubbles(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pacifica(strip));
    scene.append(new EC::FadeOutOverlay(strip, 150));
    scene.append(new EC::Bubbles(strip, true));
}

void makeFire(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = scene.append(new EC::Fire2012<NUM_LEDS>(strip));
    scene.append(new EC::Fire2012Changer<NUM_LEDS>(*fire));
}

void makeGlitterDot(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Glitter(strip, false));
    scene.append(new EC::MovingDot(strip, true /*, CRGB::Red*/));
}

void makeFireAndBalls(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto fire = scene.append(new EC::Fire2012<NUM_LEDS>(strip));
    fire->COOLING = 155;
    fire->SPARKING = 75;
    fire->setModelUpdatePeriod(10);

    scene.append(new EC::BouncingBalls<>(strip.getReversedStrip(), true));
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

void makePrideMirror(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::Pride2015(strip.getHalfStrip(true)));
    scene.append(new EC::Kaleidoscope(strip));
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

void makeRgbBlocks(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    scene.append(new EC::RgbBlocks(strip));
}

void makeRainbowTwinkle(EC::AnimationScene &scene)
{
    EC::FastLedStrip strip(leds, NUM_LEDS);

    auto rainbow = scene.append(new EC::RainbowTwinkle(strip));
    rainbow->setModelUpdatePeriod(25);
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
    &makePride,
    &makePrideMirror,
    &makePacifica,
    &makeBubbles,
    &makeWaterfall,
    &makeBalls,
    &makeFire,
    &makeFireAndBalls,
    &makeFlare,
    &makeFireworks,
    nullptr};

// EC::AnimationChanger animationChanger(allAnimations);
EC::AnimationChangerSoft animationChanger(allAnimations);

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
    }

#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    digitalWrite(LED_BUILTIN, autoMode);
#endif
    if (mustChange)
    {
        // animationDuration = defaultAnimationDuration;
        animationChanger.selectNext();
        lastChangeTime = currentMillis;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    const uint32_t currentMillis = millis();

    handleAnimationChange(currentMillis);
    animationChanger.process(currentMillis);
    FastLED.show();

// this avoids nasty flickering with ESP8266 - don't know why...?!?
#ifdef ARDUINO_ARCH_ESP8266
    delay(2);
#endif
}

//------------------------------------------------------------------------------

#if (PRINT_MEMORY_USAGE)
void printMemoryUsage()
{
    Serial.print(F("Memory usage for "));
    Serial.print(NUM_LEDS);
    Serial.println(F(" LEDs:"));
    Serial.println(F("<*> is dependant on NUM_LEDS"));

    Serial.print(F("BouncingBalls<> = "));
    Serial.println(sizeof(EC::BouncingBalls<>));

    Serial.print(F("Bubbles = "));
    Serial.println(sizeof(EC::Bubbles));

#if (1)
    Serial.print(F("FadeOut = "));
    Serial.println(sizeof(EC::FadeOutPattern));
#else
    Serial.print(F("FadeOutOverlay = "));
    Serial.println(sizeof(EC::FadeOutOverlay));

    Serial.print(F("FadeOutPattern = "));
    Serial.println(sizeof(EC::FadeOutPattern));
#endif

    Serial.print(F("Fire2012<*> = "));
    Serial.println(sizeof(EC::Fire2012<NUM_LEDS>));
    Serial.print(F("Fire2012Changer = "));
    Serial.println(sizeof(EC::Fire2012Changer<NUM_LEDS>));

    Serial.print(F("Firework<> = "));
    Serial.println(sizeof(EC::Firework<>));
    Serial.print(F("FireworkParticle = "));
    Serial.println(sizeof(EC::FireworkParticle));

    Serial.print(F("FloatingBlobs = "));
    Serial.println(sizeof(EC::FloatingBlobs));

    Serial.print(F("Glitter = "));
    Serial.println(sizeof(EC::Glitter));

    Serial.print(F("Kaleidoscope = "));
    Serial.println(sizeof(EC::Kaleidoscope));

    Serial.print(F("MovingDot = "));
    Serial.println(sizeof(EC::MovingDot));

    Serial.print(F("Pacifica = "));
    Serial.println(sizeof(EC::Pacifica));

    Serial.print(F("Pride2015 = "));
    Serial.println(sizeof(EC::Pride2015));

    Serial.print(F("Rainbow = "));
    Serial.println(sizeof(EC::Rainbow));

    Serial.print(F("RainbowBuiltin = "));
    Serial.println(sizeof(EC::RainbowBuiltin));

    Serial.print(F("RainbowTwinkle = "));
    Serial.println(sizeof(EC::RainbowTwinkle));

    Serial.print(F("RgbBlocks = "));
    Serial.println(sizeof(EC::RgbBlocks));

    Serial.print(F("StaticBackground = "));
    Serial.println(sizeof(EC::StaticBackground));

    Serial.print(F("Twinkles = "));
    Serial.println(sizeof(EC::Twinkles));

    Serial.print(F("Waterfall = "));
    Serial.println(sizeof(EC::Waterfall));
    Serial.print(F("WaterfallDroplet = "));
    Serial.println(sizeof(EC::WaterfallDroplet));
}
#endif

//------------------------------------------------------------------------------
