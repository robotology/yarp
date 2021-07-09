/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUND_H
#define YARP_SIG_SOUND_H

#include <yarp/os/Portable.h>
#include <yarp/conf/numeric.h>
#include <yarp/sig/api.h>
#include <vector>
#include <string>

namespace yarp {
namespace sig {

/**
 * \ingroup sig_class
 *
 * Class for storing sounds
 * See \ref AudioDoc for additional documentation on YARP audio.
*/
class YARP_sig_API Sound : public yarp::os::Portable
{
public:
    typedef short int audio_sample;

    Sound(size_t bytesPerSample = 2);

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
     * Comparison operator.
     * Compares two sounds
     * @return true if the two sounds are identical
     */
    bool operator==(const Sound& alt) const;

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

    /**
     * Set the sound size.
     * @param samples the number of samples
     * @param channels the number of channels
     */
    void resize(size_t samples, size_t channels = 1);

    audio_sample get(size_t sample, size_t channel = 0) const;

    void set(audio_sample value, size_t sample, size_t channel = 0);

    audio_sample getSafe(size_t sample, size_t channel = 0) const {
        if (isSample(sample,channel)) {
            return get(sample,channel);
        }
        return 0;
    }

    void setSafe(audio_sample value, size_t sample, size_t channel = 0);

    /**
     * Check whether a sample lies within the sound
     * @param sample the sample to choose
     * @param channel the channel to choose
     * @return true iff there is a sample at the given coordinate
     */
    bool isSample(size_t sample, size_t channel = 0) const;

    /**
     * set all the samples to zero (silence)
     */
    void clear();

    /**
     * Normalize a specific channel of the sound
     * @param channel the channel to normalize
     */
    void normalizeChannel(size_t channel);

    /**
     * Normalize a sound (the peak is searched among all channels)
     */
    void normalize();

    /**
     * Amplify a specific channel of the sound
     * @param[in] gain the gain (1.0 = no amplification. Minimum value: 0)
     * @param[in] channel the sample id
     */
    void amplifyChannel(size_t channel, double gain);

    /**
     * amplify a sound
     * @param[in] gain the gain (1.0 = no amplification. Minimum value: 0)
     */
    void amplify(double gain);

    /**
     * find the peak in a specific channel of the sound
     * @param[in] channelId the channel in which the peak is searched
     * @param[out] sampleId the returned sampleId in which the peak is detected
     * @param[out] sampleValue the returned value of the sample in which the peak is detected
     */
    void findPeakInChannel(size_t channelId, size_t& sampleId, audio_sample& sampleValue) const;

    /**
     * find the peak in a sound
     * @param[out] channelId the returned channelId in which the peak is detected
     * @param[out] sampleId the returned sampleId in which the peak is detected
     * @param[out] sampleValue the returned value of the sample in which the peak is detected
     */
    void findPeak(size_t& channelId, size_t& sampleId, audio_sample& sampleValue) const;

    /**
     * Utility function: return the timestamp (in seconds) given the sample id
     * @param sampleid the sample id
     * @return the timestamp of the sample
     */
    inline double sample2timestamp( size_t sampleid) const {return static_cast<double>(sampleid / m_frequency);}

    /**
     * Utility function: return the sample id given the timestamp (in seconds)
     * @param time the timestamp
     * @return the sample id
     */
    inline size_t timestamp2sample( double time) const { return static_cast<size_t>(time * m_frequency);}

    /**
     * set to zero all the samples of the specified channel
     * @ param channel the channel number
     * @return true iff operation is successful;
     */
    bool clearChannel(size_t channel);

    /**
     * Get the frequency of the sound (i.e. the number of samples per second)
     * @return the frequency of the sound
     */
    int getFrequency() const;

    /**
     * Set the frequency of the sound (i.e. the number of samples per second)
     */
    void setFrequency(int freq);

    /**
     * Get the number of bytes per sample
     * @return the number of bytes per sample
     */
    size_t getBytesPerSample() const;

    /**
     * Get the number of samples contained in the sound
     * @return the number of samples
     */
    size_t getSamples() const;

    /**
     * Get the duration of sound in seconds
     * @return the duration of the sound
     */
    double getDuration() const;

    /**
     * Get the number of channels of the sound
     * @return the number of channels
     */
    size_t getChannels() const;

    /**
     * Extract a single channel from the sound
     * @return a new sound constituted by the selected channel
     */
    Sound extractChannelAsSound(size_t channel_id) const;

    std::vector<std::reference_wrapper<audio_sample>> getChannel(size_t channel_id);

    /**
     * Replace a single channel of our current sound with a given sound constituted by a single channel
     * The two sounds must have the same number of samples
     * @param id the channel of our sound that we want to replace
     * @param channel the single-channel sound that we want to put in our sound
     */
    bool replaceChannel(size_t id, Sound channel);

    /**
     * Returns a serialized version of the sound, in interleaved format,
     * e.g. for a sound composed by 3 channels, x samples:
     * 1 11 21, 2 12 22, 3 13 23, 4 14 24 etc
     * @param vec the vector representing the serialized sound
     */
    std::vector<std::reference_wrapper<audio_sample>> getInterleavedAudioRawData() const;

    /**
     * Returns a serialized version of the sound, in non-interleaved format,
     * e.g. for a sound composed by 3 channels, x samples:
     * 1 2 3 4 5.....etc, 11 12 13 14 15.....etc, 21 22 23 24 25.....etc
     * @param vec the vector representing the serialized sound
     */
    std::vector<std::reference_wrapper<audio_sample>> getNonInterleavedAudioRawData() const;

    /**
     * Print matrix to a string. Useful for debugging.
     * The output string is represented in non-interleaved format
     *
     */
    std::string toString() const;

private:
    /**
     * This method returns a pointer to the internal data storage, allowing read/write operations.
     * It's dangerous to play with memory: the internal representation (interleaved/non-interleaved) may change in the future.
     * @return pointer to the internal storage
     */
    unsigned char *getRawData() const;

    /**
     * Returns the size of the internal storage (in bytes, not samples!)
     * @return size of the internal storage
     */
    size_t getRawDataSize() const;

public:
    bool read(yarp::os::ConnectionReader& connection) override;

    bool write(yarp::os::ConnectionWriter& connection) const override;

private:
    void init(size_t bytesPerSample);
    void synchronize();

    void *implementation;
    size_t m_samples;
    size_t m_channels;
    size_t m_bytesPerSample;
    int    m_frequency;
};

} // namespace sig
} // namespace yarp

#endif // YARP_SIG_SOUND_H
