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


ServerFrameGrabber::ServerFrameGrabber() {
    fgImage = NULL;
    fgSound = NULL;
    fgAv = NULL;
    fgCtrl = NULL;
	fgTimed = NULL;
    spoke = false;
    canDrop = true;
    addStamp = false;
    active = false;
    singleThreaded = false;
    p2 = NULL;
}

bool ServerFrameGrabber::open(yarp::os::Searchable& config) {
    if (active) {
        printf("Did you just try to open the same ServerFrameGrabber twice?\n");
        return false;
    }

    // for AV, control whether output goes on a single port or multiple
    bool separatePorts = false;

    p.setReader(*this);
        
    yarp::os::Value *name;

    if (config.check("subdevice",name,"name (or nested configuration) of device to wrap")) {
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
            Bottle subdevice = config.findGroup("subdevice").tail();
            poly.open(subdevice);
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
		poly.view(fgTimed);
    }

    canDrop = !config.check("no_drop","if present, use strict policy for sending data");
    addStamp = config.check("stamp","if present, add timestamps to data");

    p.open(config.check("name",Value("/grabber"),
                        "name of port to send data on").asString());

    double framerate=0;
    if (config.check("framerate", name, 
                     "maximum rate in Hz to read from subdevice")) {
        framerate=name->asDouble();
    }

    if (fgAv&&
        !config.check("shared-ports",
                      "If present, send audio and images on same port")) {
        separatePorts = true;
        YARP_ASSERT(p2==NULL);
        p2 = new Port;
        YARP_ASSERT(p2!=NULL);
        p2->open(config.check("name2",Value("/grabber2"),
                              "Name of second port to send data on, when audio and images sent seperately").asString());
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
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelRgb> >(p,*this,canDrop,addStamp,fgTimed));
    } else if (fgSound!=NULL) {
        printf("Grabber for sound\n");
        thread.attach(new DataWriter<yarp::sig::Sound>(p,*this,canDrop));
    } else {
        printf("subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    singleThreaded = 
        (bool)config.check("single_threaded",
                           "if present, operate in single threaded mode");
    thread.open(config.check("framerate",Value("0")).asDouble(),
                singleThreaded);
    active = true;

/*
#define VOCAB_BRIGHTNESS VOCAB3('b','r','i')
#define VOCAB_EXPOSURE VOCAB4('e','x','p','o')
#define VOCAB_SHARPNESS VOCAB4('s','h','a','r')
#define VOCAB_WHITE VOCAB4('w','h','i','t')
#define VOCAB_HUE VOCAB3('h','u','e')
#define VOCAB_SATURATION VOCAB4('s','a','t','u')
#define VOCAB_GAMMA VOCAB4('g','a','m','m')
#define VOCAB_SHUTTER VOCAB4('s','h','u','t')
#define VOCAB_GAIN VOCAB4('g','a','i','n')
#define VOCAB_IRIS VOCAB4('i','r','i','s')
*/

    DeviceResponder::makeUsage();
    addUsage("[set] [bri] $fBrightness", "set brightness");
    addUsage("[set] [expo] $fExposure", "set exposure");    
    addUsage("[set] [shar] $fSharpness", "set sharpness");
    addUsage("[set] [whit] $fBlue $fRed", "set white balance");    
    addUsage("[set] [hue] $fHue", "set hue");
    addUsage("[set] [satu] $fSaturation", "set saturation");    
    addUsage("[set] [gamm] $fGamma", "set gamma");        
    addUsage("[set] [shut] $fShutter", "set shutter");        
    addUsage("[set] [gain] $fGain", "set gain");
    addUsage("[set] [iris] $fIris", "set iris");

    addUsage("[get] [bri]",  "get brightness");
    addUsage("[get] [expo]", "get exposure");    
    addUsage("[get] [shar]", "get sharpness");
    addUsage("[get] [whit]", "get white balance");    
    addUsage("[get] [hue]",  "get hue");
    addUsage("[get] [satu]", "get saturation");    
    addUsage("[get] [gamm]", "get gamma");        
    addUsage("[get] [shut]", "get shutter");        
    addUsage("[get] [gain]", "get gain");
    addUsage("[get] [iris]", "get iris");
    
    addUsage("[get] [w]", "get width of image");
    addUsage("[get] [h]", "get height of image");

    return true;
}


bool ServerFrameGrabber::respond(const yarp::os::Bottle& cmd, 
                                 yarp::os::Bottle& response) {
    int code = cmd.get(0).asVocab();
    bool rec = false;
    bool ok = false;
    switch (code) {
    case VOCAB_SET:
        printf("set command received\n");
        {
            switch(cmd.get(1).asVocab()) 
			{
			case VOCAB_BRIGHTNESS:
                ok = setBrightness(cmd.get(2).asDouble());
                rec = true;
                break;
            case VOCAB_EXPOSURE:
                ok = setExposure(cmd.get(2).asDouble());
                rec = true;
                break;
			case VOCAB_SHARPNESS:
                ok = setSharpness(cmd.get(2).asDouble());
                rec = true;
                break;
			case VOCAB_WHITE:
				ok = setWhiteBalance(cmd.get(2).asDouble(),cmd.get(3).asDouble());
                rec = true;
				break;
			case VOCAB_HUE:
                ok = setHue(cmd.get(2).asDouble());
                rec = true;
                break;
			case VOCAB_SATURATION:
                ok = setSaturation(cmd.get(2).asDouble());
                rec = true;
                break;
			case VOCAB_GAMMA:
                ok = setGamma(cmd.get(2).asDouble());
                rec = true;
                break;
            case VOCAB_SHUTTER:
                ok = setShutter(cmd.get(2).asDouble());
                rec = true;
                break;
            case VOCAB_GAIN:
                ok = setGain(cmd.get(2).asDouble());
                rec = true;
                break;
            case VOCAB_IRIS:
                ok = setIris(cmd.get(2).asDouble());
                rec = true;
                break;
			/*
			case VOCAB_TEMPERATURE:
                ok = setTemperature(cmd.get(2).asDouble());
                rec = true;
                break;
			case VOCAB_WHITE_SHADING:
                ok = setWhiteShading(cmd.get(2).asDouble(),cmd.get(3).asDouble(),cmd.get(4).asDouble());
                rec = true;
                break;
			case VOCAB_OPTICAL_FILTER:
                ok = setOpticalFilter(cmd.get(2).asDouble());
                rec = true;
                break;
			case VOCAB_CAPTURE_QUALITY:
                ok = setCaptureQuality(cmd.get(2).asDouble());
                rec = true;
                break;
            */
            }
        }
        break;
    case VOCAB_GET:
        printf("get command received\n");
        {
            response.addVocab(VOCAB_IS);
            response.add(cmd.get(1));
            switch(cmd.get(1).asVocab()) 
            {
            case VOCAB_BRIGHTNESS:
                ok = true;
                response.addDouble(getBrightness());
                rec = true;
                break;
            case VOCAB_EXPOSURE:
                ok = true;
                response.addDouble(getExposure());
                rec = true;
                break;
            case VOCAB_SHARPNESS:
                ok = true;
                response.addDouble(getSharpness());
                rec = true;
                break;
			case VOCAB_WHITE:
			    {
			    	ok = true;
			    	double b=0;
			    	double r=0;
    
			    	getWhiteBalance(b,r);
			    	response.addDouble(b);
			    	response.addDouble(r);
			    	rec=true;
			    }
			    break;                
            case VOCAB_HUE:
                ok = true;
                response.addDouble(getHue());
                rec = true;
                break;
            case VOCAB_SATURATION:
                ok = true;
                response.addDouble(getSaturation());
                rec = true;
                break;   
            case VOCAB_GAMMA:
                ok = true;
                response.addDouble(getGamma());
                rec = true;
                break;   
            case VOCAB_SHUTTER:
                ok = true;
                response.addDouble(getShutter());
                rec = true;
                break;
            case VOCAB_GAIN:
                ok = true;
                response.addDouble(getGain());
                rec = true;
                break;
            case VOCAB_IRIS:
                ok = true;
                response.addDouble(getIris());
                rec = true;
                break;
            /*
            case VOCAB_CAPTURE_QUALITY:
                ok = true;
                response.addDouble(getCaptureQuality());
                rec = true;
                break;
            case VOCAB_OPTICAL_FILTER:
                ok = true;
                response.addDouble(getOpticalFilter());
                rec = true;
                break;
            */
            case VOCAB_WIDTH:
                // normally, this would come from stream information
                ok = true;
                response.addInt(width());
                rec = true;
                break;
            case VOCAB_HEIGHT:
                // normally, this would come from stream information
                ok = true;
                response.addInt(height());
                rec = true;
                break;
            }

            if (!ok) {
                // leave answer blank
            }
        }
        break;
    }
    if (!rec) {
        return DeviceResponder::respond(cmd,response);
    }
    return ok;
}


/*
bool ServerFrameGrabber::read(ConnectionReader& connection) {
    yarp::os::Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
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

			case VOCAB_WHITE:

				double r;

				double g;

				ok=getWhiteBalance(r, g);

				response.addDouble(r);

				response.addDouble(g);
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
*/

bool ServerFrameGrabber::startService() {
    if (singleThreaded) {
        return false;
    }
    return active;
}


bool ServerFrameGrabber::updateService() {
    if (singleThreaded) {
        if (active) {
            thread.step();
        }
        return active;
    }
    return false;
}


