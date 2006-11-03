// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


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
    if (active) {
        printf("Did you just try to open the same ServerFrameGrabber twice?\n");
        return false;
    }

    // for AV, control whether output goes on a single port or multiple
    bool separatePorts = false;

    p.setReader(*this);
        
    yarp::os::Value *name;

    if (config.check("subdevice",name)) {
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.setMonitor(config.getMonitor(),
                         name->toString().c_str()); // pass on any monitoring
            p.fromString(config.toString());
            p.put("device",name->toString());
            p.unput("subdevice");
            poly.open(p);
        } else {
            poly.open(*name);
        }
        if (!poly.isValid()) {
            //printf("cannot make <%s>\n", name->toString().c_str());
            return false;
        }
    } else {
        printf("\"--subdevice <name>\" not set for server_framegrabber\n");
        return false;
    }
    if (poly.isValid()) {
        IAudioVisualStream *str;
        poly.view(str);
        bool a = true;
        bool v = true;
        if (str!=NULL) {
            a = str->hasAudio();
            v = str->hasVideo();
        } 
        if (v) {
            poly.view(fgImage);
        }
        if (a) {
            poly.view(fgSound);
        }
        if (a&&v) {
            poly.view(fgAv);
        }
        poly.view(fgCtrl);
    }

    canDrop = !config.check("no_drop");
    addStamp = config.check("stamp");

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


    if (fgAv&&!config.check("shared-ports")) {
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
            thread.attach(new DataWriter2<yarp::sig::ImageOf<yarp::sig::PixelRgb>, yarp::sig::Sound>(p,*p2,*this,canDrop,addStamp));
        } else {
            printf("Grabber for images and sound (in shared port)\n");
            thread.attach(new DataWriter<ImageRgbSound>(p,*this,canDrop,
                                                        addStamp));
        }
    } else if (fgImage!=NULL) {
        printf("Grabber for images\n");
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelRgb> >(p,*this,canDrop,addStamp));
    } else if (fgSound!=NULL) {
        printf("Grabber for sound\n");
        thread.attach(new DataWriter<yarp::sig::Sound>(p,*this,canDrop));
    } else {
        printf("subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    thread.open(config.check("framerate",Value("0")).asDouble());
    active = true;
    return true;
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
			case VOCAB_WHITE:
				ok = setWhiteBalance(cmd.get(2).asDouble(),
									 cmd.get(3).asDouble());
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
