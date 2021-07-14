/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <fstream>

#if defined (YARP_HAS_FFMPEG)
extern "C"
{
    #include <libavutil/opt.h>
    #include <libavcodec/avcodec.h>
    #include <libavcodec/version.h>
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
#if defined (YARP_HAS_FFMPEG)
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH   4096
#endif

//#######################################################################################################
#if defined (YARP_HAS_FFMPEG)
bool decode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, Sound& sound_data)
{
    int i, ch;
    int ret, data_size;
    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0)
    {
        yCError(SOUNDFILE_MP3, "Error submitting the packet to the decoder");
        return false;
    }
    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return false;
        }
        else if (ret < 0)
        {
            yCError(SOUNDFILE_MP3, "Error during decoding");
            return false;
        }
        //this seems to be 2: S16P
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0)
        {
            /* This should not occur, checking just for paranoia */
            yCError(SOUNDFILE_MP3, "Failed to calculate data size\n");
            return false;
        }

        yarp::sig::Sound frame_sound;
        frame_sound.resize(frame->nb_samples, dec_ctx->channels);
        if (sound_data.getChannels()==0) { sound_data.resize(0, dec_ctx->channels);}

        for (i = 0; i < frame->nb_samples; i++) //1152
        {
            for (ch = 0; ch < dec_ctx->channels; ch++) //2
            {
                short int val = *((short int*)frame->data[ch] + i);
                frame_sound.set(val,i,ch);
            }
        }
        sound_data += frame_sound;
    }
    return true;
}

int check_sample_fmt(const AVCodec * codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat* p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE)
    {
        if (*p == sample_fmt) {
            return 1;
        }
        p++;
    }
    return 0;
}

int select_sample_rate(const AVCodec * codec)
{
    const int* p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates) {
        return 44100;
    }

    p = codec->supported_samplerates;
    while (*p)
    {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate)) {
            best_samplerate = *p;
        }
         p++;
    }
    return best_samplerate;
}

bool encode(AVCodecContext* ctx, AVFrame* frame, AVPacket* pkt, std::fstream& os)
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
         os.write((const char*)(pkt->data), pkt->size);
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

   if (!codec->channel_layouts) {
       return AV_CH_LAYOUT_STEREO;
   }

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
#endif

//#######################################################################################################
bool yarp::sig::file::write_mp3_file(const Sound& sound_data, const char* filename, size_t bitrate)
{
#if !defined (YARP_HAS_FFMPEG)

    yCError(SOUNDFILE_MP3) << "write_mp3_file() not supported: lib ffmpeg not found";
    return false;
#else
    const AVCodec * codec = nullptr;
    AVCodecContext * c = nullptr;
    AVFrame * frame = nullptr;
    AVPacket * pkt = nullptr;
    int ret;
    std::fstream fos;
    uint16_t * samples = nullptr;

#if LIBAVCODEC_VERSION_MAJOR < 58
    //register all the codecs, deprecated and useless in libffmpeg4.0
    avcodec_register_all();
#endif

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
    c->bit_rate = bitrate;

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

    fos.open(filename, std::fstream::out | std::fstream::binary);
    if (fos.is_open()==false)
    {
        yCError(SOUNDFILE_MP3, "Cannot open %s for writing", filename);
        return false;
    }

    // packet for holding encoded output
    pkt = av_packet_alloc();
    if (!pkt)
    {
        yCError(SOUNDFILE_MP3, "could not allocate the packet");
        fos.close();
        return false;
    }

    // frame containing input raw audio
    frame = av_frame_alloc();
    if (!frame)
    {
        yCError(SOUNDFILE_MP3, "Could not allocate audio frame");
        fos.close();
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
        fos.close();
        return false;
    }

    // encode
    size_t soundsize = sound_data.getSamples();
    size_t nframes = soundsize / c->frame_size;
    size_t rem_lastframe = soundsize % c->frame_size;
    YARP_UNUSED(rem_lastframe);
    for (size_t i = 0; i < nframes; i++)
    {
        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            exit(1);
        }

        samples = (uint16_t*)frame->data[0];
        for (int j = 0; j < c->frame_size; j++)
        {
            for (int k = 0; k < c->channels; k++) {
                samples[j * c->channels + k] = sound_data.get(j + i * c->frame_size, k);
            }
        }
        if (encode(c, frame, pkt, fos) == false)
        {
            yCError(SOUNDFILE_MP3, "Encode failed, memory could be corrupted, should I exit?");
        }
    }

    // flush the encoder
    if (encode(c, NULL, pkt, fos) == false)
    {
        yCError(SOUNDFILE_MP3, "Encode failed, memory could be corrupted, should I exit?");
    }

    fos.close();

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&c);

    return true;
#endif
}

bool read_mp3_istream(Sound& sound_data, std::istream& istream)
{
#if !defined (YARP_HAS_FFMPEG)
    yCError(SOUNDFILE_MP3) << "read_mp3_istream() not supported: lib ffmpeg not found";
    return false;
#else
    const AVCodec* codec = nullptr;
    AVCodecContext* c = nullptr;
    AVCodecParserContext* parser = nullptr;
    int len, ret;
    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t* data = nullptr;
    size_t   data_size;
    AVPacket* pkt = nullptr;
    AVFrame* decoded_frame = nullptr;

    pkt = av_packet_alloc();

#if LIBAVCODEC_VERSION_MAJOR < 58
    //register all the codecs, deprecated and useless in libffmpeg4.0
    avcodec_register_all();
#endif

    // find the MPEG audio decoder
    codec = avcodec_find_decoder(AV_CODEC_ID_MP2);
    if (!codec)
    {
        yCError(SOUNDFILE_MP3, "Codec not found");
        return false;
    }
    parser = av_parser_init(codec->id);
    if (!parser)
    {
        yCError(SOUNDFILE_MP3, "Parser not found");
        return false;
    }
    c = avcodec_alloc_context3(codec);
    if (!c)
    {
        yCError(SOUNDFILE_MP3, "Could not allocate audio codec context");
        return false;
    }
    //open the codec
    if (avcodec_open2(c, codec, NULL) < 0)
    {
        yCError(SOUNDFILE_MP3, "Could not open codec");
        return false;
    }

    // decode until eof
    data = inbuf;
    istream.read((char*)(inbuf), AUDIO_INBUF_SIZE);
    data_size = istream.gcount();
    if (data_size == 0)
    {
        yCError(SOUNDFILE_MP3, "Cannot process invalid (empty) stream");
        return false;
    }
    while (data_size > 0)
    {
        if (!decoded_frame)
        {
            if (!(decoded_frame = av_frame_alloc()))
            {
                yCError(SOUNDFILE_MP3, "Could not allocate audio frame");
                return false;
            }
        }
        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0)
        {
            yCError(SOUNDFILE_MP3, "Error while parsing");
            return false;
        }
        data += ret;
        data_size -= ret;
        if (pkt->size) {
            decode(c, pkt, decoded_frame, sound_data);
        }
        if (data_size < AUDIO_REFILL_THRESH)
        {
            memmove(inbuf, data, data_size);
            data = inbuf;
            istream.read((char*)(data + data_size), AUDIO_INBUF_SIZE - data_size);
            len = istream.gcount();
            if (len > 0) {
                data_size += len;
            }
        }
    }
    // flush the decoder
    pkt->data = NULL;
    pkt->size = 0;
    decode(c, pkt, decoded_frame, sound_data);

    //set the sample rate (is it ok? maybe some codecs allow variable sample rate?)
    sound_data.setFrequency(c->sample_rate);

    //cleanup
    avcodec_free_context(&c);
    av_parser_close(parser);
    av_frame_free(&decoded_frame);
    av_packet_free(&pkt);
    return true;
#endif
}

bool yarp::sig::file::read_mp3_file(Sound& sound_data, const char* filename)
{
    std::fstream fis;
    fis.open(filename, std::fstream::in | std::fstream::binary);
    if (fis.is_open() == false)
    {
        yCError(SOUNDFILE_MP3, "Cannot open %s for reading", filename);
        return false;
    }

    bool b = read_mp3_istream(sound_data, fis);
    fis.close();
    return b;
}

bool yarp::sig::file::read_mp3_bytestream(Sound& data, const char* bytestream, size_t streamsize)
{
    std::istringstream iss(std::string(bytestream, streamsize));
    return read_mp3_istream(data, iss);
}
