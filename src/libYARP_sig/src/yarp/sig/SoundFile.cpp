/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <cstring>

#if (YARP_HAS_FFMPEG)
extern "C" {
    /*#include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>*/

    #include <libavutil/opt.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/common.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/samplefmt.h>
}
#endif

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

namespace
{
    YARP_LOG_COMPONENT(SOUNDFILE, "yarp.sig.SoundFile")
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
    yCTrace(SOUNDFILE, "bool PcmWavHeader::parse_from_file(FILE *fp)\n");

    size_t ret;

    ret = fread(&wavHeader, sizeof(wavHeader), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&wavLength, sizeof(wavLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&formatHeader1, sizeof(formatHeader1), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&formatHeader2, sizeof(formatHeader2), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&formatLength, sizeof(formatLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmFormatTag, sizeof(pcm.pcmFormatTag), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmChannels, sizeof(pcm.pcmChannels), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmSamplesPerSecond, sizeof(pcm.pcmSamplesPerSecond), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmBytesPerSecond, sizeof(pcm.pcmBytesPerSecond), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmBlockAlign, sizeof(pcm.pcmBlockAlign), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    ret = fread(&pcm.pcmBitsPerSample, sizeof(pcm.pcmBitsPerSample), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }
    if (pcm.pcmBitsPerSample != 16)
    {
        yCError(SOUNDFILE, "sorry, lousy wav read code only does 16-bit ints\n");
        return false;
    }

    //extra bytes in pcm chuck
    int extra_size = formatLength - sizeof(pcm);
    if (extra_size != 0)
    {
        yCError(SOUNDFILE, "extra_size = %d\n", extra_size);
        pcmExtraData.allocate(extra_size);
        ret = fread(&pcmExtraData, extra_size, 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE, "failed to read wav file");
            return false;
        }
    }

    //extra chunks
    ret = fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
        return false;
    }

    while (dummyHeader != yarp::os::createVocab('d', 'a', 't', 'a'))
    {
        fread(&dummyLength, sizeof(dummyLength), 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE, "failed to read wav file");
            return false;
        }
        dummyData.clear();
        dummyData.allocate(dummyLength);
        fread(&dummyData, dummyLength, 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE, "failed to read wav file");
            return false;
        }
        fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE, "failed to read wav file");
            return false;
        }
    }

    dataHeader = dummyHeader;
    fread(&dataLength, sizeof(dataLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE, "failed to read wav file");
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

//#######################################################################################################

bool yarp::sig::file::read(Sound& data, const char* filename)
{
    const char* file_ext = strrchr(filename, '.');
    if (file_ext == nullptr)
    {
        yCError(SOUNDFILE) << "cannot find file extension in file name";
        return false;
    }

    if (strcmp(file_ext, ".wav") == 0)
    {
        return read_wav(data, filename);
    }
    else if (strcmp(file_ext, ".mp3") == 0)
    {
        return read_mp3(data, filename);
    }
    yCError(SOUNDFILE) << "Unknown file format";
    return false;
}

bool yarp::sig::file::write(const Sound& sound_data, const char* filename)
{
    const char* file_ext = strrchr(filename, '.');
    if (file_ext == nullptr)
    {
        yCError(SOUNDFILE) << "cannot find file extension in file name";
        return false;
    }

    if (strcmp(file_ext, ".wav") == 0)
    {
        return write_wav(sound_data, filename);
    }
    else if (strcmp(file_ext, ".mp3") == 0)
    {
        return write_mp3(sound_data, filename);
    }

    yCError(SOUNDFILE) << "Unknown file format";
    return false;
}

//#######################################################################################################
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AV_INPUT_BUFFER_PADDING_SIZE   64
#define AUDIO_REFILL_THRESH   4096
static void decode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, FILE* outfile)
{
    int i, ch;
    int ret, data_size;
    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error submitting the packet to the decoder\n");
        exit(1);
    }
    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to calculate data size\n");
            exit(1);
        }
        for (i = 0; i < frame->nb_samples; i++)
            for (ch = 0; ch < dec_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);
    }
}

bool yarp::sig::file::read_mp3(Sound& sound_data, const char* filename)
{
#if (!YARP_HAS_FFMPEG)

    yCError(SOUNDFILE) << "Not yet implemented";
    return false;
#else
    std::string test = "testoutdecompress.wav";
    const char* outfilename = test.c_str();
    const AVCodec* codec;
    AVCodecContext* c = NULL;
    AVCodecParserContext* parser = NULL;
    int len, ret;
    FILE* f, * outfile;
    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t* data;
    size_t   data_size;
    AVPacket* pkt;
    AVFrame* decoded_frame = NULL;

    pkt = av_packet_alloc();
    /* find the MPEG audio decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    parser = av_parser_init(codec->id);
    if (!parser) {
        fprintf(stderr, "Parser not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }
    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    outfile = fopen(outfilename, "wb");
    if (!outfile) {
        av_free(c);
        exit(1);
    }
    /* decode until eof */
    data = inbuf;
    data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);
    while (data_size > 0) {
        if (!decoded_frame) {
            if (!(decoded_frame = av_frame_alloc())) {
                fprintf(stderr, "Could not allocate audio frame\n");
                exit(1);
            }
        }
        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
            data, data_size,
            AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            fprintf(stderr, "Error while parsing\n");
            exit(1);
        }
        data += ret;
        data_size -= ret;
        if (pkt->size)
            decode(c, pkt, decoded_frame, outfile);
        if (data_size < AUDIO_REFILL_THRESH) {
            memmove(inbuf, data, data_size);
            data = inbuf;
            len = fread(data + data_size, 1,
                AUDIO_INBUF_SIZE - data_size, f);
            if (len > 0)
                data_size += len;
        }
    }
    /* flush the decoder */
    pkt->data = NULL;
    pkt->size = 0;
    decode(c, pkt, decoded_frame, outfile);
    fclose(outfile);
    fclose(f);
    avcodec_free_context(&c);
    av_parser_close(parser);
    av_frame_free(&decoded_frame);
    av_packet_free(&pkt);
    return true;
#endif
}

bool yarp::sig::file::write_mp3(const Sound& sound_data, const char* filename)
{
#if (!YARP_HAS_FFMPEG)

    yCError(SOUNDFILE) << "Not yet implemented";
    return false;
#else
    yCError(SOUNDFILE) << "Not yet implemented";
    return false;
#endif
}

bool yarp::sig::file::read_wav(Sound& sound_data, const char * filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        yCError(SOUNDFILE, "cannot open file %s for reading\n", filename);
        return false;
    }

    PcmWavHeader header;
    if (!header.parse_from_file(fp))
    {
        yCError(SOUNDFILE, "error parsing header of file %s\n", filename);
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
    yCDebug(SOUNDFILE, "%d channels %d samples %d frequency\n", channels, samples, freq);

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


bool yarp::sig::file::write_wav(const Sound& sound_data, const char * filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        yCError(SOUNDFILE, "cannot open file %s for writing\n", filename);
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

//#######################################################################################################

bool yarp::sig::file::soundStreamReader::open(const char *filename)
{
    if (fp)
    {
        yCError(SOUNDFILE, "file %s is already open\n", fname);
        return false;
    }

    fp = fopen(filename, "rb");
    if (!fp)
    {
        yCError(SOUNDFILE, "cannot open file %s for reading\n", filename);
        return false;
    }
    strcpy(fname,filename);
    PcmWavHeader header;
    if (!header.parse_from_file(fp))
    {
        yCError(SOUNDFILE, "error parsing header of file %s\n", fname);
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
        yCError(SOUNDFILE, "no files open\n");
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
        yCError(SOUNDFILE, "no files open\n");
        return false;
    }

    if ((int)sample_offset>this->soundInfo.samples)
    {
        yCError(SOUNDFILE, "invalid sample_offset\n");
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
