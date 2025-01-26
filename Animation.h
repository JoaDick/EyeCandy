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

#include <Arduino.h>

//------------------------------------------------------------------------------

#ifndef EC_DEFAULT_FRAMETIME
/** Default delay between updates of the LED strip (in ms).
 * This default value of 10ms will result in a LED strip refresh rate of 100Hz.
 */
// TODO: change to 20ms / 50Hz
#define EC_DEFAULT_FRAMETIME 10
#endif

#define EC_NEEDS_REWORK 1

//------------------------------------------------------------------------------

namespace EC
{

  /** Interface for all EyeCandy Animations.
   * Call one of the process() methods frequently from your sketch's loop()
   * function.
   * All child classes must implement the processAnimation() method.
   */
  class Animation
  {
  public:
    Animation(const Animation &) = delete;
    Animation &operator=(const Animation &) = delete;

    /// Destructor.
    virtual ~Animation() = default;

#if (EC_NEEDS_REWORK)
    /** Process the Animation (with external timing source).
     * Must be called frequently by the main loop.
     * Use this method this when you're having multiple Animations, where all of
     * them shall be processed based on the same external timer source.
     * @param currentMillis  Returnvalue of millis() for synchronized timing.
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip was updated.
     */
    bool processOLD(uint32_t currentMillis)
    {
      bool wasModified = false;
      processAnimationOLD(currentMillis, wasModified);
      return wasModified;
    }
#endif

    /** Process the Animation (and update the LEDs).
     * Must be called by the main loop when it's time to render the Animation on the LED strip.
     * @param currentMillis  Current timestamp; usually the returnvalue of millis().
     * @return Frametime, a.k.a. delay before calling this method again (in ms).
     *         Returning 0 means default frametime shall be used.
     * @see EC_DEFAULT_FRAMETIME
     */
    uint8_t process(uint32_t currentMillis)
    {
      return processAnimation(currentMillis);
    }

  protected:
    Animation() = default;

#if (EC_NEEDS_REWORK)
    /** Process the Animation.
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * \a wasModified is true when the LED strip was already modified,
     * e.g. by a previous Animation. Internally used as trigger to render the
     * Overlay content (if supported).
     * Its value shall be set to true when the Animation changed the strip's content.
     */
    virtual void processAnimationOLD(uint32_t currentMillis, bool &wasModified)
    {
      if (wasModified)
      {
        processAnimation(currentMillis);
      }
    }
#endif

    /** Process the Animation (and update the LEDs).
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @return Frametime, a.k.a. delay before calling this method again (in ms).
     *         Returning 0 means default frametime shall be used.
     * @see EC_DEFAULT_FRAMETIME
     */
    virtual uint8_t processAnimation(uint32_t currentMillis) = 0;

  private:
    friend class AnimationScene;
    friend class AnimationSceneStatic;
    Animation *nextAnimation = nullptr;
  };

  /// Small helper for updating a given Animation when its time has come.
  class AnimationUpdateHandler
  {
  public:
    /** Constructor.
     * @param animation  The Animation to take care of.
     */
    explicit AnimationUpdateHandler(Animation &animation)
        : _animation(animation)
    {
    }

    /** Process the Animation.
     * To be called frequently by every main loop cycle.
     * Calls the Animation's process() method only when it is time for an update.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip was updated.
     */
    bool process(uint32_t currentMillis)
    {
      if (currentMillis < _nextMillis)
      {
        return false;
      }

      const auto frametime = _animation.process(currentMillis);
      _nextMillis = currentMillis + (frametime ? frametime : EC_DEFAULT_FRAMETIME);
      return true;
    }

  private:
    Animation &_animation;
    uint32_t _nextMillis = 0;
  };

  //------------------------------------------------------------------------------

  /** Helper class for composing a complex Animation out of multiple separate Animations.
   * When the AnimationScene's process() method is called, it calls the process()
   * methods of all appended Animations in the order as they were appended. \n
   * Thus the first one should usually be a Pattern (which renders the entire LED strip),
   * and all subsequently appended should be Overlays (which only render certain LEDs).
   * Pseudo-Animations, which don't manipulate the LED strip (but perform other
   * processing), can be appended at any position.
   * @see append()
   * @note All appended Animations \e must be allocated on the heap. This class will
   * destroy them accordingly.
   */
  class AnimationScene
      : public Animation
  {
  public:
    /// Destructor. Also destroys all previously added Animations.
    ~AnimationScene()
    {
      reset();
    }

    /** Append the given (allocated) \a animation to the AnimationScene (given as pointer).
     * @return The given \a animation (so the caller can apply further settings)
     * @note \a animation must be allocated on the heap. The AnimationScene takes
     * care of deleting it when no more needed.
     */
    template <class AnimationType>
    AnimationType *append(AnimationType *animation)
    {
      storeAnimation(animation);
      return animation;
    }

    /** Append the given (static) \a animation to the AnimationScene (given as reference).
     * @return Pointer to the given \a animation (so the caller can apply further settings)
     * @note \a animation must have static lifetime (i.e. it is \e not allocated on the heap).
     * It will not be deleted when no more needed.
     */
    template <class AnimationType>
    AnimationType *append(AnimationType &animation)
    {
      storeAnimation(new Proxy(animation));
      return &animation;
    }

    /** Remove (and delete) all previously added Animations.
     * This AnimationScene is empty afterwards, and can be filled again with new animations.
     * @see append()
     */
    void reset()
    {
      while (_animationListHead)
      {
        Animation *toDelete = _animationListHead;
        _animationListHead = _animationListHead->nextAnimation;
        delete toDelete;
      }
    }

    class Proxy : public Animation
    {
      Animation &_staticAnimation;
      uint8_t processAnimation(uint32_t currentMillis) override
      {
        return _staticAnimation.processAnimation(currentMillis);
      }

    public:
      explicit Proxy(Animation &staticAnimation) : _staticAnimation(staticAnimation) {}
    };

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      uint8_t retval = 0;

      Animation *animation = _animationListHead;
      while (animation)
      {
        const auto frametime = animation->processAnimation(currentMillis);
        animation = animation->nextAnimation;
        if (retval == 0)
        {
          retval = frametime;
        }
      }

      return retval;
    }

    void storeAnimation(Animation *animation)
    {
      Animation **tailPtr = &_animationListHead;
      while (*tailPtr)
      {
        tailPtr = &(*tailPtr)->nextAnimation;
      }
      *tailPtr = animation;
    }

  private:
    Animation *_animationListHead = nullptr;
  };

  //------------------------------------------------------------------------------

  /// Same as AnimationScene, but the Animations must \e not be allocated on the heap.
  class AnimationSceneStatic
      : public Animation
  {
  public:
    /// Destructor. Also removes all previously added Animations.
    ~AnimationSceneStatic()
    {
      reset();
    }

    /** Append the given \a animation to the AnimationScene.
     * @return Pointer to the given \a animation (so the caller can apply further settings)
     */
    template <class AnimationType>
    AnimationType *append(AnimationType &animation)
    {
      storeAnimation(&animation);
      return &animation;
    }

    /** Remove all previously added Animations.
     * This AnimationScene is empty afterwards, and can be filled again with new animations.
     * @see append()
     */
    void reset()
    {
      while (_animationListHead)
      {
        Animation *toClear = _animationListHead;
        _animationListHead = _animationListHead->nextAnimation;
        toClear->nextAnimation = nullptr;
      }
    }

  private:
    /// @see Animation::processAnimation()
    uint8_t processAnimation(uint32_t currentMillis) override
    {
      uint8_t retval = 0;

      Animation *animation = _animationListHead;
      while (animation)
      {
        const auto frametime = animation->processAnimation(currentMillis);
        animation = animation->nextAnimation;
        if (retval == 0)
        {
          retval = frametime;
        }
      }

      return retval;
    }

    void storeAnimation(Animation *animation)
    {
      Animation **tailPtr = &_animationListHead;
      while (*tailPtr)
      {
        tailPtr = &(*tailPtr)->nextAnimation;
      }
      *tailPtr = animation;
    }

  private:
    Animation *_animationListHead = nullptr;
  };

  //------------------------------------------------------------------------------

  /// Helper class for triggering periodic actions.
  class AnimationTimer
  {
  public:
    /** Pause (in ms) between process() returning \c true
     * 0 means suspended, i.e. process() will always return \c false
     */
    uint16_t updatePeriod;

    /** Constructor.
     * @param updatePeriod  Pause (in ms) between process() returning \c true \n
     *                      0 means suspended, i.e. process() will always return \c false
     */
    explicit AnimationTimer(uint16_t updatePeriod = 0)
        : updatePeriod(updatePeriod)
    {
    }

    /** Call this method periodically.
     * @retval \c true Timer triggered; caller shall execute its corresponding action.
     * @retval \c false Nothing to do.
     */
    bool process(uint32_t currentMillis)
    {
      if (updatePeriod)
      {
        if (currentMillis >= _nextUpdate)
        {
          _nextUpdate = currentMillis + updatePeriod;
          return true;
        }
      }
      return false;
    }

  private:
    uint32_t _nextUpdate = 0;
  };

} // namespace EC
