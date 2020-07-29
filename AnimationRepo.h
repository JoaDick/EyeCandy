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

//------------------------------------------------------------------------------

namespace EC
{

  /** Interface of a repository that can store multiple Animations.
   * A set of multiple Animations that are processed sequentially is called
   * "Animation Scene".
   */
  class AnimationRepo
  {
  public:
    /** Add \a animation to the Animation Scene.
     * If too many Animations are added, the surplus ones are ignored (and false
     * is returned).
     */
    virtual bool add(Animation &animation) = 0;

    /** Add \a animation to the Animation Scene.
     * If too many Animations are added, the surplus ones are deleted immediately
     * (and false is returned).
     * @note \a animation must be allocated on the heap. The repository takes
     * care of deleting it when necessary.
     */
    virtual bool add(Animation *animation) = 0;

    /// Remove all previously added Animations.
    virtual void reset() = 0;

  protected:
    AnimationRepo() = default;
  };

  /** Pointer to a function that configures an Animation Scene.
   * @param repo  Store Animations there.
   */
  using AnimationBuilderFct = void (*)(AnimationRepo &repo);

} // namespace EC
