// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/String.h>
#include <yarp/Logger.h>

#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


bool ServerFrameGrabber::open(yarp::os::Searchable& config) {

    // for AV, control whether output goes on a single port or multiple
    bool separatePorts = false;

    p.setReader(*this);
        
    yarp::os::Value *name;

    if (config.check("subdevice",name)) {
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.fromString(config.toString());
            p.put("device",name->toString());
            p.unput("subdevice");
            poly.open(p);
        } else {
            poly.open(*name);
        }
        if (!poly.isValid()) {
            printf("cannot make <%s>\n", name->toString().c_str());
            return false;
        }
    } else {
        printf("\"--subdevice <name>\" not set for server_framegrabber\n");
        return false;
    }
    if (poly.isValid()) {
        poly.view(fgImage);
        poly.view(fgSound);
        poly.view(fgAv);
        poly.view(fgCtrl);
    }

    /*
    if (fgImage!=NULL) {
        writer.attach(p);
    } else {
        writerSound.attach(p);
    }
    */

    if (config.check("name",name)) {
        p.open(name->asString());
    } else {
        p.open("/grabber");
    }

    double framerate=0;
    int period=0;
    if (config.check("framerate", name))
        {
            framerate=name->asDouble();
        }


    if (!config.check("shared-ports")) {
        separatePorts = true;
        YARP_ASSERT(p2==NULL);
        p2 = new Port;
        YARP_ASSERT(p2!=NULL);
        if (config.check("name2",name)) {
            p2->open(name->asString());
        } else {
            p2->open("/grabber2");
        }
    }
        
    if (fgAv!=NULL) {
        if (separatePorts) {
            printf("Grabber for images and sound (in separate ports)\n");
            YARP_ASSERT(p2!=NULL);
            thread.attach(new DataWriter2<yarp::sig::ImageOf<yarp::sig::PixelRgb>, yarp::sig::Sound>(p,*p2,*this));
        } else {
            printf("Grabber for images and sound (in shared port)\n");
            thread.attach(new DataWriter<ImageRgbSound>(p,*this));
        }
    } else if (fgImage!=NULL) {
        printf("Grabber for images\n");
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelRgb> >(p,*this));
    } else if (fgSound!=NULL) {
        printf("Grabber for sound\n");
        thread.attach(new DataWriter<yarp::sig::Sound>(p,*this));
    } else {
        printf("subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    thread.open(config.check("framerate",Value("0")).asDouble());
    return true;
}


/*
void ServerFrameGrabber::run() 
{
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
*/



bool ServerFrameGrabber::read(ConnectionReader& connection) {
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
