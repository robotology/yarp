/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/SoundFileMp3.h>

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
    YARP_LOG_COMPONENT(SOUNDFILE_MP3, "yarp.sig.SoundFileMp3")
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

int check_sample_fmt(const AVCodec * codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat* p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE)
    {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

int select_sample_rate(const AVCodec * codec)
{
    const int* p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p)
    {
         if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
         p++;
    }
    return best_samplerate;
}

bool encode(AVCodecContext* ctx, AVFrame* frame, AVPacket* pkt, FILE * output)
{
    int ret;

    // send the frame for encoding 
    ret = avcodec_send_frame(ctx, frame);
    if (ret < 0)
    {
        yCError(SOUNDFILE_MP3, "Error sending the frame to the encoder\n");
        return false;
    }

     // read all the available output packets (in general there may be any
     // number of them
     while (ret >= 0)
     {
         ret = avcodec_receive_packet(ctx, pkt);
         if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
         {
             return true;
         }
         else if (ret < 0)
         {
             yCError(SOUNDFILE_MP3, "Error encoding audio frame\n");
             return false;
         }
         fwrite(pkt->data, 1, pkt->size, output);
         av_packet_unref(pkt);
     }
     return true;
}

/* select layout with the highest channel count */
int select_channel_layout(const AVCodec * codec)
{
   const uint64_t * p;
   uint64_t best_ch_layout = 0;
   int best_nb_channels = 0;

   if (!codec->channel_layouts)
      return AV_CH_LAYOUT_STEREO;

   p = codec->channel_layouts;
   while (*p)
   {
       int nb_channels = av_get_channel_layout_nb_channels(*p);

       if (nb_channels > best_nb_channels)
       {
           best_ch_layout = *p;
           best_nb_channels = nb_channels;
       }
       p++;
    }
    return best_ch_layout;
}

bool yarp::sig::file::write_mp3(const Sound& sound_data, const char* filename)
{
#if (!YARP_HAS_FFMPEG)

    yCError(SOUNDFILE) << "Not yet implemented";
    return false;
#else
    const AVCodec * codec;
    AVCodecContext * c = NULL;
    AVFrame * frame;
    AVPacket * pkt;
    int i, j, k, ret;
    FILE * f;
    uint16_t * samples;

    // find the MP3 encoder
    codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!codec)
    {
        yCError(SOUNDFILE_MP3, "Codec not found");
        return false;
    }

    c = avcodec_alloc_context3(codec);
    if (!c)
    {
        yCError(SOUNDFILE_MP3, "Could not allocate audio codec context");
        return false;
    }

    // the compressed output bitrate
    c->bit_rate = 64000;

    // check that the encoder supports s16 pcm input
    c->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!check_sample_fmt(codec, c->sample_fmt))
    {
        yCError(SOUNDFILE_MP3, "Encoder does not support sample format %s",
        av_get_sample_fmt_name(c->sample_fmt));
        return false;
    }

    // select other audio parameters supported by the encoder
    c->sample_rate = select_sample_rate(codec);
    c->channel_layout = select_channel_layout(codec);
    c->channels = av_get_channel_layout_nb_channels(c->channel_layout);

    // open it
    if (avcodec_open2(c, codec, NULL) < 0)
    {
        yCError(SOUNDFILE_MP3, "Could not open codec");
        return false;
    }

    f = fopen(filename, "wb");
    if (!f)
    {
        yCError(SOUNDFILE_MP3, "Could not open %s", filename);
        return false;
    }

    // packet for holding encoded output
    pkt = av_packet_alloc();
    if (!pkt)
    {
        yCError(SOUNDFILE_MP3, "could not allocate the packet");
        fclose(f);
        return false;
    }

    // frame containing input raw audio
    frame = av_frame_alloc();
    if (!frame)
    {
        yCError(SOUNDFILE_MP3, "Could not allocate audio frame");
        fclose(f);
        return false;
    }

    frame->nb_samples = c->frame_size;
    frame->format = c->sample_fmt;
    frame->channel_layout = c->channel_layout;
    
    // allocate the data buffers
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0)
    {
        yCError(SOUNDFILE_MP3, "Could not allocate audio data buffers");
        fclose(f);
        return false;
    }

    // encode
    size_t soundsize = sound_data.getSamples();
    for (i = 0; i < soundsize/c->frame_size ; i++)
    {
        ret = av_frame_make_writable(frame);
        if (ret < 0)  exit(1);

        samples = (uint16_t*)frame->data[0];
        for (j = 0; j < c->frame_size; j++)
        {
            for (k = 0; k < c->channels; k++)
                samples[j*c->channels + k] = sound_data.get(j+i* c->frame_size,k);
        }
        if (encode(c, frame, pkt, f) == false)
        {
            yCError(SOUNDFILE_MP3, "Encode failed, memory could be corrupted, should I exit?");
        }
    }

    // flush the encoder
    if (encode(c, NULL, pkt, f) == false)
    {
        yCError(SOUNDFILE_MP3, "Encode failed, memory could be corrupted, should I exit?");
    }

    fclose(f);

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&c);

    return true;
#endif
}
