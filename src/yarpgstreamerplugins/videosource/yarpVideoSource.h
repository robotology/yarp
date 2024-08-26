/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GST_YARP_VIDEO_SOURCE_H
#define GST_YARP_VIDEO_SOURCE_H

#include <condition_variable>

#include <gst/base/gstpushsrc.h>
#include <gst/gst.h>
#include <gst/video/video.h>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>

YARP_LOG_COMPONENT(YVS_COMP, "yarp.gstreamerplugin.yarpvideosource")

class yarp_handler_class
{
    public:
    enum port_type_enum
    {
        RGB_TYPE = 0,
        BINARY_TYPE = 1
    };

    class input_reader : public yarp::os::PortReader
    {
        port_type_enum type=port_type_enum::RGB_TYPE;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> priv_data_image;
        yarp::os::Bottle                        priv_data_bottle;

    public:
        std::mutex image_mutex;
        std::mutex cvar_mutex;
        std::condition_variable cvar;
        bool frame_ready = false;
        unsigned char* image_buffer=nullptr;
        size_t image_size=0;
        double timestamp=0;

        port_type_enum get_port_type()
        {
            return type;
        }

        void set_port_type(port_type_enum t)
        {
            type = t;
        }

    public:
        virtual bool read(yarp::os::ConnectionReader& connection) override
        {
            std::unique_lock lk(cvar_mutex);
            frame_ready = false;

            bool ret=true;

            if (type == port_type_enum::RGB_TYPE)
            {
                image_mutex.lock();
                ret = priv_data_image.read(connection);
                image_buffer = priv_data_image.getRawImage();
                image_size = priv_data_image.getRawImageSize();
                timestamp = yarp::os::Time::now();
                frame_ready = true;
                cvar.notify_all();

                image_mutex.unlock();
            }
            else if (type == port_type_enum::BINARY_TYPE)
            {
                image_mutex.lock();
                ret = priv_data_bottle.read(connection);
                priv_data_bottle.get(0).asInt64();
                image_buffer = (unsigned char*) priv_data_bottle.get(1).asBlob();
                image_size = priv_data_bottle.get(1).asBlobLength();
                timestamp = yarp::os::Time::now();
                frame_ready = true;
                cvar.notify_all();
                image_mutex.unlock();
            }
            else
            {
                ret=false;
            }

            if (ret==false)
            {
                yCError(YVS_COMP) << "Data type conversion failed in read(yarp::os::ConnectionReader&)";
            }
            return true;
        }

        input_reader() = default;
    };

public:
    yarp::os::Network yarpnet;
    yarp::os::Port input_port;
    input_reader input_port_reader;
    std::string input_port_name = "/gstreamer/yarp_plugin:i";
    std::string remote_port_name = "";
    std::string connection_protocol = "fast_tcp";
    int verbosity_level = 0;

public:
    yarp_handler_class()
    {
        input_port_reader.set_port_type(yarp_handler_class::port_type_enum::RGB_TYPE);
        input_port.setReader(input_port_reader);
    }
    virtual ~yarp_handler_class()
    {
    }
};

#endif // GST_YARP_VIDEO_SOURCE_H
