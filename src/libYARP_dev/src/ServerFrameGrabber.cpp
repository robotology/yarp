// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/String.h>

#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file


bool ServerFrameGrabber::open(yarp::os::Searchable& config) {
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
        poly.view(fgCtrl);
    }

    if (fgImage!=NULL) {
        writer.attach(p);
    } else {
        writerSound.attach(p);
    }

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
        
    if (framerate!=0)
        {
            period=(int) (1000.0/framerate+0.5);
            printf("Setting framerate to: %.0lf[Hz] (thread period %d[ms])\n", framerate, period);
        }
    else
        {
            printf("No framerate specified, polling the device\n");
            period=0; //continuous
        }
        
    RateThread::setRate(period);

    if (fgImage!=NULL||fgSound!=NULL) {
        start();
        return true;
    }
    printf("subdevice <%s> doesn't look like a framegrabber\n",
           name->toString().c_str());
    return false;
}


void ServerFrameGrabber::doLoop() 
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

