/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "H264Decoder.h"
#include "H264LogComponent.h"

#include <yarp/os/LogStream.h>


#include <gst/gst.h>
#include <glib.h>

#include <gst/app/gstappsink.h>
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


struct data_for_gst_callback
{
    data_for_gst_callback() = default;

    std::mutex *m{nullptr};
    ImageOf<PixelRgb> *img{nullptr};
    bool isNew{false};
    Semaphore *s{nullptr};
    bool isReq{false};
};
//-------------------------------------------------------------------
//---------------  CALLBACK FUNCTIONS -------------------------------
//-------------------------------------------------------------------

/*
static GstBusSyncReply bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    GstElement *pipeline = (GstElement *) data;

    switch (GST_MESSAGE_TYPE (msg))
    {

        case GST_MESSAGE_EOS:
        {
            yCTrace(H264CARRIER, "End of stream");
            gst_element_set_state (pipeline, GST_STATE_NULL);
            // g_main_loop_quit (loop);
            break;
        }

        case GST_MESSAGE_ERROR:
        {
            gchar  *debug;
            GError *error;

            gst_message_parse_error (msg, &error, &debug);
            g_free (debug);

            yCError(H264CARRIER, "GSTREAMER: Error: %s", error->message);
            g_error_free (error);

            gst_element_set_state (pipeline, GST_STATE_NULL);
            break;
        }
        default:
        {
            yCTrace("GSTREAMER: I received message of type %d", GST_MESSAGE_TYPE (msg));
            break;
        }
    }

  return GST_BUS_PASS;
}
*/

static gboolean link_videosrc2nextWithCaps(GstElement *e1, GstElement *e2)
{
    gboolean link_ok;
    GstCaps *caps;

/*
// "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96, a-framerate=(string)30"
    caps = gst_caps_new_simple("application/x-rtp",
                               "media", G_TYPE_STRING, "video",
                               "clock-rate", G_TYPE_INT, 90000,
                               "encoding-name", G_TYPE_STRING, "H264",
                               "payload", G_TYPE_INT, 96,
                               "a-framerate", G_TYPE_STRING, "30",
                               NULL);
*/
// "application/x-rtp, media=(string)video,  encoding-name=(string)H264, payload=(int)96"
    caps = gst_caps_new_simple("application/x-rtp",
                               "media", G_TYPE_STRING, "video",
                               "encoding-name", G_TYPE_STRING, "H264",
                               "payload", G_TYPE_INT, 96,
                               NULL);


    link_ok = gst_element_link_filtered(e1, e2, caps);
    if(!link_ok)
    {
         yCError(H264CARRIER) << "H264Decoder-GSTREAMER: link_videosrc2nextWithCaps failed";
    }
    else
    {
        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: link_videosrc2nextWithCaps OK";
    }

    return (link_ok);
}



static gboolean link_convert2next(GstElement *e1, GstElement *e2)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "RGB",
                               NULL);


    link_ok = gst_element_link_filtered(e1, e2, caps);

    if(!link_ok)
    {
        yCError(H264CARRIER) << "H264Decoder-GSTREAMER: link_convert2next failed";
    }
    else
    {
        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: link_convert2next OK";
    }

    return (link_ok);
}


GstFlowReturn new_sample(GstAppSink *appsink, gpointer user_data)
{
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

    auto* dec_data = (data_for_gst_callback*)user_data;

    GstSample *sample = nullptr;
    g_signal_emit_by_name (appsink, "pull-sample", &sample, NULL);
    if(!sample)
    {
        yCWarning(H264CARRIER, "GSTREAMER: could not take a sample!");
        return GST_FLOW_OK;
    }

    GstCaps *caps = gst_sample_get_caps (sample);
    if(!caps)
    {
        yCError(H264CARRIER, "GSTREAMER: could not get caps of sample!");
        return GST_FLOW_ERROR;
    }
    GstStructure *struc = gst_caps_get_structure(caps, 0);
    if(!struc)
    {
        yCError(H264CARRIER, "GSTREAMER: could not get struct of caps!");
        return GST_FLOW_ERROR;
    }
    gint width, height;
    gboolean res;
    res = gst_structure_get_int(struc, "width", &width);
    if(!res)
    {
        yCError(H264CARRIER, "GSTREAMER: could not get width!");
        return GST_FLOW_ERROR;
    }

    res = gst_structure_get_int(struc, "height", &height);
    if(!res)
    {
        yCError(H264CARRIER, "GSTREAMER: could not get height!");
        return GST_FLOW_ERROR;
    }
    yCTrace(H264CARRIER, "Image has size %d x %d", width, height);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if(!gst_buffer_map(buffer, &map, GST_MAP_READ))
    {
        yCError(H264CARRIER, "GSTREAMER: could not get map!");
        return GST_FLOW_ERROR;
    }
    //HERE I GET MY IMAGE!!!!
    //DO SOMETHING...
    //ImageOf<PixelRgb> &yframebuff = yarp_stuff_ptr->yport_ptr->prepare();
    dec_data->m->lock();
    dec_data->isNew = true;
    dec_data->img->resize(width, height);

    unsigned char *ydata_ptr = dec_data->img->getRawImage();
    memcpy(ydata_ptr, map.data, width*height*3);

    dec_data->m->unlock();
    gst_buffer_unmap(buffer, &map);

    gst_sample_unref(sample);
    if (dec_data->isReq) {
        dec_data->s->post();
    }


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








class H264DecoderHelper
{
public:
    //GMainLoop *loop;

    GstElement *pipeline;
    GstElement *source;
    GstElement *sink;
    GstElement *jitterBuff;
    GstElement *rtpDepay;
    GstElement *parser;
    GstElement *convert;
    GstElement *decoder;
    GstElement *sizeChanger;

    data_for_gst_callback gst_cbk_data;

    GstBus *bus; //maybe can be moved in function where i use it
    guint bus_watch_id;

    ImageOf<PixelRgb> myframe;

    H264DecoderHelper(std::mutex* m_ptr, Semaphore* s_ptr) :
        pipeline(nullptr),
        source(nullptr),
        sink(nullptr),
        jitterBuff(nullptr),
        rtpDepay(nullptr),
        parser(nullptr),
        convert(nullptr),
        decoder(nullptr),
        sizeChanger(nullptr),
        bus(nullptr),
        bus_watch_id(0)
    {
        gst_cbk_data.m = m_ptr;
        gst_cbk_data.img = &myframe;
        gst_cbk_data.s = s_ptr;
    }
    ~H264DecoderHelper(){;}


    bool istantiateElements(h264Decoder_cfgParamters &cfgParams)
    {
        gst_init(nullptr, nullptr);
        pipeline = gst_pipeline_new ("video-player");
        source   = gst_element_factory_make ("udpsrc",       "video-source");
        rtpDepay = gst_element_factory_make ("rtph264depay", "rtp-depay");
        parser   = gst_element_factory_make ("h264parse",    "parser");
        decoder  = gst_element_factory_make ("avdec_h264",   "decoder");
        sizeChanger  = gst_element_factory_make ("videocrop",    "cropper");
        convert  = gst_element_factory_make ("videoconvert", "convert"); //because use RGB space
        sink     = gst_element_factory_make ("appsink",      "video-output");

        if (!pipeline || !source || !rtpDepay || !parser || !decoder || !convert || !sink || !sizeChanger)
        {
            yCError(H264CARRIER) << "H264Decoder-GSTREAMER: one element could not be created. Exiting.";
            return false;
        }
        if (cfgParams.removeJitter)
        {
            jitterBuff = gst_element_factory_make("rtpjitterbuffer", "jitterBuffer");
            if (!jitterBuff)
            {
                yCError(H264CARRIER) << "H264Decoder-GSTREAMER: rtpjitterbuffer could not be created. Exiting.";
                return false;
            }
        }

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: istantiateElements OK";

        return true;
    }

    bool configureElements(h264Decoder_cfgParamters &cfgParams) //maybe i can make callbak configurable in the future.....
    {
        // 1) configure source port
        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to configure source port with value" << cfgParams.remotePort;
        g_object_set(source, "port", cfgParams.remotePort, NULL);
        yCDebug(H264CARRIER) << "H264Decoder-GSTREAMER: configured source port with" << cfgParams.remotePort;

        // 2) configure callback on new frame
        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to configure appsink.... ";
        //I decided to use callback mechanism because it should have less overhead
        g_object_set( sink, "emit-signals", false, NULL );

        GstAppSinkCallbacks cbs; // Does this need to be kept alive?

        // Set Video Sink callback methods
        cbs.eos = nullptr;
        cbs.new_preroll = nullptr;
        cbs.new_sample = &new_sample;
        gst_app_sink_set_callbacks( GST_APP_SINK( sink ), &cbs, &gst_cbk_data, nullptr );

  /*      //3) add watch ( a message handler)
        bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
        //bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
        gst_object_unref (bus);

        gst_bus_set_sync_handler(bus, bus_call, pipeline, NULL);
        gst_object_unref (bus);
    */

        //videocrop
        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to set new size: left" << cfgParams.crop.left << "right=" << cfgParams.crop.right << "top=" << cfgParams.crop.top << "bottom" << cfgParams.crop.bottom;
        g_object_set(G_OBJECT(sizeChanger), "left", cfgParams.crop.left, "right", cfgParams.crop.right, "top", cfgParams.crop.top, "bottom", cfgParams.crop.bottom, NULL);
        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: set new size: left" << cfgParams.crop.left << "right=" << cfgParams.crop.right << "top=" << cfgParams.crop.top << "bottom" << cfgParams.crop.bottom;

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: configureElements OK";
        return true;

    }

    bool linkElements()
    {

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to add elements to pipeline..... ";
        /* we add all elements into the pipeline */
        gst_bin_add_many (GST_BIN (pipeline),
                            source, rtpDepay, parser, decoder, sizeChanger, convert, sink, NULL);

        gboolean result;

        if (jitterBuff != nullptr)
        {
            result = gst_bin_add(GST_BIN(pipeline), jitterBuff);
            if (!result) { yCError(H264CARRIER) << "H264Decoder: Error adding jitterBuff to the bin"; return false; }
        }

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: elements have been added in pipeline!";

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to link_convert2next..... ";
        result = link_convert2next(convert, sink);
        if (!result) { yCError(H264CARRIER) << "H264Decoder: Error linking converter to sink "; return false; }

        /* autovideosrc ! "video/x-raw, width=640, height=480, format=(string)I420" ! videoconvert ! 'video/x-raw, format=(string)RGB'  ! yarpdevice ! glimagesink */

        if (jitterBuff)
        {
            yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to link videosrc to rtpjitterBuffer.....";
            result = link_videosrc2nextWithCaps(source, jitterBuff);
            if (!result){ yCError(H264CARRIER) << "H264Decoder: Error linking videosrc to rtpjitterBuffer "; return false;}

            yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to link jitterBuff to rtpDapay.....";
            result = gst_element_link(jitterBuff, rtpDepay);
            if (!result) { yCError(H264CARRIER) << "H264Decoder: Error linking jitterBuff to rtpDapay "; return false; }

        }
        else
        {
            yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to videosrc to rtpDepay";
            result = link_videosrc2nextWithCaps(source, rtpDepay);
            if (!result) { yCError(H264CARRIER) << "H264Decoder: Error linking videosrc to rtpDepay "; return false; }

        }

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: try to link all other elements.....";
        gst_element_link_many(rtpDepay, parser, decoder, sizeChanger, convert, NULL);

        yCTrace(H264CARRIER) << "H264Decoder-GSTREAMER: linkElements OK";
        return true;
    }


};



#define GET_HELPER(x) (*((H264DecoderHelper*)(x)))

H264Decoder::H264Decoder(h264Decoder_cfgParamters &config) :
    sysResource(new H264DecoderHelper(&mutex, &semaphore)),
    cfg(config)
{
}

bool H264Decoder::init()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    if(!helper.istantiateElements(cfg))
    {
        yCError(H264CARRIER) << "H264Decoder: Error in istantiateElements";
        return false;
    }

    if(!helper.configureElements(cfg))
    {
        yCError(H264CARRIER) << "Error in configureElements";
        return false;
    }

    if(!helper.linkElements())
    {
        yCError(H264CARRIER) << "Error in linkElements";
        return false;
    }

    yCDebug(H264CARRIER) << "H264Decoder-GSTREAMER:  init ok";
    return true;

}


bool H264Decoder::start()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    gst_element_set_state (helper.pipeline, GST_STATE_PLAYING);
    yCDebug(H264CARRIER) << "H264Decoder: pipeline started!";

    return true;

}

bool H264Decoder::stop()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    gst_element_set_state (helper.pipeline, GST_STATE_NULL);
    gst_bus_set_sync_handler(gst_pipeline_get_bus (GST_PIPELINE (helper.pipeline)), nullptr, nullptr, nullptr);
    yCDebug(H264CARRIER) << "H264Decoder: deleting pipeline";
    gst_object_unref (GST_OBJECT (helper.pipeline));
    return true;
}

H264Decoder::~H264Decoder()
{
    stop();
    delete &GET_HELPER(sysResource);


}

ImageOf<PixelRgb> & H264Decoder::getLastFrame()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    helper.gst_cbk_data.isNew = false;
    helper.gst_cbk_data.isReq = false;
    return helper.myframe;
}

bool H264Decoder::newFrameIsAvailable()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    return helper.gst_cbk_data.isNew;
}

int H264Decoder::getLastFrameSize()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    return (helper.myframe.width() * helper.myframe.height() * 3);
}

void H264Decoder::setReq()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    helper.gst_cbk_data.isReq = true;

}
