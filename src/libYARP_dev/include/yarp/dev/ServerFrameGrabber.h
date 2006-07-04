// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef _YARP2_SERVERFRAMEGRABBER_
#define _YARP2_SERVERFRAMEGRABBER_

#include <stdio.h>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>


namespace yarp {
    namespace dev {
        class ServerFrameGrabber;
    }
}

#define VOCAB_BRIGHTNESS VOCAB3('b','r','i')
#define VOCAB_SHUTTER VOCAB4('s','h','u','t')
#define VOCAB_GAIN VOCAB4('g','a','i','n')
#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_WIDTH VOCAB1('w')
#define VOCAB_HEIGHT VOCAB1('h')




class yarp::dev::ServerFrameGrabber : public DeviceDriver, 
	    public yarp::os::Thread,
            public yarp::os::PortReader,
            public IFrameGrabberImage, public IFrameGrabberControls,
            public IAudioGrabberSound
            // convenient to put these here just to make sure all
            // methods get implemented
{
private:
    bool spoke;
    yarp::os::Port p;
    yarp::os::PortWriterBuffer<yarp::sig::ImageOf<yarp::sig::PixelRgb> > writer;
    yarp::os::PortWriterBuffer<yarp::sig::Sound> writerSound;
    PolyDriver poly;
    IFrameGrabberImage *fgImage;
    IAudioGrabberSound *fgSound;
    IFrameGrabberControls *fgCtrl;
    yarp::os::Property settings;
public:
    ServerFrameGrabber() {
        fgImage = NULL;
        fgSound = NULL;
        fgCtrl = NULL;
		spoke = false;
    }
    
    virtual bool open() {
        return false;
    }
    
    virtual bool close() {
        return true;
    }
    
    virtual bool open(yarp::os::Searchable& prop) {
        p.setReader(*this);
        
        yarp::os::Value *name;
        if (prop.check("subdevice",name)) {
            printf("Subdevice %s\n", name->toString().c_str());
            if (name->isString()) {
                // maybe user isn't doing nested configuration
                yarp::os::Property p;
                p.fromString(prop.toString());
                p.put("device",name->toString());
                poly.open(p);
            } else {
                poly.open(*name);
            }
            if (!poly.isValid()) {
                printf("cannot make <%s>\n", name->toString().c_str());
            }
        } else {
            printf("\"--subdevice <name>\" not set for server_framegrabber\n");
            return false;
        }
        if (poly.isValid()) {
            poly.view(fgImage);
            poly.view(fgSound);
            poly.view(fgCtrl);
        }

        if (fgImage!=NULL) {
            writer.attach(p);
        } else {
            writerSound.attach(p);
        }

        if (prop.check("name",name)) {
            p.open(name->asString());
        } else {
            p.open("/grabber");
        }

        if (fgImage!=NULL||fgSound!=NULL) {
            start();
            return true;
        }
        printf("subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    virtual void run() {
        printf("Server grabber starting\n");
        while (!isStopping()) {
            if (fgImage!=NULL) {
                // for now, sound and image are mutually exclusive
                yarp::sig::ImageOf<yarp::sig::PixelRgb>& img = writer.get();
                getImage(img);
                if (!spoke) {
                    printf("Network framegrabber writing a %dx%d image...\n",
                           img.width(),img.height());
                    spoke = true;
                }
                writer.write();
            } else {
                // for now, sound and image are mutually exclusive
                yarp::sig::Sound& snd = writerSound.get();
                getSound(snd);
                if (!spoke) {
                    printf("Network framegrabber writing a %dx%d sound...\n",
                           snd.getSamples(),snd.getChannels());
                    spoke = true;
                }
                writerSound.write();
            }
        }
        printf("Server grabber stopping\n");
    }

    virtual bool read(ConnectionReader& connection) {
        yarp::os::Bottle cmd, response;
        cmd.read(connection);
        printf("command received: %s\n", cmd.toString().c_str());
        int code = cmd.get(0).asVocab();
        switch (code) {
        case VOCAB_SET:
            printf("set command received\n");
            {
                bool ok = false;
                switch(cmd.get(1).asVocab()) {
                case VOCAB_BRIGHTNESS:
                    ok = setBrightness(cmd.get(2).asDouble());
                    break;
                case VOCAB_SHUTTER:
                    ok = setShutter(cmd.get(2).asDouble());
                    break;
                case VOCAB_GAIN:
                    ok = setGain(cmd.get(2).asDouble());
                    break;
                }
            }
            break;
        case VOCAB_GET:
            printf("get command received\n");
            {
                bool ok = false;
                response.addVocab(VOCAB_IS);
                response.add(cmd.get(1));
                switch(cmd.get(1).asVocab()) {
                case VOCAB_BRIGHTNESS:
                    ok = true;
                    response.addDouble(getBrightness());
                    break;
                case VOCAB_SHUTTER:
                    ok = true;
                    response.addDouble(getShutter());
                    break;
                case VOCAB_GAIN:
                    ok = true;
                    response.addDouble(getGain());
                    break;
                case VOCAB_WIDTH:
                    // normally, this would come from stream information
                    ok = true;
                    response.addInt(width());
                    break;
                case VOCAB_HEIGHT:
                    // normally, this would come from stream information
                    ok = true;
                    response.addInt(height());
                    break;
                }
                if (!ok) {
                    // leave answer blank
                }
            }
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
        if (fgImage==NULL) { return false; }
        return fgImage->getImage(image);
    }
    
    virtual bool getSound(yarp::sig::Sound& sound) {
        if (fgSound==NULL) { return false; }
        return fgSound->getSound(sound);
    }
    
    virtual int height() const {
        if (fgImage==NULL) { return 0; }
        return fgImage->height();
    }

    virtual int width() const {
        if (fgImage==NULL) { return 0; }
        return fgImage->width();
    }

    virtual bool setBrightness(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setBrightness(v);
    }

    virtual bool setShutter(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setShutter(v);
    }

    virtual bool setGain(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setGain(v);
    }

    virtual double getBrightness() const {
        if (fgCtrl==NULL) { return 0; }
        return fgCtrl->getBrightness();
    }

    virtual double getShutter() const {
        if (fgCtrl==NULL) { return 0; }
        return fgCtrl->getShutter();
    }

    virtual double getGain() const {
        if (fgCtrl==NULL) { return 0; }
        return fgCtrl->getGain();
    }
};

#endif

