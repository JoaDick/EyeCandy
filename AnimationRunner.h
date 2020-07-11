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

#include "Animation.h"
#include "AnimationRepo.h"

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  /** Interface for processing multiple Animations simultaneously.
   * In the setup() function, use add() to add the animations to be processed.
   * In the loop() function, call this Runner's process() method. When it
   * returns true, eventually call FastLED.show() - done!
   * The following methods need to be implemented:
   * - AnimationRepo::add()
   * - AnimationRepo::reset()
   * - Animation::processAnimation()
   */
  class AnimationRunner
      : public Animation,
        public AnimationRepo
  {

  public:
    /// Constructor.
    AnimationRunner()
        : Animation(TYPE_SOLID_PATTERN)
    {
    }
  };

  //------------------------------------------------------------------------------

  /// Base class for AnimationRunnerS / AnimationRunnerM / AnimationRunnerL.
  template <typename UINT_T>
  class BasicAnimationRunner
      : public AnimationRunner
  {
    static constexpr uint8_t MAX_ANIMATIONS = 8 * sizeof(UINT_T);
    Animation *_animations[MAX_ANIMATIONS] = {nullptr};
    UINT_T _toDeleteFlags = 0;

  public:
    /// @see AnimationRepo::add()
    bool add(Animation &animation) override
    {
      for (uint8_t i = 0; i < MAX_ANIMATIONS; ++i)
      {
        if (_animations[i] == nullptr)
        {
          _animations[i] = &animation;
          return true;
        }
      }
      return false;
    }

    /// @see AnimationRepo::add()
    bool add(Animation *animation) override
    {
      UINT_T mask = 1;
      for (uint8_t i = 0; i < MAX_ANIMATIONS; ++i)
      {
        if (_animations[i] == nullptr)
        {
          _animations[i] = animation;
          _toDeleteFlags |= mask;
          return true;
        }
        mask <<= 1;
      }
      return false;
    }

    /// @see AnimationRepo::reset()
    void reset() override
    {
      UINT_T mask = 1;
      for (uint8_t i = 0; i < MAX_ANIMATIONS; ++i)
      {
        if (_toDeleteFlags & mask)
        {
          delete _animations[i];
        }
        _animations[i] = nullptr;
        mask <<= 1;
      }
      _toDeleteFlags = 0;
    }

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      for (uint8_t i = 0; i < MAX_ANIMATIONS; ++i)
      {
        Animation *animation = _animations[i];
        if (animation)
        {
          animation->process(currentMillis, wasModified);
        }
        else
        {
          break;
        }
      }
    }
  };

  //------------------------------------------------------------------------------

  /// "Small" AnimationRunner for max 8 Animations.
  using AnimationRunnerS = BasicAnimationRunner<uint8_t>;

  /// "Medium" AnimationRunner for max 16 Animations.
  using AnimationRunnerM = BasicAnimationRunner<uint16_t>;

  /// "Large" AnimationRunner for max 32 Animations.
  using AnimationRunnerL = BasicAnimationRunner<uint32_t>;

  //------------------------------------------------------------------------------

} // namespace EC
