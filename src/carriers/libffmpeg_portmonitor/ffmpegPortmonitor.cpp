#include "ffmpegPortmonitor.h"

#include <yarp/os/LogComponent.h>
#include <yarp/sig/all.h>

#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

extern "C" {
    #include <libavcodec/codec.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

namespace {
YARP_LOG_COMPONENT(FFMPEGMONITOR,
                   "yarp.carrier.portmonitor.ffmpeg",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


bool FfmpegMonitorObject::create(const yarp::os::Property& options)
{   
    // Check if this is sender or not
    senderSide = (options.find("sender_side").asBool());


    // Parse command line parameters and set them into global variable "paramsMap"
    std::string str = options.find("carrier").asString();
    getParamsFromCommandLine(str);

    // Set codec name if specified in command line params
    if (paramsMap.find("codec") != paramsMap.end()) {
        codecName = paramsMap["codec"].c_str();
    } else {
        codecName = "mpeg2video";   // Default codec
    }
    
    // Find encoder/decoder
    if (senderSide) {
        codec = avcodec_find_encoder_by_name(codecName);
    } else {
        codec = avcodec_find_decoder_by_name(codecName);
    }
    if (!codec) {
        yCError(FFMPEGMONITOR, "Can't find codec %s", codecName);
        return false;
    }

    // Prepare codec context
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        yCError(FFMPEGMONITOR, "Could not allocate video codec context");
        return false;
    }

    firstTime = true;

    // Set command line params
    if (setCommandLineParams() == -1)
        return false;
    
    // Pixel formats map
    pixelMap[VOCAB_PIXEL_RGB] = AV_PIX_FMT_RGB24;
    pixelMap[VOCAB_PIXEL_RGBA] = AV_PIX_FMT_RGBA;
    pixelMap[VOCAB_PIXEL_BGR] = AV_PIX_FMT_BGR24;
    pixelMap[VOCAB_PIXEL_BGRA] = AV_PIX_FMT_BGRA;
    pixelMap[VOCAB_PIXEL_YUV_420] = AV_PIX_FMT_YUV420P;

    codecPixelMap[AV_CODEC_ID_H264] = AV_PIX_FMT_YUV420P;
    codecPixelMap[AV_CODEC_ID_H265] = AV_PIX_FMT_YUV420P;
    codecPixelMap[AV_CODEC_ID_MPEG2VIDEO] = AV_PIX_FMT_YUV420P;

    return true;
}

void FfmpegMonitorObject::destroy(void)
{
    paramsMap.clear();
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);

}

bool FfmpegMonitorObject::setparam(const yarp::os::Property& params)
{
    yCTrace(FFMPEGMONITOR, "setparam");
    return false;
}

bool FfmpegMonitorObject::getparam(yarp::os::Property& params)
{
    yCTrace(FFMPEGMONITOR, "getparam");
    return false;
}

bool FfmpegMonitorObject::accept(yarp::os::Things& thing)
{
    if (senderSide) {
        yCTrace(FFMPEGMONITOR, "accept - sender");
        Image* img = thing.cast_as< Image >();
        if(img == nullptr) {
            yCError(FFMPEGMONITOR, "Expected type Image in sender side, but got wrong data type!");
            return false;
        }        
    }
    else {
        yCTrace(FFMPEGMONITOR, "accept - receiver");
        Bottle* bt = thing.cast_as<Bottle>();
        if(bt == nullptr){
            yCError(FFMPEGMONITOR, "Expected type Bottle in receiver side, but got wrong data type!");
            return false;
        }
    }
    return true;
}

yarp::os::Things& FfmpegMonitorObject::update(yarp::os::Things& thing)
{
    if (senderSide) {
        yCTrace(FFMPEGMONITOR, "update - sender");
        Image* img = thing.cast_as< Image >();
        AVPacket *packet = new AVPacket;
        bool success = true;
        if (compress(img, packet) != 0) {
            yCError(FFMPEGMONITOR, "Error in compression");
            success = false;
        }

        // Insert compressed image into a Bottle to be sent
        data.clear();
        if (!success) {
            data.addInt32(0);
        } else {
            data.addInt32(1);
            data.addInt32(img->width());
            data.addInt32(img->height());
            data.addInt32(img->getPixelCode());
            data.addInt32(img->getPixelSize());  
            Value p(packet, sizeof(*packet));
            data.add(p);
            Value d(packet->data, packet->size);
            data.add(d);
            data.addInt32(packet->buf->size);
            Value bd(packet->buf->data, packet->buf->size);
            data.add(bd);
            if (packet->side_data_elems > 0) {
                data.addInt32(packet->side_data->size);
                data.addInt32(packet->side_data->type);
                Value sd(packet->side_data->data, packet->side_data->size);
                data.add(sd);
            }
        }
        th.setPortWriter(&data);
        // av_free_packet(packet);
        // delete packet;
    }
    else {
        yCTrace(FFMPEGMONITOR, "update - receiver");
        Bottle* compressedBottle = thing.cast_as<Bottle>();

        int success = compressedBottle->get(0).asInt32();
        if (success == 0) {
            imageOut.zero();
        } else {
            // Get compressed image from Bottle and decompress
            int width = compressedBottle->get(1).asInt32();
            int height = compressedBottle->get(2).asInt32();
            int pixelCode = compressedBottle->get(3).asInt32();
            int pixelSize = compressedBottle->get(4).asInt32();
            AVPacket* tmp = (AVPacket*) compressedBottle->get(5).asBlob();
            AVPacket* packet = av_packet_alloc();
            packet->dts = tmp->dts;
            packet->duration = tmp->duration;
            packet->flags = tmp->flags;
            packet->pos = tmp->pos;
            packet->pts = tmp->pts;
            packet->side_data_elems = tmp->side_data_elems;
            packet->stream_index = tmp->stream_index;
            packet->size = tmp->size;
            packet->data = (uint8_t *) compressedBottle->get(6).asBlob();
            packet->buf = av_buffer_create((uint8_t *) compressedBottle->get(8).asBlob(),
                                            compressedBottle->get(7).asInt32(), av_buffer_default_free,
                                            nullptr, AV_BUFFER_FLAG_READONLY);
                        
            if (packet->side_data_elems > 0) {
                packet->side_data = new AVPacketSideData;
                packet->side_data->size = compressedBottle->get(9).asInt32();
                packet->side_data->type = (AVPacketSideDataType) compressedBottle->get(10).asInt32();
                packet->side_data->data = (uint8_t *) compressedBottle->get(11).asBlob();
            }
                        
            unsigned char* decompressed;
            int sizeDecompressed;
            bool success = true;
            if (decompress(packet, &decompressed, &sizeDecompressed, width, height, pixelCode) != 0) {
                yCError(FFMPEGMONITOR, "Error in decompression");
                success = false;
            }

            imageOut.zero();
            if (success) {
                imageOut.setPixelCode(pixelCode);
                imageOut.setPixelSize(pixelSize);
                imageOut.resize(width, height);
                memcpy(imageOut.getRawImage(), decompressed, sizeDecompressed);
            }

            // if (packet->side_data_elems > 0)
            //     delete packet->side_data;
            // if (success)
            //     av_packet_free(&packet);
        }
        th.setPortWriter(&imageOut);
        
    }
    return th;
}

int FfmpegMonitorObject::compress(Image* img, AVPacket *pkt) {
    
    yCTrace(FFMPEGMONITOR, "compress");
    AVFrame *startFrame;
    AVFrame *endFrame;

    int w = img->width();
    int h = img->height();

    // Allocate video frame for original frames
    startFrame = av_frame_alloc();
    if (startFrame == NULL)
        return -1;

    // Allocate an video frame for end frame
    endFrame = av_frame_alloc();
    if (endFrame == NULL)
        return -1;

    int success = av_image_alloc(startFrame->data, startFrame->linesize,
                    w, h,
                    (AVPixelFormat) pixelMap[img->getPixelCode()], 16);

    if (success < 0)
        return -1;

    startFrame->linesize[0] = img->getRowSize();
    startFrame->data[0] = img->getRawImage();
    startFrame->height = h;
    startFrame->width = w;
    startFrame->format = (AVPixelFormat) pixelMap[img->getPixelCode()];

    success = av_image_alloc(endFrame->data, endFrame->linesize,
                    w, h,
                    (AVPixelFormat) codecPixelMap[codecContext->codec_id], 16);
    
    endFrame->height = h;
    endFrame->width = w;
    endFrame->format = (AVPixelFormat) codecPixelMap[codecContext->codec_id];

    // Convert the image into end format
    static struct SwsContext *img_convert_ctx;
    
    img_convert_ctx = sws_getContext(w, h, 
                                        (AVPixelFormat) pixelMap[img->getPixelCode()], 
                                        w, h,
                                        (AVPixelFormat) codecPixelMap[codecContext->codec_id],
                                        SWS_BICUBIC,
                                        NULL, NULL, NULL);
    if (img_convert_ctx == NULL) {
        yCError(FFMPEGMONITOR, "Cannot initialize the pixel format conversion context!");
        return -1;
    }
    
    int ret = sws_scale(img_convert_ctx, startFrame->data, startFrame->linesize, 0, 
                        h, endFrame->data, endFrame->linesize);

    pkt->data = NULL;
    pkt->size = 0;
    av_init_packet(pkt);
    if (!pkt) {
        yCError(FFMPEGMONITOR, "Could not allocate packet");
        return -1;
    }

    if (firstTime) {
        codecContext->width = w;
        codecContext->height = h;
        codecContext->pix_fmt = (AVPixelFormat) codecPixelMap[codecContext->codec_id];

        ret = avcodec_open2(codecContext, codec, NULL);
        if (ret < 0) {
            yCError(FFMPEGMONITOR, "Could not open codec");
            return -1;
        }
        firstTime = false;
    }

    startFrame->pts = codecContext->frame_number;

    ret = avcodec_send_frame(codecContext, endFrame);
    if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error sending a frame for encoding");
        return -1;
    }

    ret = avcodec_receive_packet(codecContext, pkt);
    if (ret == AVERROR(EAGAIN)) {
        yCError(FFMPEGMONITOR, "Error EAGAIN");
        return -1;
    }
    else if (ret == AVERROR_EOF) {
        yCError(FFMPEGMONITOR, "Error EOF");
        return -1;
    }
    else if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error during encoding");
        return -1;
    }

    // sws_freeContext(img_convert_ctx);
    // av_free(endFrame->data);
    // av_free(startFrame->data);
    // av_frame_free(&startFrame);
    // av_frame_free(&endFrame);

    return 0;
}

int FfmpegMonitorObject::decompress(AVPacket* pkt, unsigned char** decompressed, int* sizeDecompressed,
                                    int w, int h, int pixelCode) {
    
    yCTrace(FFMPEGMONITOR, "decompress");
    AVFrame *startFrame;
    AVFrame *endFrame;

    if (firstTime) {
        codecContext->width = w;
        codecContext->height = h;
        codecContext->pix_fmt = (AVPixelFormat) codecPixelMap[codecContext->codec_id];

        int ret = avcodec_open2(codecContext, codec, NULL);
        if (ret < 0) {
            yCError(FFMPEGMONITOR, "Could not open codec");
            return -1;
        }
        firstTime = false;
    }

    // Allocate video frame
    startFrame = av_frame_alloc();
    if (startFrame == NULL)
        return -1;

    int ret = avcodec_send_packet(codecContext, pkt);
    if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error sending a frame for encoding");
        return -1;
    }

    ret = avcodec_receive_frame(codecContext, startFrame);
    if (ret == AVERROR(EAGAIN)) {
        yCError(FFMPEGMONITOR, "Error EAGAIN");
        return -1;
    }
    else if (ret == AVERROR_EOF) {
        yCError(FFMPEGMONITOR, "Error EOF");
        return -1;
    }
    else if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error during encoding");
        return -1;
    }
    
    // Allocate a video frame for end frame
    endFrame = av_frame_alloc();
    if (endFrame == NULL)
        return -1;


    int success = av_image_alloc(endFrame->data, endFrame->linesize,
                    w, h,
                    (AVPixelFormat) pixelMap[pixelCode], 16);
    
    if (success < 0) {
        yCError(FFMPEGMONITOR, "Error allocating frame!");
        return -1;
    }

    endFrame->height = h;
    endFrame->width = w;
    endFrame->format = (AVPixelFormat) pixelMap[pixelCode];

    // Convert the image into RGB format
    static struct SwsContext *img_convert_ctx;
    
    img_convert_ctx = sws_getContext(w, h, 
                                        (AVPixelFormat) codecPixelMap[codecContext->codec_id], 
                                        w, h,
                                        (AVPixelFormat) pixelMap[pixelCode],
                                        SWS_BICUBIC,
                                        NULL, NULL, NULL);
    if (img_convert_ctx == NULL) {
        yCError(FFMPEGMONITOR, "Cannot initialize the pixel format conversion context!");
        return -1;
    }
    
    ret = sws_scale(img_convert_ctx, startFrame->data, startFrame->linesize, 0, 
                        h, endFrame->data, endFrame->linesize);


    *decompressed = endFrame->data[0];
    *sizeDecompressed = success;

    // sws_freeContext(img_convert_ctx);
    // av_free(startFrame->data);
    // av_free(endFrame->data);
    // av_frame_free(&endFrame);
    // av_frame_free(&startFrame);


    return 0;

}

void FfmpegMonitorObject::getParamsFromCommandLine(string carrierString) {
    
    string paramsList[] = { "codec",
                            "qmin",
                            "qmax",
                            "bit_rate",
                            "time_base",
                            "gop_size",
                            "max_b_frames",
                            "framerate" };
    
    for (string paramKey : paramsList) {
        int startPosition = carrierString.find(paramKey);
        if (startPosition != -1) {
            string paramValue = carrierString.substr(startPosition + paramKey.length() + 1);
            
            int endPosition = paramValue.find('+');
            if (endPosition != -1) {
                paramValue = paramValue.substr(0, endPosition);
            }
            if (paramValue != "") {
                paramsMap.insert( pair<string, string>(paramKey, paramValue) );
            }
        }
    }
}

int FfmpegMonitorObject::setCommandLineParams() {
        
    try {
        // Qmin
        if (paramsMap.find("qmin") != paramsMap.end()) {
            codecContext->qmin = stoi(paramsMap["qmin"]);

        } else {
            if (codecName == "h264") {
                codecContext->qmin = 18;
            } else if (codecName == "h265") {
                codecContext->qmin = 24;
            } else if (codecName == "mpeg2video") {
                codecContext->qmin = 3;
            }
        }

        // Qmax
        if (paramsMap.find("qmax") != paramsMap.end()){
            codecContext->qmax = stoi(paramsMap["qmax"]);

        } else {
            if (codecName == "h264") {
                codecContext->qmax = 28;
            } else if (codecName == "h265") {
                codecContext->qmax = 34;
            } else if (codecName == "mpeg2video") {
                codecContext->qmax = 5;
            }
        }

        // Bit rate
        if (paramsMap.find("bit_rate") != paramsMap.end()) {
            codecContext->bit_rate = stoi(paramsMap["bit_rate"]);

        } else {
            codecContext->bit_rate = 400000; 
        }

        // Gop size
        if (paramsMap.find("gop_size") != paramsMap.end()) {
            codecContext->gop_size = stoi(paramsMap["gop_size"]);

        } else {
            codecContext->gop_size = 10;
        }

        // Max b frames
        if (paramsMap.find("max_b_frames") != paramsMap.end()) {
            codecContext->max_b_frames = stoi(paramsMap["max_b_frames"]);

        } else {
            codecContext->max_b_frames = 1;
        }

        // Framerate & time_base
        if (paramsMap.find("framerate") != paramsMap.end()) {
            int framerate = stoi(paramsMap["framerate"]);
            codecContext->framerate = (AVRational){ framerate , 1 };
            codecContext->time_base = (AVRational){ 1 , framerate };

        } else {
            codecContext->time_base = (AVRational){1, 30};
            codecContext->framerate = (AVRational){30, 1};
        }        
        return 0;

    } catch (invalid_argument e) {
        return -1;
    } catch (out_of_range e) {
        return -1;
    }
    
}