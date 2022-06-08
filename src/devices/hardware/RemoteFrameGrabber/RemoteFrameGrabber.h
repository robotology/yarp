/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_REMOTEFRAMEGRABBER_H
#define YARP_DEV_REMOTEFRAMEGRABBER_H

#include <cstring>          // for memcpy

#include <yarp/os/Network.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>

#include <yarp/proto/framegrabber/FrameGrabberControlsDC1394_Forwarder.h>
#include <yarp/proto/framegrabber/FrameGrabberControls_Forwarder.h>
#include <yarp/proto/framegrabber/RgbVisualParams_Forwarder.h>
#include <yarp/proto/framegrabber/CameraVocabs.h>

#include <mutex>

YARP_DECLARE_LOG_COMPONENT(REMOTEFRAMEGRABBER)

/**
 * @ingroup dev_impl_network_clients
 *
 * \section remoteFrameGrabber
 *
 * \brief `remote_grabber`: Connect to a ServerFrameGrabber.  See ServerFrameGrabber for
 * the network protocol used.
 */
class RemoteFrameGrabber :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameGrabberImage,
        public yarp::proto::framegrabber::FrameGrabberControls_Forwarder,
        public yarp::proto::framegrabber::FrameGrabberControlsDC1394_Forwarder,
        public yarp::proto::framegrabber::RgbVisualParams_Forwarder
{
public:
    RemoteFrameGrabber();
    RemoteFrameGrabber(const RemoteFrameGrabber&) = delete;
    RemoteFrameGrabber(RemoteFrameGrabber&&) = delete;
    RemoteFrameGrabber& operator=(const RemoteFrameGrabber&) = delete;
    RemoteFrameGrabber& operator=(RemoteFrameGrabber&&) = delete;
    ~RemoteFrameGrabber() override = default;


    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override
    {
        mutex.lock();
        if(no_stream == true)
        {
            image.zero();
            mutex.unlock();
            return false;
        }

        if (reader.read(true)!=nullptr) {
            image = *(reader.lastRead());
            lastHeight = image.height();
            lastWidth = image.width();
            mutex.unlock();
            return true;
        }
        mutex.unlock();
        return false;
    }

    bool getImageCrop(cropType_id_t cropType, yarp::sig::VectorOf<std::pair<int, int> > vertices, yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override
    {
        yarp::os::Bottle cmd;
        yarp::os::Bottle response;
        cmd.addVocab32(VOCAB_FRAMEGRABBER_IMAGE);
        cmd.addVocab32(VOCAB_GET);
        cmd.addVocab32(VOCAB_CROP);
        cmd.addInt32(cropType);
        yarp::os::Bottle & list = cmd.addList();
        for(size_t i=0; i<vertices.size(); i++)
        {
            list.addInt32(vertices[i].first);
            list.addInt32(vertices[i].second);
        }
        port.write(cmd,response);

        // Parse the response
        image.zero();
        if( (response.get(0).asVocab32() != VOCAB_CROP) || (response.size() != 5) || (!response.get(4).isBlob()))
        {
            yCError(REMOTEFRAMEGRABBER) << "getImageCrop: malformed response message. Size is " << response.size();
            return false;
        }

        image.resize(response.get(2).asInt32(), response.get(3).asInt32());
        unsigned char *pixelOut    = image.getRawImage();

        if (response.get(4).asBlob()) {
            memcpy(pixelOut, response.get(4).asBlob(), (size_t)image.getRawImageSize());
        }

        return true;
    }

    // this is bad!
    int height() const override
    {
        return lastHeight;
    }

    int width() const override
    {
        return lastWidth;
    }

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> local </TD><TD> Port name of this client. </TD></TR>
     * <TR><TD> remote </TD><TD> Port name of server to connect to. </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override
    {
        yCTrace(REMOTEFRAMEGRABBER);
        yCDebug(REMOTEFRAMEGRABBER) << "config is " << config.toString();

        remote = config.check("remote",yarp::os::Value(""),
                              "port name of real grabber").asString();
        local = config.check("local",yarp::os::Value("..."),
                             "port name to use locally").asString();
        std::string carrier =
            config.check("stream",yarp::os::Value("tcp"),
                         "carrier to use for streaming").asString();
        port.open(local);
        if (remote!="") {
            yCInfo(REMOTEFRAMEGRABBER) << "connecting "  << local << " to " << remote;

            if(!config.check("no_stream") )
            {
                no_stream = false;
                if (!yarp::os::Network::connect(remote, local, carrier)) {
                    yCError(REMOTEFRAMEGRABBER) << "cannot connect " << local << " to " << remote;
                }
            } else {
                no_stream = true;
            }

            // reverse connection for RPC
            // could choose to do this only on need

            yarp::os::Network::connect(local,remote);
        }
        reader.attach(port);
        return true;
    }

    bool close() override
    {
        port.close();
//        mutex.lock();   // why does it need this?
        return true;
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    bool setBrightness(double v) override
    {
        return setCommand(VOCAB_BRIGHTNESS, v);
    }
    double getBrightness() override
    {
        return getCommand(VOCAB_BRIGHTNESS);
    }
    bool setExposure(double v) override
    {
        return setCommand(VOCAB_EXPOSURE, v);
    }
    double getExposure() override
    {
        return getCommand(VOCAB_EXPOSURE);
    }

    bool setSharpness(double v) override
    {
        return setCommand(VOCAB_SHARPNESS, v);
    }
    double getSharpness() override
    {
        return getCommand(VOCAB_SHARPNESS);
    }

    bool setWhiteBalance(double blue, double red) override
    {
        return setCommand(VOCAB_WHITE, blue, red);
    }
    bool getWhiteBalance(double &blue, double &red) override
    {
        return getCommand(VOCAB_WHITE, blue, red);
    }

    bool setHue(double v) override
    {
        return setCommand(VOCAB_HUE,v);
    }
    double getHue() override
    {
        return getCommand(VOCAB_HUE);
    }

    bool setSaturation(double v) override
    {
        return setCommand(VOCAB_SATURATION,v);
    }
    double getSaturation() override
    {
        return getCommand(VOCAB_SATURATION);
    }

    bool setGamma(double v) override
    {
        return setCommand(VOCAB_GAMMA,v);
    }
    double getGamma() override
    {
        return getCommand(VOCAB_GAMMA);
    }

    bool setShutter(double v) override
    {
        return setCommand(VOCAB_SHUTTER,v);
    }
    double getShutter() override
    {
        return getCommand(VOCAB_SHUTTER);
    }

    bool setGain(double v) override
    {
        return setCommand(VOCAB_GAIN,v);
    }
    double getGain() override
    {
        return getCommand(VOCAB_GAIN);
    }

    bool setIris(double v) override
    {
        return setCommand(VOCAB_IRIS,v);
    }
    double getIris() override
    {
        return getCommand(VOCAB_IRIS);
    }
#endif

private:
    yarp::os::PortReaderBuffer<yarp::sig::ImageOf<yarp::sig::PixelRgb> > reader;
    yarp::os::Port port;
    std::string remote;
    std::string local;
    std::mutex mutex;
    int lastHeight{0};
    int lastWidth{0};
    bool no_stream{false};

protected:

    IFrameGrabberControlsDC1394 *Ifirewire{nullptr};

    bool setCommand(int code, double v)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab32(VOCAB_SET);
        cmd.addVocab32(code);
        cmd.addFloat64(v);
        port.write(cmd,response);
        return true;
    }

    bool setCommand(int code, double b, double r)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab32(VOCAB_SET);
        cmd.addVocab32(code);
        cmd.addFloat64(b);
        cmd.addFloat64(r);
        port.write(cmd,response);
        return true;
    }

    double getCommand(int code) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab32(VOCAB_GET);
        cmd.addVocab32(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asFloat64();
    }

    bool getCommand(int code, double &b, double &r) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab32(VOCAB_GET);
        cmd.addVocab32(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        b=response.get(2).asFloat64();
        r=response.get(3).asFloat64();
        return true;
    }
};

#endif // YARP_DEV_REMOTEFRAMEGRABBER_H
