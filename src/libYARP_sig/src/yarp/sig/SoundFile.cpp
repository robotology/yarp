/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/SoundFile.h>

#include <yarp/conf/system.h>

#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/Sound.h>

#include <cstdio>
#include <cstring>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

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
    printf("bool PcmWavHeader::parse_from_file(FILE *fp)\n");

    size_t ret;

    ret = fread(&wavHeader, sizeof(wavHeader), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&wavLength, sizeof(wavLength), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&formatHeader1, sizeof(formatHeader1), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&formatHeader2, sizeof(formatHeader2), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&formatLength, sizeof(formatLength), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmFormatTag, sizeof(pcm.pcmFormatTag), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmChannels, sizeof(pcm.pcmChannels), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmSamplesPerSecond, sizeof(pcm.pcmSamplesPerSecond), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmBytesPerSecond, sizeof(pcm.pcmBytesPerSecond), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmBlockAlign, sizeof(pcm.pcmBlockAlign), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmBitsPerSample, sizeof(pcm.pcmBitsPerSample), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }
    if (pcm.pcmBitsPerSample != 16)
    {
        printf("sorry, lousy wav read code only does 16-bit ints\n");
        return false;
    }

    //extra bytes in pcm chuck
    int extra_size = formatLength - sizeof(pcm);
    if (extra_size != 0) {
        printf("extra_size = %d\n", extra_size);
        pcmExtraData.allocate(extra_size);
        ret = fread(&pcmExtraData, extra_size, 1, fp);
        if (ret != 1) {
            printf("failed to read wav file");
            return false;
        }
    }

    //extra chunks
    ret = fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    while (dummyHeader != yarp::os::createVocab('d', 'a', 't', 'a'))
    {
        fread(&dummyLength, sizeof(dummyLength), 1, fp);
        if (ret != 1) {
            printf("failed to read wav file");
            return false;
        }
        dummyData.clear();
        dummyData.allocate(dummyLength);
        fread(&dummyData, dummyLength, 1, fp);
        if (ret != 1) {
            printf("failed to read wav file");
            return false;
        }
        fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
        if (ret != 1) {
            printf("failed to read wav file");
            return false;
        }
    }

    dataHeader = dummyHeader;
    fread(&dataLength, sizeof(dataLength), 1, fp);
    if (ret != 1) {
        printf("failed to read wav file");
        return false;
    }

    return true;
}

void PcmWavHeader::setup_to_write(const Sound& src, FILE *fp)
{
    int bitsPerSample = 16;
    int channels = src.getChannels();
    int bytes = channels*src.getSamples()*2;
    int align = channels*((bitsPerSample+7)/8);

    wavHeader = yarp::os::createVocab('R','I','F','F');
    wavLength = bytes + sizeof(PcmWavHeader) - 2*sizeof(NetInt32);
    formatHeader1 = yarp::os::createVocab('W','A','V','E');
    formatHeader2 = yarp::os::createVocab('f','m','t',' ');
    formatLength = sizeof(pcm);

    pcm.pcmFormatTag = 1; /* PCM! */
    pcm.pcmChannels = channels;
    pcm.pcmSamplesPerSecond = (int)src.getFrequency();
    pcm.pcmBytesPerSecond = align*pcm.pcmSamplesPerSecond;
    pcm.pcmBlockAlign = align;
    pcm.pcmBitsPerSample = bitsPerSample;

    dataHeader = yarp::os::createVocab('d','a','t','a');
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

bool yarp::sig::file::read(Sound& dest, const char *src)
{
    FILE *fp = fopen(src, "rb");
    if (!fp) {
        printf("cannot open file %s for reading\n", src);
        return false;
    }

    PcmWavHeader header;
    if (!header.parse_from_file(fp))
    {
        printf("error parsing header of file %s\n", src);
        fclose(fp);
        return false;
    };


    int freq = header.pcm.pcmSamplesPerSecond;
    int channels = header.pcm.pcmChannels;
    int bits = header.pcm.pcmBitsPerSample;
    int samples = header.dataLength/(bits/8)/channels;
    dest.resize(samples,channels);
    dest.setFrequency(freq);
    ManagedBytes bytes(header.dataLength);
    printf("%d channels %d samples %d frequency\n", channels, samples, freq);

    size_t result;
    result = fread(bytes.get(),bytes.length(),1,fp);
    YARP_UNUSED(result);

    auto* data = reinterpret_cast<NetInt16*>(bytes.get());
    int ct = 0;
    for (int i=0; i<samples; i++) {
        for (int j=0; j<channels; j++) {
            dest.set(data[ct],i,j);
            ct++;
        }
    }

    fclose(fp);
    return true;
}


bool yarp::sig::file::write(const Sound& src, const char *dest)
{
    FILE *fp = fopen(dest, "wb");
    if (!fp) {
        printf("cannot open file %s for writing\n", dest);
        return false;
    }

    PcmWavHeader header;
    header.setup_to_write(src, fp);

    ManagedBytes bytes(header.dataLength);
    auto* data = reinterpret_cast<NetInt16*>(bytes.get());
    int ct = 0;
    size_t samples = src.getSamples();
    size_t channels = src.getChannels();
    for (size_t i=0; i<samples; i++) {
        for (size_t j=0; j<channels; j++) {
            int v = src.get(i,j);
            data[ct] = v;
            ct++;
        }
    }
    fwrite(bytes.get(),bytes.length(),1,fp);

    fclose(fp);
    return true;
}

bool yarp::sig::file::soundStreamReader::open(const char *filename)
{
    if (fp)
    {
        printf("file %s is already open\n", fname);
        return false;
    }

    fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("cannot open file %s for reading\n", filename);
        return false;
    }
    strcpy(fname,filename);
    PcmWavHeader header;
    if (!header.parse_from_file(fp))
    {
        printf("error parsing header of file %s\n", fname);
        fclose(fp);
        return false;
    }
    this->soundInfo.freq = header.pcm.pcmSamplesPerSecond;
    this->soundInfo.channels = header.pcm.pcmChannels;
    this->soundInfo.bits = header.pcm.pcmBitsPerSample;
    this->soundInfo.samples = header.dataLength/(this->soundInfo.bits/8)/this->soundInfo.channels;
    this->soundInfo.data_start_offset = ftell(fp);

    return true;
}

bool yarp::sig::file::soundStreamReader::close()
{
    if (!fp)
    {
        printf("no files open\n");
        return false;
    }

    fclose(fp);
    fname[0]=0;
    index=0;
    return true;
}

size_t yarp::sig::file::soundStreamReader::readBlock(Sound& dest, size_t block_size)
{
    int expected_bytes = (int)(block_size*(soundInfo.bits/8)*soundInfo.channels);

    //this probably works only if soundInfo.bits=16
    int expected_words=expected_bytes/(soundInfo.bits/8);
    auto* data = new NetInt16 [expected_words];

    size_t bytes_read = fread(data,1,expected_bytes,fp);
    size_t samples_read = bytes_read/(soundInfo.bits/8)/soundInfo.channels;

    dest.resize((int)samples_read,soundInfo.channels);
    dest.setFrequency(soundInfo.freq);

    int ct = 0;
    for (size_t i=0; i<samples_read; i++) {
        for (size_t j=0; j< (size_t) soundInfo.channels; j++) {
            dest.set(data[ct],i,j);
            ct++;
        }
    }
    index+=samples_read;

    delete [] data;
    return samples_read;
}

bool  yarp::sig::file::soundStreamReader::rewind(size_t sample_offset)
{
    if (!fp)
    {
        printf("no files open\n");
        return false;
    }

    if ((int)sample_offset>this->soundInfo.samples)
    {
        printf("invalid sample_offset\n");
        return false;
    }

    fseek(fp,(long int)(this->soundInfo.data_start_offset+(sample_offset*this->soundInfo.channels*this->soundInfo.bits/2)),SEEK_SET);
    index=sample_offset;

    return true;
}

size_t yarp::sig::file::soundStreamReader::getIndex()
{
    return index;
}
