/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/SoundFilters.h>

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::soundfilters;

namespace
{
    YARP_LOG_COMPONENT(SOUNDFILTERS, "yarp.sig.SoundFilters")
}

#if defined (YARP_HAS_SOXR)
#include <soxr.h>
#endif

//#define MULTICHANS_TESTED

//#######################################################################################################

bool yarp::sig::soundfilters::resample(yarp::sig::Sound& snd, size_t frequency)
{
    if (frequency == 0)
    {
        yCError(SOUNDFILTERS) << "invalid frequency value = 0";
        return false;
    }
#ifndef MULTICHANS_TESTED
    if (snd.getChannels() != 1)
    {
        yCError(SOUNDFILTERS) << "only 1 channel is currently supported";
        return false;
    }
#endif
    if (snd.getSamples() == 0)
    {
        yCError(SOUNDFILTERS) << "empty sound received?!";
        return false;
    }
    if (static_cast<unsigned>(snd.getFrequency()) == frequency)
    {
        yCWarning(SOUNDFILTERS) << "no resampling needed";
        return true;
    }

#if !defined (YARP_HAS_SOXR)
    yCError(SOUNDFILTERS) << "libsoxr not available";
    return false;
#else
    //configuration
    soxr_io_spec_t tit;
    tit.itype = SOXR_INT16_I;
    tit.otype = SOXR_INT16_I;
    tit.scale = 1.0;

    double irate = snd.getFrequency();
    double orate = double(frequency);
    size_t ismp = snd.getSamples();
#ifdef MULTICHANS_TESTED
    size_t ichans = snd.getChannels();
#else
    size_t ichans = 1;
#endif
    size_t ilen = ismp*ichans;
    yarp::sig::Sound::audio_sample* arri = new yarp::sig::Sound::audio_sample[ilen];

    //copy from sound to buffer
    for (size_t t = 0; t < ismp; t++)
    {
        for (size_t c = 0; c < ichans; c++)
        {
            arri[t] = snd.getSafe(t, c);
        }
    }

    size_t osmp = (size_t)(ilen * orate / irate + .5);
    size_t ochans = ichans;
    size_t olen = osmp*ochans;
    yarp::sig::Sound::audio_sample* arro = new yarp::sig::Sound::audio_sample[olen];

    //resample
    size_t odone;
    soxr_error_t error = soxr_oneshot(irate, orate, ichans,            // Rates and # of channels
        arri, ilen, NULL,           // Input
        arro, olen, &odone,         // Output
        &tit, NULL, NULL);          // Configuration
    if (error != 0)
    {
        yCError(SOUNDFILTERS) << "libsoxr resample failed";
        delete[]arri;
        delete[]arro;
    }

    //copy from buffer to sound
    snd.setFrequency(size_t(orate));
    snd.resize(olen);
    for (size_t t = 0; t < osmp; t++)
    {
        for (size_t c = 0; c < ochans; c++)
        {
            snd.setSafe(arro[t], t, c);
        }
    }

    //clear buffer
    delete[]arri;
    delete[]arro;

    return true;
#endif
}
