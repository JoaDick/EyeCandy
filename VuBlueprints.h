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
#include "VuOverlayLine.h"
#include "VuOverlayStripe.h"
#include "VuOverlayDot.h"
#include "VuOverlayRainbowLine.h"
#include "VuOverlayRainbowStripe.h"
#include "VuOverlayRainbowDot.h"
#include "VuOverlayPeakGlitter.h"
#include "VuSourcePeakHold.h"
#include "VuSourcePeakGravity.h"
#include "VuSourcePeakForce.h"

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  struct BlueprintBasicVU
  {
    /// Recommended fading speed for this Blueprint.
    static constexpr uint8_t fadeRate_default = 50;

    VuSourcePeakHold *peakSource;

    VuOverlayLine *vuLevelBar;
    VuOverlayDot *vuPeakDot;

    explicit BlueprintBasicVU(SetupEnv &env,
                              uint8_t fadeRate = fadeRate_default)
        : BlueprintBasicVU(env.strip(), env.scene(), env.addVuBackground(fadeRate))
    {
    }

    BlueprintBasicVU(FastLedStrip strip, AnimationScene &scene, VuSource &vuSource)
    {
      peakSource = scene.append(new VuSourcePeakHold(vuSource));
      vuLevelBar = scene.append(new VuOverlayLine(strip, vuSource));
      vuPeakDot = scene.append(new VuOverlayDot(strip, *peakSource));
    }

    VuSource &getVuSource()
    {
      return vuLevelBar->getInputVuSource();
    }
  };

  //------------------------------------------------------------------------------

  struct BlueprintRainbowVU
  {
    /// Recommended fading speed for this Blueprint.
    static constexpr uint8_t fadeRate_default = 50;

    VuSourcePeakGravity *peakSource;

    VuOverlayRainbowLine *vuLevelBar;
    VuOverlayRainbowDot *vuPeakDot;

    void setBPM(float bpm)
    {
      vuLevelBar->color.bpm = bpm;
      vuPeakDot->color.bpm = bpm;
    }

    void setVuRange(float vuHueRange)
    {
      vuLevelBar->color.hueRange = vuHueRange;
      vuPeakDot->color.hueRange = vuHueRange;
    }

    void setVolume(float volume)
    {
      vuLevelBar->color.volume = volume;
      vuPeakDot->color.volume = volume;
    }

    explicit BlueprintRainbowVU(SetupEnv &env,
                                uint8_t fadeRate = fadeRate_default)
        : BlueprintRainbowVU(env.strip(), env.scene(), env.addVuBackground(fadeRate))
    {
    }

    BlueprintRainbowVU(FastLedStrip strip, AnimationScene &scene, VuSource &vuSource)
    {
      peakSource = scene.append(new VuSourcePeakGravity(vuSource));
      vuLevelBar = scene.append(new VuOverlayRainbowLine(strip, vuSource));
      vuPeakDot = scene.append(new VuOverlayRainbowDot(strip, *peakSource, vuSource));
    }

    VuSource &getVuSource()
    {
      return vuLevelBar->getLevelVuSource();
    }
  };

  //------------------------------------------------------------------------------

  struct BlueprintEjectingDotVu
  {
    /// Recommended fading speed for this Blueprint.
    static constexpr uint8_t fadeRate_default = 25;

    VuSourcePeakGravity *vuPeakSource1;
    VuSourcePeakGravity *vuPeakSource2;

    VuOverlayRainbowLine *vuLevelBar;
    VuOverlayRainbowDot *vuPeakDot1;
    VuOverlayRainbowDot *vuPeakDot2;

    explicit BlueprintEjectingDotVu(SetupEnv &env,
                                    uint8_t fadeRate = fadeRate_default)
        : BlueprintEjectingDotVu(env.strip(), env.scene(), env.addVuBackground(fadeRate))
    {
    }

    BlueprintEjectingDotVu(FastLedStrip strip, AnimationScene &scene, VuSource &vuSource)
    {
      vuPeakSource1 = scene.append(new VuSourcePeakGravity(vuSource));
      vuPeakSource1->vuPeakHandler.a0 = -0.6;
      vuPeakSource1->vuPeakHandler.v0 = 0.3;

      vuPeakSource2 = scene.append(new VuSourcePeakGravity(vuPeakSource1->asVuSource()));
      vuPeakSource2->vuPeakHandler.a0 = 0.2;
      vuPeakSource2->vuPeakHandler.v0 = 0.0;

      vuLevelBar = scene.append(new VuOverlayRainbowLine(strip, vuSource));
      vuPeakDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource1, vuSource));
      vuPeakDot2 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource2, vuSource));

      vuPeakDot1->color.hueRange = 1.0;
      vuPeakDot2->color.hueRange = vuPeakDot1->color.hueRange;
    }

    VuSource &getVuSource()
    {
      return vuLevelBar->getLevelVuSource();
    }
  };

  //------------------------------------------------------------------------------

  struct BlueprintFranticVu
  {
    /// Recommended fading speed for this Blueprint.
    static constexpr uint8_t fadeRate_default = 45;

    VuSourcePeakGravity *vuPeakSource1;
    VuSourcePeakGravity *vuDipSource1;

    VuOverlayRainbowStripe *vuLevelStrip;
    VuOverlayRainbowDot *vuPeakDot1;
    VuOverlayRainbowDot *vuDipDot1;

    void setdotVuHueRange(float dotVuHueRange)
    {
      vuPeakDot1->color.hueRange = dotVuHueRange;
      vuDipDot1->color.hueRange = dotVuHueRange;
    }

    explicit BlueprintFranticVu(SetupEnv &env,
                                uint8_t fadeRate = fadeRate_default)
        : BlueprintFranticVu(env.strip(), env.scene(), env.addVuBackground(fadeRate))
    {
    }

    BlueprintFranticVu(FastLedStrip strip, AnimationScene &scene, VuSource &vuSource)
    {
      vuPeakSource1 = scene.append(new VuSourcePeakGravity(vuSource));
      vuPeakSource1->vuPeakHandler.a0 = -0.6;
      vuPeakSource1->vuPeakHandler.v0 = 0.3;

      vuDipSource1 = scene.append(new VuSourcePeakGravity(vuSource));
      vuDipSource1->vuPeakHandler = vuPeakSource1->vuPeakHandler;
      vuDipSource1->vuPeakHandler.enableDipMode = true;

      vuLevelStrip = scene.append(new VuOverlayRainbowStripe(strip, vuSource));
      vuPeakDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource1, *vuDipSource1));
      vuDipDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuDipSource1, *vuPeakSource1));

      vuLevelStrip->color.bpm = 6.0;
      vuLevelStrip->color.hueRange = 0.1;
      setdotVuHueRange(1.5);
    }

    VuSource &getVuSource()
    {
      return vuLevelStrip->getLevelVuSource();
    }
  };

  //------------------------------------------------------------------------------

  struct BlueprintCrazyVu
  {
    /// Recommended fading speed for this Blueprint.
    static constexpr uint8_t fadeRate_default = 45;

    VuSourcePeakGravity *vuPeakSource1;
    VuSourcePeakGravity *vuPeakSource2;
    VuSourcePeakGravity *vuDipSource1;
    VuSourcePeakGravity *vuDipSource2;

    VuOverlayRainbowStripe *vuLevelStrip;
    VuOverlayRainbowDot *vuPeakDot1;
    VuOverlayRainbowDot *vuPeakDot2;
    VuOverlayRainbowDot *vuDipDot1;
    VuOverlayRainbowDot *vuDipDot2;

    void setdotVuHueRange(float dotVuHueRange)
    {
      vuPeakDot1->color.hueRange = dotVuHueRange;
      vuPeakDot2->color.hueRange = dotVuHueRange;
      vuDipDot1->color.hueRange = dotVuHueRange;
      vuDipDot2->color.hueRange = dotVuHueRange;
    }

    explicit BlueprintCrazyVu(SetupEnv &env,
                              uint8_t fadeRate = fadeRate_default)
        : BlueprintCrazyVu(env.strip(), env.scene(), env.addVuBackground(fadeRate))
    {
    }

    BlueprintCrazyVu(FastLedStrip strip, AnimationScene &scene, VuSource &vuSource)
    {
      vuPeakSource1 = scene.append(new VuSourcePeakGravity(vuSource));
      vuPeakSource1->vuPeakHandler.a0 = -0.6;
      vuPeakSource1->vuPeakHandler.v0 = 0.3;

      vuPeakSource2 = scene.append(new VuSourcePeakGravity(vuPeakSource1->asVuSource()));
      vuPeakSource2->vuPeakHandler.a0 = 0.2;
      vuPeakSource2->vuPeakHandler.v0 = 0.0;

      vuDipSource1 = scene.append(new VuSourcePeakGravity(vuSource));
      vuDipSource1->vuPeakHandler = vuPeakSource1->vuPeakHandler;
      vuDipSource1->vuPeakHandler.enableDipMode = true;

      vuDipSource2 = scene.append(new VuSourcePeakGravity(vuDipSource1->asVuSource()));
      vuDipSource2->vuPeakHandler = vuPeakSource2->vuPeakHandler;
      vuDipSource2->vuPeakHandler.enableDipMode = true;

      vuLevelStrip = scene.append(new VuOverlayRainbowStripe(strip, vuSource));

      vuPeakDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource1, *vuDipSource1));
      vuPeakDot2 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource2, *vuDipSource1));

      vuDipDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuDipSource1, *vuPeakSource1));
      vuDipDot2 = scene.append(new VuOverlayRainbowDot(strip, *vuDipSource2, *vuPeakSource1));

      vuLevelStrip->color.bpm = 6.0;
      vuLevelStrip->color.hueRange = 0.1;
      setdotVuHueRange(1.5);
    }

    VuSource &getVuSource()
    {
      return vuLevelStrip->getLevelVuSource();
    }
  };

  //------------------------------------------------------------------------------

  struct BlueprintBeyondCrazyVu
  {
    /// Recommended fading speed for this Blueprint.
    static constexpr uint8_t fadeRate_default = 45;

    VuSourcePeakForce *vuLevelSource;

    VuSourcePeakGravity *vuPeakSource1;
    VuSourcePeakGravity *vuPeakSource2;
    VuSourcePeakGravity *vuPeakSource3;
    VuSourcePeakGravity *vuDipSource1;
    VuSourcePeakGravity *vuDipSource2;
    VuSourcePeakGravity *vuDipSource3;

    VuOverlayRainbowStripe *vuLevelStrip;
    VuOverlayRainbowDot *vuPeakDot1;
    VuOverlayRainbowDot *vuPeakDot2;
    VuOverlayRainbowDot *vuPeakDot3;
    VuOverlayRainbowDot *vuDipDot1;
    VuOverlayRainbowDot *vuDipDot2;
    VuOverlayRainbowDot *vuDipDot3;

    void setdotVuHueRange(float dotVuHueRange)
    {
      vuPeakDot1->color.hueRange = dotVuHueRange;
      vuPeakDot2->color.hueRange = dotVuHueRange;
      vuPeakDot3->color.hueRange = dotVuHueRange;
      vuDipDot1->color.hueRange = dotVuHueRange;
      vuDipDot2->color.hueRange = dotVuHueRange;
      vuDipDot3->color.hueRange = dotVuHueRange;
    }

    explicit BlueprintBeyondCrazyVu(SetupEnv &env,
                                    uint8_t fadeRate = fadeRate_default)
        : BlueprintBeyondCrazyVu(env.strip(), env.scene(), env.addVuBackground(fadeRate))
    {
    }

    BlueprintBeyondCrazyVu(FastLedStrip strip, AnimationScene &scene, VuSource &vuSource)
    {
      vuLevelSource = scene.append(new EC::VuSourcePeakForce(vuSource));
      vuLevelSource->vuPeakHandler.mass = 0.75;

      vuPeakSource1 = scene.append(new VuSourcePeakGravity(vuLevelSource->asVuSource()));
      vuPeakSource1->vuPeakHandler.a0 = -0.6;
      vuPeakSource1->vuPeakHandler.v0 = 0.3;

      vuPeakSource2 = scene.append(new VuSourcePeakGravity(vuPeakSource1->asVuSource()));
      vuPeakSource2->vuPeakHandler.a0 = -0.25;
      vuPeakSource2->vuPeakHandler.v0 = 0.05;

      vuPeakSource3 = scene.append(new VuSourcePeakGravity(vuPeakSource2->asVuSource()));
      vuPeakSource3->vuPeakHandler.a0 = 0.2;
      vuPeakSource3->vuPeakHandler.v0 = 0.0;

      vuDipSource1 = scene.append(new VuSourcePeakGravity(vuLevelSource->asVuSource()));
      vuDipSource1->vuPeakHandler = vuPeakSource1->vuPeakHandler;
      vuDipSource1->vuPeakHandler.enableDipMode = true;

      vuDipSource2 = scene.append(new VuSourcePeakGravity(vuDipSource1->asVuSource()));
      vuDipSource2->vuPeakHandler = vuPeakSource2->vuPeakHandler;
      vuDipSource2->vuPeakHandler.enableDipMode = true;

      vuDipSource3 = scene.append(new VuSourcePeakGravity(vuDipSource2->asVuSource()));
      vuDipSource3->vuPeakHandler = vuPeakSource3->vuPeakHandler;
      vuDipSource3->vuPeakHandler.enableDipMode = true;

      vuLevelStrip = scene.append(new VuOverlayRainbowStripe(strip, vuLevelSource->asVuSource()));

      vuPeakDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource1, *vuDipSource1));
      vuPeakDot2 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource2, *vuDipSource1));
      vuPeakDot3 = scene.append(new VuOverlayRainbowDot(strip, *vuPeakSource3, *vuDipSource1));

      vuDipDot1 = scene.append(new VuOverlayRainbowDot(strip, *vuDipSource1, *vuPeakSource1));
      vuDipDot2 = scene.append(new VuOverlayRainbowDot(strip, *vuDipSource2, *vuPeakSource1));
      vuDipDot3 = scene.append(new VuOverlayRainbowDot(strip, *vuDipSource3, *vuPeakSource1));

      vuLevelStrip->color.bpm = 6.0;
      vuLevelStrip->color.hueRange = 0.1;
      setdotVuHueRange(1.5);
    }

    VuSource &getVuSource()
    {
      return vuLevelSource->getInputVuSource();
    }
  };

  //------------------------------------------------------------------------------

} // namespace EC
