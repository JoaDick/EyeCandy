#pragma once
/*******************************************************************************

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

#include <FastLED.h>
#include "Animation.h"
#include "SetupEnv.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for cycling through different Animation Scenes.
   */
  class AnimationChanger
      : public Animation
  {
  public:
    /** Constructor.
     * @param setupEnv  Setup environment for Animation Scenes.
     * @param allAnimations Array with all functions that set up an AnimationScene.
     *                      Last entry must be NULL.
     */
    AnimationChanger(SetupEnv &setupEnv,
                     AnimationSceneMakerFct allAnimations[])
        : _setupEnv{setupEnv},
          _allAnimationBuilders(allAnimations)

    {
      selectFirst();
    }

    /// Select the first AnimationScene.
    void selectFirst()
    {
      _nextIndex = 0;
      selectNext();
    }

    /** Select the next AnimationScene.
     * @return Index of currently selected AnimationScene.
     */
    uint8_t selectNext()
    {
      const uint8_t retval = _nextIndex;
      AnimationSceneMakerFct animationBuilder = _allAnimationBuilders[_nextIndex];
      if (animationBuilder)
      {
        _setupEnv.reset();
        animationBuilder(_setupEnv);
        if (_allAnimationBuilders[++_nextIndex] == nullptr)
        {
          _nextIndex = 0;
        }
      }
      return retval;
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      return _setupEnv.scene().process(currentMillis);
    }

  private:
    SetupEnv &_setupEnv;
    AnimationSceneMakerFct *_allAnimationBuilders;
    uint8_t _nextIndex;
  };

  //------------------------------------------------------------------------------

  /** Helper class for cycling through different Animation Scenes.
   * Implements a soft fade-out / fade-in effect when changing the Animation.
   * @note This class controls the overall brightness of the LED strip via
   * FastLED.setBrightness()
   */
  class AnimationChangerSoft
      : public Animation
  {
  public:
    /** Maximum brightness of the LED strip.
     * This setting can be adjusted at runtime.
     */
    uint8_t maxBrightness = 255;

    /** Total duration of the fade-out & fade-in effect (in ms).
     * This setting can be adjusted at runtime.
     */
    uint16_t fadingDuration = 1000;

    /** Constructor.
     * @param setupEnv  Setup environment for Animation Scenes.
     * @param allAnimations  Array with all functions that set up an AnimationScene.
     *                       Last entry must be NULL.
     */
    AnimationChangerSoft(SetupEnv &setupEnv,
                         AnimationSceneMakerFct allAnimations[])
        : _setupEnv{setupEnv},
          _allAnimationBuilders(allAnimations)
    {
      selectFirst();
    }

    /// Select the first AnimationScene.
    void selectFirst()
    {
      _nextIndex = 0;
      selectNext();
    }

    /** Select the next AnimationScene.
     * @return Index of currently selected AnimationScene.
     */
    uint8_t selectNext()
    {
      const uint8_t retval = _nextIndex;
      _nextAnimationBuilder = _allAnimationBuilders[_nextIndex];
      if (_nextAnimationBuilder)
      {
        if (_allAnimationBuilders[++_nextIndex] == nullptr)
        {
          _nextIndex = 0;
        }
      }
      return retval;
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      FastLED.setBrightness(processTakeover(currentMillis));
      return _setupEnv.scene().process(currentMillis);
    }

    uint8_t processTakeover(uint32_t currentMillis)
    {
      int16_t linearBrightness = 255;

      // must fade out?
      if (_nextAnimationBuilder)
      {
        if (_fadingStartTime == 0)
        {
          _fadingStartTime = currentMillis;
        }
        linearBrightness = map(currentMillis, _fadingStartTime, _fadingStartTime + fadingDuration / 2, 255, 0);
        if (linearBrightness <= 0)
        {
          linearBrightness = 0;
          FastLED.clear();
          _setupEnv.reset();
          _nextAnimationBuilder(_setupEnv);
          _nextAnimationBuilder = nullptr;
          _fadingStartTime = currentMillis;
        }
      }
      else
      {
        // must fade in?
        if (_fadingStartTime > 0)
        {
          linearBrightness = map(currentMillis, _fadingStartTime, _fadingStartTime + fadingDuration / 2, 0, 255);
          if (linearBrightness >= 255)
          {
            linearBrightness = 255;
            _fadingStartTime = 0;
          }
        }
      }

      const uint16_t quadBrightness = (linearBrightness * linearBrightness) >> 8;
      return map(quadBrightness, 0, 255, 0, maxBrightness);
    }

  private:
    SetupEnv &_setupEnv;
    AnimationSceneMakerFct *_allAnimationBuilders;
    AnimationSceneMakerFct _nextAnimationBuilder = nullptr;
    uint8_t _nextIndex;
    uint32_t _fadingStartTime = 0;
  };

} // namespace EC
