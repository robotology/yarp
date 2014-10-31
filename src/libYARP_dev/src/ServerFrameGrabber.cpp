// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Log.h>

#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


ServerFrameGrabber::ServerFrameGrabber() {
    fgImage = NULL;
    fgImageRaw = NULL;
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
        bool vraw = true;
        if (str!=NULL) {
            a = str->hasAudio();
            v = str->hasVideo();
            vraw = str->hasRawVideo();
        }
        if (v) {
            poly.view(fgImage);
        }
        if (vraw) {
            poly.view(fgImageRaw);
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

    p.promiseType(Type::byName("yarp/image")); // TODO: reflect audio options
    p.setWriteOnly();
    p.open(config.check("name",Value("/grabber"),
                        "name of port to send data on").asString());

    /*
    double framerate=0;
    if (config.check("framerate", name,
                     "maximum rate in Hz to read from subdevice")) {
        framerate=name->asDouble();
    }
    */

    if (fgAv&&
        !config.check("shared-ports",
                      "If present, send audio and images on same port")) {
        separatePorts = true;
        yAssert(p2==NULL);
        p2 = new Port;
        yAssert(p2!=NULL);
        p2->open(config.check("name2",Value("/grabber2"),
                              "Name of second port to send data on, when audio and images sent separately").asString());
    }

    if (fgAv!=NULL) {
        if (separatePorts) {
            yInfo("Grabber for images and sound (in separate ports)");
            yAssert(p2!=NULL);
            thread.attach(new DataWriter2<yarp::sig::ImageOf<yarp::sig::PixelRgb>, yarp::sig::Sound>(p,*p2,*this,canDrop,addStamp));
        } else {
            yInfo("Grabber for images and sound (in shared port)");
            thread.attach(new DataWriter<ImageRgbSound>(p,*this,canDrop,
                                                        addStamp));
        }
    } else if (fgImage!=NULL) {
        yDebug("Grabber for rgb images");
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelRgb> >(p,*this,canDrop,addStamp,fgTimed));
    } else if (fgImageRaw!=NULL) {
        yDebug("Grabber for raw images");
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelMono> >(p,*this,canDrop,addStamp,fgTimed));
    } else if (fgSound!=NULL) {
        yInfo("Grabber for sound\n");
        thread.attach(new DataWriter<yarp::sig::Sound>(p,*this,canDrop));
    } else {
        printf("subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    singleThreaded =
        config.check("single_threaded",
                     "if present, operate in single threaded mode")!=0;
    thread.open(config.check("framerate",Value("0"),
                             "maximum rate in Hz to read from subdevice").asDouble(),
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

	IFrameGrabberControlsDC1394* fgCtrlDC1394=dynamic_cast<IFrameGrabberControlsDC1394*>(fgCtrl);

    //printf("%s\n",cmd.toString().c_str());

	switch (code)
	{
	case VOCAB_SET:
        printf("set command received\n");

        switch(cmd.get(1).asVocab())
        {
        case VOCAB_BRIGHTNESS:
            response.addInt(int(setBrightness(cmd.get(2).asDouble())));
            return true;
        case VOCAB_EXPOSURE:
            response.addInt(int(setExposure(cmd.get(2).asDouble())));
            return true;
        case VOCAB_SHARPNESS:
            response.addInt(int(setSharpness(cmd.get(2).asDouble())));
            return true;
        case VOCAB_WHITE:
            response.addInt(int(setWhiteBalance(cmd.get(2).asDouble(),cmd.get(3).asDouble())));
            return true;
        case VOCAB_HUE:
            response.addInt(int(setHue(cmd.get(2).asDouble())));
            return true;
        case VOCAB_SATURATION:
            response.addInt(int(setSaturation(cmd.get(2).asDouble())));
            return true;
        case VOCAB_GAMMA:
            response.addInt(int(setGamma(cmd.get(2).asDouble())));
            return true;
        case VOCAB_SHUTTER:
            response.addInt(int(setShutter(cmd.get(2).asDouble())));
            return true;
        case VOCAB_GAIN:
            response.addInt(int(setGain(cmd.get(2).asDouble())));
            return true;
        case VOCAB_IRIS:
            response.addInt(int(setIris(cmd.get(2).asDouble())));
            return true;
        /*
        case VOCAB_TEMPERATURE:
            response.addInt(int(setTemperature(cmd.get(2).asDouble())));
            return true;
        case VOCAB_WHITE_SHADING:
            response.addInt(int(setWhiteShading(cmd.get(2).asDouble(),cmd.get(3).asDouble(),cmd.get(4).asDouble())));
            return true;
        case VOCAB_OPTICAL_FILTER:
            response.addInt(int(setOpticalFilter(cmd.get(2).asDouble())));
            return true;
        case VOCAB_CAPTURE_QUALITY:
            response.addInt(int(setCaptureQuality(cmd.get(2).asDouble())));
            return true;
        */
        }

        return DeviceResponder::respond(cmd,response);

    case VOCAB_GET:
		printf("get command received\n");

		response.addVocab(VOCAB_IS);
		response.add(cmd.get(1));

        switch(cmd.get(1).asVocab())
		{
		case VOCAB_BRIGHTNESS:
			response.addDouble(getBrightness());
			return true;
		case VOCAB_EXPOSURE:
			response.addDouble(getExposure());
		    return true;
		case VOCAB_SHARPNESS:
			response.addDouble(getSharpness());
			return true;
		case VOCAB_WHITE:
		    {
				double b=0.0;
				double r=0.0;

				getWhiteBalance(b,r);
				response.addDouble(b);
				response.addDouble(r);
			}
		    return true;
		case VOCAB_HUE:
			response.addDouble(getHue());
	        return true;
		case VOCAB_SATURATION:
			response.addDouble(getSaturation());
			return true;
		case VOCAB_GAMMA:
			response.addDouble(getGamma());
			return true;
		case VOCAB_SHUTTER:
			response.addDouble(getShutter());
			return true;
		case VOCAB_GAIN:
			response.addDouble(getGain());
			return true;
		case VOCAB_IRIS:
			response.addDouble(getIris());
			return true;
		/*
		case VOCAB_CAPTURE_QUALITY:
			response.addDouble(getCaptureQuality());
			return true;
		case VOCAB_OPTICAL_FILTER:
			response.addDouble(getOpticalFilter());
			return true;
		*/
		case VOCAB_WIDTH:
			// normally, this would come from stream information
			response.addInt(width());
			return true;
		case VOCAB_HEIGHT:
			// normally, this would come from stream information
			response.addInt(height());
			return true;
		}

	    return DeviceResponder::respond(cmd,response);

		//////////////////
		// DC1394 COMMANDS
		//////////////////
	default:
		if (fgCtrlDC1394) switch(code)
		{
			case VOCAB_DRHASFEA: // VOCAB_DRHASFEA 00
				response.addInt(int(fgCtrlDC1394->hasFeatureDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRSETVAL: // VOCAB_DRSETVAL 01
				response.addInt(int(fgCtrlDC1394->setFeatureDC1394(cmd.get(1).asInt(),cmd.get(2).asDouble())));
                return true;
			case VOCAB_DRGETVAL: // VOCAB_DRGETVAL 02
				response.addDouble(fgCtrlDC1394->getFeatureDC1394(cmd.get(1).asInt()));
				return true;

			case VOCAB_DRHASACT: // VOCAB_DRHASACT 03
				response.addInt(int(fgCtrlDC1394->hasOnOffDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRSETACT: // VOCAB_DRSETACT 04
				response.addInt(int(fgCtrlDC1394->setActiveDC1394(cmd.get(1).asInt(),(cmd.get(2).asInt()!=0))));
                return true;
			case VOCAB_DRGETACT: // VOCAB_DRGETACT 05
				response.addInt(int(fgCtrlDC1394->getActiveDC1394(cmd.get(1).asInt())));
				return true;

			case VOCAB_DRHASMAN: // VOCAB_DRHASMAN 06
				response.addInt(int(fgCtrlDC1394->hasManualDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRHASAUT: // VOCAB_DRHASAUT 07
				response.addInt(int(fgCtrlDC1394->hasAutoDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRHASONP: // VOCAB_DRHASONP 08
				response.addInt(int(fgCtrlDC1394->hasOnePushDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRSETMOD: // VOCAB_DRSETMOD 09
				response.addInt(int(fgCtrlDC1394->setModeDC1394(cmd.get(1).asInt(),(cmd.get(2).asInt()!=0))));
                return true;
			case VOCAB_DRGETMOD: // VOCAB_DRGETMOD 10
				response.addInt(int(fgCtrlDC1394->getModeDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRSETONP: // VOCAB_DRSETONP 11
				response.addInt(int(fgCtrlDC1394->setOnePushDC1394(cmd.get(1).asInt())));
                return true;
			case VOCAB_DRGETMSK: // VOCAB_DRGETMSK 12
				response.addInt(int(fgCtrlDC1394->getVideoModeMaskDC1394()));
				return true;
			case VOCAB_DRGETVMD: // VOCAB_DRGETVMD 13
				response.addInt(int(fgCtrlDC1394->getVideoModeDC1394()));
				return true;
			case VOCAB_DRSETVMD: // VOCAB_DRSETVMD 14
				response.addInt(int(fgCtrlDC1394->setVideoModeDC1394(cmd.get(1).asInt())));
                return true;
			case VOCAB_DRGETFPM: // VOCAB_DRGETFPM 15
				response.addInt(int(fgCtrlDC1394->getFPSMaskDC1394()));
				return true;
			case VOCAB_DRGETFPS: // VOCAB_DRGETFPS 16
				response.addInt(int(fgCtrlDC1394->getFPSDC1394()));
				return true;
			case VOCAB_DRSETFPS: // VOCAB_DRSETFPS 17
				response.addInt(int(fgCtrlDC1394->setFPSDC1394(cmd.get(1).asInt())));
                return true;

			case VOCAB_DRGETISO: // VOCAB_DRGETISO 18
				response.addInt(int(fgCtrlDC1394->getISOSpeedDC1394()));
				return true;
			case VOCAB_DRSETISO: // VOCAB_DRSETISO 19
				response.addInt(int(fgCtrlDC1394->setISOSpeedDC1394(cmd.get(1).asInt())));
                return true;

			case VOCAB_DRGETCCM: // VOCAB_DRGETCCM 20
				response.addInt(int(fgCtrlDC1394->getColorCodingMaskDC1394(cmd.get(1).asInt())));
				return true;
			case VOCAB_DRGETCOD: // VOCAB_DRGETCOD 21
				response.addInt(int(fgCtrlDC1394->getColorCodingDC1394()));
				return true;
			case VOCAB_DRSETCOD: // VOCAB_DRSETCOD 22
				response.addInt(int(fgCtrlDC1394->setColorCodingDC1394(cmd.get(1).asInt())));
                return true;

			case VOCAB_DRSETWHB: // VOCAB_DRSETWHB 23
				response.addInt(int(fgCtrlDC1394->setWhiteBalanceDC1394(cmd.get(1).asDouble(),cmd.get(2).asDouble())));
                return true;
			case VOCAB_DRGETWHB: // VOCAB_DRGETWHB 24
				{
					double b,r;
					fgCtrlDC1394->getWhiteBalanceDC1394(b,r);
					response.addDouble(b);
					response.addDouble(r);
				}
				return true;

			case VOCAB_DRGETF7M: // VOCAB_DRGETF7M 25
				{
					unsigned int xstep,ystep,xdim,ydim,xoffstep,yoffstep;
					fgCtrlDC1394->getFormat7MaxWindowDC1394(xdim,ydim,xstep,ystep,xoffstep,yoffstep);
					response.addInt(xdim);
					response.addInt(ydim);
					response.addInt(xstep);
					response.addInt(ystep);
					response.addInt(xoffstep);
					response.addInt(yoffstep);
				}
				return true;
			case VOCAB_DRGETWF7: // VOCAB_DRGETWF7 26
				{
					unsigned int xdim,ydim;
                    int x0,y0;
					fgCtrlDC1394->getFormat7WindowDC1394(xdim,ydim,x0,y0);
					response.addInt(xdim);
					response.addInt(ydim);
                    response.addInt(x0);
					response.addInt(y0);
				}
				return true;
			case VOCAB_DRSETWF7: // VOCAB_DRSETWF7 27
				response.addInt(int(fgCtrlDC1394->setFormat7WindowDC1394(cmd.get(1).asInt(),cmd.get(2).asInt(),cmd.get(3).asInt(),cmd.get(4).asInt())));
                return true;
			case VOCAB_DRSETOPM: // VOCAB_DRSETOPM 28
				response.addInt(int(fgCtrlDC1394->setOperationModeDC1394(cmd.get(1).asInt()!=0)));
                return true;
			case VOCAB_DRGETOPM: // VOCAB_DRGETOPM 29
				response.addInt(fgCtrlDC1394->getOperationModeDC1394());
				return true;

			case VOCAB_DRSETTXM: // VOCAB_DRSETTXM 30
				response.addInt(int(fgCtrlDC1394->setTransmissionDC1394(cmd.get(1).asInt()!=0)));
                return true;
			case VOCAB_DRGETTXM: // VOCAB_DRGETTXM 31
				response.addInt(fgCtrlDC1394->getTransmissionDC1394());
				return true;
		    /*
			case VOCAB_DRSETBAY: // VOCAB_DRSETBAY 32
				response.addInt(int(fgCtrlDC1394->setBayerDC1394(bool(cmd.get(1).asInt()))));
                return true;
			case VOCAB_DRGETBAY: // VOCAB_DRGETBAY 33
				response.addInt(fgCtrlDC1394->getBayerDC1394());
				return true;
			*/
			case VOCAB_DRSETBCS: // VOCAB_DRSETBCS 34
				response.addInt(int(fgCtrlDC1394->setBroadcastDC1394(cmd.get(1).asInt()!=0)));
                return true;
			case VOCAB_DRSETDEF: // VOCAB_DRSETDEF 35
				response.addInt(int(fgCtrlDC1394->setDefaultsDC1394()));
                return true;
			case VOCAB_DRSETRST: // VOCAB_DRSETRST 36
				response.addInt(int(fgCtrlDC1394->setResetDC1394()));
                return true;
			case VOCAB_DRSETPWR: // VOCAB_DRSETPWR 37
				response.addInt(int(fgCtrlDC1394->setPowerDC1394(cmd.get(1).asInt()!=0)));
                return true;
			case VOCAB_DRSETCAP: // VOCAB_DRSETCAP 38
				response.addInt(int(fgCtrlDC1394->setCaptureDC1394(cmd.get(1).asInt()!=0)));
                return true;
			case VOCAB_DRSETBPP: // VOCAB_DRSETCAP 39
				response.addInt(int(fgCtrlDC1394->setBytesPerPacketDC1394(cmd.get(1).asInt())));
                return true;
			case VOCAB_DRGETBPP: // VOCAB_DRGETTXM 40
				response.addInt(fgCtrlDC1394->getBytesPerPacketDC1394());
				return true;
		}
	}

    return DeviceResponder::respond(cmd,response);
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


