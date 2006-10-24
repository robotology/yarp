// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * This file has been modified for the yarp.
 *
 * The behaviour and appearence of the program code below can differ to a major
 * extent from the version distributed by the original author(s).
 *
 */

/* @(#)wav.c	1.4 01/10/27 Copyright 1998,1999 Heiko Eissfeldt */
/***
 * CopyPolicy: GNU Public License 2 applies
 * Copyright (C) by Heiko Eissfeldt
 *
 *
 */

// Summary for YARP:
// Copyright: 1998,1999 Heiko Eissfeldt


#include <ace/OS.h>
#include <yarp/os/NetInt32.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/os/ManagedBytes.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

typedef short NetInt16; // need to add this to yarp, do right thing everywhere
typedef NetInt32 FOURCC;

typedef struct CHUNKHDR {
    FOURCC ckid;	/* chunk ID */
    NetInt32 dwSize; 	/* chunk size */
} CHUNKHDR;

/* flags for 'wFormatTag' field of WAVEFORMAT */
#define WAVE_FORMAT_PCM 1

/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
    NetInt16 wFormatTag;	/* format type */
    NetInt16 nChannels;	/* number of channels (i.e. mono, stereo, etc.) */
    NetInt32 nSamplesPerSec; /* sample rate */
    NetInt32  nAvgBytesPerSec; /* for buffer size estimate */
    NetInt16  nBlockAlign;     /* block size of data */
    NetInt16 wBitsPerSample;
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT *PPCMWAVEFORMAT;

// noops when NetInt32 and NetInt16 do right thing
#define cpu_to_le32(x) (x)
#define cpu_to_le16(x) (x)

/* MMIO macros */
#define mmioFOURCC(ch0, ch1, ch2, ch3) \
  ((NetInt32)((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | \
  ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24)))

#define FOURCC_RIFF	mmioFOURCC ('R', 'I', 'F', 'F')
#define FOURCC_LIST	mmioFOURCC ('L', 'I', 'S', 'T')
#define FOURCC_WAVE	mmioFOURCC ('W', 'A', 'V', 'E')
#define FOURCC_FMT	mmioFOURCC ('f', 'm', 't', ' ')
#define FOURCC_DATA	mmioFOURCC ('d', 'a', 't', 'a')


/* simplified Header for standard WAV files */
typedef struct WAVEHDR {
    CHUNKHDR chkRiff;
    FOURCC fccWave;
    CHUNKHDR chkFmt;
    NetInt16 wFormatTag;	/* format type */
    NetInt16 nChannels;	/* number of channels (i.e. mono, stereo, etc.) */
    NetInt32 nSamplesPerSec; /* sample rate */
    NetInt32 nAvgBytesPerSec;/* for buffer estimation */
    NetInt16 nBlockAlign;	/* block size of data */
    NetInt16 wBitsPerSample;
    CHUNKHDR chkData;
} WAVEHDR;

#define IS_STD_WAV_HEADER(waveHdr) ( \
  waveHdr.chkRiff.ckid == FOURCC_RIFF && \
  waveHdr.fccWave == FOURCC_WAVE && \
  waveHdr.chkFmt.ckid == FOURCC_FMT && \
  waveHdr.chkData.ckid == FOURCC_DATA && \
  waveHdr.wFormatTag == WAVE_FORMAT_PCM)


static int _InitSound (WAVEHDR& waveHdr ,
                       long channels , 
                       unsigned long rate , 
                       long nBitsPerSample , 
                       unsigned long expected_bytes ) {
    unsigned long nBlockAlign = channels * ((nBitsPerSample + 7) / 8);
    unsigned long nAvgBytesPerSec = nBlockAlign * rate;
    unsigned long temp = expected_bytes + sizeof(WAVEHDR) - sizeof(CHUNKHDR);

    waveHdr.chkRiff.ckid    = cpu_to_le32(FOURCC_RIFF);
    waveHdr.fccWave         = cpu_to_le32(FOURCC_WAVE);
    waveHdr.chkFmt.ckid     = cpu_to_le32(FOURCC_FMT);
    waveHdr.chkFmt.dwSize   = cpu_to_le32(sizeof (PCMWAVEFORMAT));
    waveHdr.wFormatTag      = cpu_to_le16(WAVE_FORMAT_PCM);
    waveHdr.nChannels       = (NetInt16) cpu_to_le16(channels);
    waveHdr.nSamplesPerSec  = cpu_to_le32(rate);
    waveHdr.nBlockAlign     = (NetInt16) cpu_to_le16(nBlockAlign);
    waveHdr.nAvgBytesPerSec = cpu_to_le32(nAvgBytesPerSec);
    waveHdr.wBitsPerSample  = (NetInt16) cpu_to_le16(nBitsPerSample);
    waveHdr.chkData.ckid    = cpu_to_le32(FOURCC_DATA);
    waveHdr.chkRiff.dwSize  = cpu_to_le32(temp);
    waveHdr.chkData.dwSize  = cpu_to_le32(expected_bytes);
    return 0;
}


bool yarp::sig::file::read(Sound& dest, const char *src) {
    WAVEHDR waveHdr;
	FILE *fp = ACE_OS::fopen(src, "rb");
	if (!fp) {
        ACE_OS::printf("cannot open file %s for reading\n", src);
        return false;
    }
    ACE_OS::fread(&waveHdr,sizeof(waveHdr),1,fp);
    int freq = waveHdr.nSamplesPerSec;
    int channels = waveHdr.nChannels;
    int expect = waveHdr.chkData.dwSize;
    int bits = waveHdr.wBitsPerSample;
    if (bits!=16) {
        ACE_OS::printf("sorry, lousy wav read code only does 16-bit ints\n");
        return false;
    }
    int samples = (expect/2)/channels;
    printf("%d channels %d samples %d frequency\n",
           channels, samples, freq);
    dest.resize(samples,channels);
    dest.setFrequency(freq);

    /*
    if (expect!=dest.getRawDataSize()) {
        printf("expect to read %ld bytes but would read %ld\n",
               dest.getRawDataSize(), expect);
        ACE_OS::fclose(fp);
        return false;
    }
    fread(dest.getRawData(),dest.getRawDataSize(),1,fp);
    */

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

    ACE_OS::fclose(fp);
    return false;
}

bool yarp::sig::file::write(const Sound& src, const char *dest) {
    WAVEHDR waveHdr;
    int expect = src.getChannels()*src.getSamples()*2;
    _InitSound(waveHdr,
               src.getChannels(),
               (int)src.getFrequency(),
               16,
               expect);
    
	FILE *fp = ACE_OS::fopen(dest, "wb");
	if (!fp) {
        ACE_OS::printf("cannot open file %s for writing\n", dest);
        return false;
    }
    /*
    if (expect!=src.getRawDataSize()) {
        printf("expect to write %ld bytes but would write %ld\n",
               expect, src.getRawDataSize());
        ACE_OS::fclose(fp);
        return false;
    }
    */

    ACE_OS::fwrite((void*)&waveHdr, 1, sizeof(waveHdr), fp);


    ManagedBytes bytes(expect);
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

    /*
    ACE_OS::fwrite((void*)src.getRawData(), (size_t)src.getRawDataSize(), 
                   1, fp);
    */
    ACE_OS::fclose(fp);
    return true;
}


