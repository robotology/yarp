/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_REMOTEFRAMEGRABBER_H
#define YARP_DEV_REMOTEFRAMEGRABBER_H

#include <cstring>          // for memcpy

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/FrameGrabberControlImpl.h>
#include <yarp/dev/IVisualParamsImpl.h>
#include <yarp/dev/GenericVocabs.h>

#include <mutex>


class ImplementDC1394 :
        public yarp::dev::IFrameGrabberControlsDC1394
{
private:
    yarp::os::Port* _port{nullptr};

public:
    void init(yarp::os::Port *__port)
    {
        _port = __port;
    }

private:
    bool setCommand(int code, double v)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addFloat64(v);
        _port->write(cmd,response);
        return true;
    }

    bool setCommand(int code, double b, double r)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addFloat64(b);
        cmd.addFloat64(r);
        _port->write(cmd,response);
        return true;
    }

    double getCommand(int code) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        _port->write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asFloat64();
    }

    bool getCommand(int code, double &b, double &r) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        _port->write(cmd,response);
        // response should be [cmd] [name] value
        b=response.get(2).asFloat64();
        r=response.get(3).asFloat64();
        return true;
    }

public:

    // 12
    unsigned int getVideoModeMaskDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETMSK);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 13
    unsigned int getVideoModeDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETVMD);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 14
    bool setVideoModeDC1394(int video_mode) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETVMD);
        cmd.addInt32(video_mode);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 15
    unsigned int getFPSMaskDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETFPM);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 16
    unsigned int getFPSDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETFPS);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 17
    bool setFPSDC1394(int fps) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETFPS);
        cmd.addInt32(fps);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 18
    unsigned int getISOSpeedDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETISO);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 19
    bool setISOSpeedDC1394(int speed) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETISO);
        cmd.addInt32(speed);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 20
    unsigned int getColorCodingMaskDC1394(unsigned int video_mode) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETCCM);
        cmd.addInt32(video_mode);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 21
    unsigned int getColorCodingDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETCOD);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt32();
        //return response.get(0).asInt32()!=0? true:false;
    }
    // 22
    bool setColorCodingDC1394(int coding) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETCOD);
        cmd.addInt32(coding);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    bool getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETF7M);
        _port->write(cmd,response);

        xdim=response.get(0).asInt32();
        ydim=response.get(1).asInt32();
        xstep=response.get(2).asInt32();
        ystep=response.get(3).asInt32();
        xoffstep=response.get(4).asInt32();
        yoffstep=response.get(5).asInt32();
        return response.get(0).asInt32()!=0? true:false;
    }
    // 26
    bool getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETWF7);
        _port->write(cmd,response);
        xdim=response.get(0).asInt32();
        ydim=response.get(1).asInt32();
        x0=response.get(2).asInt32();
        y0=response.get(3).asInt32();
        return response.get(0).asInt32()!=0? true:false;
    }
    // 27
    bool setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETWF7);
        cmd.addInt32(xdim);
        cmd.addInt32(ydim);
        cmd.addInt32(x0);
        cmd.addInt32(y0);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 28
    bool setOperationModeDC1394(bool b1394b) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETOPM);
        cmd.addInt32(int(b1394b));
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }
    // 29
    bool getOperationModeDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETOPM);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 30
    bool setTransmissionDC1394(bool bTxON) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETTXM);
        cmd.addInt32(int(bTxON));
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }
    // 31
    bool getTransmissionDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETTXM);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 34
    bool setBroadcastDC1394(bool onoff) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETBCS);
        cmd.addInt32((int)onoff);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }
    // 35
    bool setDefaultsDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETDEF);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }
    // 36
    bool setResetDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETRST);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }
    // 37
    bool setPowerDC1394(bool onoff) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETPWR);
        cmd.addInt32((int)onoff);
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 38
    bool setCaptureDC1394(bool bON) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETCAP);
        cmd.addInt32(int(bON));
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 39
    bool setBytesPerPacketDC1394(unsigned int bpp) override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRSETBPP);
        cmd.addInt32(int(bpp));
        _port->write(cmd,response);
        return response.get(0).asInt32()!=0? true:false;
    }

    // 40
    unsigned int getBytesPerPacketDC1394() override
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
        cmd.addVocab(VOCAB_DRGETBPP);
        _port->write(cmd,response);
        return (unsigned)response.get(0).asInt32();
    }
};

/**
 * @ingroup dev_impl_network_clients
 *
 * \section remoteFrameGrabber
 * Connect to a ServerFrameGrabber.  See ServerFrameGrabber for
 * the network protocol used.
 */
class RemoteFrameGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::FrameGrabberControls_Sender,
        public ImplementDC1394,
        public yarp::dev::Implement_RgbVisualParams_Sender,
        public yarp::dev::DeviceDriver
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

        if (reader.read(true)!=NULL) {
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
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_IMAGE);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_CROP);
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
        if( (response.get(0).asVocab() != VOCAB_CROP) || (response.size() != 5) || (!response.get(4).isBlob()))
        {
            yError() << "getImageCrop: malformed response message. Size is " << response.size();
            return false;
        }

        image.resize(response.get(2).asInt32(), response.get(3).asInt32());
        unsigned char *pixelOut    = image.getRawImage();

        if(response.get(4).asBlob())
            memcpy(pixelOut, response.get(4).asBlob(), (size_t) image.getRawImageSize());

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
        yTrace();
        yDebug() << "config is " << config.toString();

        remote = config.check("remote",yarp::os::Value(""),
                              "port name of real grabber").asString();
        local = config.check("local",yarp::os::Value("..."),
                             "port name to use locally").asString();
        std::string carrier =
            config.check("stream",yarp::os::Value("tcp"),
                         "carrier to use for streaming").asString();
        port.open(local);
        if (remote!="") {
            yInfo() << "connecting "  << local << " to " << remote;

            if(!config.check("no_stream") )
            {
                no_stream = false;
                if(!yarp::os::Network::connect(remote,local,carrier))
                    yError() << "cannot connect "  << local << " to " << remote;
            }
            else
                no_stream = true;

            // reverse connection for RPC
            // could choose to do this only on need

            yarp::os::Network::connect(local,remote);
        }
        reader.attach(port);
        ImplementDC1394::init(&port);
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
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addFloat64(v);
        port.write(cmd,response);
        return true;
    }

    bool setCommand(int code, double b, double r)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addFloat64(b);
        cmd.addFloat64(r);
        port.write(cmd,response);
        return true;
    }

    double getCommand(int code) const {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asFloat64();
    }

    bool getCommand(int code, double &b, double &r) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        b=response.get(2).asFloat64();
        r=response.get(3).asFloat64();
        return true;
    }
};

#endif // YARP_DEV_REMOTEFRAMEGRABBER_H
