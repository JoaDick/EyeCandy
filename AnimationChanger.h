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

#include "AnimationRunner.h"
#include "AnimationScene.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for cycling through different Animation Scenes.
   */
  class AnimationChanger
      : public Animation
  {
    AnimationRunner &_runner;
    AnimationBuilderFct *_allAnimationBuilders;
    uint8_t _nextIndex;

  public:
    /** Constructor.
     * @param runner Use this #AnimationRunner as underlying engine.
     *               Don't call its process() method from your sketch!
     * @param allAnimations Array with all functions that set up an Animation Scene.
     *                      Last entry must be NULL.
     */
    AnimationChanger(AnimationRunner &runner,
                     AnimationBuilderFct allAnimations[])
        : _runner(runner), _allAnimationBuilders(allAnimations)
    {
      selectFirst();
    }

    /// Select the first Animation Scene.
    void selectFirst()
    {
      _nextIndex = 0;
      selectNext();
    }

    /** Select the next Animation Scene.
     * @return Index of currently selected Animation Scene.
     */
    uint8_t selectNext()
    {
      const uint8_t retval = _nextIndex;
      AnimationBuilderFct animationBuilder = _allAnimationBuilders[_nextIndex];
      if (animationBuilder)
      {
        _runner.reset();
        animationBuilder(_runner);
        if (_allAnimationBuilders[++_nextIndex] == nullptr)
        {
          _nextIndex = 0;
        }
      }
      return retval;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      _runner.process(currentMillis, wasModified);
    }
  };

  //------------------------------------------------------------------------------

  /** Alternative helper class for cycling through different Animation Scenes.
   * Same as AnimationChanger, but uses an internal AnimationScene (instead of
   * an external AnimationRunner).
   */
  class AnimationSceneChanger
      : public Animation
  {
    AnimationScene _scene;
    AnimationBuilderFct *_allAnimationBuilders;
    uint8_t _nextIndex;

  public:
    /** Constructor.
     * @param allAnimations Array with all functions that set up an Animation Scene.
     *                      Last entry must be NULL.
     */
    explicit AnimationSceneChanger(AnimationBuilderFct allAnimations[])
        : _allAnimationBuilders(allAnimations)
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
      AnimationBuilderFct animationBuilder = _allAnimationBuilders[_nextIndex];
      if (animationBuilder)
      {
        _scene.reset();
        animationBuilder(_scene);
        if (_allAnimationBuilders[++_nextIndex] == nullptr)
        {
          _nextIndex = 0;
        }
      }
      return retval;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      _scene.process(currentMillis, wasModified);
    }
  };

} // namespace EC
