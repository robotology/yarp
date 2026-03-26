/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/SoundUtils.h>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool prepareSound(Sound& outSound,double duration_s, size_t channels, size_t sampleRate, double frequency, size_t amplitude)
{
    if (duration_s <= 0 || channels == 0 || sampleRate == 0 || frequency == 0 || amplitude == 0)
    {
        yError() << "yarp::sig::utils::makeTone: invalid parameters";
        return false;
    }

    // Sound parameters
    size_t totalSamples = sampleRate * duration_s;

    // Create the sound object
    outSound = yarp::sig::Sound();
    outSound.resize(totalSamples, channels);
    outSound.setFrequency(sampleRate);

    return true;
}

bool utils::makeSawTooth(Sound& outSound,double duration_s, size_t channels, size_t sampleRate, double frequency, size_t amplitude)
{
    if (!prepareSound(outSound, duration_s, channels, sampleRate, frequency, amplitude)) { return false; }

    size_t totalSamples = outSound.getSamples();

    for (size_t i = 0; i < totalSamples; i++)
    {
        double t = static_cast<double>(i) / sampleRate;
        double value = 2.0 * (t * frequency - floor(t * frequency + 0.5));
        short sample = static_cast<short>(amplitude * value);
        for (size_t ch = 0; ch < channels; ch++)
        {
            outSound.set(sample, i, ch);
        }
    }

    return true;
}

bool utils::makeSquareWave(Sound& outSound,double duration_s, size_t channels, size_t sampleRate, double frequency, size_t amplitude)
{
    if (!prepareSound(outSound, duration_s, channels, sampleRate, frequency, amplitude)) { return false; }

    double twoPiF = 2.0 * M_PI * frequency;

    for (size_t i = 0; i < outSound.getSamples(); i++)
    {
        double t = static_cast<double>(i) / sampleRate;

        double s = sin(twoPiF * t);
        short sample = (s >= 0) ? amplitude : -amplitude;

        for (size_t ch = 0; ch < channels; ch++)
        {
            outSound.set(sample, i, ch);
        }
    }

    return true;
}

bool utils::makeTone(Sound& outSound,double duration_s, size_t channels, size_t sampleRate, double frequency, size_t amplitude)
{
    if (!prepareSound(outSound, duration_s, channels, sampleRate, frequency, amplitude)) { return false; }

    // Fill the sound buffer with a sine wave
    double twoPiF = 2.0 * M_PI * frequency;
    for (size_t i = 0; i < outSound.getSamples(); i++)
    {
        double t = static_cast<double>(i) / sampleRate;
        short sample = static_cast<short>((double)(amplitude) * sin(twoPiF * t));
        for (size_t ch = 0; ch < channels; ch++)
        {
            outSound.set(sample, i, ch);
        }
    }

    return true;
}

Sound utils::mix(const Sound& A, const Sound& B, double percentage)
{
    Sound result;

    // Ensure percentage is clamped between 0 and 1
    if (percentage < 0.0) percentage = 0.0;
    if (percentage > 1.0) percentage = 1.0;

    // Check for matching sample rates and number of channels
    if (A.getFrequency() != B.getFrequency() || A.getChannels() != B.getChannels())
    {
        yError() << "Cannot mix sounds with different sample rates or channel counts.";
        return result;
    }

    // Determine the length of the resulting sound
    size_t maxSamples = std::max(A.getSamples(), B.getSamples());
    size_t channels = A.getChannels();
    double alpha = percentage;
    double beta = 1.0 - percentage;

    // Create the resulting sound
    result.resize(maxSamples, channels);
    result.setFrequency(A.getFrequency());

    // Mix samples
    for (size_t ch = 0; ch < channels; ++ch)
    {
        for (size_t i = 0; i < maxSamples; ++i)
        {
            double sampleA = (i < A.getSamples()) ? A.getSafe(i, ch) : 0.0;
            double sampleB = (i < B.getSamples()) ? B.getSafe(i, ch) : 0.0;
            yarp::sig::Sound::audio_sample sampleO = static_cast<yarp::sig::Sound::audio_sample>(alpha * sampleA + beta * sampleB);
            result.setSafe(sampleO, i, ch);
        }
    }

    return result;
}
