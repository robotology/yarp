#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>

#include <gst/gst.h>
#include <gst/video/gstvideosink.h>
#include <gst/video/video.h>
#include <iostream>

#include "yarpVideoSink.h"

GST_DEBUG_CATEGORY_STATIC(yarp_video_sink_debug);
#define GST_CAT_DEFAULT yarp_video_sink_debug

/* Structure to hold all relevant information for this element */
typedef struct _GstYarpVideoSink
{
    GstVideoSink parent;
} GstYarpVideoSink;

typedef struct _GstYarpVideoSinkClass
{
    GstVideoSinkClass parent_class;
} GstYarpVideoSinkClass;


G_DEFINE_TYPE(GstYarpVideoSink, gst_yarp_video_sink, GST_TYPE_VIDEO_SINK);

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
static GstFlowReturn gst_yarp_video_sink_show_frame(GstVideoSink* sink, GstBuffer* buf);
static void gst_yarp_video_sink_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_yarp_video_sink_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);
static gboolean gst_yarp_video_sink_start(GstBaseSink* sink);
static gboolean gst_yarp_video_sink_stop(GstBaseSink* sink);

/* Initialize the class */
static void gst_yarp_video_sink_class_init(GstYarpVideoSinkClass* klass)
{
    GObjectClass*      gobject_class = (GObjectClass*)klass;
    GstElementClass*   gstelement_class = GST_ELEMENT_CLASS(klass);
    GstVideoSinkClass* gstvideo_sink_class = GST_VIDEO_SINK_CLASS(klass);
    GstBaseSinkClass*  gstbase_sink_class = GST_BASE_SINK_CLASS(klass);

    gobject_class->set_property = gst_yarp_video_sink_set_property;
    gobject_class->get_property = gst_yarp_video_sink_get_property;
    gstbase_sink_class->start = gst_yarp_video_sink_start;
    gstbase_sink_class->stop = gst_yarp_video_sink_stop;

    gst_element_class_set_static_metadata(gstelement_class,
                                          "YARP Test Sink",
                                          "Sink/Video",
                                          "Sinks",
                                          "Your Name <your.email@example.com>");

    gstvideo_sink_class->show_frame = GST_DEBUG_FUNCPTR(gst_yarp_video_sink_show_frame);

    g_object_class_install_property(gobject_class, PROP_LOCAL_PORTNAME, g_param_spec_string("localPortname", "localPortname (string)", "Name of the local port", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_REMOTE_PORTNAME, g_param_spec_string("remotePortname", "remotePortname (string)", "Name of the remote port to perform automatic connection (disabled by default)", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_CONNECTION_PROTO, g_param_spec_string("connectionProtocol", "connectionProtocol (string)", "Name of the protocol to performa automatic conenction (disabled by default)", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_PORT_TYPE, g_param_spec_string("portType", "portType (string)", "(default rgb)", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_YARPVERBOSELEVEL, g_param_spec_int("yarpverbose", "yarpverbose (int)", "Verbosity level", 0, 100, 0, G_PARAM_READWRITE));

    GST_DEBUG_CATEGORY_INIT(yarp_video_sink_debug, "yarpvideosink", 0, "Yarp Video Sink");

    //Define the sink capabilities
    GstStaticPadTemplate sink_pad_template = GST_STATIC_PAD_TEMPLATE(
      "sink",
      GST_PAD_SINK,
      GST_PAD_ALWAYS,
      GST_STATIC_CAPS (
                       "video/x-raw,"
                       "format=(string){RGB};"
                       "video/x-h264,"
                       "stream-format=(string){avc,byte-stream},"
                       "alignment=(string){au,nal};"
                       "video/x-h265,"
                       "stream-format=(string){avc,byte-stream},"
                       "alignment=(string){au,nal}"
                      ));
    gst_element_class_add_static_pad_template(gstelement_class, &sink_pad_template);
}

static void gst_yarp_video_sink_init(GstYarpVideoSink* sink)
{
    yarp_handler = new yarp_handler_class;
    yTrace();
}

/* Set/get Property methods */
static void gst_yarp_video_sink_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec)
{
    switch (prop_id)
    {
        case PROP_YARPVERBOSELEVEL:
            yarp_handler->verbosity_level = (g_value_get_int(value));
            break;
        case PROP_LOCAL_PORTNAME:
            yarp_handler->output_port_name = std::string(g_value_dup_string(value));
            yCInfo(YVSNK_COMP) << "Local port name set to:" << yarp_handler->output_port_name;
            break;
        case PROP_REMOTE_PORTNAME:
            yarp_handler->remote_port_name = std::string(g_value_dup_string(value));
            yCInfo(YVSNK_COMP) << "Remote port name set to:" << yarp_handler->remote_port_name;
            break;
        case PROP_CONNECTION_PROTO:
            yarp_handler->connection_protocol = std::string(g_value_dup_string(value));
            yCInfo(YVSNK_COMP) << "Connection protocol set to:" << yarp_handler->connection_protocol;
            break;
        case PROP_PORT_TYPE:
            if (strcmp(g_value_dup_string(value), "rgb")==0)
            {
                yarp_handler->port_type = yarp_handler_class::port_type_enum::RGB_TYPE;
                yCInfo(YVSNK_COMP) << "Port type (on request) = rgb";
            }
            else if (strcmp(g_value_dup_string(value), "bin")==0)
            {
                yarp_handler->port_type = yarp_handler_class::port_type_enum::BINARY_TYPE;
                yCInfo(YVSNK_COMP) << "Port type (on request) = bin";
            }
            else
            {
                yarp_handler->port_type = yarp_handler_class::port_type_enum::RGB_TYPE;
                yCInfo(YVSNK_COMP) << "Port type (default value) = rgb";
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
    yTrace();
}

static void gst_yarp_video_sink_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec)
{
    switch (prop_id)
    {
        case PROP_YARPVERBOSELEVEL:
            g_value_set_int(value, yarp_handler->verbosity_level);
            break;
        case PROP_LOCAL_PORTNAME:
            g_value_set_string(value, yarp_handler->output_port_name.c_str());
            break;
        case PROP_REMOTE_PORTNAME:
            g_value_set_string(value, yarp_handler->remote_port_name.c_str());
            break;
        case PROP_CONNECTION_PROTO:
            g_value_set_string(value, yarp_handler->connection_protocol.c_str());
            break;
        case PROP_PORT_TYPE:
            if (yarp_handler->port_type == yarp_handler_class::port_type_enum::RGB_TYPE)
            {
                g_value_set_string(value, "rgb");
            }
            else if (yarp_handler->port_type == yarp_handler_class::port_type_enum::BINARY_TYPE)
            {
                g_value_set_string(value, "bin");
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

/* Start/Stop methods */
static gboolean gst_yarp_video_sink_start(GstBaseSink* sink)
{
    bool b = yarp_handler->output_port.open(yarp_handler->output_port_name);
    if (!b)
    {
        yCError(YVSNK_COMP) << "Unable to open port: " << yarp_handler->output_port_name;
        return FALSE;
    }
    if (!yarp_handler->remote_port_name.empty())
    {
        b = yarp::os::Network::connect(yarp_handler->output_port_name, yarp_handler->remote_port_name, yarp_handler->connection_protocol);
        if (!b)
        {
            yCError(YVSNK_COMP) << "Could not perform connection between: " << yarp_handler->output_port_name
                     << " and " << yarp_handler->remote_port_name << " via " << yarp_handler->connection_protocol;
            return FALSE;
        }
    }
    return TRUE;
}

static gboolean gst_yarp_video_sink_stop(GstBaseSink* sink)
{
    yarp_handler->output_port.close();

    if (yarp_handler)
    {
        delete yarp_handler;
        yarp_handler = nullptr;
    }
    return TRUE;
}

/* Frame methods */
static GstFlowReturn gst_yarp_video_sink_show_frame(GstVideoSink* sink, GstBuffer* buf)
{
    GstMapInfo info;
    if (gst_buffer_map(buf, &info, GST_MAP_READ))
    {
        if (yarp_handler->verbosity_level>0)
        {
            yCDebug(YVSNK_COMP) << "Received frame of size: " << info.size << " bytes";
        }
        if (yarp_handler->output_port.getOutputCount() > 0)
        {
            //Get info about the stream
            GstCaps *caps = gst_pad_get_current_caps(GST_BASE_SINK_PAD(sink));
            if (caps==nullptr)
            {
                yCError(YVSNK_COMP) << "gst_pad_get_current_caps() failed";
                return  GST_FLOW_ERROR;
            }
            const GstStructure* structure = gst_caps_get_structure(caps,0);
            const gchar* format = gst_structure_get_name(structure);

            //if the stream contains rgb frames...
            if (g_str_has_prefix (format, "video/x-raw"))
            {
                if (yarp_handler->port_type == yarp_handler_class::port_type_enum::RGB_TYPE)
                {
                   int width = 0;
                   int height = 0;
                   gst_structure_get_int(structure, "width", &width);
                   gst_structure_get_int(structure, "height", &height);
                   //yCDebug(YVSNK_COMP) << width << height;

                   static yarp::sig::ImageOf<yarp::sig::PixelRgb> data;
                   //data.resize(width, height);
                   data.setQuantum(8);
                   data.setExternal(info.data, width, height);
                   yarp_handler->output_port.write(data);
                }
                else if (yarp_handler->port_type == yarp_handler_class::port_type_enum::BINARY_TYPE)
                {
                   yarp::os::Bottle bot;
                   yarp::os::Value data_val(info.data,info.size);
                   bot.addInt64(info.size);
                   bot.add(data_val);
                   yarp_handler->output_port.write(bot);
                }
                else
                {
                   yCError(YVSNK_COMP) << "Invalid yarp output format. Please choose either yarp image or binary.";
                }
            }
            //if the stream contains encoded binary data
            else if (g_str_has_prefix (format, "video/x-h264") ||
                     g_str_has_prefix (format, "video/x-h265"))
            {
                if (yarp_handler->port_type == yarp_handler_class::port_type_enum::BINARY_TYPE)
                {
                   yarp::os::Bottle bot;
                   yarp::os::Value data_val(info.data, info.size);
                   bot.addInt64(info.size);
                   bot.add(data_val);
                   yarp_handler->output_port.write(bot);
                }
                else
                {
                   yCError(YVSNK_COMP) << "The input frame is encoded. Only binary output is allowed on the yarp port.";
                }
            }
            else
            {
                yCError(YVSNK_COMP) << "Invalid input stream. Only video/x-raw or video/x-h264 video/x-h265 is currently implemented.";
            }
        }

        gst_buffer_unmap(buf, &info);
    }
    return GST_FLOW_OK;
}

/* Register the plugin */
static gboolean yarp_video_sink_plugin_init(GstPlugin* plugin)
{
    return gst_element_register(plugin, "yarpvideosink", GST_RANK_NONE, gst_yarp_video_sink_get_type());
}

#define VERSION "1.0"
#define PACKAGE "BBB"

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    yarpvideosink,
    "Yarp Video Sink",
    yarp_video_sink_plugin_init,
    "1.0",
    "LGPL",
    "GStreamer",
    "https://gstreamer.freedesktop.org/")

// TO test it:
// export GST_PLUGIN_PATH=$GST_PLUGIN_PATH:C:\Software\iCubSoftware\yarp\build\bin\Release
// set GST_PLUGIN_PATH=%GST_PLUGIN_PATH%;C:\Software\iCubSoftware\yarp\build\bin\Release
// gst-inspect-1.0 yarpvideosinkd
// gst-launch-1.0 videotestsource  !yarpvideosinkd
// gst-launch-1.0 yarptestsourced localPortname="/aaa" !videoconvert !x264enc !h264parse !avdec_h264 !videoconvert !autovideosink

// grabber_yarp          ->             -> fast_tcp -> *porta_rgb_2_gs_image* -> sink
// telecamera -> h264enc -> blob2porta  -> fast_tcp -> porta_blob_2_gs_image -> 264dec -> sink

/*
C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0.lib
C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstapp-1.0.lib
C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstbase-1.0.lib
C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstbase-1.0.lib
C:\Software\gstreamer\1.0\msvc_x86_64\lib\gstvideo-1.0.lib
*/
