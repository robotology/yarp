/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/SoundUtils.h>
#include <cstring>
#include <cmath>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;


bool utils::makeTone(Sound& outSound,double duration_s, size_t channels, size_t sampleRate)
{
    if (duration_s <= 0 || channels == 0 || sampleRate == 0)
    {
        yError() << "yarp::sig::utils::makeTone: invalid parameters";
        return false;
    }

    // Sound parameters
    size_t totalSamples = sampleRate * duration_s;

    // Generate sine wave parameters
    double frequency = 440.0;  // A4 standard tone
    double amplitude = 30000;  // max ~32767 for 16-bit
    double twoPiF = 2.0 * 3.14 * frequency;

    // Create the sound object
    outSound = yarp::sig::Sound();
    outSound.resize(totalSamples, channels);
    outSound.setFrequency(sampleRate);

    // Fill the sound buffer with a sine wave
    for (size_t i = 0; i < totalSamples; i++)
    {
        double t = static_cast<double>(i) / sampleRate;
        short sample = static_cast<short>(amplitude * sin(twoPiF * t));
        outSound.set(sample, i, 0);
    }
    return true;
}
