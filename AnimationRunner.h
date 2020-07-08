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

  /** Helper class for processing multiple Animations simultaneously.
   * In the setup() function, use add() to add the animations to be processed.
   * In the loop() function, call this Runner's process() method. When it
   * returns true, eventually call FastLED.show() - done!
   * @tparam MAX_ANIMATIONS Maximum number of Animations that can be added.
   */
  template <uint8_t MAX_ANIMATIONS>
  class AnimationRunner
      : public Animation,
        public AnimationRepo
  {
    static_assert(MAX_ANIMATIONS <= 16, "Max. 16 Animations possible");
    Animation *_animations[MAX_ANIMATIONS] = {nullptr};
    uint16_t _toDeleteFlags = 0;

  public:
    /// Constructor.
    AnimationRunner()
        : Animation(TYPE_SOLID_PATTERN)
    {
    }

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
      uint16_t mask = 1;
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
      uint16_t mask = 1;
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
    bool processAnimation(uint32_t currentMillis, bool wasModified) override
    {
      for (uint8_t i = 0; i < MAX_ANIMATIONS; ++i)
      {
        Animation *animation = _animations[i];
        if (animation)
        {
          wasModified = animation->process(currentMillis, wasModified);
        }
        else
        {
          break;
        }
      }
      return wasModified;
    }
  };

} // namespace EC
