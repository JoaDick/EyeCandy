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
    // Only for use by friend classes.
    Animation *next = nullptr;
    friend class AnimationScene;
  };

} // namespace EC
