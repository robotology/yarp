/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_GSTREAMERDECODER_H
#define YARP_GSTREAMERDECODER_H

#include <mutex>
#include <yarp/sig/Image.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/LogComponent.h>

#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>

struct GstYarpDecoder_cfgParamters
{
    GstYarpDecoder_cfgParamters() :
        remotePort(-1)
    {}

    int remotePort; // the port on which the server send data
};

//---------------------------------------------------------------------------------------------
struct data_for_gst_callback
{
    data_for_gst_callback() = default;

    std::mutex* mutex_pointer = nullptr;
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img_pointer = nullptr;
    yarp::os::Semaphore* sem_pointer_gst = nullptr;
    yarp::os::Semaphore* sem_pointer_stream = nullptr;
};

//---------------------------------------------------------------------------------------------
class GstYarpDecoder
{
private:
    GstElement*                              m_pipeline = nullptr;

    data_for_gst_callback                    m_gst_cbk_data;

    yarp::sig::ImageOf<yarp::sig::PixelRgb>* m_pointer_frame = nullptr;
    std::mutex*                              m_pointer_mutex = nullptr;
    yarp::os::Semaphore                      m_semaphore_reading_from_gst = 0;
    yarp::os::Semaphore                      m_semaphore_reading_from_stream;


    GstYarpDecoder_cfgParamters              m_cfg;

public:
    GstYarpDecoder(std::mutex* m, yarp::os::Semaphore* s, GstYarpDecoder_cfgParamters& config);
    ~GstYarpDecoder();
    bool init(std::string pipeline_string);
    bool start();
    bool stop();
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* getLastFrame();
    void prepareNextFrame();
};

#endif
