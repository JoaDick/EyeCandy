/*******************************************************************************

Many many Animations!

********************************************************************************

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

//------------------------------------------------------------------------------

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
    random16_set_seed(analogRead(A3));
    randomSeed(random16_get_seed());

    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR // only with Arduino boards
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
    Serial.println();
    Serial.println(F("Welcome to EyeCandy"));
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
uint16_t animationDuration = defaultAnimationDuration;

// ---------- Animations ----------

void make_Balls(EC::SetupEnv &env)
{
    env.add(new EC::BouncingBalls<>(env.strip(), false));
}

void make_Blur(EC::SetupEnv &env)
{
    env.add(new EC::Blur(env.strip()));
}

void make_Bubbles(EC::SetupEnv &env)
{
    env.add(new EC::Pacifica(env.strip()));
    env.add(new EC::BgFadeToBlack(env.strip(), true, 150));
    env.add(new EC::Bubbles(env.strip(), true));
}

void make_Fire(EC::SetupEnv &env)
{
    auto &fire = env.add(new EC::Fire2012<NUM_LEDS>(env.strip()));
    env.add(new EC::Fire2012Changer<NUM_LEDS>(fire));
}

void make_FireAndBalls(EC::SetupEnv &env)
{
    auto &fire = env.add(new EC::Fire2012<NUM_LEDS>(env.strip()));
    fire.COOLING = 155;
    fire.SPARKING = 75;
    fire.setModelUpdatePeriod(10);

    env.add(new EC::BouncingBalls<>(env.strip().getReversedStrip(), true));
}

void make_Fireworks(EC::SetupEnv &env)
{
    env.add(new EC::Firework<>(env.strip(), false, 1500));
    env.add(new EC::Firework<>(env.strip(), true, 3100));
    env.add(new EC::Firework<>(env.strip(), true, 4700));
    env.add(new EC::Firework<>(env.strip(), true, 6300));
    env.add(new EC::Firework<>(env.strip(), true, 7900));
}

void make_Flare(EC::SetupEnv &env)
{
    const uint16_t fireLedCount = NUM_LEDS / 2 + NUM_LEDS / 10;
    EC::FastLedStrip fireStrip = env.strip().getSubStrip(0, fireLedCount, true);

    auto &fire = env.add(new EC::Fire2012<NUM_LEDS>(fireStrip));
    fire.SPARKING = 75;
    fire.setModelUpdatePeriod(10);

    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_FloatingBlobs(EC::SetupEnv &env)
{
    env.add(new EC::FloatingBlobs(env.strip()));
}

void make_Lavalamp(EC::SetupEnv &env)
{
    env.add(new EC::Lavalamp(env.strip()));
}

void make_Pacifica(EC::SetupEnv &env)
{
    env.add(new EC::Pacifica(env.strip()));
}

void make_Pride(EC::SetupEnv &env)
{
    env.add(new EC::Pride2015(env.strip()));
}

void make_PrideMirror(EC::SetupEnv &env)
{
    env.add(new EC::Pride2015(env.strip().getHalfStrip(true)));
    env.add(new EC::Kaleidoscope(env.strip()));
}

void make_Rainbow(EC::SetupEnv &env)
{
    env.add(new EC::Rainbow(env.strip()));
}

void make_RainbowBuiltin(EC::SetupEnv &env)
{
    env.add(new EC::RainbowBuiltin(env.strip()));
}

void make_RainbowDrips(EC::SetupEnv &env)
{
    env.add(new EC::RainbowDrips(env.strip()));
}

void make_RainbowTwinkle(EC::SetupEnv &env)
{
    auto &rainbow = env.add(new EC::RainbowTwinkle(env.strip()));
    rainbow.color.bpm = 10.0;
}

void make_RgbBlocks(EC::SetupEnv &env)
{
    env.add(new EC::RgbBlocks(env.strip()));
}

void make_Twinkles(EC::SetupEnv &env)
{
    env.add(new EC::Twinkles(env.strip(), false));
}

void make_Waterfall(EC::SetupEnv &env)
{
    env.add(new EC::Waterfall(env.strip()));
}

//------------------------------------------------------------------------------

EC::AnimationSceneMakerFct allAnimations[] = {
    &make_Twinkles,
    &make_RainbowBuiltin,
    &make_Rainbow,
    // &make_ColorClouds,
    // &make_FloatingBlobs,
    // &make_KITT,
    // &make_MeteorGlitter,
    // &make_RetroPartyLight,
    // &make_BallLightning,
    // &make_UnicornMeteor,
    &make_RainbowTwinkle,
    &make_Blur,
    &make_Pride,
    &make_PrideMirror,
    // &make_RainbowDrips,
    &make_Pacifica,
    &make_Bubbles,
    // &make_Waterfall,
    // &make_Lavalamp,
    &make_Balls,
    &make_Fire,
    &make_FireAndBalls,
    &make_Flare,
    // &make_Fireworks,
    &make_RgbBlocks,
    nullptr};

//------------------------------------------------------------------------------

EC::AnimationScene mainScene;
EC::SetupEnv animationSetupEnv({leds, NUM_LEDS}, mainScene);
EC::AnimationChangerSoft animationChanger(animationSetupEnv, allAnimations);

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
        FastLED.show();
    }

    // // this avoids nasty flickering with ESP8266 - don't know why...?!?
    // #ifdef ARDUINO_ARCH_ESP8266
    //     delay(2);
    // #endif
}

//------------------------------------------------------------------------------
