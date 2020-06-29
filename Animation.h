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
   * All child classes must implement the processAnimation() method.
   */
  class Animation
  {
  public:
    /// Type of Animation.
    enum Type
    {
      /// Invalid.
      TYPE_NONE = 0,

      /** This Pattern renders the content of \e all LEDs.
       * Previous LED content is completely overwritten.
       */
      TYPE_SOLID_PATTERN = 0x31,

      /** This Pattern fades out the existing LED content.
       * Previous LED content is only partially overwritten.
       */
      TYPE_FADING_PATTERN = 0x32,

      /// This Overlay can be used on top of any Pattern type.
      TYPE_OVERLAY = 0x10,

      /// This Overlay is preferably used on top of a #TYPE_SOLID_PATTERN.
      TYPE_OVERLAY_SOLID = 0x11,

      /// This Overlay is preferably used on top of a #TYPE_FADING_PATTERN.
      TYPE_OVERLAY_FADING = 0x12
    };

    /// Type of Animation.
    const Type animationType;

    /// Destructor.
    virtual ~Animation() = default;

    /** Process the Animation.
     * Must be called frequently by the main loop.
     * @param wasModified  True when the LED strip was already modified, e.g.
     *                     by a previous Animation. Internally used as trigger
     *                     to render the Overlay content (if supported).
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip must be updated (or \a wasModified).
     */
    bool process(bool wasModified = false)
    {
      return processAnimation(millis(), wasModified);
    }

    /** Process the Animation (with external timing source).
     * Must be called frequently by the main loop.
     * Use this method this when you're having multiple Animations, where all of
     * them shall be processed based on the same external timer source.
     * @param currentMillis  Returnvalue of millis() for synchronized timing.
     * @param wasModified  True when the LED strip was already modified, e.g.
     *                     by a previous Animation. Internally used as trigger
     *                     to render the Overlay content (if supported).
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip must be updated (or \a wasModified).
     */
    bool process(uint32_t currentMillis, bool wasModified = false)
    {
      return processAnimation(currentMillis, wasModified);
    }

  protected:
    /** Constructor.
     * @param animationType  Type of Animation.
     */
    explicit Animation(Type animationType)
        : animationType(animationType)
    {
    }

  private:
    /** Process the Animation.
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @param wasModified  True when the LED strip was already modified, e.g.
     *                     by a previous Animation. Internally used as trigger
     *                     to render the Overlay content (if supported).
     * @retval false  No changes to the LED strip.
     * @retval true   LED strip must be updated (or \a wasModified).
     */
    virtual bool processAnimation(uint32_t currentMillis, bool wasModified) = 0;
  };

} // namespace EC
