#include "yarp/os/Time.h"
#include "yarp/os/Network.h"
#include "yarp/os/Log.h"
#include "yarp/os/LogStream.h"

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include <gst/video/video.h>
#include <iostream>

#include "yarpVideoSource.h"

GST_DEBUG_CATEGORY_STATIC(gst_yarp_video_source_debug);
#define GST_CAT_DEFAULT gst_yarp_video_source_debug

/* Structure to hold all relevant information for this element */
typedef struct _GstYarpVideoSource
{
    GstPushSrc parent;
    GstVideoInfo info;
} GstYarpVideoSource;

typedef struct _GstYarpVideoSourceClass
{
    GstPushSrcClass parent_class;
} GstYarpVideoSourceClass;

G_DEFINE_TYPE(GstYarpVideoSource, gst_yarp_video_source, GST_TYPE_PUSH_SRC);
//#define GST_TYPE_YARP_VIDEO_SOURCE (gst_yarp_video_source_get_type())
//G_DECLARE_FINAL_TYPE(GstYarpVideoSource, gst_yarp_test_source, GST, YARP_VIDEO_SOURCE, GstPushSrc)


/* Yarp stuff */
yarp_handler_class* yarp_handler = nullptr;

enum
{
    PROP_0,
    PROP_LOCAL_PORTNAME,
    PROP_REMOTE_PORTNAME,
    PROP_CONNECTION_PROTO,
    PROP_PORT_TYPE,
    PROP_YARPVERBOSELEVEL
};

/* Function prototypes */
static void gst_yarp_video_source_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_yarp_video_source_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);
static GstCaps* gst_yarp_video_source_get_caps(GstBaseSrc* src, GstCaps* filter);
static gboolean gst_yarp_video_source_set_caps(GstBaseSrc* src, GstCaps* caps);
static gboolean gst_yarp_video_source_start(GstBaseSrc* src);
static gboolean gst_yarp_video_source_stop(GstBaseSrc* src);

static GstFlowReturn gst_yarp_video_source_fill(GstPushSrc* src, GstBuffer* buf);
static GstFlowReturn gst_yarp_video_source_alloc(GstPushSrc* src, GstBuffer** buf);


#define MY_SOURCE_CAPS           \
    "video/x-raw, "              \
    "width = (int) 640, "        \
    "height = (int) 480, "       \
    "format=(string){RGB};"      \
    "video/x-h264, "             \
    "stream-format=(string){avc,byte-stream}," \
    "alignment=(string){au,nal};" \
    "video/x-h265, " \
    "stream-format=(string){avc,byte-stream}," \
    "alignment=(string){au,nal}"

/*
#define MY_SOURCE_CAPS \
    "video/x-raw, " \
    "format=(string) {RGB}, " //\
//    "width = (int) 640, " \
//    "height = (int) 480 "
*/

#define VTS_VIDEO_CAPS GST_VIDEO_CAPS_MAKE(GST_VIDEO_FORMATS_ALL) MY_SOURCE_CAPS

/*
#define VTS_VIDEO_CAPS GST_VIDEO_CAPS_MAKE(GST_VIDEO_FORMATS_ALL) ","                                                           \
                                                                  "multiview-mode = { mono, left, right }"                      \
                                                                  ";"                                                           \
                                                                  "video/x-bayer, format=(string) { bggr, rggb, grbg, gbrg }, " \
                                                                  "width = " GST_VIDEO_SIZE_RANGE ", "                          \
                                                                  "height = " GST_VIDEO_SIZE_RANGE ", "                         \
                                                                  "framerate = " GST_VIDEO_FPS_RANGE ", "                       \
                                                                  "multiview-mode = { mono, left, right }"
*/

static GstStaticPadTemplate gst_video_test_src_template = GST_STATIC_PAD_TEMPLATE("src",
                                                                                  GST_PAD_SRC,
                                                                                  GST_PAD_ALWAYS,
                                                                                  GST_STATIC_CAPS(MY_SOURCE_CAPS));

/* Initialize the class */
static void gst_yarp_video_source_class_init(GstYarpVideoSourceClass* klass)
{
    GObjectClass*    gobject_class    = (GObjectClass*)klass;
    GstElementClass* gstelement_class = (GstElementClass*)klass;
    GstBaseSrcClass* gstbasesrc_class = (GstBaseSrcClass*)klass;
    GstPushSrcClass* gstpushsrc_class = (GstPushSrcClass*)klass;

    gobject_class->set_property = gst_yarp_video_source_set_property;
    gobject_class->get_property = gst_yarp_video_source_get_property;

    gst_element_class_set_static_metadata(gstelement_class,
                                          "YARP Test Source",
                                          "Source/Video",
                                          "Generates a video alternating between green and red",
                                          "Your Name <your.email@example.com>");

    gstbasesrc_class->get_caps = gst_yarp_video_source_get_caps;
    gstbasesrc_class->set_caps = gst_yarp_video_source_set_caps;
    gstbasesrc_class->start = gst_yarp_video_source_start;
    gstbasesrc_class->stop = gst_yarp_video_source_stop;

    //gstpushsrc_class->create = gst_yarp_video_source_create;
    gstpushsrc_class->fill = gst_yarp_video_source_fill;
    gstpushsrc_class->alloc = gst_yarp_video_source_alloc;

    g_object_class_install_property(gobject_class, PROP_LOCAL_PORTNAME, g_param_spec_string("localPortname",        "localPortname (string)",      "Name of the local port", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_REMOTE_PORTNAME, g_param_spec_string("remotePortname",      "remotePortname (string)",     "Name of the remote port to perform automatic connection (disabled by default)", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_CONNECTION_PROTO, g_param_spec_string("connectionProtocol", "connectionProtocol (string)", "Name of the protocol to performa automatic conenction (disabled by default)", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_PORT_TYPE, g_param_spec_string("portType", "portType (string)", "(default rgb)", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_YARPVERBOSELEVEL, g_param_spec_int("yarpverbose", "yarpverbose (int)", "Verbosity level", 0, 100, 0, G_PARAM_READWRITE));

    GST_DEBUG_CATEGORY_INIT(gst_yarp_video_source_debug, "yarpvideosource", 0, "YARP Video Source");

    gst_element_class_add_static_pad_template(gstelement_class, &gst_video_test_src_template);
}

static void gst_yarp_video_source_init(GstYarpVideoSource* src)
{
    yarp_handler = new yarp_handler_class;
    yTrace();
    gst_base_src_set_format(GST_BASE_SRC(src), GST_FORMAT_TIME);
}

/* Set/get Property methods */
static void gst_yarp_video_source_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec)
{
    switch (prop_id)
    {
        case PROP_YARPVERBOSELEVEL:
            yarp_handler->verbosity_level = (g_value_get_int(value));
            break;
        case PROP_LOCAL_PORTNAME:
            yarp_handler->input_port_name = std::string(g_value_dup_string(value));
            yCInfo(YVS_COMP) << "Local port name set to:" << yarp_handler->input_port_name;
            break;
        case PROP_REMOTE_PORTNAME:
            yarp_handler->remote_port_name = std::string(g_value_dup_string(value));
            yCInfo(YVS_COMP) << "Remote port name set to:" << yarp_handler->remote_port_name;
            break;
        case PROP_CONNECTION_PROTO:
            yarp_handler->connection_protocol = std::string(g_value_dup_string(value));
            yCInfo(YVS_COMP) << "Connection protocol set to:" << yarp_handler->connection_protocol;
            break;
        case PROP_PORT_TYPE:
            if (strcmp(g_value_dup_string(value), "rgb") == 0)
            {
                yarp_handler->input_port_reader.set_port_type( yarp_handler_class::port_type_enum::RGB_TYPE );
                yCInfo(YVS_COMP) << "Port type (on request) = rgb";
            }
            else if (strcmp(g_value_dup_string(value), "bin") == 0)
            {
                yarp_handler->input_port_reader.set_port_type(yarp_handler_class::port_type_enum::BINARY_TYPE);
                yCInfo(YVS_COMP) << "Port type (on request) = bin";
            }
            else
            {
                yarp_handler->input_port_reader.set_port_type(yarp_handler_class::port_type_enum::RGB_TYPE);
                yCInfo(YVS_COMP) << "Port type (default value) = rgb";
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
    yTrace();
}

static void gst_yarp_video_source_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec)
{
    switch (prop_id)
    {
        case PROP_YARPVERBOSELEVEL:
            g_value_set_int(value, yarp_handler->verbosity_level);
            break;
        case PROP_LOCAL_PORTNAME:
            g_value_set_string(value, yarp_handler->input_port_name.c_str());
            break;
        case PROP_REMOTE_PORTNAME:
            g_value_set_string(value, yarp_handler->remote_port_name.c_str());
            break;
        case PROP_CONNECTION_PROTO:
            g_value_set_string(value, yarp_handler->connection_protocol.c_str());
            break;
        case PROP_PORT_TYPE:
            if (yarp_handler->input_port_reader.get_port_type() == yarp_handler_class::port_type_enum::RGB_TYPE)
            {
                g_value_set_string(value, "rgb");
            }
            else if (yarp_handler->input_port_reader.get_port_type() == yarp_handler_class::port_type_enum::BINARY_TYPE)
            {
                g_value_set_string(value, "bin");
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static GstCaps* gst_yarp_video_source_get_caps(GstBaseSrc* src, GstCaps* filter)
{
    yTrace();

    GstCaps* caps = gst_caps_from_string(MY_SOURCE_CAPS);

    if (filter)
    {
        GstCaps* intersection;
        intersection = gst_caps_intersect_full(filter, caps, GST_CAPS_INTERSECT_FIRST);
        gst_caps_unref(caps);
        return intersection;
    }
    else
    {
        return caps;
    }
}

static gboolean gst_yarp_video_source_set_caps(GstBaseSrc* src, GstCaps* caps)
{
    GstYarpVideoSource* yarp_src;
    yarp_src = (GstYarpVideoSource*)(src);

    return gst_video_info_from_caps(&yarp_src->info, caps);
}

/* Start/Stop methods */
static gboolean gst_yarp_video_source_start(GstBaseSrc* src)
{
    //open YARP port
    bool b = yarp_handler->input_port.open(yarp_handler->input_port_name);
    if (!b)
    {
        yCError(YVS_COMP) << "Unable to open port: " << yarp_handler->input_port_name;
        return FALSE;
    }
    if (!yarp_handler->remote_port_name.empty())
    {
        b = yarp::os::Network::connect(yarp_handler->remote_port_name, yarp_handler->input_port_name, yarp_handler->connection_protocol);
        if (!b)
        {
            yCError(YVS_COMP) << "Could not perform connection between: " << yarp_handler->remote_port_name 
                     << " and " << yarp_handler->input_port_name << " via " << yarp_handler->connection_protocol;
            return FALSE;
        }
    }

    return TRUE;
}

static gboolean gst_yarp_video_source_stop(GstBaseSrc* src)
{
    // Close YARP port
    yarp_handler->input_port.close();

    if (yarp_handler)
    {
        delete yarp_handler;
        yarp_handler = nullptr;
    }
    return TRUE;
}

/* Frame methods */
static GstFlowReturn gst_yarp_video_source_fill(GstPushSrc* src, GstBuffer* buf)
{
    //yCInfo(YVS_COMP) << ">>Fill";
    GstMapInfo map;
    gst_buffer_map(buf, &map, GST_MAP_WRITE);

    GstYarpVideoSource* yarp_src = (GstYarpVideoSource*)(src);
    guint gst_size = yarp_src->info.width * yarp_src->info.height * 3; // RGB format

    yarp_handler->input_port_reader.image_mutex.lock();
    if (yarp_handler->input_port_reader.get_port_type() == yarp_handler_class::port_type_enum::RGB_TYPE)
    {
        if (yarp_handler->input_port_reader.image_size == gst_size)
        {
            //yCDebug(YVS_COMP) << "bcopy" << yarp_handler->input_port_reader.image_size << gst_size;
            memcpy(map.data, yarp_handler->input_port_reader.image_buffer, yarp_handler->input_port_reader.image_size);
            //yCDebug(YVS_COMP) << "acopy";
        }
        else if (yarp_handler->input_port_reader.image_size == 0)
        {
            yCError(YVS_COMP) << "No image received yet (rgb image mode)";
        }
        else
        {
            yCError(YVS_COMP) << "size mismatch! gst:" << gst_size << "vs yarp:" << yarp_handler->input_port_reader.image_size;
        }
    }
    else if (yarp_handler->input_port_reader.get_port_type() == yarp_handler_class::port_type_enum::BINARY_TYPE)
    {
        if (yarp_handler->input_port_reader.image_size != 0)
        {
            //yCDebug() << "bbinary";
            memcpy(map.data, yarp_handler->input_port_reader.image_buffer, yarp_handler->input_port_reader.image_size);
            //yCDebug() << "abinary";
        }
        else if (yarp_handler->input_port_reader.image_size == 0)
        {
            yCError(YVS_COMP) << "No image received yet (binary mode)";
        }
    }
    else
    {
        yCError(YVS_COMP) << "unreachable";
        //unreachable code
    }

    yarp_handler->input_port_reader.image_mutex.unlock();

    gst_buffer_unmap(buf, &map);

    return GST_FLOW_OK;
}

static GstFlowReturn gst_yarp_video_source_alloc(GstPushSrc* src, GstBuffer** buf)
{
    if (yarp_handler->input_port.getInputCount() == 0)
    {
        yCInfo(YVS_COMP) << "Waiting port connection..";
    }
    std::unique_lock lk(yarp_handler->input_port_reader.cvar_mutex);
    yarp_handler->input_port_reader.cvar.wait(lk, []
                                              { return yarp_handler->input_port_reader.frame_ready; });
    yarp_handler->input_port_reader.frame_ready = false;

    GstYarpVideoSource* yarp_src = (GstYarpVideoSource*)(src);
    yarp_src->info.width=640;   //<<<<<<<<<<<<<<<<<<<<<<<<
    yarp_src->info.height=480;  //<<<<<<<<<<<<<<<<<<<<<<<<
    guint size = yarp_src->info.width * yarp_src->info.height * 3; // RGB format

    GstBuffer* buffer = gst_buffer_new_allocate(NULL, size, NULL);
    *buf = buffer;

    return GST_FLOW_OK;
}

/* Register the plugin */
static gboolean yarp_video_source_plugin_init(GstPlugin* plugin)
{
    return gst_element_register(plugin, "yarpvideosource", GST_RANK_NONE, gst_yarp_video_source_get_type());
}

#define VERSION "1.0"
#define PACKAGE "AAA"

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    yarpvideosource,
    "YARP Video Source",
    yarp_video_source_plugin_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/")


    // TO test it:
// export GST_PLUGIN_PATH=$GST_PLUGIN_PATH:C:\Software\iCubSoftware\yarp\build\bin\Release
// set GST_PLUGIN_PATH=%GST_PLUGIN_PATH%;C:\Software\iCubSoftware\yarp\build\bin\Release
// gst-inspect-1.0 yarpvideosource
//  gst-launch-1.0 yarpvideosource localPortname="/aaa" portType = "rgb" ! videoconvert ! autovideosink
// gst-launch-1.0 yarpvideosource localPortname="/aaa"  ! videoconvert ! autovideosink
//gst-launch-1.0  yarpvideosource ! "video/x-raw, format=(string)I420, width=(int)640, height=(int)480" ! autovideosink
//gst-launch-1.0  yarpvideosource ! "video/x-raw, format=(string)I420, width=(int)640, height=(int)480" ! autovideosink
// gst-launch-1.0  yarpvideosource  ! glimagesink
//gst-launch-1.0 yarpvideosource localPortname="/aaa" !videoconvert !x264enc !h264parse !avdec_h264 !videoconvert !autovideosink

//grabber_yarp          ->             -> fast_tcp -> *porta_rgb_2_gs_image* -> sink
//telecamera -> h264enc -> blob2porta  -> fast_tcp -> porta_blob_2_gs_image -> 264dec -> sink

    /*
    C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0.lib 
    C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstapp-1.0.lib 
    C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstbase-1.0.lib
    C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstbase-1.0.lib
    C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstvideo-1.0.lib
    */
    
