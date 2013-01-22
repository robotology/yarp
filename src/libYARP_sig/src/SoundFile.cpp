// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Vocab.h>

#include <stdio.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

#include <yarp/os/begin_pack_for_net.h>
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

    NetInt32 dataHeader;
    NetInt32 dataLength;

    void setup(const Sound& sound);
} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

void PcmWavHeader::setup(const Sound& src) {
    int bitsPerSample = 16;
    int channels = src.getChannels();
    int bytes = channels*src.getSamples()*2;
    int align = channels*((bitsPerSample+7)/8);
    
    wavHeader = VOCAB4('R','I','F','F');
    wavLength = bytes + sizeof(PcmWavHeader) - 2*sizeof(NetInt32);
    formatHeader1 = VOCAB4('W','A','V','E');
    formatHeader2 = VOCAB4('f','m','t',' ');
    formatLength = sizeof(pcm);

    pcm.pcmFormatTag = 1; /* PCM! */
    pcm.pcmChannels = channels;
    pcm.pcmSamplesPerSecond = (int)src.getFrequency();
    pcm.pcmBytesPerSecond = align*pcm.pcmSamplesPerSecond;
    pcm.pcmBlockAlign = align;
    pcm.pcmBitsPerSample = bitsPerSample;

    dataHeader = VOCAB4('d','a','t','a');
    dataLength = bytes;
}

bool yarp::sig::file::read(Sound& dest, const char *src) {
	FILE *fp = fopen(src, "rb");
	if (!fp) {
        printf("cannot open file %s for reading\n", src);
        return false;
    }

    PcmWavHeader header;
    fread(&header,sizeof(header),1,fp);

    int freq = header.pcm.pcmSamplesPerSecond;
    int channels = header.pcm.pcmChannels;
    int expect = header.dataLength;
    int bits = header.pcm.pcmBitsPerSample;
    if (bits!=16) {
        printf("sorry, lousy wav read code only does 16-bit ints\n");
        fclose(fp);
        return false;
    }
    int samples = (expect/2)/channels;
    printf("%d channels %d samples %d frequency\n",
           channels, samples, freq);
    dest.resize(samples,channels);
    dest.setFrequency(freq);

    ManagedBytes bytes(expect);
    fread(bytes.get(),bytes.length(),1,fp);

    NetInt16 *data = (NetInt16*)bytes.get();
    int ct = 0;
    for (int i=0; i<samples; i++) {
        for (int j=0; j<channels; j++) {
            dest.set(data[ct],i,j);
            ct++;
        }
    }

    fclose(fp);
    return false;
}


bool yarp::sig::file::write(const Sound& src, const char *dest) {
	FILE *fp = fopen(dest, "wb");
	if (!fp) {
        printf("cannot open file %s for writing\n", dest);
        return false;
    }

    PcmWavHeader header;
    header.setup(src);
    fwrite((void*)&header, 1, sizeof(header), fp);
 
    ManagedBytes bytes(header.dataLength);
    NetInt16 *data = (NetInt16*)bytes.get();
    int ct = 0;
    int samples = src.getSamples();
    int channels = src.getChannels();
    for (int i=0; i<samples; i++) {
        for (int j=0; j<channels; j++) {
            int v = src.get(i,j);
            data[ct] = v;
            ct++;
        }
    }
    fwrite(bytes.get(),bytes.length(),1,fp);

    fclose(fp);
    return true;
}
