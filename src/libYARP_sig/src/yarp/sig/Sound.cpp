/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Sound.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>
#include <functional>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

#include <cstring>
#include <cstdio>
#include <vector>

using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(SOUND, "yarp.sig.Sound")
}

#ifndef NetUint8
#define NetUint8 unsigned char
#endif

#ifndef NetUint16
#define NetUint16 short unsigned int
#endif

YARP_BEGIN_PACK
class SoundHeader
{
public:
    yarp::os::NetInt32 outerListTag{ 0 };
    yarp::os::NetInt32 outerListLen{ 0 };
    yarp::os::NetInt32 samplesSizeTag{ 0 };
    yarp::os::NetInt32 samplesSize{ 0 };
    yarp::os::NetInt32 channelsSizeTag{ 0 };
    yarp::os::NetInt32 channelsSize{ 0 };
    yarp::os::NetInt32 bytesPerSampleTag{ 0 };
    yarp::os::NetInt32 bytesPerSample{ 0 };
    yarp::os::NetInt32 frequencyTag{ 0 };
    yarp::os::NetInt32 frequency{ 0 };
    yarp::os::NetInt32 listTag{ 0 };
    yarp::os::NetInt32 listLen{ 0 };

    SoundHeader() = default;
};
YARP_END_PACK

Sound::Sound(size_t bytesPerSample)
{
    init(bytesPerSample);
    m_frequency = 0;

}

Sound::Sound(const Sound& alt) : yarp::os::Portable()
{
    init(alt.m_bytesPerSample);
    m_frequency = alt.m_frequency;
    m_channels = alt.m_channels;
    m_samples = alt.m_samples;
    if (m_bytesPerSample == 1)
    {
        *(std::vector<NetUint8>*)(implementation) = *(std::vector<NetUint8>*)(alt.implementation);
    }
    if (m_bytesPerSample == 2)
    {
        *(std::vector<NetUint16>*)(implementation) = *(std::vector<NetUint16>*)(alt.implementation);
    }
    else
    {
        yCError(SOUND, "sound only implemented for 8-16 bit samples");
        yCAssert(SOUND,false); // that's all that's implemented right now
    }
}

void Sound::overwrite(const Sound& alt, size_t offset, size_t len)
{
    if (alt.m_channels != m_channels)
    {
        yCError(SOUND, "unable to overwrite sounds with different number of channels!");
        return;
    }
    if (alt.m_frequency != m_frequency)
    {
        yCError(SOUND, "unable to overwrite sounds with different sample rate!");
        return;
    }

    size_t current_size = (*this).getSamples();
    size_t alt_size     = alt.getSamples();
    if (len == 0) len = alt_size;
    if ( offset + len > current_size)
    {
        len = current_size - offset;
    }

    for (size_t ch = 0; ch < m_channels; ch++)
    {
        size_t pdst = ch * this->getBytesPerSample() * this->m_samples + offset* this->getBytesPerSample();
        unsigned char* dst = &this->getRawData()[pdst];
        size_t psrc = ch * this->getBytesPerSample() * alt.m_samples;
        unsigned char* src = &alt.getRawData()[psrc];
        memcpy((void*) dst, (void*) src, len * this->m_bytesPerSample);
    }
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

    unsigned char* orig_start  = orig.getRawData();
    unsigned char* alt_start   = alt.getRawData();
    unsigned char* pout  = this->getRawData();
    size_t orig_singlechannel_size = orig.getBytesPerSample() * orig.m_samples;
    size_t  alt_singlechannel_size =  alt.getBytesPerSample() * alt.m_samples;

    for (size_t ch=0; ch<m_channels; ch++)
    {
        size_t out1 = ch* this->getBytesPerSample() * this->m_samples;
        size_t out2 = ch* this->getBytesPerSample() * this->m_samples + this->getBytesPerSample() * orig.m_samples;

        size_t orig_pointer = ch * orig.getBytesPerSample() * orig.m_samples;
        size_t alt_pointer = ch * orig.getBytesPerSample() * alt.m_samples;

        memcpy((void *) &pout[out1], (void *) (orig_start + orig_pointer), orig_singlechannel_size);
        memcpy((void *) &pout[out2], (void *) (alt_start  + alt_pointer),  alt_singlechannel_size);
    }

    return *this;
}

const Sound& Sound::operator = (const Sound& alt)
{
    init(alt.m_bytesPerSample);
    m_frequency = alt.m_frequency;
    m_channels = alt.m_channels;
    m_samples = alt.m_samples;

    if (m_bytesPerSample == 1)
    {
        *(std::vector<NetUint8>*)(implementation) = *(std::vector<NetUint8>*)(alt.implementation);
        return *this;
    }
    else if (m_bytesPerSample == 2)
    {
        *(std::vector<NetUint16>*)(implementation) = *(std::vector<NetUint16>*)(alt.implementation);
        return *this;
    }

    yCError(SOUND, "sound only implemented for 8-16 bit samples");
    yCAssert(SOUND, false); // that's all that's implemented right now
    return *this;
}

Sound Sound::subSound(size_t first_sample, size_t len)
{
    Sound s;

    // Nothing to copy
    if (len == 0 || first_sample >= m_samples) {
        s.setFrequency(m_frequency);
        s.resize(0, m_channels);
        return s;
    }

    size_t last_sample = std::min(first_sample + len, m_samples);
    size_t out_len = last_sample - first_sample;

    s.resize(out_len, this->m_channels);
    s.setFrequency(this->m_frequency);

    //safe implementation
    size_t j=0;
    for (size_t i=first_sample; i<last_sample; i++)
    {
        for (size_t c = 0; c < this->m_channels; c++) {
            s.set(this->get(i, c), j, c);
        }
        j++;
    }
    return s;
}

void Sound::init(size_t bytesPerSample)
{
    delete_implementation();
    if (bytesPerSample == 1)
    {
        implementation = new std::vector<NetUint8>;
    }
    else if (bytesPerSample==2)
    {
        implementation = new std::vector<NetUint16>;
    }
    else
    {
        yCError(SOUND, "sound only implemented for 8-16 bit samples");
        yCAssert(SOUND, false); // that's all that's implemented right now
    }

    yCAssert(SOUND, implementation!=nullptr);
    m_samples = 0;
    m_channels = 1;
    this->m_bytesPerSample = bytesPerSample;
}

void Sound::delete_implementation()
{
    if (implementation != nullptr)
    {
        if (m_bytesPerSample == 1)
        {
            std::vector<NetUint8>*p = (std::vector<NetUint8>*)(implementation);
            delete p;
            implementation = nullptr;
        }
        else if (m_bytesPerSample == 2)
        {
            std::vector<NetUint16>* p = (std::vector<NetUint16>*)(implementation);
            delete p;
            implementation = nullptr;
        }
        else
        {
            yCError(SOUND, "sound only implemented for 8-16 bit samples");
            yCAssert(SOUND, false); // that's all that's implemented right now
        }
    }
}

Sound::~Sound()
{
    delete_implementation();
}

void Sound::resize(size_t samples, size_t channels)
{
    delete_implementation();
    if (m_bytesPerSample == 1)
    {
        implementation = new std::vector<NetUint8>;
        ((std::vector<NetUint8>*)(implementation))->resize(samples * channels);
        m_channels = channels;
        m_samples = samples;
    }
    else if (m_bytesPerSample == 2)
    {
        implementation = new std::vector<NetUint16>;
        ((std::vector<NetUint16>*)(implementation))->resize(samples*channels);
        m_channels=channels;
        m_samples=samples;
    }
    else
    {
        yCError(SOUND, "sound only implemented for 8-16 bit samples");
        yCAssert(SOUND, false); // that's all that's implemented right now
    }
}

Sound::audio_sample Sound::get(size_t location, size_t channel) const
{
    if (m_bytesPerSample == 1)
    {
        auto* pp = ((std::vector<NetUint8>*)(implementation));
        NetUint8* addr = pp->data() + location + channel * this->m_samples;
        return *(reinterpret_cast<NetUint8*>(addr));
    }
    else if (m_bytesPerSample == 2)
    {
        auto* pp = ((std::vector<NetUint16>*)(implementation));
        NetUint16* addr = pp->data() + location + channel * this->m_samples;
        return *(reinterpret_cast<NetUint16*>(addr));
    }
    else
    {
        yCError(SOUND, "sound only implemented for 8-16 bit samples");
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
    if (chan > this->m_channels) {
        return false;
    }
    for (size_t i = 0; i < this->m_samples; i++)
    {
        set(0, i, chan);
    }
    return true;
}

void Sound::set(audio_sample value, size_t location, size_t channel)
{
    if (m_bytesPerSample == 1)
    {
        auto* pp = ((std::vector<NetUint8>*)(implementation));
        NetUint8* addr = pp->data() + location  +channel * this->m_samples;
        *(reinterpret_cast<NetUint8*>(addr)) = value;
        return;
    }
    else if (m_bytesPerSample == 2)
    {
        auto* pp = ((std::vector<NetUint16>*)(implementation));
        NetUint16* addr = pp->data() + location + channel * this->m_samples;
        *(reinterpret_cast<NetUint16*>(addr)) = value;
        return;
    }
    else
    {
        yCError(SOUND, "sound only implemented for 8-16 bit samples");
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
    connection.convertTextMode();
    SoundHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) {
        return false;
    }

    init(header.bytesPerSample);
    m_frequency = header.frequency;
    m_channels = header.channelsSize;
    m_samples = header.samplesSize;
    size_t data_size = header.listLen;

    if (data_size != m_channels * m_samples)
    {
        return false;
    }

    auto* pp = ((std::vector<NetUint16>*)(implementation));
    pp->resize(data_size);
    for (size_t l = 0; l < data_size; l++)
    {
        pp->at(l) = connection.expectInt16();
    }
    return true;
}


bool Sound::write(yarp::os::ConnectionWriter& connection) const
{
    SoundHeader header;
    header.outerListTag = BOTTLE_TAG_LIST;
    header.outerListLen = 5;
    header.samplesSizeTag = BOTTLE_TAG_INT32;
    header.samplesSize = m_samples;
    header.bytesPerSampleTag = BOTTLE_TAG_INT32;
    header.bytesPerSample = m_bytesPerSample;
    header.channelsSizeTag = BOTTLE_TAG_INT32;
    header.channelsSize = m_channels;
    header.frequencyTag = BOTTLE_TAG_INT32;
    header.frequency = m_frequency;

    auto* pp = ((std::vector<NetUint16>*)(implementation));
    size_t siz = pp->size();
    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT16;
    header.listLen = siz; //this must be equal to m_samples*m_channels
    //BOTTLE_TAG_LIST is special and must be followed by its size.
    connection.appendBlock((char*)&header, sizeof(header));
    for (size_t l = 0; l < pp->size(); l++) {
        connection.appendInt16(pp->at(l));
    }

    connection.convertTextMode();
    return true;
}

unsigned char *Sound::getRawData() const
{
    if (m_bytesPerSample == 1)
    {
        auto* pp = ((std::vector<NetUint8>*)(implementation));
        NetUint8* addr = pp->data();
        return addr;
    }
    else if (m_bytesPerSample == 2)
    {
        auto* pp = ((std::vector<NetUint16>*)(implementation));
        NetUint16* addr = pp->data();
        return (unsigned char*)(addr);
    }

    yCError(SOUND, "sound only implemented for 8-16 bit samples");
    yCAssert(SOUND, false); // that's all that's implemented right now
}

size_t Sound::getRawDataSize() const
{
    return this->m_bytesPerSample*this->m_channels*this->m_samples;
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
    if (this->m_channels != alt.getChannels()) {
        return false;
    }
    if (this->m_bytesPerSample != alt.getBytesPerSample()) {
        return false;
    }
    if (this->m_frequency != alt.getFrequency()) {
        return false;
    }
    if (this->m_samples != alt.getSamples()) {
        return false;
    }

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
    if (schannel.getChannels() != 1) {
        return false;
    }
    if (this->m_samples != schannel.getSamples()) {
        return false;
    }
    for (size_t s = 0; s < this->m_samples; s++)
    {
        this->setSafe(schannel.getSafe(s, 0), s, id);
    }
    return true;
}

std::vector<std::reference_wrapper<Sound::audio_sample>> Sound::getChannel(size_t channel_id)
{
    auto* pp = ((std::vector<NetUint16>*)(implementation))->data() + channel_id*m_samples;

    std::vector<std::reference_wrapper<audio_sample>> vec;
    vec.reserve(this->m_samples);
    for (size_t t = 0; t < this->m_samples; t++)
    {
        unsigned char *addr = (unsigned char* )(pp);
        audio_sample*  addr2 = reinterpret_cast<audio_sample*>(addr);
        vec.push_back(std::ref(*addr2));
    }
    return vec;
}

std::vector<std::reference_wrapper<Sound::audio_sample>> Sound::getInterleavedAudioRawData() const
{
    auto* pp = ((std::vector<NetUint16>*)(implementation))->data();

    std::vector<std::reference_wrapper<audio_sample>> vec;
    vec.reserve(this->m_samples*this->m_channels);
    for (size_t t = 0; t < this->m_samples; t++)
    {
        for (size_t c = 0; c < this->m_channels; c++)
        {
            audio_sample*  addr  = (audio_sample*)(pp) + t + c*m_samples;
            vec.push_back(std::ref(*addr));
        }
    }
    return vec;
}

std::vector<std::reference_wrapper<Sound::audio_sample>> Sound::getNonInterleavedAudioRawData() const
{
    auto* pp = ((std::vector<NetUint16>*)(implementation))->data();

    std::vector<std::reference_wrapper<audio_sample>> vec;
    vec.reserve(this->m_samples*this->m_channels);
    for (size_t c = 0; c < this->m_channels; c++)
    {
        for (size_t t = 0; t < this->m_samples; t++)
        {
            audio_sample* addr = (audio_sample*)(pp)+ t + c * m_samples;
            vec.push_back(std::ref(*addr));
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
    return (double)(this->m_samples)*(double)(1.0 / this->m_frequency);
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
