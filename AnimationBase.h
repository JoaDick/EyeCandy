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
#include "FastLedStrip.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Universal base class for most Animations.
   * This base class provides support for...
   * - custom Patterns that update the entire LED strip at a fixed rate: showPattern()
   * - the oftentimes used default Patterns fading background and black background.
   * - Overlays, which are rendered on top of a Pattern: showOverlay()
   * - Mathematical models, which are periodically calculated in the background, independently
   *   from the Pattern's update rate - but without manipulating the LED strip: updateModel()
   *
   * Patterns render their content on the LED strip at a fixed frequency, and can be used either
   * standalone or as first Animation in an AnimationScene. As Pattern in an In an AnimationScene,
   * they also serve as trigger for the following Oberlays to render their content on the LED
   * strip. \n
   * Another typical (but not mandatory) property of a Pattern is that it manipulates all LEDs of
   * the strip, and not only selected ones like most Overlays do.
   */
  class AnimationBase
      : public Animation
  {
  public:
    /** Fading speed.
     * Lower value = longer glowing; 0 = solid black background.
     * This value is usually configured by the Animation implementation.
     * Change it only if the child class explicitly uses this as configuration option,
     * i.e. usually when it offers a static \c fadeRate_default() method. \n
     * Not relevant in Overlay mode.
     */
    uint8_t fadeRate;

    /** Set the pause (in ms) between updates of the mathematical model.
     * This value is usually configured by the Animation implementation.
     * Change it only if the child class explicitly uses this as configuration option,
     * i.e. when it also offers a static \c modelUpdatePeriod_default() method. \n
     * It determines how frequent updateModel() is getting called; 0 (the default) means that
     * updateModel() isn't called.
     */
    void setModelUpdatePeriod(uint16_t period) { _modelUpdateTimer.updatePeriod = period; }

    /** Get the pause (in ms) between updates of the mathematical model.
     * @see setModelUpdatePeriod()
     */
    uint16_t getModelUpdatePeriod() { return _modelUpdateTimer.updatePeriod; }

    /** Get the pause (in ms) between updates of the LED strip's Pattern.
     * It reflects how frequent showPattern() is getting called. \n
     * 0 means Overlay mode; Pattern updates are \e not triggered. \n
     * @note By default, EC_DEFAULT_UPDATE_PERIOD is used - unless the Animation implementation
     * configures a differnt value.
     */
    uint16_t getPatternUpdatePeriod() { return _patternUpdateTimer.updatePeriod; }

    /// Only for debugging.
    FastLedStrip getStrip() { return strip; }

  protected:
    /** Constructor for Animations that can be used as both, Pattern or Overlay.
     * @param strip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     *                  Only relevant when the default implementation of showPattern() is used.
     */
    AnimationBase(FastLedStrip strip,
                  bool overlayMode,
                  uint8_t fadeRate = 0)
        : fadeRate(fadeRate), _patternUpdateTimer(overlayMode ? 0 : EC_DEFAULT_UPDATE_PERIOD), strip(strip)
    {
    }

    /** Constructor for pure Patterns (without Overlay option).
     * @param strip  The LED strip.
     */
    explicit AnimationBase(FastLedStrip strip)
        : fadeRate(0), _patternUpdateTimer(EC_DEFAULT_UPDATE_PERIOD), strip(strip)
    {
    }

    /** Constructor for pure Patterns with custom update rate.
     * Use this only if you have an urgent reason for a different update rate
     * than EC_DEFAULT_UPDATE_PERIOD.
     * @param patternUpdatePeriod  Period (in ms) for calling showPattern().
     * @param strip  The LED strip.
     */
    AnimationBase(uint8_t patternUpdatePeriod,
                  FastLedStrip strip)
        : fadeRate(0), _patternUpdateTimer(patternUpdatePeriod), strip(strip)
    {
    }

    /// The LED strip for child classes to draw their Animation.
    FastLedStrip strip;

    /** Render the Animation's Pattern content.
     * This method must only be implemented by child classes that provide a custom Pattern
     * functionality. The default implementation provides the oftentimes used fading background
     * or black background (depending on the setting of fadeRate).
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method is \e not called in Overlay mode.
     */
    virtual void showPattern(uint32_t currentMillis)
    {
      strip.showDefaultPattern(fadeRate);
    }

    /** Render the Animation's Overlay content.
     * This method must only be implemented by child classes that provide an Overlay functionality.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method is \e only called in Overlay mode.
     */
    virtual void showOverlay(uint32_t currentMillis)
    {
    }

    /** Update the Animation's mathematical model.
     * This method can optionally be implemented by all child classes that have some internal
     * state, which shall be updated periodically in the background (independently from the
     * Pattern's update rate).
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method must not manipulate the LED strip!
     * @see setModelUpdatePeriod()
     */
    virtual void updateModel(uint32_t currentMillis)
    {
    }

  protected:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (_modelUpdateTimer.process(currentMillis))
      {
        updateModel(currentMillis);
      }

      if (_patternUpdateTimer.process(currentMillis))
      {
        wasModified = true;
        showPattern(currentMillis);
      }

      if (wasModified)
      {
        showOverlay(currentMillis);
      }
    }

  private:
    AnimationTimer _patternUpdateTimer;
    AnimationTimer _modelUpdateTimer;
  };

} // namespace EC
