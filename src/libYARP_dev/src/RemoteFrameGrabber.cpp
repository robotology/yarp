// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/String.h>

using namespace yarp::os;
using namespace yarp::sig;


namespace yarp{
    namespace dev {
      class RemoteFrameGrabber;
    }
}

class yarp::dev::RemoteFrameGrabber : public IFrameGrabberImage, 
            public IFrameGrabberControls,
            public DeviceDriver {
private:
    Port port;
    PortReaderBuffer<ImageOf<PixelRgb> > reader;
    String remote;
    String local;
    int lastHeight;
    int lastWidth;
public:
    RemoteFrameGrabber() {
        lastHeight = 0;
        lastWidth = 0;
    }

    virtual ~RemoteFrameGrabber() {
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        
        if (reader.read(true)!=NULL) {
            image = *(reader.lastRead());
            lastHeight = image.height();
            lastWidth = image.width();
            return true;
        }
        return false;
    }

    virtual int height() const {
        return lastHeight;
    }

    virtual int width() const {
        return lastWidth;
    }

    virtual bool open(){
        return true;
    }

    virtual bool open(Property& config){
        remote = config.find("remote").asString().c_str();
        local = config.find("local").asString().c_str();
        if (local!="") {
            port.open(local.c_str());
        }
        if (remote!="") {
            Network::connect(remote.c_str(),local.c_str());

            // reverse connection for RPC
            // could choose to do this only on need
            Network::connect(local.c_str(),remote.c_str());
        }
        reader.attach(port);
        return true;
    }

    virtual bool close() {
        return true;
    }

    bool setCommand(int code, double v) {
        Bottle cmd;
        cmd.addVocab(VOCAB3('s','e','t'));
        cmd.addVocab(code);
        cmd.addDouble(v);
        port.write(cmd);
        return true;
    }

    double getCommand(int code) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB3('g','e','t'));
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asDouble();
    }

    virtual bool setBrightness(double v) {
        return setCommand(VOCAB3('b','r','i'),v);
    }

    virtual bool setShutter(double v) {
        return setCommand(VOCAB4('s','h','u','t'),v);
    }

    virtual bool setGain(double v) {
        return setCommand(VOCAB4('g','a','i','n'),v);
    }

    virtual double getBrightness() const {
        return getCommand(VOCAB3('b','r','i'));
    }

    virtual double getShutter() const {
        return getCommand(VOCAB4('s','h','u','t'));
    }

    virtual double getGain() const {
        return getCommand(VOCAB4('g','a','i','n'));
    }
};

yarp::dev::DeviceDriver *createRemoteFrameGrabber() {
    return new yarp::dev::RemoteFrameGrabber();
}
