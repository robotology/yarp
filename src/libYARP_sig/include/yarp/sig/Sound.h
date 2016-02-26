/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SOUND
#define YARP2_SOUND

#include <yarp/os/Portable.h>

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
    Sound subSound(int first_sample, int last_sample);

    void resize(int samples, int channels = 1);

    int get(int sample, int channel = 0) const;
    void set(int value, int sample, int channel = 0);

    int getSafe(int sample, int channel = 0) {
        if (isSample(sample,channel)) {
            return get(sample,channel);
        }
        return 0;
    }

    void setSafe(int value, int sample, int channel = 0) {
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
    inline bool isSample(int sample, int channel=0) const {
        return (sample>=0 && channel>=0 && sample<samples && channel<channels);
    }

    /**
    * set all the samples to zero (silence)
    */
    void clear();

    int getFrequency() const;

    void setFrequency(int freq);

    int getBytesPerSample() const { return bytesPerSample; }

    int getSamples() const { return samples; }

    int getChannels() const { return channels; }

    virtual bool read(yarp::os::ConnectionReader& connection);

    virtual bool write(yarp::os::ConnectionWriter& connection);

    unsigned char *getRawData() const;

    int getRawDataSize() const;

private:
    void init(int bytesPerSample);
    void synchronize();

    void *implementation;
    int samples;
    int channels;
    int bytesPerSample;
    int frequency;
};

#endif
