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

#ifndef EC_DEFAULT_UPDATE_PERIOD
/** Default period (in ms) for updating the LED strip's Pattern.
 * This default value of 10ms will result in a LED strip refresh rate of 100Hz.
 */
#define EC_DEFAULT_UPDATE_PERIOD 10
#endif

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

    /** Process the Animation.
     * Must be called frequently by the main loop.
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip was updated.
     */
    bool process()
    {
      bool wasModified = false;
      processAnimation(millis(), wasModified);
      return wasModified;
    }

    /** Process the Animation (with external timing source).
     * Must be called frequently by the main loop.
     * Use this method this when you're having multiple Animations, where all of
     * them shall be processed based on the same external timer source.
     * @param currentMillis  Returnvalue of millis() for synchronized timing.
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip was updated.
     */
    bool process(uint32_t currentMillis)
    {
      bool wasModified = false;
      processAnimation(currentMillis, wasModified);
      return wasModified;
    }

    /** Process the Animation.
     * Must be called frequently by the main loop.
     * @param wasModified  Shall be true when the LED strip was already modified,
     * e.g. by a previous Animation. Internally used as trigger to render the
     * Overlay content (if supported).
     * Its value is set to true when the Animation changed the strip's content.
     */
    void process(bool &wasModified)
    {
      processAnimation(millis(), wasModified);
    }

    /** Process the Animation (with external timing source).
     * Must be called frequently by the main loop.
     * Use this method this when you're having multiple Animations, where all of
     * them shall be processed based on the same external timer source.
     * @param currentMillis  Returnvalue of millis() for synchronized timing.
     * @param wasModified  Shall be true when the LED strip was already modified,
     * e.g. by a previous Animation. Internally used as trigger to render the
     * Overlay content (if supported).
     * Its value is set to true when the Animation changed the strip's content.
     */
    void process(uint32_t currentMillis, bool &wasModified)
    {
      processAnimation(currentMillis, wasModified);
    }

  protected:
    Animation() = default;

    /** Process the Animation.
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * \a wasModified is true when the LED strip was already modified,
     * e.g. by a previous Animation. Internally used as trigger to render the
     * Overlay content (if supported).
     * Its value shall be set to true when the Animation changed the strip's content.
     */
    virtual void processAnimation(uint32_t currentMillis, bool &wasModified) = 0;

  private:
    friend class AnimationScene;
    friend class AnimationSceneStatic;
    Animation *nextAnimation = nullptr;
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
      void processAnimation(uint32_t currentMillis, bool &wasModified) override
      {
        _staticAnimation.process(currentMillis, wasModified);
      }

    public:
      explicit Proxy(Animation &staticAnimation) : _staticAnimation(staticAnimation) {}
    };

  private:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      Animation *animation = _animationListHead;
      while (animation)
      {
        animation->process(currentMillis, wasModified);
        animation = animation->nextAnimation;
      }
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

  /** Pointer to a function that composes an AnimationScene.
   * @param scene  Append Animations there.
   */
  using AnimationSceneBuilderFct = void (*)(AnimationScene &scene);

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
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      Animation *animation = _animationListHead;
      while (animation)
      {
        animation->process(currentMillis, wasModified);
        animation = animation->nextAnimation;
      }
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
    uint8_t updatePeriod;

    /** Constructor.
     * @param updatePeriod  Pause (in ms) between process() returning \c true \n
     *                      0 means suspended, i.e. process() will always return \c false
     */
    explicit AnimationTimer(uint8_t updatePeriod = 0)
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
