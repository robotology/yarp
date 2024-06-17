#ifndef GST_YARP_VIDEO_SINK_H
#define GST_YARP_VIDEO_SINK_H

#include <gst/base/gstpushsrc.h>
#include <gst/gst.h>
#include <gst/video/video.h>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>

YARP_LOG_COMPONENT(YVSNK_COMP, "yarp.gstreamerplugin.yarpvideosink")

class yarp_handler_class
{
public:
    enum port_type_enum
    {
        RGB_TYPE = 0,
        BINARY_TYPE = 1
    } port_type;
    yarp::os::Network yarpnet;
    yarp::os::Port output_port;
    std::string output_port_name = "/gstreamer/yarp_plugin:o";
    std::string remote_port_name = "";
    std::string connection_protocol = "fast_tcp";
    int verbosity_level = 0;

public:
    yarp_handler_class()
    {
    }
    virtual ~yarp_handler_class()
    {
    }
};

#endif // GST_YARP_VIDEO_SINK_H