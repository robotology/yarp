/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "H264Decoder.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>


#include <gst/gst.h>
#include <glib.h>

#include <gst/app/gstappsink.h>
#include <stdio.h>
#include <string.h>

//#define debug_time 1

#ifdef debug_time
    #include <yarp/os/Time.h>
    #define DBG_TIME_PERIOD_PRINTS 10 //10 sec
#endif

using namespace yarp::sig;
using namespace yarp::os;


typedef struct
{
    Mutex *m;
    ImageOf<PixelRgb> *img;
    bool isNew;
    Semaphore *s;
    bool isReq;

} data_for_gst_callback;
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
            g_print ("End of stream\n");
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

            g_printerr ("GSTREAMER: Error: %s\n", error->message);
            g_error_free (error);

            gst_element_set_state (pipeline, GST_STATE_NULL);
            break;
        }
        default:
        {
            g_print("GSTREAMER: I received message of type %d\n", GST_MESSAGE_TYPE (msg));
            break;
        }
    }

  return GST_BUS_PASS;
}
*/

static gboolean link_videosrc2rtpdepay(GstElement *e1, GstElement *e2, bool verbose)
{
    gboolean link_ok;
    GstCaps *caps;


// "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96, a-framerate=(string)30"
    caps = gst_caps_new_simple("application/x-rtp",
                               "media", G_TYPE_STRING, "video",
                               "clock-rate", G_TYPE_INT, 90000,
                               "encoding-name", G_TYPE_STRING, "H264",
                               "payload", G_TYPE_INT, 96,
                               "a-framerate", G_TYPE_STRING, "30",
                               NULL);


    link_ok = gst_element_link_filtered(e1, e2, caps);
    //print error anyway, while print ok message only is verbose
    if(!link_ok)
    {
        g_print("GSTREAMER: failed link videosrc2convert with caps!!\n");
    }
    else if(verbose)
    {
        g_print("GSTREAMER: link videosrc2convert with caps OK!!!!!!\n");
    }

    return (link_ok);
}



static gboolean link_convert2next(GstElement *e1, GstElement *e2, bool verbose)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "RGB",
                              NULL);


    link_ok = gst_element_link_filtered(e1, e2, caps);

    //print error anyway, while print ok message only is verbose
    if(!link_ok)
    {
        g_print("GSTREAMER: failed link_convert2next with caps\n");
    }
    else if(verbose)
    {
        g_print("GSTREAMER: link_convert2next with caps OK\n");
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

    data_for_gst_callback *dec_data = (data_for_gst_callback*)user_data;

    GstSample *sample = NULL;
    g_signal_emit_by_name (appsink, "pull-sample", &sample, NULL);
    if(!sample)
    {
        g_print("GSTREAMER: could not take a sample!\n");
        return GST_FLOW_OK;
    }

    GstCaps *caps = gst_sample_get_caps (sample);
    if(!caps)
    {
        g_print("GSTREAMER: could not get caps of sample!\n");
        return GST_FLOW_ERROR;
    }
    GstStructure *struc = gst_caps_get_structure(caps, 0);
    if(!struc)
    {
        g_print("GSTREAMER: could not get struct of caps!\n");
        return GST_FLOW_ERROR;
    }
    gint width, height;
    gboolean res;
    res = gst_structure_get_int(struc, "width", &width);
    if(!res)
    {
        g_print("GSTREAMER: could not get width!\n");
        return GST_FLOW_ERROR;
    }

    res = gst_structure_get_int(struc, "height", &height);
    if(!res)
    {
        g_print("GSTREAMER: could not get height!\n");
        return GST_FLOW_ERROR;
    }
    //g_print("Image has size %d x %d", width, height);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if(!gst_buffer_map(buffer, &map, GST_MAP_READ))
    {
        g_print("GSTREAMER: could not get map!\n");
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
    if(dec_data->isReq)
        dec_data->s->post();


#ifdef debug_time
    end_time = Time::now();
    sumOf_timeOfNewSampleFunc += (end_time-start_time);
    count++;
    isFirst=false;

    if(count>=MAX_COUNT)
    {
        g_print("On %d times: NewSampleFunc is long %.6f sec and sleeps %.6f sec\n",
                MAX_COUNT, (sumOf_timeOfNewSampleFunc/MAX_COUNT), (sumOf_timeBetweenCalls/MAX_COUNT) );
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
    GstElement *rtpDepay;
    GstElement *parser;
    GstElement *convert;
    GstElement *decoder;
    GstElement *sizeChanger;

    data_for_gst_callback gst_cbk_data;
    bool verbose;

    GstBus *bus; //maybe can be moved in function where i use it
    guint bus_watch_id;

    ImageOf<PixelRgb> myframe;

    H264DecoderHelper( Mutex * m_ptr, Semaphore *s_ptr)
    {
        gst_cbk_data.m = m_ptr;
        gst_cbk_data.img = &myframe;
        gst_cbk_data.s = s_ptr;
    }
    ~H264DecoderHelper(){;}

    bool istantiateElements(void)
    {
        gst_init(NULL, NULL);
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
            g_printerr ("GSTREAMER: one element could not be created. Exiting.\n");
            return false;
        }
        return true;
    }

    bool configureElements(h264Decoder_cfgParamters &cfgParams) //maybe i can make callbak configurable in the future.....
    {
        // 1) configure source port
        if(verbose) g_print("GSTREAMER: try to configure source port with %d.... \n", cfgParams.remotePort);
        g_object_set(source, "port", cfgParams.remotePort, NULL);
        g_print("GSTREAMER: configured source port with %d.... \n", cfgParams.remotePort);

        // 2) configure callback on new frame
        if(verbose) g_print("GSTREAMER: try to configure appsink.... \n");
        //I decided to use callbaxk mechanism because it should have less overhead
        if(verbose) g_object_set( sink, "emit-signals", false, NULL );

        GstAppSinkCallbacks cbs; // Does this need to be kept alive?

        // Set Video Sink callback methods
        cbs.eos = NULL;
        cbs.new_preroll = NULL;
        cbs.new_sample = &new_sample;
        gst_app_sink_set_callbacks( GST_APP_SINK( sink ), &cbs, &gst_cbk_data, NULL );

  /*      //3) add watch ( a message handler)
        bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
        //bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
        gst_object_unref (bus);

        gst_bus_set_sync_handler(bus, bus_call, pipeline, NULL);
        gst_object_unref (bus);
    */

        //videocrop
        if(verbose) g_print("try to set new size: left=%d right=%d \n", cfgParams.crop.left, cfgParams.crop.right);
        g_object_set(G_OBJECT(sizeChanger), "left", cfgParams.crop.left, "right", cfgParams.crop.right, "top", cfgParams.crop.top, "bottom", cfgParams.crop.bottom, NULL);
        g_print("GSTREAMER: set crop parameters: left=%d, right=%d, top=%d, bottom=%d\n", cfgParams.crop.left, cfgParams.crop.right, cfgParams.crop.top, cfgParams.crop.bottom);
        return true;

    }

    bool linkElements(void)
    {

        if(verbose) g_print("GSTREAMER: try to add elements to pipeline..... \n");
        /* we add all elements into the pipeline */
        gst_bin_add_many (GST_BIN (pipeline),
                        source, rtpDepay, parser, decoder, sizeChanger, convert, sink, NULL);

        if(verbose) g_print("GSTREAMER: elements have been added in pipeline!\n");


        /* autovideosrc ! "video/x-raw, width=640, height=480, format=(string)I420" ! videoconvert ! 'video/x-raw, format=(string)RGB'  ! yarpdevice ! glimagesink */
        if(verbose) g_print("GSTREAMER: try to link_videosrc2convert..... \n");
        gboolean result = link_videosrc2rtpdepay(source, rtpDepay, verbose);
        if(!result)
        {
            return false;
        }

        if(verbose) g_print("GSTREAMER: try to link_convert2next..... \n");
        result = link_convert2next(convert, sink, verbose);
        if(!result)
        {
            return false;
        }

        if(verbose)g_print("GSTREAMER: try to link all other elements..... \n");
        gst_element_link_many(rtpDepay, parser, decoder, sizeChanger, convert, NULL);

        return true;
    }


};



#define GET_HELPER(x) (*((H264DecoderHelper*)(x)))

H264Decoder::H264Decoder(h264Decoder_cfgParamters &config) : sysResource(nullptr), cfg(config)
{
    sysResource = new H264DecoderHelper(&mutex, &semaphore);
    yAssert(sysResource != nullptr);

    H264DecoderHelper &helper = GET_HELPER(sysResource);
    helper.verbose = cfg.verbose;
}

bool H264Decoder::init(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    if(!helper.istantiateElements())
    {
        yError() << "H264Decoder: Error in istantiateElements";
        return false;
    }

    if(!helper.configureElements(cfg))
    {
        yError() << "Error in configureElements";
        return false;
    }

    if(!helper.linkElements())
    {
        yError() << "Error in linkElements";
        return false;
    }

    yDebug() << "gstreamer init ok";
    return true;

}


bool H264Decoder::start()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    gst_element_set_state (helper.pipeline, GST_STATE_PLAYING);
    yDebug() << "H264Decoder: pipeline started!";

    return true;

}

bool H264Decoder::stop()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    gst_element_set_state (helper.pipeline, GST_STATE_NULL);
    gst_bus_set_sync_handler(gst_pipeline_get_bus (GST_PIPELINE (helper.pipeline)), nullptr, nullptr, nullptr);
    yDebug() << "H264Decoder: deleting pipeline";
    gst_object_unref (GST_OBJECT (helper.pipeline));
    return true;
}

H264Decoder::~H264Decoder()
{
    stop();
    delete &GET_HELPER(sysResource);


}

ImageOf<PixelRgb> & H264Decoder::getLastFrame(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    helper.gst_cbk_data.isNew = false;
    helper.gst_cbk_data.isReq = false;
    return helper.myframe;
}

bool H264Decoder::newFrameIsAvailable(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    return helper.gst_cbk_data.isNew;
}

int H264Decoder::getLastFrameSize(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    return (helper.myframe.width() * helper.myframe.height() * 3);
}

void H264Decoder::setReq(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    helper.gst_cbk_data.isReq = true;

}
