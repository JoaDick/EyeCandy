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

#include "AnimationRepo.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Helper class for cycling through different Animation Scenes.
   */
  class AnimationChanger
  {
    AnimationRepo &_repo;
    AnimationBuilderFct *_allAnimationBuilders;
    uint8_t _index = -1;

  public:
    /** Constructor.
     * @param repo Store the Animation Scene there.
     * @param allAnimations Array with all functions that set up an Animation Scene.
     *                      Last entry must be NULL.
     */
    AnimationChanger(AnimationRepo &repo,
                     AnimationBuilderFct allAnimations[])
        : _repo(repo), _allAnimationBuilders(allAnimations)
    {
      selectNext();
    }

    /** Select the next Animation Scene.
     * @retval true The first Animation Scene was selected.
     */
    bool selectNext()
    {
      bool retval = 0;
      AnimationBuilderFct animationBuilder = _allAnimationBuilders[++_index];
      if (animationBuilder == nullptr)
      {
        _index = 0;
        animationBuilder = _allAnimationBuilders[0];
        retval = true;
      }
      _repo.reset();
      animationBuilder(_repo);
    }
  };

} // namespace EC
