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

#include "PatternPresets.h"
#include "VuBlueprints.h"

#include "BallLightning.h"
#include "DancingJellyfishVU.h"
#include "Fire2012VU.h"
#include "FlowingBeatVU.h"
#include "LightbulbVU.h"
#include "RetroPartyVU.h"

//------------------------------------------------------------------------------

inline void make_BallLightningVU(EC::SetupEnv &env)
{
  env.add(new EC::TriggerPattern());
  auto &vuLevelSource = env.addVuSource();
  env.add(new EC::BallLightningVU(env.strip(), vuLevelSource));
}

inline void make_BeyondCrazyVU(EC::SetupEnv &env)
{
  EC::BlueprintBeyondCrazyVU bp(env);
}

inline void make_BeyondCrazyVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_BeyondCrazyVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_BeyondCrazyVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_BeyondCrazyVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_BlackHoleVU(EC::SetupEnv &env, bool inward)
{
  auto workStrip = env.strip().getHalfStrip(inward);

  env.add(new EC::BgFadeToBlack(20, workStrip, 40));
  env.add(new EC::BgRotate(workStrip, true));
  auto &vuLevelSource = env.addVuSource();

  auto &levelVu = env.add(new EC::VuOverlayRainbowStripe(workStrip, vuLevelSource));
  levelVu.color.hueRange = 0.5;
  levelVu.color.volume = 255;

  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_BlackHoleVU(EC::SetupEnv &env)
{
  make_BlackHoleVU(env, true);
}

inline void make_BlackAntiHoleVU(EC::SetupEnv &env)
{
  make_BlackHoleVU(env, false);
}

inline void make_CrazyVU(EC::SetupEnv &env)
{
  EC::BlueprintCrazyVU bp(env);
}

inline void make_CrazyVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_CrazyVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_CrazyVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_CrazyVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_DancingJellyfishVU(EC::SetupEnv &env)
{
  auto &vuLevelSource = env.addVuBackground(EC::DancingJellyfishVU::fadeRate_default);
  env.add(new EC::DancingJellyfishVU(env.strip(), vuLevelSource));
}

inline void make_DancingDotVU(EC::SetupEnv &env)
{
  auto &vuLevelSource = env.addVuBackground(0);
  auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));

  auto &levelVu = env.add(new EC::VuOverlayRainbowLine(env.strip(), vuLevelSource));
  levelVu.color.volume = 64;

  auto &peakVu = env.add(new EC::VuOverlayStripe(env.strip(), vuPeakSource));
}

inline void make_DoubleDancingDotVU1(EC::SetupEnv &env)
{
  env.add(new EC::Pride2015(env.strip().getHalfStrip(/*true*/)));
  env.add(new EC::Kaleidoscope(env.strip()));
  env.add(new EC::BgFadeToBlack(env.strip(), true, 150));

  auto &vuLevelSource = env.addVuSource();
  auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(vuLevelSource));
  vuPeakSource.vuPeakHandler.mass = 0.5;

  auto &vu1 = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource, vuLevelSource, 0.075));
  vu1.color.bpm = 0.7;

  auto &vu2 = env.add(new EC::VuOverlayRainbowDot(env.strip().getReversedStrip(), vuPeakSource, vuLevelSource, 0.075));
}

inline void make_DoubleDancingDotVU2(EC::SetupEnv &env)
{
  env.add(new EC::FloatingBlobs(env.strip()));
  env.add(new EC::BgFadeToBlack(env.strip(), true, 230));

  auto &vuLevelSource = env.addVuSource();
  auto &vuPeakSource1 = env.add(new EC::VuSourcePeakForce(vuLevelSource));
  vuPeakSource1.vuPeakHandler.coupling = 50.0;
  auto &vuPeakSource2 = env.add(new EC::VuSourcePeakForce(vuLevelSource));
  vuPeakSource2.vuPeakHandler.friction = 5.0;

  auto &vu1 = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource1, 0.125));
  vu1.color.bpm = 0.7;

  auto &vu2 = env.add(new EC::VuOverlayRainbowDot(env.strip().getReversedStrip(), vuPeakSource2, 0.125));
}

inline void make_EjectingDotVU(EC::SetupEnv &env)
{
  EC::BlueprintEjectingDotVU bp(env);
}

inline void make_EjectingDotVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_EjectingDotVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_EjectingDotVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_EjectingDotVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

template <uint16_t LedCount>
void make_FireVU(EC::SetupEnv &env)
{
  auto &fire = env.add(new EC::Fire2012<LedCount>(env.strip()));
  auto &vuSource = env.addVuSource();
  env.add(new EC::Fire2012VU<LedCount>(fire, vuSource));
}

template <uint16_t LedCount>
void make_FlareVU(EC::SetupEnv &env)
{
  const int16_t ledCount = env.strip().ledCount();
  const int16_t fireLedCount = ledCount / 2 + ledCount / 10;
  EC::FastLedStrip fireStrip = env.strip().getSubStrip(0, fireLedCount, true);

  auto &fire = env.add(new EC::Fire2012<LedCount>(fireStrip));
  fire.setModelUpdatePeriod(10);

  auto &vuSource = env.addVuSource();
  env.add(new EC::Fire2012VU<LedCount>(fire, vuSource));
  env.add(new EC::Kaleidoscope(env.strip()));
}

template <uint16_t LedCount>
void make_FlareDoubleVU(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_FlareVU<LedCount>(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

template <uint16_t LedCount>
void make_FlareInwardVU(EC::SetupEnv &env)
{
  EC::FastLedStrip fireStrip1 = env.strip().getHalfStrip();
  auto &fire1 = env.add(new EC::Fire2012<LedCount>(fireStrip1));
  fire1.setModelUpdatePeriod(11);

  EC::FastLedStrip fireStrip2 = env.strip().getSubStrip(fireStrip1.ledCount(), 0, true);
  auto &fire2 = env.add(new EC::Fire2012<LedCount>(fireStrip2));
  fire2.setModelUpdatePeriod(13);

  auto &vuSource = env.addVuSource();
  env.add(new EC::Fire2012VU<LedCount>(fire1, vuSource));
  env.add(new EC::Fire2012VU<LedCount>(fire2, vuSource));
}

inline void make_FlowingBeatVU(EC::SetupEnv &env)
{
  env.add(new EC::TriggerPattern(EC::FlowingBeatVU::patternUpdatePeriod));
  auto &vuLevelSource = env.addVuSource();
  env.add(new EC::FlowingBeatVU(env.strip(), vuLevelSource));
}

inline void make_FlowingBeatVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_FlowingBeatVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_FlowingBeatVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_FlowingBeatVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_FranticVU(EC::SetupEnv &env)
{
  EC::BlueprintFranticVU bp(env);
}

inline void make_FranticVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_FranticVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_FranticVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_FranticVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_LightbulbVU(EC::SetupEnv &env)
{
  env.add(new EC::TriggerPattern());
  auto &vuLevelSource = env.addVuSource();
  env.add(new EC::LightbulbVU(env.strip(), vuLevelSource));
}

inline void make_MeteorTrailVU(EC::SetupEnv &env)
{
  env.add(new EC::BgMeteorFadeToBlack(env.strip(), false));
  auto &vuLevelSource = env.addVuSource();
  auto &levelVu = env.add(new EC::VuOverlayRainbowStripe(env.strip(), vuLevelSource));
  levelVu.color.hueRange = 0.67;
  levelVu.color.volume = 255;
}

inline void make_PeakGlitterVU(EC::SetupEnv &env)
{
  auto &vuSource = env.addVuBackground(EC::VuOverlayPeakGlitter::fadeRate_default);
  env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuSource));
}

inline void make_PeakMothsVU(EC::SetupEnv &env)
{
  auto &vuLevelSource = env.addVuBackground(0);
  auto &vuLevelSource1 = env.add(new EC::VuSourcePeakForce(vuLevelSource));

  auto &levelVu = env.add(new EC::VuOverlayRainbowLine(env.strip(), vuLevelSource1));
  levelVu.color.volume = 64;

  auto &peakVu1 = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuLevelSource1));
  peakVu1.color.hueRange = 0.75;
  peakVu1.color.volume = 255;

  EC::VuSource *vuSource = &vuLevelSource1.asVuSource();
  for (uint8_t i = 1; i <= 4; ++i)
  {
    auto &vuPeakSource = env.add(new EC::VuSourcePeakForce(*vuSource));
    vuPeakSource.vuPeakHandler.mass = 1.55;
    // vuPeakSource.vuPeakHandler.friction = 10.0;
    // vuPeakSource.vuPeakHandler.coupling = 100.0;

    auto &peakVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuPeakSource));
    peakVu.color = peakVu1.color;

    vuSource = &vuPeakSource.asVuSource();
  }
}

inline void make_RainbowBallVU(EC::SetupEnv &env)
{
  EC::BlueprintRainbowVU bp(env);
  bp.peakSource->vuPeakHandler.presetPunchedBall();
  // bp.setVuRange(0.67);
}

inline void make_RainbowBalllVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_RainbowBallVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_RainbowBalllVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_RainbowBallVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_RainbowBubbleVU(EC::SetupEnv &env)
{
  EC::BlueprintRainbowVU bp(env);
  bp.peakSource->vuPeakHandler.presetFloatingBubble();
  bp.setVuRange(0.67);
}

inline void make_RainbowBubbleVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_RainbowBubbleVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_RainbowBubbleVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_RainbowBubbleVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_RainingVU(EC::SetupEnv &env)
{
  env.add(new EC::BgFadeToBlack(20, env.strip(), 20));
  env.add(new EC::BgRotate(env.strip(), true));
  auto &vuLevelSource = env.addVuSource();

  auto &levelVu = env.add(new EC::VuOverlayRainbowDot(env.strip(), vuLevelSource));
  levelVu.color.hueRange = 0.67;
  levelVu.color.volume = 192;

  auto &peakGlitter = env.add(new EC::VuOverlayPeakGlitter(env.strip(), vuLevelSource));
  peakGlitter.vuPeakHandler.peakHold = 500;
  peakGlitter.vuPeakHandler.peakDecay = 500;
}

inline void make_RainingVU_inward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip(true);
  make_RainingVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_RainingVU_outward(EC::SetupEnv &env)
{
  auto subEnv = env.clone_halfStrip();
  make_RainingVU(subEnv);
  env.add(new EC::Kaleidoscope(env.strip()));
}

inline void make_RetroPartyVU(EC::SetupEnv &env)
{
  env.add(new EC::TriggerPattern());
  auto &vuLevelSource = env.addVuSource();
  env.add(new EC::RetroPartyVU(env.strip(), vuLevelSource));
}
