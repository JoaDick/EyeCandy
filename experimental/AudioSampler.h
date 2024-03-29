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

#include "AudioNormalizer.h"

//------------------------------------------------------------------------------

namespace EC
{

  /** Interface for classes that provide audio samples.
   * The audio samples are normalized, i.e. scaled to the range between -1.0 ... +1.0 (lowest /
   * highest possible value). Too high or too low samples are clipped to that range.
   */
  class AudioSampler
  {
  public:
    /** Get the last sampled audio value.
     * @return Audio sample in the range between -1.0 ... +1.0
     */
    float getAudioSample()
    {
      return _sample;
    }

  protected:
    AudioSampler() = default;
    AudioSampler(const AudioSampler &) = default;
    AudioSampler &operator=(const AudioSampler &) = default;
    ~AudioSampler() = default;

    /// For child classes to store the audio sample value.
    void storeAudioSample(float sample)
    {
      _sample = constrain(sample, 0.0, 1.0);
    }

  private:
    float _sample = 0.0;
  };

  //------------------------------------------------------------------------------

  /** Helper class for normalizing raw ADC values from an analog pin.
   * Normalizing means eliminating the DC offset, and scaling each sample to the
   * defined range between -1.0 ... +1.0 (lowest / highest possible value).
   */
  class AnalogPinAudioSampler
      : public AudioSampler
  {
  public:
    /** Constructor
     * @param analogPin  Analog input pin to read from.
     * @param maxRawValue  Maximum possible value from the ADC.
     *                     Example: Arduino's 10 bit ADC --> 1023
     * @param avgLength  Number of samples to incorporate for DC offset calculation.
     */
    explicit AnalogPinAudioSampler(uint8_t analogPin,
                                   uint16_t maxRawValue = 1023,
                                   uint16_t avgLength = 5000)
        : _analogPin(analogPin), _normalizer(maxRawValue, avgLength)
    {
    }

    /// Read next value from ADC and normalize that sample.
    void process()
    {
      storeAudioSample(_normalizer.process(analogRead(_analogPin)));
    }

  private:
    const uint8_t _analogPin;
    AdcSampleNormalizer _normalizer;
  };

}
