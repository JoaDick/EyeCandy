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
#include <Animation_IO_config.h>
#include <ButtonHandler.h>

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

// run max. 8 Animations simultaneously
EC::AnimationRunnerS animationRunner;

ButtonHandler selectButton;

bool autoMode = true;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_SELECT_BTN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    Serial.begin(115200);
    Serial.println();
    Serial.println(F("Welcome to EyeCandy"));
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
const uint16_t animationDuration = defaultAnimationDuration;

void makeBalls(EC::AnimationRepo &repo)
{
    repo.add(new EC::BouncingBalls<>(leds, NUM_LEDS));
}

void makeBlobs(EC::AnimationRepo &repo)
{
    repo.add(new EC::FloatingBlobs(leds, NUM_LEDS));
}

void makeBubbles(EC::AnimationRepo &repo)
{
    repo.add(new EC::Pacifica(leds, NUM_LEDS));
    repo.add(new EC::FadeOut(leds, NUM_LEDS, true, 150));
    repo.add(new EC::Bubbles(leds, NUM_LEDS, true));
}

void makeFire(EC::AnimationRepo &repo)
{
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, NUM_LEDS);
    auto fireChanger = new EC::Fire2012Changer<NUM_LEDS>(*fire);

    repo.add(fireChanger);
    repo.add(fire);
}

void makeFireAndBalls(EC::AnimationRepo &repo)
{
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, NUM_LEDS);
    fire->COOLING = 155;
    fire->SPARKING = 75;
    fire->animationDelay = 10;

    auto balls = new EC::BouncingBalls<>(leds, NUM_LEDS, true);
    balls->mirrored = true;

    repo.add(fire);
    repo.add(balls);
}

void makeFireworks(EC::AnimationRepo &repo)
{
    repo.add(new EC::FadeOut(leds, NUM_LEDS, EC::Firework_fadeRate_default()));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 1500));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 3000));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 4500));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 6000));
    repo.add(new EC::Firework<>(leds, NUM_LEDS, true, 7500));
    // animationDuration = 3 * defaultAnimationDuration;
}

void makeFlare(EC::AnimationRepo &repo)
{
    const uint16_t fireLedCount = NUM_LEDS / 2 + NUM_LEDS / 10;
    auto fire = new EC::Fire2012<NUM_LEDS>(leds, fireLedCount);
    fire->SPARKING = 75;
    fire->animationDelay = 10;
    fire->mirrored = true;

    repo.add(fire);
    repo.add(new EC::Kaleidoscope(leds, NUM_LEDS));
}

void makePacifica(EC::AnimationRepo &repo)
{
    repo.add(new EC::Pacifica(leds, NUM_LEDS));
}

void makePride(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope(leds, NUM_LEDS);

    auto pride = new EC::Pride2015(leds, NUM_LEDS);
    pride->resizeStrip(kaleidoscope->remainLedCount());
    pride->mirrored = true;

    repo.add(pride);
    repo.add(kaleidoscope);
}

void makeRainbow(EC::AnimationRepo &repo)
{
    repo.add(new EC::Rainbow(leds, NUM_LEDS));
}

void makeRainbowBuiltin(EC::AnimationRepo &repo)
{
    repo.add(new EC::RainbowBuiltin(leds, NUM_LEDS));
}

void makeRainbowTwinkle(EC::AnimationRepo &repo)
{
    auto rainbow = new EC::RainbowTwinkle(leds, NUM_LEDS);
    rainbow->animationDelay = 25;
    repo.add(rainbow);
}

void makeTwinkles(EC::AnimationRepo &repo)
{
    repo.add(new EC::Twinkles(leds, NUM_LEDS));
}

void makeWaterfall(EC::AnimationRepo &repo)
{
    repo.add(new EC::Waterfall(leds, NUM_LEDS));
}

//------------------------------------------------------------------------------

EC::AnimationBuilderFct allAnimations[] = {
    &makeTwinkles,
    &makeRainbowBuiltin,
    &makeRainbow,
    &makeBlobs,
    &makeRainbowTwinkle,
    &makePride,
    &makePacifica,
    &makeBubbles,
    &makeWaterfall,
    &makeBalls,
    &makeFire,
    &makeFireAndBalls,
    &makeFlare,
    &makeFireworks,
    nullptr};

EC::AnimationChangerSoft animationChanger(animationRunner, allAnimations);

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

    digitalWrite(LED_BUILTIN, autoMode);
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
}

//------------------------------------------------------------------------------
