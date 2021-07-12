/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/SoundFileWav.h>

#include <yarp/conf/system.h>

#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/Sound.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <cstring>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

namespace
{
    YARP_LOG_COMPONENT(SOUNDFILE_WAV, "yarp.sig.SoundFileWav")
}

YARP_BEGIN_PACK
class PcmWavHeader {
public:
    NetInt32 wavHeader;
    NetInt32 wavLength;
    NetInt32 formatHeader1;
    NetInt32 formatHeader2;
    NetInt32 formatLength;

    struct {
        NetInt16 pcmFormatTag;
        NetInt16 pcmChannels;
        NetInt32 pcmSamplesPerSecond;
        NetInt32 pcmBytesPerSecond;
        NetInt16 pcmBlockAlign;
        NetInt16 pcmBitsPerSample;
    } pcm;
    ManagedBytes pcmExtraData;

    NetInt32     dummyHeader;
    NetInt32     dummyLength;
    ManagedBytes dummyData;

    NetInt32 dataHeader;
    NetInt32 dataLength;

    void setup_to_write(const Sound& sound, FILE *fp);
    bool parse_from_file(FILE *fp);
};
YARP_END_PACK

bool PcmWavHeader::parse_from_file(FILE *fp)
{
    yCTrace(SOUNDFILE_WAV, "bool PcmWavHeader::parse_from_file(FILE *fp)\n");

    size_t ret;

    ret = fread(&wavHeader, sizeof(wavHeader), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&wavLength, sizeof(wavLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&formatHeader1, sizeof(formatHeader1), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&formatHeader2, sizeof(formatHeader2), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&formatLength, sizeof(formatLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmFormatTag, sizeof(pcm.pcmFormatTag), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmChannels, sizeof(pcm.pcmChannels), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmSamplesPerSecond, sizeof(pcm.pcmSamplesPerSecond), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmBytesPerSecond, sizeof(pcm.pcmBytesPerSecond), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmBlockAlign, sizeof(pcm.pcmBlockAlign), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmBitsPerSample, sizeof(pcm.pcmBitsPerSample), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }
    if (pcm.pcmBitsPerSample != 16)
    {
        yCError(SOUNDFILE_WAV, "sorry, lousy .wav read code only does 16-bit ints\n");
        return false;
    }

    //extra bytes in pcm chuck
    int extra_size = formatLength - sizeof(pcm);
    if (extra_size != 0)
    {
        yCError(SOUNDFILE_WAV, "extra_size = %d\n", extra_size);
        pcmExtraData.allocate(extra_size);
        ret = fread(&pcmExtraData, extra_size, 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
    }

    //extra chunks
    ret = fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    while (dummyHeader != yarp::os::createVocab32('d', 'a', 't', 'a'))
    {
        ret = fread(&dummyLength, sizeof(dummyLength), 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
        dummyData.clear();
        dummyData.allocate(dummyLength);
        ret = fread(&dummyData, dummyLength, 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
        ret = fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
    }

    dataHeader = dummyHeader;
    ret = fread(&dataLength, sizeof(dataLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    return true;
}

void PcmWavHeader::setup_to_write(const Sound& src, FILE *fp)
{
    int bitsPerSample = 16;
    size_t channels = src.getChannels();
    size_t bytes = channels*src.getSamples()*2;
    int align = channels*((bitsPerSample+7)/8);

    wavHeader = yarp::os::createVocab32('R','I','F','F');
    wavLength = bytes + sizeof(PcmWavHeader) - 2*sizeof(NetInt32);
    formatHeader1 = yarp::os::createVocab32('W','A','V','E');
    formatHeader2 = yarp::os::createVocab32('f','m','t',' ');
    formatLength = sizeof(pcm);

    pcm.pcmFormatTag = 1; /* PCM! */
    pcm.pcmChannels = channels;
    pcm.pcmSamplesPerSecond = (int)src.getFrequency();
    pcm.pcmBytesPerSecond = align*pcm.pcmSamplesPerSecond;
    pcm.pcmBlockAlign = align;
    pcm.pcmBitsPerSample = bitsPerSample;

    dataHeader = yarp::os::createVocab32('d','a','t','a');
    dataLength = bytes;


    fwrite(&wavHeader,sizeof(wavHeader),1,fp);
    fwrite(&wavLength,sizeof(wavLength),1,fp);
    fwrite(&formatHeader1,sizeof(formatHeader1),1,fp);

    fwrite(&formatHeader2,sizeof(formatHeader2),1,fp);
    fwrite(&formatLength,sizeof(formatLength),1,fp);

    fwrite(&pcm.pcmFormatTag,sizeof(pcm.pcmFormatTag),1,fp);
    fwrite(&pcm.pcmChannels,sizeof(pcm.pcmChannels),1,fp);
    fwrite(&pcm.pcmSamplesPerSecond,sizeof(pcm.pcmSamplesPerSecond),1,fp);
    fwrite(&pcm.pcmBytesPerSecond,sizeof(pcm.pcmBytesPerSecond),1,fp);
    fwrite(&pcm.pcmBlockAlign,sizeof(pcm.pcmBlockAlign),1,fp);
    fwrite(&pcm.pcmBitsPerSample,sizeof(pcm.pcmBitsPerSample),1,fp);

    fwrite(&dataHeader,sizeof(dataHeader),1,fp);
    fwrite(&dataLength,sizeof(dataLength),1,fp);

}

//#######################################################################################################

bool yarp::sig::file::read_wav_file(Sound& sound_data, const char * filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        yCError(SOUNDFILE_WAV, "cannot open file %s for reading\n", filename);
        return false;
    }

    PcmWavHeader header;
    if (!header.parse_from_file(fp))
    {
        yCError(SOUNDFILE_WAV, "error parsing header of file %s\n", filename);
        fclose(fp);
        return false;
    };


    int freq = header.pcm.pcmSamplesPerSecond;
    int channels = header.pcm.pcmChannels;
    int bits = header.pcm.pcmBitsPerSample;
    int samples = header.dataLength/(bits/8)/channels;
    sound_data.resize(samples,channels);
    sound_data.setFrequency(freq);
    ManagedBytes bytes(header.dataLength);
    yCDebug(SOUNDFILE_WAV, "%d channels %d samples %d frequency\n", channels, samples, freq);

    size_t result;
    result = fread(bytes.get(),bytes.length(),1,fp);
    YARP_UNUSED(result);

    auto* data = reinterpret_cast<NetInt16*>(bytes.get());
    int ct = 0;
    for (int i=0; i<samples; i++) {
        for (int j=0; j<channels; j++) {
            sound_data.set(data[ct],i,j);
            ct++;
        }
    }

    fclose(fp);
    return true;
}

bool yarp::sig::file::read_wav_bytestream(Sound& data, const char* bytestream)
{
    yCError(SOUNDFILE_WAV, "read_wav_bytestream() Not yet implemented");
    return true;
}

bool yarp::sig::file::write_wav_file(const Sound& sound_data, const char * filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        yCError(SOUNDFILE_WAV, "cannot open file %s for writing\n", filename);
        return false;
    }

    PcmWavHeader header;
    header.setup_to_write(sound_data, fp);

    ManagedBytes bytes(header.dataLength);
    auto* data = reinterpret_cast<NetInt16*>(bytes.get());
    int ct = 0;
    size_t samples = sound_data.getSamples();
    size_t channels = sound_data.getChannels();
    for (size_t i=0; i<samples; i++) {
        for (size_t j=0; j<channels; j++) {
            int v = sound_data.get(i,j);
            data[ct] = v;
            ct++;
        }
    }
    fwrite(bytes.get(),bytes.length(),1,fp);

    fclose(fp);
    return true;
}
