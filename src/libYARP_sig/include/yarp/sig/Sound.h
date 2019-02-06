/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_SOUND_H
#define YARP_SIG_SOUND_H

#include <yarp/os/Portable.h>
#include <yarp/conf/numeric.h>
#include <yarp/sig/api.h>

namespace yarp {
    namespace sig {
        class Sound;
    }
}

/**
 * \ingroup sig_class
 *
 * Class for storing sounds
 */
class YARP_sig_API yarp::sig::Sound : public yarp::os::Portable {
public:
    Sound(int bytesPerSample = 2);

    /**
     * Copy constructor.
     * Clones the content of another sound.
     * @param alt the image to sound
     */
    Sound(const Sound& alt);

    virtual ~Sound();

    /**
     * Assignment operator.
     * Clones the content of another sound.
     * @param alt the image to sound
     */
    const Sound& operator=(const Sound& alt);

     /**
     * Addition assignment operator.
     * Appends a sound to another sound (internal memory reallocation)
     * @param alt the sound to append
     */
    Sound& operator+=(const Sound& alt);

     /**
     * Returns a subpart of the sound
     * @param first_sample the starting sample number
     * @param last_sample the ending sample number
     */
    Sound subSound(size_t first_sample, size_t last_sample);

    void resize(size_t samples, size_t channels = 1);

    int get(size_t sample, size_t channel = 0) const;
    void set(int value, size_t sample, size_t channel = 0);

    int getSafe(size_t sample, size_t channel = 0) {
        if (isSample(sample,channel)) {
            return get(sample,channel);
        }
        return 0;
    }

    void setSafe(int value, size_t sample, size_t channel = 0) {
        if (isSample(sample,channel)) {
            set(value,sample,channel);
        }
    }

    /**
     * Check whether a sample lies within the sound
     * @param sample the sample to choose
     * @param channel the channel to choose
     * @return true iff there is a sample at the given coordinate
     */
    inline bool isSample(size_t sample, size_t channel=0) const {
        return (sample<samples && channel<channels);
    }

    /**
    * set all the samples to zero (silence)
    */
    void clear();

    size_t getFrequency() const;

    void setFrequency(size_t freq);

    size_t getBytesPerSample() const { return bytesPerSample; }

    size_t getSamples() const { return samples; }

    size_t getChannels() const { return channels; }

    bool read(yarp::os::ConnectionReader& connection) override;

    bool write(yarp::os::ConnectionWriter& connection) const override;

    unsigned char *getRawData() const;

    size_t getRawDataSize() const;

private:
    void init(size_t bytesPerSample);
    void synchronize();

    void *implementation;
    size_t samples;
    size_t channels;
    size_t bytesPerSample;
    size_t frequency;
};

#endif // YARP_SIG_SOUND_H
