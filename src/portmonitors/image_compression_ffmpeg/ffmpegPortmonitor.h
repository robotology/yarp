/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ffmpegPortmonitor.h
 * @author Giulia Martino, Fabio Valla
 * @brief Header file of FfmpegPortmonitor: a port monitor for video compression/decompression
 * @version 1.0
 * @date 2021-01-04
 */

#ifndef YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_H
#define YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_H

// YARP imports
#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/sig/Image.h>
#include <yarp/os/MonitorObject.h>

// Ffmpeg imports
extern "C" {
    #include <libavcodec/avcodec.h>
}

/**
 * @brief This class extends the MonitorObject class and uses Ffmpeg to compress and decompress video streams with a specified codec.
 *
 */
class FfmpegMonitorObject : public yarp::os::MonitorObject
{
    public:
        /**
         * @brief This function is called when the object is created and it is used to initialize all its attributes. It is called both on sender and receiver side, and it performs different initializations basing on side check.
         *
         * @param options Parameters passed to the object to initialize it (e.g. command line parameters).
         * @return true   If the creation was successful and all needed variables are correctly initialized.
         * @return false  Otherwise.
         */
        bool create(const yarp::os::Property& options) override;

        /**
         * @brief This function is called when the execution is terminated and the object is destroyed. It frees the memory allocated during the "create" function.
         *
         */
        void destroy(void) override;

        bool setparam(const yarp::os::Property& params) override;
        bool getparam(yarp::os::Property& params) override;

        /**
         * @brief This function is used by the port monitor to decide if an incoming packet can be accepted (it tries to cast it to the right type). The accepted packets are then handled by the function "update"; the others are discarded.
         *
         * @param thing The incoming packet; must be a yarp::sig::Image in sender side, must be a yarp::sig::Bottle in receiver side.
         * @return true If the packet was successfully cast.
         * @return false Otherwise.
         */
        bool accept(yarp::os::Things& thing) override;

        /**
         * @brief This function is the one that manipulates the incoming packet. In sender side, it takes the Image and sends it to the "compress" function. Then it fills a Bottle containing the compressed Image and all the information needed for decompression. In receiver side, it takes the incoming Bottle and unpacks it; then it sends all compressed data to the "decompress" function; it finally creates a new Image object filled with decompressed data.
         *
         * @param thing The incoming packet; it is a yarp::sig::Image in sender side, a yarp::sig::Bottle in receiver side.
         * @return yarp::os::Things& The newly created object; it is a yarp::sig::Bottle in sender side (containing compressed data to be sent to receiver); it is a yarp::sig::FlexImage in receiver side (to be sent to the original destination).
         */
        yarp::os::Things& update(yarp::os::Things& thing) override;

    protected:
        /**
         * @brief This function performs all the compression actions on the incoming Image and saves the resulting compressed data into the AVPacket passed as parameter.
         *
         * @param img The incoming image.
         * @param pkt The packet containing all the compressed data.
         * @return int 0 on success, -1 otherwise.
         */
        int compress(yarp::sig::Image* img, AVPacket* pkt);

        /**
         * @brief This function decompresses the incoming AVPacket passed as parameter and saves decompressed data into the attribute imageOut (yarp::sig::FlexImage).
         *
         * @param pkt The incoming packet containing all the compressed data.
         * @param w   The width of the image (in pixels).
         * @param h   The height of the image (in pixels).
         * @param pixelCode The YARP pixel format code of the image.
         * @return int 0 on success, -1 otherwise.
         */
        int decompress(AVPacket* pkt, int w, int h, int pixelCode);

        /**
         * @brief This function parses the command line parameters from a string containing the entire command used to execute the program and saves the parameters into the attribute paramsMap (std::map<std::string, std::string>).
         *
         * @param carrierString The entire command used to execute the program.
         * @param codecId       The codec for video compression / decompression.
         * @return int          0 on success, -1 otherwise.
         */
        int getParamsFromCommandLine(std::string carrierString, AVCodecID &codecId);

        /**
         * @brief This function iterates over the attribute paramsMap and sets all the specified parameters into the attribute codecContext.
         *
         * @return int  0 on success, -1 otherwise.
         */
        int setCommandLineParams();

    public:
        /**
         * @brief The object returned by the "update" function; it can be a yarp::os::Bottle (sender side) or a yarp::sig::Image (receiver side).
         *
         */
        yarp::os::Things th;

        /**
         * @brief The bottle that is filled with compressed image and all the information needed for decompression (it will be sent from sender to receiver).
         *
         */
        yarp::os::Bottle data;

        /**
         * @brief The final decompressed image that will be sent to the original destination.
         *
         */
        yarp::sig::FlexImage imageOut;

        /**
         * @brief Boolean variable that tells if the current execution is in sender side or not.
         *
         */
        bool senderSide;

        /**
         * @brief The string containing codec name.
         *
         */
        std::string codecName;

        /**
         * @brief Ffmpeg structure containing all codec information needed for compression / decompression.
         *
         */
        const AVCodec *codec;

        /**
         * @brief Ffmpeg structure containing all codec context information needed for compression / decompression.
         *
         */
        AVCodecContext *codecContext = NULL;

        /**
         * @brief Boolean variable used to check if the current call to the "compression" (or "decompression") function is the first one or not.
         *
         */
        bool firstTime;

        /**
         * @brief Structure that maps every parameter inserted from command line into its value (both as strings).
         *
         */
        std::map<std::string, std::string> paramsMap;
};

#endif
