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

//------------------------------------------------------------------------------

// the LED strip
CRGB leds[NUM_LEDS];

// run max. 10 Animations simultaneously
EC::AnimationRunner<10> animations;

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_FLIP_BTN, INPUT_PULLUP);
    pinMode(PIN_COLOR_POT, INPUT_PULLUP);
    pinMode(PIN_SPEED_POT, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    Serial.begin(115200);
    Serial.println(F("Welcome to EyeCandy"));
}

//------------------------------------------------------------------------------

const uint16_t defaultAnimationDuration = 20;
uint16_t animationDuration = defaultAnimationDuration;

void makeBalls(EC::AnimationRepo &repo)
{
    repo.add(new EC::BouncingBalls_FL<>(leds, NUM_LEDS));
}

void makeBlobs(EC::AnimationRepo &repo)
{
    repo.add(new EC::FloatingBlobs_FL(leds, NUM_LEDS));
}

void makeDoubleFire(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope_FL(leds, NUM_LEDS);

    auto fire = new EC::Fire2012_FL<NUM_LEDS>(leds, NUM_LEDS);
    fire->resizeStrip(kaleidoscope->remainLedCount() + 2);
    // fire->COOLING = 155;
    fire->SPARKING = 75;
    fire->animationDelay = 10;
    fire->mirrored = true;

    repo.add(fire);
    repo.add(kaleidoscope);
}

void makeFire(EC::AnimationRepo &repo)
{
    repo.add(new EC::Fire2012_FL<NUM_LEDS>(leds, NUM_LEDS));
}

void makeFireAndBalls(EC::AnimationRepo &repo)
{
    auto fire = new EC::Fire2012_FL<NUM_LEDS>(leds, NUM_LEDS);
    fire->COOLING = 155;
    fire->SPARKING = 75;
    fire->animationDelay = 10;

    auto balls = new EC::BouncingBalls_FL<>(leds, NUM_LEDS, true);
    balls->mirrored = true;

    repo.add(fire);
    repo.add(balls);
}

void makeFireworks(EC::AnimationRepo &repo)
{
    repo.add(new EC::FadeOut_FL(leds, NUM_LEDS, EC::Firework_fadeRate_default()));
    repo.add(new EC::Firework_FL<>(leds, NUM_LEDS, true, 1500));
    repo.add(new EC::Firework_FL<>(leds, NUM_LEDS, true, 3000));
    repo.add(new EC::Firework_FL<>(leds, NUM_LEDS, true, 4500));
    repo.add(new EC::Firework_FL<>(leds, NUM_LEDS, true, 6000));
    repo.add(new EC::Firework_FL<>(leds, NUM_LEDS, true, 7500));
    animationDuration = 3 * defaultAnimationDuration;
}

void makePride(EC::AnimationRepo &repo)
{
    auto kaleidoscope = new EC::Kaleidoscope_FL(leds, NUM_LEDS);

    auto pride = new EC::Pride2015_FL(leds, NUM_LEDS);
    pride->resizeStrip(kaleidoscope->remainLedCount());
    pride->mirrored = true;

    repo.add(pride);
    repo.add(kaleidoscope);
}

void makeRainbow(EC::AnimationRepo &repo)
{
    repo.add(new EC::Rainbow_FL(leds, NUM_LEDS));
}

void makeRainbowBuiltin(EC::AnimationRepo &repo)
{
    repo.add(new EC::RainbowBuiltin_FL(leds, NUM_LEDS));
}

void makeRainbowTwinkle(EC::AnimationRepo &repo)
{
    auto rainbow = new EC::RainbowTwinkle_FL(leds, NUM_LEDS);
    rainbow->animationDelay = 25;
    repo.add(rainbow);
}

void makeTwinkles(EC::AnimationRepo &repo)
{
    repo.add(new EC::Twinkles_FL(leds, NUM_LEDS));
}

//------------------------------------------------------------------------------

EC::AnimationBuilderFct allAnimations[] = {
    &makeRainbowBuiltin,
    &makeTwinkles,
    &makeRainbow,
    &makeBlobs,
    &makeRainbowTwinkle,
    &makePride,
    &makeBalls,
    &makeFire,
    &makeFireAndBalls,
    &makeDoubleFire,
    &makeFireworks,
    nullptr};

EC::AnimationChanger animationChanger(animations, allAnimations);

//------------------------------------------------------------------------------

void handleAnimationChange()
{
    static uint32_t changeTrigger = animationDuration * 1000;

    const uint32_t now = millis();

    if (now > changeTrigger)
    {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        animationDuration = defaultAnimationDuration;
        animationChanger.selectNext();
        changeTrigger = now + animationDuration * 1000;
    }
}

//------------------------------------------------------------------------------

void loop()
{
    handleAnimationChange();

    if (animations.process())
    {
        FastLED.show();
    }
}

//------------------------------------------------------------------------------
