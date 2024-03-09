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

  //------------------------------------------------------------------------------

  /// Generic base class for simple Patterns.
  class PatternBase
      : public Animation
  {
  public:
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
    /** Constructor.
     * @param ledStrip  The LED strip.
     * @param patternUpdatePeriod  Period (in ms) for calling showPattern().
     *                             Change this only if you have an urgent reason for a different
     *                             update rate than EC_DEFAULT_UPDATE_PERIOD.
     */
    explicit PatternBase(FastLedStrip ledStrip,
                         uint8_t patternUpdatePeriod = EC_DEFAULT_UPDATE_PERIOD)
        : _patternUpdateTimer(patternUpdatePeriod), strip(ledStrip)
    {
    }

    /// The LED strip for child classes to draw their Animation.
    FastLedStrip strip;

    /** Render the Animation's Pattern content.
     * This method must be implemented by all child classes.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     */
    virtual void showPattern(uint32_t currentMillis) = 0;

    /** Adjust the Pattern's update rate.
     * @param period  Period (in ms) for calling showPattern().
     * @note Think twice if you really have an urgent reason for a different update rate than
     * EC_DEFAULT_UPDATE_PERIOD. \n
     * Consider using AnimationModelBase instead for manipulating the Animation's speed.
     */
    void setPatternUpdatePeriod(uint8_t period) { _patternUpdateTimer.updatePeriod = period; }

  protected:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (_patternUpdateTimer.process(currentMillis))
      {
        wasModified = true;
        showPattern(currentMillis);
      }
    }

  private:
    AnimationTimer _patternUpdateTimer;
  };

  //------------------------------------------------------------------------------

  /** Universal base class for most Animations.
   * This base class provides support for...
   * - custom Patterns that update the entire LED strip at a fixed rate: showPattern()
   * - the oftentimes used default Patterns fading background and black background.
   * - Overlays, which are rendered on top of a Pattern: showOverlay()
   *
   * Patterns render their content on the LED strip at a fixed frequency, and can be used either
   * standalone or as first Animation in an AnimationScene. As Pattern in an In an AnimationScene,
   * they also serve as trigger for the following Oberlays to render their content on the LED
   * strip. \n
   * Another typical (but not mandatory) property of a Pattern is that it manipulates all LEDs of
   * the strip, and not only selected ones like most Overlays do.
   */
  class AnimationBase
      : public PatternBase
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

  protected:
    /** Constructor for Animations that can be used as both, Pattern or Overlay.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     *                  Only relevant when the default implementation of showPattern() is used.
     */
    explicit AnimationBase(FastLedStrip ledStrip,
                           bool overlayMode = false,
                           uint8_t fadeRate = 0)
        : PatternBase(ledStrip, overlayMode ? 0 : EC_DEFAULT_UPDATE_PERIOD),
          fadeRate(fadeRate)
    {
    }

    /** Render the Animation's Pattern content.
     * This method must only be implemented by child classes that provide a custom Pattern
     * functionality. The default implementation provides the oftentimes used fading background
     * or black background (depending on the setting of fadeRate).
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method is \e not called in Overlay mode.
     */
    virtual void showPattern(uint32_t currentMillis)
    {
      showDefaultPattern(strip, fadeRate);
    }

    /** Render the Animation's Overlay content.
     * This method must only be implemented by child classes that provide an Overlay functionality.
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method is \e only called in Overlay mode.
     */
    virtual void showOverlay(uint32_t currentMillis)
    {
    }

  protected:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      PatternBase::processAnimation(currentMillis, wasModified);
      if (wasModified)
      {
        showOverlay(currentMillis);
      }
    }
  };

  //------------------------------------------------------------------------------

  /** Universal base class for Animations with custom background activities.
   * This base class provides support for all features from AnimationBase.
   * Additionally, a background activity can e.g. be a mathematical model, which is periodically
   * calculated in the background, independently from the Pattern's update rate - but without
   * manipulating the LED strip.
   * @see updateModel()
   */
  class AnimationModelBase
      : public AnimationBase
  {
  public:
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

  protected:
    /** Constructor.
     * @param modelUpdatePeriod  Period (in ms) for calling updateModel().
     *                           0 means that updateModel() isn't called.
     * @param ledStrip  The LED strip.
     * @param overlayMode  Set to \c true when the Animation shall be an Overlay.
     * @param fadeRate  Fading speed: Lower value = longer glowing; 0 = black background.
     *                  Only relevant when the default implementation of showPattern() is used.
     */
    explicit AnimationModelBase(uint16_t modelUpdatePeriod,
                                FastLedStrip ledStrip,
                                bool overlayMode = false,
                                uint8_t fadeRate = 0)
        : AnimationBase(ledStrip, overlayMode, fadeRate),
          _modelUpdateTimer(modelUpdatePeriod)
    {
    }

    /** Update the Animation's mathematical model.
     * This method must be implemented by all child classes to update their internal state
     * periodically in the background (independently from the Pattern's update rate).
     * @param currentMillis  Current time, i.e. the returnvalue of millis().
     * @note This method must not manipulate the LED strip!
     * @see setModelUpdatePeriod()
     */
    virtual void updateModel(uint32_t currentMillis) = 0;

  protected:
    /// @see Animation::processAnimation()
    void processAnimation(uint32_t currentMillis, bool &wasModified) override
    {
      if (_modelUpdateTimer.process(currentMillis))
      {
        updateModel(currentMillis);
      }
      AnimationBase::processAnimation(currentMillis, wasModified);
    }

  private:
    AnimationTimer _modelUpdateTimer;
  };

} // namespace EC
