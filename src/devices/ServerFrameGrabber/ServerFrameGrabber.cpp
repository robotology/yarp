/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ServerFrameGrabber.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/proto/framegrabber/CameraVocabs.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

namespace {
YARP_LOG_COMPONENT(SERVERFRAMEGRABBER, "yarp.device.grabber")
}

bool ServerFrameGrabber::close()
{
    if (!active) {
        return false;
    }
    active = false;
    thread.stop();
    if (p2!=nullptr) {
        delete p2;
        p2 = nullptr;
    }
    return true;
}

bool ServerFrameGrabber::open(yarp::os::Searchable& config)
{
    if (active) {
        yCError(SERVERFRAMEGRABBER, "Did you just try to open the same ServerFrameGrabber twice?\n");
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
            //yCError(SERVERFRAMEGRABBER, "cannot make <%s>\n", name->toString().c_str());
            return false;
        }
    } else {
        yCError(SERVERFRAMEGRABBER, "\"--subdevice <name>\" not set for server_framegrabber\n");
        return false;
    }
    if (poly.isValid()) {
        IAudioVisualStream *str;
        poly.view(str);
        bool a = true;
        bool v = true;
        bool vraw = true;
        if (str!=nullptr) {
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
        if(fgCtrl)
            ifgCtrl_Responder.configure(fgCtrl);
        poly.view(fgTimed);
        poly.view(rgbVis_p);

        bool conf = rgbParser.configure(rgbVis_p);

        if(!conf)
        {
            yCWarning(SERVERFRAMEGRABBER) << "ServerFrameGrabber: error configuring interfaces for parsers";
        }
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
        framerate=name->asFloat64();
    }
    */

    if (fgAv&&
        !config.check("shared-ports",
                      "If present, send audio and images on same port")) {
        separatePorts = true;
        yCAssert(SERVERFRAMEGRABBER, p2==nullptr);
        p2 = new Port;
        yCAssert(SERVERFRAMEGRABBER, p2!=nullptr);
        p2->open(config.check("name2",Value("/grabber2"),
                              "Name of second port to send data on, when audio and images sent separately").asString());
    }

    if (fgAv!=nullptr) {
        if (separatePorts) {
            yCAssert(SERVERFRAMEGRABBER, p2!=nullptr);
            thread.attach(new DataWriter2<yarp::sig::ImageOf<yarp::sig::PixelRgb>, yarp::sig::Sound>(p,*p2,*this,canDrop,addStamp));
        } else {
            thread.attach(new DataWriter<ImageRgbSound>(p,*this,canDrop,
                                                        addStamp));
        }
    } else if (fgImage!=nullptr) {
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelRgb> >(p,*this,canDrop,addStamp,fgTimed));
    } else if (fgImageRaw!=nullptr) {
        thread.attach(new DataWriter<yarp::sig::ImageOf<yarp::sig::PixelMono> >(p,*this,canDrop,addStamp,fgTimed));
    } else {
        yCError(SERVERFRAMEGRABBER, "subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    singleThreaded =
        config.check("single_threaded",
                     "if present, operate in single threaded mode")!=0;
    thread.open(config.check("framerate",Value("0"),
                             "maximum rate in Hz to read from subdevice").asFloat64(),
                singleThreaded);
    active = true;


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
                                 yarp::os::Bottle& response)
{
    int code = cmd.get(0).asVocab32();

    auto* fgCtrlDC1394=dynamic_cast<IFrameGrabberControlsDC1394*>(fgCtrl);

    switch (code)
    {
    // first check if requests are coming from new iFrameGrabberControl2 interface and process them
    case VOCAB_FRAMEGRABBER_CONTROL:
    {
        return ifgCtrl_Responder.respond(cmd, response);    // I don't like all those returns everywhere!!! :-(
    } break;
    case VOCAB_RGB_VISUAL_PARAMS:
    {
        return rgbParser.respond(cmd,response);
    } break;
        //////////////////
        // DC1394 COMMANDS
        //////////////////
    case VOCAB_FRAMEGRABBER_CONTROL_DC1394:
    {
        if (fgCtrlDC1394)
        {
            int codeDC1394 = cmd.get(1).asVocab32();
            switch(codeDC1394)
            {
            case VOCAB_DRGETMSK: // VOCAB_DRGETMSK 12
                response.addInt32(int(fgCtrlDC1394->getVideoModeMaskDC1394()));
                return true;
            case VOCAB_DRGETVMD: // VOCAB_DRGETVMD 13
                response.addInt32(int(fgCtrlDC1394->getVideoModeDC1394()));
                return true;
            case VOCAB_DRSETVMD: // VOCAB_DRSETVMD 14
                response.addInt32(int(fgCtrlDC1394->setVideoModeDC1394(cmd.get(1).asInt32())));
                return true;
            case VOCAB_DRGETFPM: // VOCAB_DRGETFPM 15
                response.addInt32(int(fgCtrlDC1394->getFPSMaskDC1394()));
                return true;
            case VOCAB_DRGETFPS: // VOCAB_DRGETFPS 16
                response.addInt32(int(fgCtrlDC1394->getFPSDC1394()));
                return true;
            case VOCAB_DRSETFPS: // VOCAB_DRSETFPS 17
                response.addInt32(int(fgCtrlDC1394->setFPSDC1394(cmd.get(1).asInt32())));
                return true;

            case VOCAB_DRGETISO: // VOCAB_DRGETISO 18
                response.addInt32(int(fgCtrlDC1394->getISOSpeedDC1394()));
                return true;
            case VOCAB_DRSETISO: // VOCAB_DRSETISO 19
                response.addInt32(int(fgCtrlDC1394->setISOSpeedDC1394(cmd.get(1).asInt32())));
                return true;

            case VOCAB_DRGETCCM: // VOCAB_DRGETCCM 20
                response.addInt32(int(fgCtrlDC1394->getColorCodingMaskDC1394(cmd.get(1).asInt32())));
                return true;
            case VOCAB_DRGETCOD: // VOCAB_DRGETCOD 21
                response.addInt32(int(fgCtrlDC1394->getColorCodingDC1394()));
                return true;
            case VOCAB_DRSETCOD: // VOCAB_DRSETCOD 22
                response.addInt32(int(fgCtrlDC1394->setColorCodingDC1394(cmd.get(1).asInt32())));
                return true;
            case VOCAB_DRGETF7M: // VOCAB_DRGETF7M 25
                {
                    unsigned int xstep,ystep,xdim,ydim,xoffstep,yoffstep;
                    fgCtrlDC1394->getFormat7MaxWindowDC1394(xdim,ydim,xstep,ystep,xoffstep,yoffstep);
                    response.addInt32(xdim);
                    response.addInt32(ydim);
                    response.addInt32(xstep);
                    response.addInt32(ystep);
                    response.addInt32(xoffstep);
                    response.addInt32(yoffstep);
                }
                return true;
            case VOCAB_DRGETWF7: // VOCAB_DRGETWF7 26
                {
                    unsigned int xdim,ydim;
                    int x0,y0;
                    fgCtrlDC1394->getFormat7WindowDC1394(xdim,ydim,x0,y0);
                    response.addInt32(xdim);
                    response.addInt32(ydim);
                    response.addInt32(x0);
                    response.addInt32(y0);
                }
                return true;
            case VOCAB_DRSETWF7: // VOCAB_DRSETWF7 27
                response.addInt32(int(fgCtrlDC1394->setFormat7WindowDC1394(cmd.get(1).asInt32(),cmd.get(2).asInt32(),cmd.get(3).asInt32(),cmd.get(4).asInt32())));
                return true;
            case VOCAB_DRSETOPM: // VOCAB_DRSETOPM 28
                response.addInt32(int(fgCtrlDC1394->setOperationModeDC1394(cmd.get(1).asInt32()!=0)));
                return true;
            case VOCAB_DRGETOPM: // VOCAB_DRGETOPM 29
                response.addInt32(fgCtrlDC1394->getOperationModeDC1394());
                return true;

            case VOCAB_DRSETTXM: // VOCAB_DRSETTXM 30
                response.addInt32(int(fgCtrlDC1394->setTransmissionDC1394(cmd.get(1).asInt32()!=0)));
                return true;
            case VOCAB_DRGETTXM: // VOCAB_DRGETTXM 31
                response.addInt32(fgCtrlDC1394->getTransmissionDC1394());
                return true;
            /*
            case VOCAB_DRSETBAY: // VOCAB_DRSETBAY 32
                response.addInt32(int(fgCtrlDC1394->setBayerDC1394(bool(cmd.get(1).asInt32()))));
                return true;
            case VOCAB_DRGETBAY: // VOCAB_DRGETBAY 33
                response.addInt32(fgCtrlDC1394->getBayerDC1394());
                return true;
            */
            case VOCAB_DRSETBCS: // VOCAB_DRSETBCS 34
                response.addInt32(int(fgCtrlDC1394->setBroadcastDC1394(cmd.get(1).asInt32()!=0)));
                return true;
            case VOCAB_DRSETDEF: // VOCAB_DRSETDEF 35
                response.addInt32(int(fgCtrlDC1394->setDefaultsDC1394()));
                return true;
            case VOCAB_DRSETRST: // VOCAB_DRSETRST 36
                response.addInt32(int(fgCtrlDC1394->setResetDC1394()));
                return true;
            case VOCAB_DRSETPWR: // VOCAB_DRSETPWR 37
                response.addInt32(int(fgCtrlDC1394->setPowerDC1394(cmd.get(1).asInt32()!=0)));
                return true;
            case VOCAB_DRSETCAP: // VOCAB_DRSETCAP 38
                response.addInt32(int(fgCtrlDC1394->setCaptureDC1394(cmd.get(1).asInt32()!=0)));
                return true;
            case VOCAB_DRSETBPP: // VOCAB_DRSETCAP 39
                response.addInt32(int(fgCtrlDC1394->setBytesPerPacketDC1394(cmd.get(1).asInt32())));
                return true;
            case VOCAB_DRGETBPP: // VOCAB_DRGETTXM 40
                response.addInt32(fgCtrlDC1394->getBytesPerPacketDC1394());
                return true;
            default:
                return DeviceResponder::respond(cmd,response);
            }
        }
    }
    }
    yCError(SERVERFRAMEGRABBER) << "ServerFrameGrabber: command not recognized" << cmd.toString();
    return DeviceResponder::respond(cmd,response);
}

/*
bool ServerFrameGrabber::read(ConnectionReader& connection)
{
    yarp::os::Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
    yCDebug(SERVERFRAMEGRABBER, "command received: %s\n", cmd.toString().c_str());
    int code = cmd.get(0).asVocab32();
    switch (code) {
    case VOCAB_SET:
        yCDebug(SERVERFRAMEGRABBER, "set command received\n");
        {
            bool ok = false;
            switch(cmd.get(1).asVocab32()) {
            case VOCAB_BRIGHTNESS:
                ok = setBrightness(cmd.get(2).asFloat64());
                break;
            case VOCAB_SHUTTER:
                ok = setShutter(cmd.get(2).asFloat64());
                break;
            case VOCAB_GAIN:
                ok = setGain(cmd.get(2).asFloat64());
                break;
            case VOCAB_WHITE:
                ok = setWhiteBalance(cmd.get(2).asFloat64(),
                                     cmd.get(3).asFloat64());
                break;
            }
        }
        break;
    case VOCAB_GET:
        yCDebug(SERVERFRAMEGRABBER, "get command received\n");
        {
            bool ok = false;
            response.addVocab32(VOCAB_IS);
            response.add(cmd.get(1));
            switch(cmd.get(1).asVocab32()) {
            case VOCAB_BRIGHTNESS:
                ok = true;
                response.addFloat64(getBrightness());
                break;
            case VOCAB_SHUTTER:
                ok = true;
                response.addFloat64(getShutter());
                break;
            case VOCAB_GAIN:
                ok = true;
                response.addFloat64(getGain());
                break;
            case VOCAB_WIDTH:
                // normally, this would come from stream information
                ok = true;
                response.addInt32(width());
                break;
            case VOCAB_HEIGHT:
                // normally, this would come from stream information
                ok = true;
                response.addInt32(height());
                break;

            case VOCAB_WHITE:

                double r;

                double g;

                ok=getWhiteBalance(r, g);

                response.addFloat64(r);

                response.addFloat64(g);
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
            yCDebug(SERVERFRAMEGRABBER, "response sent: %s\n", response.toString().c_str());
        }
    }
    return true;
}
*/

bool ServerFrameGrabber::getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    return getImage(image);
}

bool ServerFrameGrabber::getDatum(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    return getImage(image);
}

bool ServerFrameGrabber::getDatum(ImageRgbSound& imageSound)
{
    return getDatum(imageSound.head,imageSound.body);
}

bool ServerFrameGrabber::getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                  yarp::sig::Sound& sound)
{
    return getAudioVisual(image,sound);
}

bool ServerFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    if (fgImage==nullptr) { return false; }
    return fgImage->getImage(image);
}

bool ServerFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    if (fgImageRaw==nullptr) { return false; }
    return fgImageRaw->getImage(image);
}

bool ServerFrameGrabber::getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                        yarp::sig::Sound& sound)
{
    if (fgAv==nullptr) { return false; }
    return fgAv->getAudioVisual(image,sound);
}

int ServerFrameGrabber::height() const
{
    if (fgImage) { return fgImage->height(); }
    if (fgImageRaw) { return fgImageRaw->height(); }
    return 0;
}

int ServerFrameGrabber::width() const
{
    if (fgImage) { return fgImage->width(); }
    if (fgImageRaw) { return fgImageRaw->width(); }
    return 0;
}

bool ServerFrameGrabber::stopService()
{
    return close();
}

bool ServerFrameGrabber::startService()
{
    if (singleThreaded) {
        return false;
    }
    return active;
}


bool ServerFrameGrabber::updateService()
{
    if (singleThreaded) {
        if (active) {
            thread.step();
        }
        return active;
    }
    return false;
}

YARP_WARNING_POP
