/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/os/Time.h"
#include "yarp/os/Network.h"
#include "yarp/os/Log.h"
#include "yarp/os/LogStream.h"

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include <gst/video/video.h>
#include <iostream>

#include "yarpVideoPassthrough.h"

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <string.h>

enum
{
    PROP_0,
    PROP_YARPNAME,
    PROP_YARPVERBOSELEVEL
};

GST_DEBUG_CATEGORY_STATIC(yarp_video_passthrough_debug);
#define GST_CAT_DEFAULT yarp_video_passthrough_debug

// Define the structures for the class and instance
typedef struct _GstYarpVideoPassthrough
{
    GstVideoFilter parent;
    yarp::os::Network* yarpnet = nullptr;
    std::string s_name;
    int verbosity_level = 0;
    double prev_time = yarp::os::Time::now();
    int frame_counter=0;
} GstYarpVideoPassthrough;

typedef struct _GstYarpVideoPassthroughClass
{
    GstVideoFilterClass parent_class;
} GstYarpVideoPassthroughClass;

G_DEFINE_TYPE(GstYarpVideoPassthrough, gst_yarp_video_passthrough, GST_TYPE_VIDEO_FILTER)
#define GST_MY_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), gst_yarp_video_passthrough_get_type(), GstYarpVideoPassthrough))

// G_DEFINE_TYPE(GstYarpVideoPassthrough, gst_yarp_video_passthrough, GST_TYPE_VIDEO_SINK)

// #define GST_TYPE_GRAY_FILTER (gst_gray_filter_get_type())

// G_DECLARE_FINAL_TYPE(GstYarpVideoPassthrough, gst_yarp_video_passthrough, GST, GRAY_FILTER, GstBaseTransform)

/* Pads */

#define MY_SOURCE_CAPS           \
    "video/x-raw, "              \
    "format=(string){RGB,I420,NV12,YUY2};"      \
    "video/x-h264, "             \
    "stream-format=(string){avc,byte-stream}," \
    "alignment=(string){au,nal};" \
    "video/x-h265, " \
    "stream-format=(string){avc,byte-stream}," \
    "alignment=(string){au,nal}"

static GstStaticPadTemplate gst_yarp_video_passthrough_sink_template = GST_STATIC_PAD_TEMPLATE("sink",
                                                                                               GST_PAD_SINK,
                                                                                               GST_PAD_ALWAYS,
                                                                                               GST_STATIC_CAPS(MY_SOURCE_CAPS));

static GstStaticPadTemplate gst_yarp_video_passthrough_src_template = GST_STATIC_PAD_TEMPLATE("src",
                                                                                              GST_PAD_SRC,
                                                                                              GST_PAD_ALWAYS,
                                                                                              GST_STATIC_CAPS(MY_SOURCE_CAPS));

/* Function prototypes */
static GstFlowReturn gst_yarp_video_passthrough_transform_frame(GstVideoFilter* filter, GstVideoFrame* inframe, GstVideoFrame* outframe);
static void gst_yarp_video_passthrough_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_yarp_video_passthrough_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);
//static gboolean gst_yarp_video_passthrough_start(GstBaseTransform* elem);
//static gboolean gst_yarp_video_passthrough_stop(GstBaseTransform* elem);

    /* Initialize the class */
static void gst_yarp_video_passthrough_class_init(GstYarpVideoPassthroughClass* klass)
{
    GObjectClass* gobject_class = (GObjectClass*)klass;
    GstElementClass* gstelement_class = GST_ELEMENT_CLASS(klass);
    GstVideoFilterClass* video_filter_class = GST_VIDEO_FILTER_CLASS(klass);
    video_filter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_yarp_video_passthrough_transform_frame);
 //   GstBaseTransformClass* gstbase_class = GST_BASE_TRANSFORM_CLASS(klass);

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_static_pad_template_get(&gst_yarp_video_passthrough_sink_template));

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_static_pad_template_get(&gst_yarp_video_passthrough_src_template));

    gst_element_class_set_static_metadata(gstelement_class,
                                          "YARP Test Sink",
                                          "Sink/Video",
                                          "Sinks",
                                          "Your Name <your.email@example.com>");

    gobject_class->set_property = gst_yarp_video_passthrough_set_property;
    gobject_class->get_property = gst_yarp_video_passthrough_get_property;
    //video_filter_class->start = gst_yarp_video_passthrough_start;
   // gstbase_class->stop = gst_yarp_video_passthrough_stop;
    g_object_class_install_property(gobject_class, PROP_YARPNAME, g_param_spec_string("yarpname", "yarpname (string)", "Name of the component", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_YARPVERBOSELEVEL, g_param_spec_int("yarpverbose", "yarpverbose (int)", "Verbosity level", 0,100, 0, G_PARAM_READWRITE));

    GST_DEBUG_CATEGORY_INIT(yarp_video_passthrough_debug, "yarpvideopassthrough", 0, "Yarp Video Passthrough");
}

static void gst_yarp_video_passthrough_init(GstYarpVideoPassthrough* filter)
{
    filter->yarpnet = new yarp::os::Network;
}

static void gst_yarp_video_passthrough_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec)
{
    _GstYarpVideoPassthrough* self = GST_MY_PLUGIN(object);

    switch (prop_id) {
    case PROP_YARPNAME:
        self->s_name = (g_value_get_string(value));
        yCInfo(YVP_COMP, "set name: %s", self->s_name.c_str());
        break;
    case PROP_YARPVERBOSELEVEL:
        self->verbosity_level = (g_value_get_int(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
    yTrace();
}

static void gst_yarp_video_passthrough_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec)
{
    _GstYarpVideoPassthrough* self = GST_MY_PLUGIN(object);

    switch (prop_id) {
    case PROP_YARPNAME:
        g_value_set_string(value, self->s_name.c_str());
        yCInfo(YVP_COMP, "get name: %s", self->s_name.c_str());
        break;
    case PROP_YARPVERBOSELEVEL:
        g_value_set_int(value, self->verbosity_level);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* Frame methods */
static GstFlowReturn gst_yarp_video_passthrough_transform_frame(GstVideoFilter* filter, GstVideoFrame* inframe, GstVideoFrame* outframe)
{
    if (inframe == nullptr || inframe->buffer ==nullptr)
    {
        printf ("----------------\n");
        return GST_FLOW_ERROR;
    }
    if (outframe == nullptr || outframe->buffer == nullptr)
    {
        printf ("44444444444444444\n");
        return GST_FLOW_ERROR;
    }

    _GstYarpVideoPassthrough* self = GST_MY_PLUGIN(filter);

    double time1 = yarp::os::Time::now();
    gst_video_frame_copy(outframe, inframe);
    double time2 = yarp::os::Time::now();

    double diff = time1 - self->prev_time;
    self->prev_time = time1;

    switch (self->verbosity_level)
    {
        case 0:
        break;
        case 1:
            //yCInfo(YVP_COMP, "%s Timestamp: %+3.3f %+3.3f", s_name.c_str(), time1, time2 - time1);
        printf("%s %d Timestamp: %+3.3f %+3.3f\n", self->s_name.c_str(), self->frame_counter, time1, time2 - time1);
        break;
        case 2:
            //yCInfo(YVP_COMP, "%s Timestamp: %+3.3f %+3.3f %+3.3f", s_name.c_str(), time1, time2 - time1, diff);
        printf("%s %d Timestamp: %+3.3f %+3.3f %+3.3f\n", self->s_name.c_str(), self->frame_counter, time1, time2 - time1, diff);
        break;
        case 3:
        if (diff > 0.04)
            //yCInfo(YVP_COMP, "%s Timestamp: %+3.3f %+3.3f %+3.3f", s_name.c_str(), time1, time2 - time1, diff);
            printf("%s %d Timestamp: %+3.3f %+3.3f %+3.3f\n", self->s_name.c_str(), self->frame_counter, time1, time2 - time1, diff);
        break;
        case 4:
        if (diff > 0.035 || diff < 0.025)
            // yCInfo(YVP_COMP, "%s Timestamp: %+3.3f %+3.3f %+3.3f", s_name.c_str(), time1, time2 - time1, diff);
            printf("%s %d Timestamp: %+3.3f %+3.3f %+3.3f\n", self->s_name.c_str(), self->frame_counter, time1, time2 - time1, diff);
        break;
        case 5:
        if (self->frame_counter%(30*10)==0)
            printf("%s %d Timestamp: %+3.3f %+3.3f %+3.3f\n", self->s_name.c_str(), self->frame_counter, time1, time2 - time1, diff);
        break;
    }






    /*
    GstMapInfo info;
    gst_buffer_map(buf, &info, GST_MAP_WRITE);

    guint8* data = info.data;
    for (guint i = 0; i < info.size; i += 3) {
        guint8 gray = (data[i] + data[i + 1] + data[i + 2]) / 3;
        data[i] = data[i + 1] = data[i + 2] = gray;
    }

    gst_buffer_unmap(buf, &info);
    */

    self->frame_counter++;

    return GST_FLOW_OK;
}

/* Start/Stop methods */
static gboolean gst_yarp_video_passthrough_start(GstBaseTransform* elem)
{
    return TRUE;
}

static gboolean gst_yarp_video_passthrough_stop(GstBaseTransform* elem)
{
    _GstYarpVideoPassthrough* self = GST_MY_PLUGIN(elem);
    if (self->yarpnet)
    {
        delete self->yarpnet;
        self->yarpnet = nullptr;
    }
    return TRUE;
}

    /* Register the plugin */
static gboolean yarp_video_passthrough_init(GstPlugin* plugin)
{
    return gst_element_register(plugin, "yarpvideopassthrough", GST_RANK_NONE, gst_yarp_video_passthrough_get_type());
}

#define PACKAGE "CCC"

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    yarpvideopassthrough,
    "Grayscale video filter",
    yarp_video_passthrough_init,
    "1.0",
    "LGPL",
    "GStreamer",
    "https://gstreamer.freedesktop.org/")


///----------------------------------------------------------------------
