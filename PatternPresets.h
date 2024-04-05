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

#include "SetupEnv.h"
#include "Backgrounds.h"
#include "ColorChanger.h"

#include "BallLightning.h"
#include "Blur.h"
#include "BouncingBalls.h"
#include "Bubbles.h"
#include "ColorClouds.h"
#include "Fire2012.h"
#include "Fire2012Changer.h"
#include "Firework.h"
#include "FloatingBlobs.h"
#include "Glitter.h"
#include "Kaleidoscope.h"
#include "KITT.h"
#include "Lavalamp.h"
#include "Meteor.h"
#include "Pacifica.h"
#include "Pride2015.h"
#include "Rainbow.h"
#include "RainbowBuiltin.h"
#include "RainbowDrips.h"
#include "RainbowTwinkle.h"
#include "RetroPartyLight.h"
#include "RgbBlocks.h"
#include "Twinkles.h"
#include "Waterfall.h"

//------------------------------------------------------------------------------

inline void make_BallLightning(EC::SetupEnv &env)
{
  auto &animation = env.add(new EC::BallLightning(env.strip()));
  auto &colorChanger = env.add(new EC::ColorChangerNoiseRGB(animation.color));
  colorChanger.colorSource.presetTwitchy();
  // colorChanger.colorSource.presetSmooth();
  env.add(new EC::Glitter(env.strip(), true, CRGB(224, 192, 112), 10));
}

inline void make_Blur(EC::SetupEnv &env)
{
  env.add(new EC::Blur(env.strip()));
}

inline void make_BouncingBalls(EC::SetupEnv &env)
{
  env.add(new EC::BouncingBalls<>(env.strip(), false));
}

inline void make_Bubbles(EC::SetupEnv &env)
{
  env.add(new EC::Pacifica(env.strip()));
  env.add(new EC::BgFadeToBlack(env.strip(), true, 150));
  env.add(new EC::Bubbles(env.strip(), true));
}

inline void make_ColorClouds(EC::SetupEnv &env)
{
  env.add(new EC::ColorClouds(env.strip()));
}

template <uint16_t LedCount>
void make_Fire(EC::SetupEnv &env)
{
  auto &fire = env.add(new EC::Fire2012<LedCount>(env.strip()));
  env.add(new EC::Fire2012Changer<LedCount>(fire));
}

inline void make_Fireworks(EC::SetupEnv &env)
{
  env.add(new EC::Firework<>(env.strip(), false, 1500));
  env.add(new EC::Firework<>(env.strip(), true, 3100));
  env.add(new EC::Firework<>(env.strip(), true, 4700));
  env.add(new EC::Firework<>(env.strip(), true, 6300));
  env.add(new EC::Firework<>(env.strip(), true, 7900));
}

template <uint16_t LedCount>
void make_Flare(EC::SetupEnv &env)
{
  const uint16_t fireLedCount = LedCount / 2 + LedCount / 10;
  EC::FastLedStrip fireStrip = env.strip().getSubStrip(0, fireLedCount, true);

  auto &fire = env.add(new EC::Fire2012<LedCount>(fireStrip));
  fire.SPARKING = 75;
  fire.setModelUpdatePeriod(10);

  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_FloatingBlobs(EC::SetupEnv &env)
{
  env.add(new EC::FloatingBlobs(env.strip()));
}

inline void make_KITT(EC::SetupEnv &env)
{
  env.add(new EC::KITT(env.strip()));
}

inline void make_Lavalamp(EC::SetupEnv &env)
{
  env.add(new EC::Lavalamp(env.strip()));
}

inline void make_MeteorGlitter(EC::SetupEnv &env)
{
  auto &meteor = env.add(new EC::Meteor(env.strip(), false));
  auto &glitter = env.add(new EC::Glitter(env.strip(), true));
  glitter.effectRate = 10;
  env.add(new EC::ColorChangerRainbow(glitter.color, 2.0, 0, 112, 160));
}

inline void make_UnicornMeteor(EC::SetupEnv &env)
{
  auto &meteor = env.add(new EC::Meteor(env.strip(), false, 10.0, 0.5));
  meteor.size = 0.01;
  meteor.color.bpm = 90;
}

inline void make_Pacifica(EC::SetupEnv &env)
{
  env.add(new EC::Pacifica(env.strip()));
}

inline void make_Pride(EC::SetupEnv &env)
{
  env.add(new EC::Pride2015(env.strip()));
}

inline void make_PrideMirror(EC::SetupEnv &env)
{
  env.add(new EC::Pride2015(env.strip().getHalfStrip(true)));
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_Rainbow(EC::SetupEnv &env)
{
  env.add(new EC::Rainbow(env.strip()));
}

inline void make_RainbowBuiltin(EC::SetupEnv &env)
{
  env.add(new EC::RainbowBuiltin(env.strip()));
}

inline void make_RainbowDrips(EC::SetupEnv &env)
{
  env.add(new EC::RainbowDrips(env.strip()));
}

inline void make_RainbowTwinkle(EC::SetupEnv &env)
{
  auto &rainbow = env.add(new EC::RainbowTwinkle(env.strip()));
  rainbow.color.bpm = 10.0;
}

inline void make_RetroPartyLight(EC::SetupEnv &env)
{
  env.add(new EC::RetroPartyLight(env.strip()));
}

inline void make_RgbBlocks(EC::SetupEnv &env)
{
  env.add(new EC::RgbBlocks(env.strip()));
}

inline void make_Twinkles(EC::SetupEnv &env)
{
  env.add(new EC::Twinkles(env.strip(), false));
}

inline void make_Waterfall(EC::SetupEnv &env)
{
  env.add(new EC::Waterfall(env.strip()));
}
