/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Sound.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>
#include <functional>

#include <cstring>
#include <cstdio>

using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(SOUND, "yarp.sig.Sound")
}

#define HELPER(x) (*((FlexImage*)(x)))

Sound::Sound(size_t bytesPerSample)
{
    init(bytesPerSample);
    m_frequency = 0;
}

Sound::Sound(const Sound& alt) : yarp::os::Portable()
{
    init(alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    m_frequency = alt.m_frequency;
    synchronize();
}

Sound& Sound::operator += (const Sound& alt)
{
    if (alt.m_channels!= m_channels)
    {
        yCError(SOUND, "unable to concatenate sounds with different number of channels!");
        return *this;
    }
    if (alt.m_frequency!= m_frequency)
    {
        yCError(SOUND, "unable to concatenate sounds with different sample rate!");
        return *this;
    }

    Sound orig= *this;
    this->resize(this->m_samples+alt.m_samples,m_channels);

    unsigned char* p1    = orig.getRawData();
    unsigned char* p2    = alt.getRawData();
    unsigned char* pout  = this->getRawData();

    for (size_t ch=0; ch<m_channels; ch++)
    {
        size_t out1 = ch* this->getBytesPerSample() * this->m_samples;
        size_t out2 = ch* this->getBytesPerSample() * this->m_samples + this->getBytesPerSample() * orig.m_samples;

        size_t ori1 = ch * orig.getBytesPerSample() * orig.m_samples;
        size_t s1   = orig.getBytesPerSample() * orig.m_samples;

        size_t alt1 = ch * orig.getBytesPerSample() * alt.m_samples;
        unsigned int s2 = alt.getBytesPerSample() * alt.m_samples;

        memcpy((void *) &pout[out1], (void *) (p1+ori1), s1);
        memcpy((void *) &pout[out2], (void *) (p2+alt1), s2);
    }

    this->synchronize();
    return *this;
}

const Sound& Sound::operator = (const Sound& alt)
{
    yCAssert(SOUND, getBytesPerSample()==alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    m_frequency = alt.m_frequency;
    synchronize();
    return *this;
}

void Sound::synchronize()
{
    FlexImage& img = HELPER(implementation);
    m_samples = img.width();
    m_channels = img.height();
}

Sound Sound::subSound(size_t first_sample, size_t last_sample)
{
    if (last_sample  > this->m_samples)
        last_sample = m_samples;
    if (first_sample > this->m_samples)
        first_sample = m_samples;
    if (last_sample < first_sample)
        last_sample = first_sample;

    Sound s;

    s.resize(last_sample-first_sample, this->m_channels);
    s.setFrequency(this->m_frequency);

    /*
    //faster implementation but currently not working
    unsigned char* p1    = this->getRawData();
    unsigned char* p2    = s.getRawData();
    int j=0;
    for (int i=first_sample; i<last_sample*2; i++)
    {
        p2[j++]=p1[i];
    }
    */

    //safe implementation
    size_t j=0;
    for (size_t i=first_sample; i<last_sample; i++)
    {
        for (size_t c=0; c< this->m_channels; c++)
            s.set(this->get(i,c),j,c);
        j++;
    }

    s.synchronize();

    return s;
}

void Sound::init(size_t bytesPerSample)
{
    implementation = new FlexImage();
    yCAssert(SOUND, implementation!=nullptr);

    yCAssert(SOUND, bytesPerSample==2); // that's all that's implemented right now
    HELPER(implementation).setPixelCode(VOCAB_PIXEL_MONO16);
    HELPER(implementation).setQuantum(2);

    m_samples = 0;
    m_channels = 0;
    this->m_bytesPerSample = bytesPerSample;
}

Sound::~Sound()
{
    if (implementation!=nullptr)
    {
        delete &HELPER(implementation);
        implementation = nullptr;
    }
}

void Sound::resize(size_t samples, size_t m_channels)
{
    FlexImage& img = HELPER(implementation);
    img.resize(samples,m_channels);
    synchronize();
}

Sound::audio_sample Sound::get(size_t location, size_t channel) const
{
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (m_bytesPerSample ==2)
    {
        return *(reinterpret_cast<NetUint16*>(addr));
    }
    else
    {
        yCError(SOUND, "sound only implemented for 16 bit samples");
    }
    return 0;
}

void Sound::clear()
{
    size_t size = this->getRawDataSize();
    unsigned char* p  = this->getRawData();
    memset(p,0,size);
}

bool Sound::clearChannel(size_t chan)
{
    if (chan > this->m_channels) return false;
    for (size_t i = 0; i < this->m_samples; i++)
    {
        set(0, i, chan);
    }
    return true;
}

void Sound::set(audio_sample value, size_t location, size_t channel)
{
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (m_bytesPerSample ==2)
    {
        *(reinterpret_cast<NetUint16*>(addr)) = value;
        return;
    }
    else
    {
        yCError(SOUND, "sound only implemented for 16 bit samples");
    }
}

int Sound::getFrequency() const
{
    return m_frequency;
}

void Sound::setFrequency(int freq)
{
    this->m_frequency = freq;
}

bool Sound::read(ConnectionReader& connection)
{
    // lousy format - fix soon!
    FlexImage& img = HELPER(implementation);
    Bottle bot;
    bool ok = PortablePair<FlexImage,Bottle>::readPair(connection,img,bot);
    m_frequency = bot.get(0).asInt32();
    synchronize();
    return ok;
}


bool Sound::write(ConnectionWriter& connection) const
{
    // lousy format - fix soon!
    FlexImage& img = HELPER(implementation);
    Bottle bot;
    bot.addInt32(m_frequency);
    return PortablePair<FlexImage,Bottle>::writePair(connection,img,bot);
}

unsigned char *Sound::getRawData() const
{
    FlexImage& img = HELPER(implementation);
    return img.getRawImage();
}

size_t Sound::getRawDataSize() const
{
    FlexImage& img = HELPER(implementation);
    return img.getRawImageSize();
}

void Sound::setSafe(audio_sample value, size_t sample, size_t channel)
{
    if (isSample(sample, channel))
    {
        set(value, sample, channel);
    }
    else
    {
        yCError(SOUND) << "Sample out of bound:" << sample << "," << channel;
    }
}

Sound Sound::extractChannelAsSound(size_t channel_id) const
{
    Sound news(this->m_bytesPerSample);
    news.setFrequency(this->m_frequency);
    news.resize(this->m_samples, 1);

    unsigned char* p_src = this->getRawData();
    unsigned char* p_dst = news.getRawData();
    size_t j = 0;
    //pointer to the first element of the row of the matrix we need to copy
    size_t first_sample = 0 + (this->m_samples * this->m_bytesPerSample)*channel_id;
    //pointer to the last element of the row of the matrix we need to copy
    size_t last_sample = first_sample + (this->m_samples * this->m_bytesPerSample);
    for (auto i = first_sample; i < last_sample; i++)
    {
        p_dst[j++] = p_src[i];
    }
    return news;
}

bool Sound::operator==(const Sound& alt) const
{
    if (this->m_channels != alt.getChannels()) return false;
    if (this->m_bytesPerSample != alt.getBytesPerSample()) return false;
    if (this->m_frequency != alt.getFrequency()) return false;
    if (this->m_samples != alt.getSamples()) return false;

    for (size_t ch = 0; ch < this->m_channels; ch++)
    {
        for (size_t s = 0; s < this->m_samples; s++)
        {
            if (this->getSafe(s, ch) != alt.getSafe(s, ch))
            {
                return false;
            }
        }
    }

    return true;
}

bool Sound::replaceChannel(size_t id, Sound schannel)
{
    if (schannel.getChannels() != 1) return false;
    if (this->m_samples != schannel.getSamples()) return false;
    for (size_t s = 0; s < this->m_samples; s++)
    {
        this->setSafe(schannel.getSafe(s, 0), s, id);
    }
    return true;
}

std::vector<std::reference_wrapper<Sound::audio_sample>> Sound::getChannel(size_t channel_id)
{
    FlexImage& img = HELPER(implementation);

    std::vector<std::reference_wrapper<audio_sample>> vec;
    vec.reserve(this->m_samples);
    for (size_t t = 0; t < this->m_samples; t++)
    {
        unsigned char *addr = img.getPixelAddress(t, channel_id);
        audio_sample*  addr2 = reinterpret_cast<audio_sample*>(addr);
        vec.push_back(std::ref(*addr2));
    }
    return vec;
}

std::vector<std::reference_wrapper<Sound::audio_sample>> Sound::getInterleavedAudioRawData() const
{
    FlexImage& img = HELPER(implementation);

    std::vector<std::reference_wrapper<audio_sample>> vec;
    vec.reserve(this->m_samples*this->m_channels);
    for (size_t t = 0; t < this->m_samples; t++)
    {
        for (size_t c = 0; c < this->m_channels; c++)
        {
            unsigned char *addr  = img.getPixelAddress(t, c);
            audio_sample*  addr2 = reinterpret_cast<audio_sample*>(addr);
            vec.push_back(std::ref(*addr2));
        }
    }
    return vec;
}

std::vector<std::reference_wrapper<Sound::audio_sample>> Sound::getNonInterleavedAudioRawData() const
{
    FlexImage& img = HELPER(implementation);

    std::vector<std::reference_wrapper<audio_sample>> vec;
    vec.reserve(this->m_samples*this->m_channels);
    for (size_t c = 0; c < this->m_channels; c++)
    {
        for (size_t t = 0; t < this->m_samples; t++)
        {
            unsigned char *addr = img.getPixelAddress(t, c);
            audio_sample*  addr2 = reinterpret_cast<audio_sample*>(addr);
            vec.push_back(std::ref(*addr2));
        }
    }
    return vec;
}

std::string Sound::toString() const
{
    std::string s;
    for (size_t c = 0; c < this->m_channels; c++)
    {
        for (size_t t = 0; t < this->m_samples; t++)
        {
            s += " ";
            s += std::to_string(this->get(t, c));
        }
        s += '\n';
    }
    return s;
}

bool Sound::isSample(size_t sample, size_t channel) const
{
    return (sample<this->m_samples && channel<this->m_channels);
}

size_t Sound::getBytesPerSample() const
{
    return this->m_bytesPerSample;
}

size_t Sound::getSamples() const
{
    return this->m_samples;
}

size_t Sound::getChannels() const
{
    return this->m_channels;
}

double Sound::getDuration() const
{
    return (double)(this->m_samples)*(double)(1 / this->m_frequency);
}

void Sound::normalizeChannel(size_t channel)
{
    size_t maxsampleid = 0;
    audio_sample maxsamplevalue = 0;
    findPeakInChannel(channel, maxsampleid, maxsamplevalue);
    double gain = 1 / (maxsamplevalue / 32767.0);
    amplifyChannel(channel,gain);
}

void Sound::normalize()
{
    size_t maxsampleid = 0;
    size_t maxchannelid = 0;
    audio_sample maxsamplevalue = 0;
    findPeak(maxchannelid, maxsampleid, maxsamplevalue);
    double gain = 1 / (maxsamplevalue/32767.0);
    amplify(gain);
}

void Sound::amplifyChannel(size_t channel, double gain)
{
    unsigned char* pc = this->getRawData();
    audio_sample* p = reinterpret_cast<audio_sample*>(pc);
    p+= this->m_samples * channel;

    for (size_t t = 0; t < this->m_samples; t++, p++)
    {
        double amplified_value = (*p) * gain;
        *p = (int)(amplified_value); //should i limit this range
    }
}

void Sound::amplify(double gain)
{
    for (size_t c = 0; c < this->m_channels; c++)
    {
        amplifyChannel(c,gain);
    }
}

void Sound::findPeakInChannel(size_t channelId, size_t& sampleId, audio_sample& sampleValue) const
{
    sampleId = 0;
    sampleValue = 0;
    unsigned char* pc = this->getRawData();
    audio_sample* p = reinterpret_cast<audio_sample*>(pc);
    p += this->m_samples * channelId;

    for (size_t t = 0; t < this->m_samples; t++, p++)
    {
        if (*p > sampleValue)
        {
            sampleValue = (*p);
            sampleId= t;
        }
    }
}

void Sound::findPeak(size_t& channelId, size_t& sampleId, audio_sample& sampleValue) const
{
    for (size_t c = 0; c < this->m_channels; c++)
    {
        size_t maxsampleid_inchannel=0;
        audio_sample maxsamplevalue_inchannel=0;
        findPeakInChannel(c, maxsampleid_inchannel, maxsamplevalue_inchannel);
        if (maxsamplevalue_inchannel > sampleValue)
        {
            sampleValue = maxsamplevalue_inchannel;
            sampleId = maxsampleid_inchannel;
            channelId = c;
        }
    }
}
