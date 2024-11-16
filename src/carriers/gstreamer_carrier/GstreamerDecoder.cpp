/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "GstreamerDecoder.h"

#include <yarp/os/LogStream.h>


#include <gst/gst.h>
#include <glib.h>

#include <cstdio>
#include <cstring>
#include <mutex>

//#define debug_time 1

#ifdef debug_time
    #include <yarp/os/Time.h>
    #define DBG_TIME_PERIOD_PRINTS 10 //10 sec
#endif

using namespace yarp::sig;
using namespace yarp::os;

YARP_LOG_COMPONENT(GSTREAMER_DECODER,
                   "yarp.carrier.gstreamer.GstDecoder",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

//-------------------------------------------------------------------
//---------------  CALLBACK FUNCTIONS -------------------------------
//-------------------------------------------------------------------

GstFlowReturn new_sample_func(GstAppSink *appsink, gpointer user_data)
{
    static yarp::sig::ImageOf<yarp::sig::PixelRgb> curr_frame;
#ifdef debug_time
    static bool isFirst = true;
    double start_time = Time::now();
    double end_time=0;

    static double last_call;
    static double sumOf_timeBetweenCalls = 0;
    static double sumOf_timeOfNewSampleFunc = 0;
    static uint32_t count=0;
    #define MAX_COUNT  100


    if(!isFirst)
        sumOf_timeBetweenCalls+=(start_time -last_call);

    last_call = start_time;
#endif

    data_for_gst_callback* dec_data = (data_for_gst_callback*)user_data;
    dec_data->img_pointer = &curr_frame;
    dec_data->sem_pointer_stream->wait();

    GstSample* sample = gst_app_sink_pull_sample(appsink);
    if (!sample)
    {
        yCWarning(GSTREAMER_DECODER, "could not take a sample!");
        return GST_FLOW_OK;
    }

    GstCaps *caps = gst_sample_get_caps (sample);
    if(!caps)
    {
        yCError(GSTREAMER_DECODER, "could not get caps of sample!");
        return GST_FLOW_OK;
    }
    GstStructure *struc = gst_caps_get_structure(caps, 0);
    if(!struc)
    {
        yCError(GSTREAMER_DECODER, "could not get struct of caps!");
        return GST_FLOW_OK;
    }
    gint width, height;
    gboolean res;
    res = gst_structure_get_int(struc, "width", &width);
    if(!res)
    {
        yCError(GSTREAMER_DECODER, "could not get width!");
        return GST_FLOW_ERROR;
    }

    res = gst_structure_get_int(struc, "height", &height);
    if(!res)
    {
        yCError(GSTREAMER_DECODER, "GSTREAMER: could not get height!");
        return GST_FLOW_ERROR;
    }
    yCTrace(GSTREAMER_DECODER, "Image has size %d x %d", width, height);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if(!gst_buffer_map(buffer, &map, GST_MAP_READ))
    {
        yCError(GSTREAMER_DECODER, "could not get map!");
        return GST_FLOW_ERROR;
    }

    dec_data->mutex_pointer->lock();
    curr_frame.resize(width, height);
    size_t map_size = map.size;
    size_t img_size = width * height * 3;
    size_t img_size2 = curr_frame.getRawImageSize();
    unsigned char* ydata_ptr = curr_frame.getRawImage();
    memcpy(ydata_ptr, map.data, map_size);

    dec_data->mutex_pointer->unlock();
    gst_buffer_unmap(buffer, &map);

    gst_sample_unref(sample);
    dec_data->sem_pointer_gst->post();

#ifdef debug_time
    end_time = Time::now();
    sumOf_timeOfNewSampleFunc += (end_time-start_time);
    count++;
    isFirst=false;

    if(count>=MAX_COUNT)
    {
        yCDebug(H264CARRIER,
                "On %d times: NewSampleFunc is long %.6f sec and sleeps %.6f sec",
                MAX_COUNT,
                (sumOf_timeOfNewSampleFunc/MAX_COUNT),
                (sumOf_timeBetweenCalls/MAX_COUNT) );
        count = 0;
        isFirst = true;
        sumOf_timeBetweenCalls = 0;
        sumOf_timeOfNewSampleFunc = 0;
    }
#endif

    return GST_FLOW_OK;
}




//----------------------------------------------------------------------

GstYarpDecoder::GstYarpDecoder(std::mutex* _m, yarp::os::Semaphore* _s, GstYarpDecoder_cfgParamters& config)
{
    m_pointer_mutex = _m;
    m_gst_cbk_data.mutex_pointer = m_pointer_mutex;
    m_gst_cbk_data.sem_pointer_gst     = &m_semaphore_reading_from_gst;
    m_gst_cbk_data.sem_pointer_stream =  &m_semaphore_reading_from_stream;
}

bool GstYarpDecoder::init(std::string pipeline_string)
{
    gst_init(nullptr, nullptr);

    // Create the pipeline and add an appsink to it
      pipeline_string += " ! videoconvert ! video/x-raw,format=RGB ! appsink name = myappsink ";

    yCInfo(GSTREAMER_DECODER) << "Using the following pipeline string:" << pipeline_string;

    GError* error_out = nullptr;
    m_pipeline = gst_parse_launch(pipeline_string.c_str(), &error_out);
    if (m_pipeline == nullptr)
    {
        yCError(GSTREAMER_DECODER) << "Pipeline syntax failure(1):" << pipeline_string;
        return false;
    }
    if (error_out)
    {
        yCError(GSTREAMER_DECODER) << "Pipeline syntax failure(2):" << pipeline_string << error_out->message;
        g_clear_error(&error_out);
        return false;
    }

    //configure to appsink to use the new_sample_func callback
    GstElement* appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "myappsink");
    if (appsink == nullptr)
    {
        yCError(GSTREAMER_DECODER) << "Pipeline syntax failure(3):" << pipeline_string;
        return false;
    }
    g_object_set(appsink, "emit-signals", false, NULL);
    GstAppSinkCallbacks callbacks = {nullptr, nullptr, new_sample_func};
    gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, &m_gst_cbk_data, nullptr);

    yCDebug(GSTREAMER_DECODER) << "init ok";
    return true;
}

bool GstYarpDecoder::start()
{
    GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        yCDebug(GSTREAMER_DECODER) << "pipeline failed to start!";
        return false;
    }
    yCDebug(GSTREAMER_DECODER) << "pipeline started!";
    this->prepareNextFrame();
    return true;
}

bool GstYarpDecoder::stop()
{
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_bus_set_sync_handler(gst_pipeline_get_bus(GST_PIPELINE(m_pipeline)), nullptr, nullptr, nullptr);
    yCDebug(GSTREAMER_DECODER) << "deleting pipeline";
    gst_object_unref(GST_OBJECT(m_pipeline));
    return true;
}

GstYarpDecoder::~GstYarpDecoder()
{
    stop();
}

ImageOf<PixelRgb>* GstYarpDecoder::getLastFrame()
{
    m_semaphore_reading_from_gst.wait();
    return m_gst_cbk_data.img_pointer;
}

void GstYarpDecoder::prepareNextFrame()
{
    m_semaphore_reading_from_stream.post();
}
