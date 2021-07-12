/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ffmpegPortmonitor.cpp
 * @author Giulia Martino, Fabio Valla
 * @brief Implementation of FfmpegPortmonitor: a port monitor for video compression/decompression
 * @version 1.0
 * @date 2021-01-04
 */


// Local imports
#include "ffmpegPortmonitor.h"
#include "constants.h"
// YARP imports
#include <yarp/os/LogComponent.h>
#include <yarp/sig/all.h>
// Standard imports
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

// Ffmpeg imports
extern "C" {
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

// YARP logging component
YARP_LOG_COMPONENT(FFMPEGMONITOR,
                   "yarp.carrier.portmonitor.image_compression_ffmpeg",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

/**
 * @brief This function simply splits a string into a vector of strings basing on a delimiter character.
 * It it used for command line parameters parsing.
 *
 * @param s         The initial string.
 * @param delim     The delimiter character.
 * @param elements  The final vector of strings.
 */
void split(const std::string &s, char delim, vector<string> &elements) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        elements.push_back(item);
    }
}

bool FfmpegMonitorObject::create(const yarp::os::Property& options)
{
    // Check if this is sender or not
    senderSide = (options.find("sender_side").asBool());

    // Set default codec
    AVCodecID codecId = AV_CODEC_ID_MPEG2VIDEO;
    codecName = "mpeg2video";

    // Parse command line parameters and set them into global variable "paramsMap"
    std::string str = options.find("carrier").asString();
    if (getParamsFromCommandLine(str, codecId) == -1)
        return false;

    // Find encoder/decoder
    if (senderSide) {
        codec = avcodec_find_encoder(codecId);
    } else {
        codec = avcodec_find_decoder(codecId);
    }
    if (!codec) {
        yCError(FFMPEGMONITOR, "Can't find codec %s", codecName.c_str());
        return false;
    }

    // Prepare codec context
    if (codecContext == NULL) {
        codecContext = avcodec_alloc_context3(codec);
    } else {
        yCError(FFMPEGMONITOR, "Codec context is already allocated");
        return false;
    }
    if (!codecContext) {
        yCError(FFMPEGMONITOR, "Could not allocate video codec context");
        return false;
    }

    firstTime = true;

    // Set time base parameter
    codecContext->time_base.num = 1;
    codecContext->time_base.den = 15;
    // Set command line params
    if (setCommandLineParams() == -1)
        return false;

    return true;
}

void FfmpegMonitorObject::destroy(void)
{
    paramsMap.clear();

    // Check if codec context is freeable, if yes free it.
    if (codecContext != NULL) {
        avcodec_close(codecContext);
        avcodec_free_context(&codecContext);
        codecContext = NULL;
    }
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
        // If sender side
        yCTrace(FFMPEGMONITOR, "accept - sender");
        // Try to cast the Thing into an Image
        Image* img = thing.cast_as< Image >();
        // If cast fails, return error
        if(img == nullptr) {
            yCError(FFMPEGMONITOR, "Expected type Image in sender side, but got wrong data type!");
            return false;
        }
    }
    else {
        // If receiver side
        yCTrace(FFMPEGMONITOR, "accept - receiver");
        // Try to cast the Thing into a Bottle
        Bottle* bt = thing.cast_as<Bottle>();
        // If cast fails, return error
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
        bool success = true;
        yCTrace(FFMPEGMONITOR, "update - sender");
        // Cast Thing into an Image
        Image* img = thing.cast_as< Image >();
        // Allocate memory for packet
        AVPacket *packet = av_packet_alloc();
        if (packet == NULL) {
            yCError(FFMPEGMONITOR, "Error in packet allocation");
            success = false;
        }

        // Call compress function
        if (success && compress(img, packet) != 0) {
            yCError(FFMPEGMONITOR, "Error in compression");
            success = false;
        }

        // Insert compressed image into a Bottle to be sent
        data.clear();

        int successCode = success ? 1 : 0;
        data.addInt32(successCode);
        data.addInt32(img->width());
        data.addInt32(img->height());
        data.addInt32(img->getPixelCode());
        data.addInt32(img->getPixelSize());

        if (success) {  // If compression was successful, insert also compressed data
            // Packet
            Value p(packet, sizeof(*packet));
            data.add(p);
            // Packet data
            Value d(packet->data, packet->size);
            data.add(d);
            // Buffer size
            data.addInt32(packet->buf->size);
            // Buffer data
            Value bd(packet->buf->data, packet->buf->size);
            data.add(bd);

            // Side data elements
            for (int i = 0; i < packet->side_data_elems; i++) {
                data.addInt32(packet->side_data[i].size);
                data.addInt32(packet->side_data[i].type);
                Value sd(packet->side_data[i].data, packet->side_data[i].size);
                data.add(sd);
            }
        }
        th.setPortWriter(&data);
        // Free the memory allocated for the packet
        av_packet_unref(packet);
    }
    else {

        yCTrace(FFMPEGMONITOR, "update - receiver");
        // Cast the Thin as a Bottle
        Bottle* compressedBottle = thing.cast_as<Bottle>();
        // Fill the final image with zeros
        imageOut.zero();
        // Extract decompression data from the Bottle
        int width = compressedBottle->get(1).asInt32();
        int height = compressedBottle->get(2).asInt32();
        int pixelCode = compressedBottle->get(3).asInt32();
        int pixelSize = compressedBottle->get(4).asInt32();
        // Set information into final image
        imageOut.setPixelCode(pixelCode);
        imageOut.setPixelSize(pixelSize);
        imageOut.resize(width, height);

        // Check if compression was successful
        if (compressedBottle->get(0).asInt32() == 1) {
            bool success = true;
            // Get compressed image from Bottle
            AVPacket* tmp = (AVPacket*) compressedBottle->get(5).asBlob();
            // Allocate memory for packet
            AVPacket* packet = av_packet_alloc();
            // Set all packet parameters
            packet->dts = tmp->dts;
            packet->duration = tmp->duration;
            packet->flags = tmp->flags;
            packet->pos = tmp->pos;
            packet->pts = tmp->pts;
            packet->side_data_elems = 0;
            packet->stream_index = tmp->stream_index;
            packet->size = tmp->size;
            // Image data
            packet->data = (uint8_t *) compressedBottle->get(6).asBlob();
            // Buffer data
            packet->buf = av_buffer_create((uint8_t *) compressedBottle->get(8).asBlob(),
                                            compressedBottle->get(7).asInt32(), av_buffer_default_free,
                                            nullptr, AV_BUFFER_FLAG_READONLY);

            // Packet side data
            for (int i = 0; i < tmp->side_data_elems; i++) {

                int ret = av_packet_add_side_data(packet,
                                                    (AVPacketSideDataType) compressedBottle->get(10).asInt32(),     // Type
                                                    (uint8_t *) compressedBottle->get(11).asBlob(),                 // Data
                                                    compressedBottle->get(9).asInt32());                            // Size
                if (ret < 0) {
                    success = false;
                    break;
                }
            }

            // Call to decompress function
            if (success && decompress(packet, width, height, pixelCode) != 0) {
                yCError(FFMPEGMONITOR, "Error in decompression");
                success = false;
            }

            // Free memory allocated for side data and packet
            av_freep(&packet->side_data);
            av_freep(&packet);

        }
        th.setPortWriter(&imageOut);

    }
    return th;
}

int FfmpegMonitorObject::compress(Image* img, AVPacket *pkt) {

    yCTrace(FFMPEGMONITOR, "compress");
    AVFrame *startFrame;
    AVFrame *endFrame;

    // Get width and height
    int w = img->width();
    int h = img->height();

    // Allocate video frame for original frames
    startFrame = av_frame_alloc();
    if (startFrame == NULL) {
        yCError(FFMPEGMONITOR, "Cannot allocate starting frame!");
        return -1;
    }

    // Allocate a video frame for end frame
    endFrame = av_frame_alloc();
    if (endFrame == NULL) {
        yCError(FFMPEGMONITOR, "Cannot allocate end frame!");
        av_frame_free(&startFrame);
        return -1;
    }

    // Allocate space into start frame to contain data
    int success = av_image_alloc(startFrame->data, startFrame->linesize,
                    w, h,
                    (AVPixelFormat) FFMPEGPORTMONITOR_PIXELMAP[img->getPixelCode()], 16);

    if (success < 0) {
        yCError(FFMPEGMONITOR, "Cannot allocate starting frame buffer!");
        av_frame_free(&startFrame);
        av_frame_free(&endFrame);
        return -1;
    }

    // Set Image data into AVFrame
    startFrame->linesize[0] = img->getRowSize();
    // Free old pointer (because we will use the buffer contained into img)
    av_freep(&startFrame->data[0]);
    startFrame->data[0] = img->getRawImage();
    startFrame->height = h;
    startFrame->width = w;
    startFrame->format = (AVPixelFormat) FFMPEGPORTMONITOR_PIXELMAP[img->getPixelCode()];

    // Allocate memory for end frame data
    success = av_image_alloc(endFrame->data, endFrame->linesize,
                    w, h,
                    (AVPixelFormat) FFMPEGPORTMONITOR_CODECPIXELMAP[codecContext->codec_id], 16);

    if (success < 0) {
        yCError(FFMPEGMONITOR, "Cannot allocate end frame buffer!");
        av_frame_free(&startFrame);
        av_frame_free(&endFrame);
        return -1;
    }

    // Set end frame parameters
    endFrame->height = h;
    endFrame->width = w;
    endFrame->format = (AVPixelFormat) FFMPEGPORTMONITOR_CODECPIXELMAP[codecContext->codec_id];

    // Convert the image from start format into end format
    static struct SwsContext *img_convert_ctx;

    // Allocate context for conversion
    img_convert_ctx = sws_getContext(w, h,
                                        (AVPixelFormat) FFMPEGPORTMONITOR_PIXELMAP[img->getPixelCode()],
                                        w, h,
                                        (AVPixelFormat) FFMPEGPORTMONITOR_CODECPIXELMAP[codecContext->codec_id],
                                        SWS_BICUBIC,
                                        NULL, NULL, NULL);
    if (img_convert_ctx == NULL) {
        yCError(FFMPEGMONITOR, "Cannot initialize pixel format conversion context!");
        av_freep(&endFrame->data[0]);
        av_frame_free(&startFrame);
        av_frame_free(&endFrame);
        return -1;
    }

    // Perform conversion
    int ret = sws_scale(img_convert_ctx, startFrame->data, startFrame->linesize, 0,
                        h, endFrame->data, endFrame->linesize);

    if (ret < 0) {
        yCError(FFMPEGMONITOR, "Could not convert pixel format!");
        sws_freeContext(img_convert_ctx);
        av_freep(&endFrame->data[0]);
        av_frame_free(&startFrame);
        av_frame_free(&endFrame);
        return -1;
    }

    if (firstTime) {
        // If this is the first compression

        // Set codec context parameters
        codecContext->width = w;
        codecContext->height = h;
        codecContext->pix_fmt = (AVPixelFormat) FFMPEGPORTMONITOR_CODECPIXELMAP[codecContext->codec_id];

        // Open codec
        ret = avcodec_open2(codecContext, codec, NULL);
        if (ret < 0) {
            yCError(FFMPEGMONITOR, "Could not open codec");
            sws_freeContext(img_convert_ctx);
            av_freep(&endFrame->data[0]);
            av_frame_free(&startFrame);
            av_frame_free(&endFrame);
            return -1;
        }
        firstTime = false;
    }

    // Set presentation timestamp
    endFrame->pts = codecContext->frame_number;

    // Send image frame to codec
    ret = avcodec_send_frame(codecContext, endFrame);
    if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error sending a frame for encoding");
        sws_freeContext(img_convert_ctx);
        av_freep(&endFrame->data[0]);
        av_frame_free(&startFrame);
        av_frame_free(&endFrame);
        return -1;
    }

    // Receive compressed data into packet
    ret = avcodec_receive_packet(codecContext, pkt);
    sws_freeContext(img_convert_ctx);
    av_freep(&endFrame->data[0]);
    av_frame_free(&startFrame);
    av_frame_free(&endFrame);

    if (ret == AVERROR(EAGAIN)) {
        // Not enough data
        yCError(FFMPEGMONITOR, "Error EAGAIN");
        return -1;
    } else if (ret == AVERROR_EOF) {
        // End of file reached
        yCError(FFMPEGMONITOR, "Error EOF");
        return -1;
    } else if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error during encoding");
        return -1;
    }

    return 0;
}

int FfmpegMonitorObject::decompress(AVPacket* pkt, int w, int h, int pixelCode) {

    yCTrace(FFMPEGMONITOR, "decompress");
    AVFrame *startFrame;
    AVFrame *endFrame;

    if (firstTime) {
        // If this is the first decompression

        // Set codec context parameters
        codecContext->width = w;
        codecContext->height = h;
        codecContext->pix_fmt = (AVPixelFormat) FFMPEGPORTMONITOR_CODECPIXELMAP[codecContext->codec_id];

        // Open codec
        int ret = avcodec_open2(codecContext, codec, NULL);
        if (ret < 0) {
            yCError(FFMPEGMONITOR, "Could not open codec");
            return -1;
        }
        firstTime = false;
    }

    // Allocate video frame
    startFrame = av_frame_alloc();
    if (startFrame == NULL) {
        yCError(FFMPEGMONITOR, "Could not allocate start frame!");
        return -1;
    }

    // Send compressed packet to codec
    int ret = avcodec_send_packet(codecContext, pkt);
    if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error sending a frame for encoding");
        av_frame_free(&startFrame);
        return -1;
    }

    // Receive decompressed image into an AVFrame
    ret = avcodec_receive_frame(codecContext, startFrame);
    if (ret == AVERROR(EAGAIN)) {
        // No enough data
        yCError(FFMPEGMONITOR, "Error EAGAIN");
        av_frame_free(&startFrame);
        return -1;
    }
    else if (ret == AVERROR_EOF) {
        // End of file reached
        yCError(FFMPEGMONITOR, "Error EOF");
        av_frame_free(&startFrame);
        return -1;
    }
    else if (ret < 0) {
        yCError(FFMPEGMONITOR, "Error during encoding");
        av_frame_free(&startFrame);
        return -1;
    }

    // Allocate a video frame for end frame
    endFrame = av_frame_alloc();
    if (endFrame == NULL) {
        yCError(FFMPEGMONITOR, "Could not allocate start frame!");
        av_frame_free(&startFrame);
        return -1;
    }

    // Allocate memory into end frame to contain data
    int success = av_image_alloc(endFrame->data, endFrame->linesize,
                    w, h,
                    (AVPixelFormat) FFMPEGPORTMONITOR_PIXELMAP[pixelCode], 16);

    if (success < 0) {
        yCError(FFMPEGMONITOR, "Error allocating end frame buffer!");
        av_frame_free(&startFrame);
        av_frame_free(&endFrame);
        return -1;
    }

    // Set end frame parameters
    endFrame->height = h;
    endFrame->width = w;
    endFrame->format = (AVPixelFormat) FFMPEGPORTMONITOR_PIXELMAP[pixelCode];

    // Convert the image into RGB format
    static struct SwsContext *img_convert_ctx;

    // Allocate conversion context
    img_convert_ctx = sws_getContext(w, h,
                                        (AVPixelFormat) FFMPEGPORTMONITOR_CODECPIXELMAP[codecContext->codec_id],
                                        w, h,
                                        (AVPixelFormat) FFMPEGPORTMONITOR_PIXELMAP[pixelCode],
                                        SWS_BICUBIC,
                                        NULL, NULL, NULL);
    if (img_convert_ctx == NULL) {
        yCError(FFMPEGMONITOR, "Cannot initialize the pixel format conversion context!");
        av_freep(&endFrame->data[0]);
        av_frame_free(&endFrame);
        av_frame_free(&startFrame);
        return -1;
    }

    // Perform conversion
    ret = sws_scale(img_convert_ctx, startFrame->data, startFrame->linesize, 0,
                        h, endFrame->data, endFrame->linesize);

    if (ret < 0) {
        yCError(FFMPEGMONITOR, "Could not convert pixel format!");
        av_freep(&endFrame->data[0]);
        av_frame_free(&endFrame);
        av_frame_free(&startFrame);
        sws_freeContext(img_convert_ctx);
        return -1;
    }

    // Copy decompressed data from end frame to imageOut
    memcpy(imageOut.getRawImage(), endFrame->data[0], success);

    // Free allocated memory
    av_freep(&endFrame->data[0]);
    av_frame_free(&endFrame);
    av_frame_free(&startFrame);
    sws_freeContext(img_convert_ctx);

    return 0;

}

int FfmpegMonitorObject::getParamsFromCommandLine(string carrierString, AVCodecID &codecId) {

    vector<string> parameters;
    // Split command line string using '+' delimiter
    split(carrierString, '+', parameters);

    // Iterate over result strings
    for (string param: parameters) {

        // Skip YARP initial parameters
        if (find(FFMPEGPORTMONITOR_IGNORE_PARAMS.begin(), FFMPEGPORTMONITOR_IGNORE_PARAMS.end(), param) != FFMPEGPORTMONITOR_IGNORE_PARAMS.end()) {
            continue;
        }

        // If there is no '.', the param is bad formatted, return error
        auto pointPosition = param.find('.');
        if (pointPosition == string::npos) {
            yCError(FFMPEGMONITOR, "Error parsing parameters!");
            return -1;
        }

        // Otherwise, separate key and value
        string paramKey = param.substr(0, pointPosition);
        string paramValue = param.substr(pointPosition + 1, param.length());

        // Parsing codec
        if (paramKey == FFMPEGPORTMONITOR_CL_CODEC_KEY) {
            bool found = false;
            // Iterate over codecs command line possibilities
            for (size_t i = 0; i < FFMPEGPORTMONITOR_CL_CODECS.size(); i++) {
                // If found
                if (paramValue == FFMPEGPORTMONITOR_CL_CODECS[i]) {
                    // Set codec id basing on codec command line name
                    codecId = (AVCodecID) FFMPEGPORTMONITOR_CODE_CODECS[i];
                    codecName = paramValue;
                    found = true;
                    break;
                }
            }

            // If not found, unrecognized codec, return error
            if (!found) {
                yCError(FFMPEGMONITOR, "Unrecognized codec: %s", paramValue.c_str());
                return -1;
            } else {
                continue;
            }

        }

        // Save param into params map
        paramsMap.insert( pair<string, string>(paramKey, paramValue) );
    }
    return 0;

}

int FfmpegMonitorObject::setCommandLineParams() {

    // Iterate over all saved parameters
    for (auto const& x : paramsMap) {

        // Get key and value
        string key = x.first;
        string value = x.second;

        // Try to set this pair (key, value) into codec context (global parameters).
        int globalError = av_opt_set(codecContext, key.c_str(), value.c_str(), 0);
        // Try to set this pair (key, value) into codec context -> priv data (parameters that are specific for a codec).
        int privError = av_opt_set(codecContext->priv_data, key.c_str(), value.c_str(), 0);

        // If the param exists, but the value is out of range
        if (globalError == AVERROR(ERANGE) || privError == AVERROR(ERANGE)) {
            yCError(FFMPEGMONITOR, "Parameter out of range: %s", key.c_str());
            return -1;
        }
        // If the param exists, but the value is invalid
        else if (globalError == AVERROR(EINVAL) || privError == AVERROR(EINVAL)) {
            yCError(FFMPEGMONITOR, "Invalid value for parameter: %s", key.c_str());
            return -1;
        }
        // If the param doesn't exists (we check only in sender side because some parameters doesn't exist in the decoders)
        else if (senderSide && globalError == AVERROR_OPTION_NOT_FOUND && privError == AVERROR_OPTION_NOT_FOUND) {
            yCError(FFMPEGMONITOR, "Parameter not found: %s", key.c_str());
            return -1;
        }

    }
    return 0;
}
