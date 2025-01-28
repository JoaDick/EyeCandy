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
#include "Backgrounds.h"
#include "FastLedStrip.h"
#include "VuSource.h"

//------------------------------------------------------------------------------

namespace EC
{

  //------------------------------------------------------------------------------

  /** Environment for setting up Animation Scenes.
   */
  class SetupEnv
  {
  public:
    /** Pointer to callback function for creating a VuSource for the AnimationScene.
     * @param env  Environment for setting up the AnimationScene (i.e. *this).
     */
    using VuSourceMakerFct = VuSource &(*)(SetupEnv &env);

    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param scene  Operate on that AnimationScene.
     * @param vuSourceMakerFct  Callback function for creating the AnimationScene's VuSource.
     */
    SetupEnv(FastLedStrip ledStrip,
             AnimationScene &scene,
             VuSourceMakerFct vuSourceMakerFct = nullptr)
        : _strip(ledStrip), _scene(scene), _makeVuSource(vuSourceMakerFct)
    {
    }

    /// Get the AnimationScene to set up.
    AnimationScene &scene() { return _scene; }

    /// Get the LED strip to be used by the AnimationScene.
    const FastLedStrip &strip() { return _strip; }

    /** Add the given (allocated) \a animation to the AnimationScene (given as pointer).
     * This is a convenience function for \c scene().append(animation)
     * @return Reference to the given \a animation (so the caller can apply further settings)
     * @note \a animation must be allocated on the heap.
     * The AnimationScene takes care of deleting it when no more needed.
     */
    template <class AnimationType>
    AnimationType &add(AnimationType *animation)
    {
      return *_scene.append(animation);
    }

    /** Add the given (static) \a animation to the AnimationScene (given as reference).
     * This is a convenience function for \c scene().append(animation)
     * @return The given \a animation (so the caller can apply further settings)
     * @note \a animation must have static lifetime (i.e. it is \e not allocated on the heap).
     * It will not be deleted when no more needed.
     */
    template <class AnimationType>
    AnimationType &add(AnimationType &animation)
    {
      return *_scene.append(animation);
    }

    /** Add a VuSource to the AnimationScene.
     * @return The VuSource
     * @note This method must be called only once per VU AnimationScene setup.
     */
    VuSource &addVuSource()
    {
      return _makeVuSource ? _makeVuSource(*this) : VuSource::getNull();
    }

    /** Add a VuSource and (fading) background to the AnimationScene.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     * @return The VuSource
     * @note This method must be called once and as first of the AnimationScene setup.
     */
    VuSource &addVuBackground(uint8_t fadeRate)
    {
      _scene.append(new BgFadeToBlack(_strip, fadeRate));
      return addVuSource();
    }

    /// Prepare for setting up a new AnimationScene.
    void reset()
    {
      _scene.reset();
      _strip.clear();
    }

    /** Clone this SetupEnv but with a reversed LED strip.
     * @see FastLedStrip::getReversedStrip()
     */
    SetupEnv clone_reversedStrip() const
    {
      SetupEnv retval(*this);
      retval._strip = _strip.getReversedStrip();
      return retval;
    }

    /** Clone this SetupEnv but with a LED strip that has only half of the original strip's size.
     * @param reversed  Draw the new strip's content in reverse direction.
     * @see FastLedStrip::getHalfStrip()
     */
    SetupEnv clone_halfStrip(bool reversed = false) const
    {
      SetupEnv retval(*this);
      retval._strip = _strip.getHalfStrip(reversed);
      return retval;
    }

    /** Clone this SetupEnv but with a sub-strip of the original LED strip.
     * @param offset  New strip starts at this LED.
     * @param newSize  Number of LEDs in the new strip.
     *                 0 means all from \a offset up to the end of the strip.
     * @param reversed  Draw the new strip's content in reverse direction.
     * @see FastLedStrip::getSubStrip()
     */
    SetupEnv clone_subStrip(int16_t offset, int16_t newSize, bool reversed = false) const
    {
      SetupEnv retval(*this);
      retval._strip = _strip.getSubStrip(offset, newSize, reversed);
      return retval;
    }

#ifdef EC_SETUP_ENV_USER_DATA_TYPE
    using UserDataType = EC_SETUP_ENV_USER_DATA_TYPE;
    UserDataType *userData = nullptr;
#endif

  private:
    FastLedStrip _strip;
    AnimationScene &_scene;
    VuSourceMakerFct _makeVuSource;
  };

  //------------------------------------------------------------------------------

  /** Pointer to a function that composes an AnimationScene.
   * @param env  Environment for setting up the AnimationScene.
   */
  using AnimationSceneMakerFct = void (*)(SetupEnv &env);

  //------------------------------------------------------------------------------

} // namespace EC

/** Convenience template for Patterns that have no other arguments beside the LED strip.
 * @see AnimationSceneMakerFct
 */
template <class AnimationType>
void make_Pattern(EC::SetupEnv &env)
{
  env.add(new AnimationType(env.strip()));
}

//------------------------------------------------------------------------------
