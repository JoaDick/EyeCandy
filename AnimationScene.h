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

#include "Animation.h"
#include "AnimationRepo.h" // Deprecated - only temporary.

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for composing a complex Animation out of multiple separate Animations.
   * When the AnimationScene's process() method gets called, it calls the process()
   * methods of all appended Animations in the order as they were appended. \n
   * The first one should be a Pattern (which renders the entire LED strip),
   * and all subsequently appended should be Overlays (which only render certain LEDs).
   * Pseudo-Animations, which don't manipulate the LED strip (but perform other
   * processing), can be appended at any position.
   * @see append()
   * @note All appended Animations \e must be allocated on the heap. This class will
   * destroy them accordingly.
   */
  class AnimationScene
      : public Animation,
        public AnimationRepo // Deprecated - only temporary for compatibility.
  {
  public:
    /// Destructor. Also destroys all previously added Animations.
    ~AnimationScene()
    {
      reset();
    }

    /** Append the given \a animation to the AnimationScene.
     * @note \a animation must be allocated on the heap. The AnimationScene takes
     * care of deleting it when no more needed.
     */
    template <class AnimationType>
    AnimationType *append(AnimationType *animation)
    {
#if (1)
      add(animation);
#else
      Animation *tail = findTail();
      if (tail)
      {
        tail->next = animation;
      }
      else
      {
        _head = animation;
      }
#endif
      return animation;
    }

    /** Remove (and delete) all previously added Animations.
     * This AnimationScene is empty afterwards, and can be filled again with new animations.
     * @see append()
     */
    void reset()
    {
      while (_head)
      {
        Animation *toDelete = _head;
        _head = _head->next;
        toDelete->next = nullptr;
        delete toDelete;
      }
    }

  private:
    /// @see AnimationRepo::add()
    /// Deprecated - only temporary for compatibility.
    bool add(Animation *animation) override
    {
#if (0)
      append(animation);
#else
      Animation *tail = findTail();
      if (tail)
      {
        tail->next = animation;
      }
      else
      {
        _head = animation;
      }
#endif
      return true;
    }

    /// @see AnimationRepo::add()
    /// Deprecated - only temporary for compatibility.
    bool add(Animation &animation) override
    {
      return false;
    }

    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      Animation *next = _head;
      while (next)
      {
        next->process(currentMillis, wasModified);
        next = next->next;
      }
    }

    Animation *findTail()
    {
      Animation *tail = nullptr;
      Animation *next = _head;
      while (next)
      {
        tail = next;
        next = next->next;
      }
      return tail;
    }

  private:
    Animation *_head = nullptr;
  };

} // namespace EC
