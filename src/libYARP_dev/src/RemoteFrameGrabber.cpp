// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/String.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


namespace yarp{
    namespace dev {
      class RemoteFrameGrabber;
      class ServerFrameGrabber;
    }
}


class yarp::dev::ServerFrameGrabber : public DeviceDriver, public Thread,
            public PortReader,
            public IFrameGrabberImage, public IFrameGrabberControls
            // convenient to put these here just to make sure all
            // methods get implemented
{
private:
    Port p;
    DeviceDriver *dd;
    IFrameGrabberImage *fgImage;
    IFrameGrabberControls *fgCtrl;
    Property settings;
    int ct;
public:
    ServerFrameGrabber() {
        dd = NULL;
        fgImage = NULL;
        fgCtrl = NULL;
        ct = 0;
    }
    
    virtual bool open() {
        return false;
    }
    
    virtual bool close() {
        if (dd!=NULL) {
            dd->close();
        }
        return true;
    }
    
    virtual bool open(Property& prop) {
        p.setReader(*this);
        
        BottleBit *name;
        if (prop.check("name",name)) {
            p.open(name->asString());
        } else {
            p.open("/grabber");
        }
        start();
        return true;
    }

    virtual void run() {
        printf("Fake framegrabber starting\n");
        while (!isStopping()) {
            ImageOf<PixelRgb> img;
            img.resize(40,20);
            img.zero();
            for (int i=0; i<img.width(); i++) {
                img.pixel(i,ct).r = 255;
            }
            ct++;
            if (ct>=img.height()) {
                ct = 0;
            }
            printf("Fake framegrabber wrote an image...\n");
            p.write(img);
            Time::delay(1);
        }
        printf("Fake framegrabber stopping\n");
    }

    virtual bool read(ConnectionReader& connection) {
        Bottle cmd, response;
        cmd.read(connection);
        printf("command received: %s\n", cmd.toString().c_str());
        int code = cmd.get(0).asVocab();
        switch (code) {
        case VOCAB3('s','e','t'):
            printf("set command received\n");
            settings.put(cmd.get(1).asString().c_str(),cmd.get(2));
            break;
        case VOCAB3('g','e','t'):
            printf("get command received\n");
            response.addVocab(VOCAB2('i','s'));
            response.addBit(cmd.get(1));
            response.addBit(settings.find(cmd.get(1).asString().c_str()));
            break;
        }
        if (response.size()>=1) {
            ConnectionWriter *writer = connection.getWriter();
            if (writer!=NULL) {
                response.write(*writer);
                printf("response sent: %s\n", response.toString().c_str());
            }
        }
        return true;
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        return fgImage->getImage(image);
    }
    
    virtual int height() const {
        return fgImage->height();
    }

    virtual int width() const {
        return fgImage->width();
    }

    virtual bool setBrightness(double v) {
        return fgCtrl->setBrightness(v);
    }

    virtual bool setShutter(double v) {
        return fgCtrl->setShutter(v);
    }

    virtual bool setGain(double v) {
        return fgCtrl->setGain(v);
    }

    virtual double getBrightness() const {
        return fgCtrl->getBrightness();
    }

    virtual double getShutter() const {
        return fgCtrl->getShutter();
    }

    virtual double getGain() const {
        return fgCtrl->getGain();
    }
};


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
        return true;;
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

yarp::dev::DeviceDriver *createServerFrameGrabber() {
    return new yarp::dev::ServerFrameGrabber();
}
