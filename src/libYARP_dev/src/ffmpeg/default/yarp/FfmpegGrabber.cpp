// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/FfmpegGrabber.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

// based on Martim Boehme's example at:
//   http://www.inb.uni-luebeck.de/~boehme/using_libavcodec.html

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

#include <stdio.h>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::file;

class DecoderState {
public:
    int      bytesRemaining;
    int      bytesDecoded;
    int      frameFinished;
    int      index;

    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;

    // video buffers
    AVFrame         *pFrame; 
    AVFrame         *pFrameRGB;
    uint8_t         *buffer;

    DecoderState() {
        index = -1;
        pCodec = NULL;
        pFrame = NULL;
        pFrameRGB = NULL;
        buffer = NULL;
        frameFinished = 0;
    }

    bool isFinished() {
        return frameFinished!=0;
    }

    int getIndex() {
        return index;
    }

    virtual ~DecoderState() {
        if (pCodecCtx!=NULL) {
            avcodec_close(pCodecCtx);
        }
        if (buffer!=NULL) {
            delete [] buffer;
        }
        if (pFrameRGB!=NULL) {
            av_free(pFrameRGB);
        }
        if (pFrame!=NULL) {
            av_free(pFrame);
        }
    }

    int getStream(AVFormatContext *pFormatCtx, CodecType code, 
                  const char *name) {
        // Find the first stream
        int videoStream=-1;
        for(int i=0; i<pFormatCtx->nb_streams; i++)
            if(pFormatCtx->streams[i]->codec->codec_type==code)
                {
                    videoStream=i;
                    printf("First %s stream is stream #%d\n", name);
                    break;
                }
        if(videoStream==-1) {
            printf("Could not find %s stream\n", name);
        }
        index = videoStream;

        return index;
    }

    bool getCodec(AVFormatContext *pFormatCtx) {
        // Get a pointer to the codec context for the video stream
        pCodecCtx=pFormatCtx->streams[index]->codec;

        // Find the decoder for the video stream
        pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec==NULL) {
            printf("Codec not found\n");
            return false; // Codec not found
        }
        
        // Open codec
        if(avcodec_open(pCodecCtx, pCodec)<0) {
            printf("Could not open codec\n");
            return false; // Could not open codec
        }

        return true;
    }


    bool allocateImage() {
        // Allocate video frame
        pFrame=avcodec_alloc_frame();
        
        // Allocate an AVFrame structure
        pFrameRGB=avcodec_alloc_frame();
        if(pFrameRGB==NULL) {
            printf("Could not allocate a frame\n");
            return false;
        }
        
        // Determine required buffer size and allocate buffer
        int numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                        pCodecCtx->height);
        buffer=new uint8_t[numBytes];
        
        // Assign appropriate parts of buffer to image planes in pFrameRGB
        avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
                       pCodecCtx->width, pCodecCtx->height);
        return true;
    }

    int getWidth() {
        return pCodecCtx->width;
    }

    int getHeight() {
        return pCodecCtx->height;
    }

    bool getVideo(AVPacket& packet,ImageOf<PixelRgb>& image) {
        // Decode video frame
        avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
                             packet.data, packet.size);

        // Did we get a video frame?
        if(frameFinished) {
            // Convert the image from its native format to RGB
            img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                        (AVPicture*)pFrame, pCodecCtx->pix_fmt, 
                        pCodecCtx->width, 
                        pCodecCtx->height);
        }

        if (frameFinished) {
            FlexImage flex;
            flex.setPixelCode(VOCAB_PIXEL_RGB);
            flex.setQuantum((pFrameRGB->linesize[0]));
            flex.setExternal(pFrameRGB->data[0],
                             pCodecCtx->width,
                             pCodecCtx->height);
            image.copy(flex); 
        }

        return frameFinished;
    }
};


DecoderState videoDecoder;
DecoderState audioDecoder;


bool FfmpegGrabber::open(yarp::os::Searchable & config) {

    ConstString fname = 
        config.check("source",Value("default.avi")).asString();

    // Register all formats and codecs
    av_register_all();

    // Open video file
    if(av_open_input_file(&pFormatCtx, fname.c_str(), NULL, 0, NULL)!=0) {
        printf("Could not open avi file %s\n", fname.c_str());
        return false; // Couldn't open file
    }

    // Retrieve stream information
    if(av_find_stream_info(pFormatCtx)<0) {
        printf("Could not find stream information in %s\n", fname.c_str());
        return false; // Couldn't find stream information
    }

    // Dump information about file onto standard error
    dump_format(pFormatCtx, 0, fname.c_str(), false);

    // Find the first video stream
    int videoStream = videoDecoder.getStream(pFormatCtx,CODEC_TYPE_VIDEO,
                                             "video");
    // Find the first audio stream
    int audioStream = audioDecoder.getStream(pFormatCtx,CODEC_TYPE_AUDIO,
                                             "audio");

    if (videoStream==-1&&audioStream==-1) {
        return false;
    }

    bool ok = true;
    ok = ok && videoDecoder.getCodec(pFormatCtx);
    ok = ok && audioDecoder.getCodec(pFormatCtx);
    if (!ok) return false;
    ok = ok && videoDecoder.allocateImage();
    if (!ok) return false;

    m_w = videoDecoder.getWidth();
    m_h = videoDecoder.getHeight();
    printf("  avi dimension %dx%d\n", m_w, m_h);

    active = true;
    return true;
}
  
bool FfmpegGrabber::close() {
    if (!active) {
        return false;
    }
    
    // Close the video file
    if (pFormatCtx!=NULL) {
        av_close_input_file(pFormatCtx);
    }

    active = false;
    return true;
}
  
bool FfmpegGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) {
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        printf("frame ");
        bool done = false;
        if(packet.stream_index==videoDecoder.getIndex()) {
            printf("video ");
            done = videoDecoder.getVideo(packet,image);
        } else {
            printf("other ");
        }
        av_free_packet(&packet);
        printf("%d\n", done);
        if (done) {
            return true;
        }
    }
    image.resize(m_w,m_h);
    image.zero();
    return false;
}
  
bool FfmpegGrabber::getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                   yarp::sig::Sound& sound) {
    bool ok = getImage(image);
    sound.resize(100);
    return ok;
}


