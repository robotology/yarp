/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Log.h>
#include <ServerGrabber.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/LogStream.h>
#include <cstring>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

yarp::dev::DriverCreator *createServerGrabber()
{
    return new yarp::dev::DriverCreatorOf<yarp::dev::ServerGrabber>
            ("grabberDual", "grabberDual", "yarp::dev::ServerGrabber");
}

yarp::dev::DC1394::DC1394Parser::DC1394Parser() : fgCtrl_DC1394(YARP_NULLPTR) {};

bool yarp::dev::DC1394::DC1394Parser::configure(IFrameGrabberControlsDC1394 *interface)
{
    fgCtrl_DC1394 = interface;
    return true;
}

bool yarp::dev::DC1394::DC1394Parser::respond(const Bottle& cmd, Bottle& response)
{
    int code = cmd.get(0).asVocab();
    if (fgCtrl_DC1394)
    {
        switch(code)
        {
        case VOCAB_DRHASFEA: // VOCAB_DRHASFEA 00
            response.addInt(int(fgCtrl_DC1394->hasFeatureDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRSETVAL: // VOCAB_DRSETVAL 01
            response.addInt(int(fgCtrl_DC1394->setFeatureDC1394(cmd.get(1).asInt(),cmd.get(2).asDouble())));
            return true;
        case VOCAB_DRGETVAL: // VOCAB_DRGETVAL 02
            response.addDouble(fgCtrl_DC1394->getFeatureDC1394(cmd.get(1).asInt()));
            return true;

        case VOCAB_DRHASACT: // VOCAB_DRHASACT 03
            response.addInt(int(fgCtrl_DC1394->hasOnOffDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRSETACT: // VOCAB_DRSETACT 04
            response.addInt(int(fgCtrl_DC1394->setActiveDC1394(cmd.get(1).asInt(),(cmd.get(2).asInt()!=0))));
            return true;
        case VOCAB_DRGETACT: // VOCAB_DRGETACT 05
            response.addInt(int(fgCtrl_DC1394->getActiveDC1394(cmd.get(1).asInt())));
            return true;

        case VOCAB_DRHASMAN: // VOCAB_DRHASMAN 06
            response.addInt(int(fgCtrl_DC1394->hasManualDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRHASAUT: // VOCAB_DRHASAUT 07
            response.addInt(int(fgCtrl_DC1394->hasAutoDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRHASONP: // VOCAB_DRHASONP 08
            response.addInt(int(fgCtrl_DC1394->hasOnePushDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRSETMOD: // VOCAB_DRSETMOD 09
            response.addInt(int(fgCtrl_DC1394->setModeDC1394(cmd.get(1).asInt(),(cmd.get(2).asInt()!=0))));
            return true;
        case VOCAB_DRGETMOD: // VOCAB_DRGETMOD 10
            response.addInt(int(fgCtrl_DC1394->getModeDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRSETONP: // VOCAB_DRSETONP 11
            response.addInt(int(fgCtrl_DC1394->setOnePushDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRGETMSK: // VOCAB_DRGETMSK 12
            response.addInt(int(fgCtrl_DC1394->getVideoModeMaskDC1394()));
            return true;
        case VOCAB_DRGETVMD: // VOCAB_DRGETVMD 13
            response.addInt(int(fgCtrl_DC1394->getVideoModeDC1394()));
            return true;
        case VOCAB_DRSETVMD: // VOCAB_DRSETVMD 14
            response.addInt(int(fgCtrl_DC1394->setVideoModeDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRGETFPM: // VOCAB_DRGETFPM 15
            response.addInt(int(fgCtrl_DC1394->getFPSMaskDC1394()));
            return true;
        case VOCAB_DRGETFPS: // VOCAB_DRGETFPS 16
            response.addInt(int(fgCtrl_DC1394->getFPSDC1394()));
            return true;
        case VOCAB_DRSETFPS: // VOCAB_DRSETFPS 17
            response.addInt(int(fgCtrl_DC1394->setFPSDC1394(cmd.get(1).asInt())));
            return true;

        case VOCAB_DRGETISO: // VOCAB_DRGETISO 18
            response.addInt(int(fgCtrl_DC1394->getISOSpeedDC1394()));
            return true;
        case VOCAB_DRSETISO: // VOCAB_DRSETISO 19
            response.addInt(int(fgCtrl_DC1394->setISOSpeedDC1394(cmd.get(1).asInt())));
            return true;

        case VOCAB_DRGETCCM: // VOCAB_DRGETCCM 20
            response.addInt(int(fgCtrl_DC1394->getColorCodingMaskDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRGETCOD: // VOCAB_DRGETCOD 21
            response.addInt(int(fgCtrl_DC1394->getColorCodingDC1394()));
            return true;
        case VOCAB_DRSETCOD: // VOCAB_DRSETCOD 22
            response.addInt(int(fgCtrl_DC1394->setColorCodingDC1394(cmd.get(1).asInt())));
            return true;

        case VOCAB_DRSETWHB: // VOCAB_DRSETWHB 23
            response.addInt(int(fgCtrl_DC1394->setWhiteBalanceDC1394(cmd.get(1).asDouble(),cmd.get(2).asDouble())));
            return true;
        case VOCAB_DRGETWHB: // VOCAB_DRGETWHB 24
            {
                double b,r;
                fgCtrl_DC1394->getWhiteBalanceDC1394(b,r);
                response.addDouble(b);
                response.addDouble(r);
            }
            return true;

        case VOCAB_DRGETF7M: // VOCAB_DRGETF7M 25
            {
                unsigned int xstep,ystep,xdim,ydim,xoffstep,yoffstep;
                fgCtrl_DC1394->getFormat7MaxWindowDC1394(xdim,ydim,xstep,ystep,xoffstep,yoffstep);
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
                fgCtrl_DC1394->getFormat7WindowDC1394(xdim,ydim,x0,y0);
                response.addInt(xdim);
                response.addInt(ydim);
                response.addInt(x0);
                response.addInt(y0);
            }
            return true;
        case VOCAB_DRSETWF7: // VOCAB_DRSETWF7 27
            response.addInt(int(fgCtrl_DC1394->setFormat7WindowDC1394(cmd.get(1).asInt(),cmd.get(2).asInt(),cmd.get(3).asInt(),cmd.get(4).asInt())));
            return true;
        case VOCAB_DRSETOPM: // VOCAB_DRSETOPM 28
            response.addInt(int(fgCtrl_DC1394->setOperationModeDC1394(cmd.get(1).asInt()!=0)));
            return true;
        case VOCAB_DRGETOPM: // VOCAB_DRGETOPM 29
            response.addInt(fgCtrl_DC1394->getOperationModeDC1394());
            return true;

        case VOCAB_DRSETTXM: // VOCAB_DRSETTXM 30
            response.addInt(int(fgCtrl_DC1394->setTransmissionDC1394(cmd.get(1).asInt()!=0)));
            return true;
        case VOCAB_DRGETTXM: // VOCAB_DRGETTXM 31
            response.addInt(fgCtrl_DC1394->getTransmissionDC1394());
            return true;
        case VOCAB_DRSETBCS: // VOCAB_DRSETBCS 34
            response.addInt(int(fgCtrl_DC1394->setBroadcastDC1394(cmd.get(1).asInt()!=0)));
            return true;
        case VOCAB_DRSETDEF: // VOCAB_DRSETDEF 35
            response.addInt(int(fgCtrl_DC1394->setDefaultsDC1394()));
            return true;
        case VOCAB_DRSETRST: // VOCAB_DRSETRST 36
            response.addInt(int(fgCtrl_DC1394->setResetDC1394()));
            return true;
        case VOCAB_DRSETPWR: // VOCAB_DRSETPWR 37
            response.addInt(int(fgCtrl_DC1394->setPowerDC1394(cmd.get(1).asInt()!=0)));
            return true;
        case VOCAB_DRSETCAP: // VOCAB_DRSETCAP 38
            response.addInt(int(fgCtrl_DC1394->setCaptureDC1394(cmd.get(1).asInt()!=0)));
            return true;
        case VOCAB_DRSETBPP: // VOCAB_DRSETCAP 39
            response.addInt(int(fgCtrl_DC1394->setBytesPerPacketDC1394(cmd.get(1).asInt())));
            return true;
        case VOCAB_DRGETBPP: // VOCAB_DRGETTXM 40
            response.addInt(fgCtrl_DC1394->getBytesPerPacketDC1394());
            return true;
        }
    }
    else
    {
        yWarning()<<"DC1394Parser:  firewire interface not implemented in subdevice, some features could not be available";
//        response.clear();
//        response.addVocab(VOCAB_FAILED);
        return DeviceResponder::respond(cmd,response);
    }

    return true;
}

ServerGrabber::ServerGrabber():RateThread(DEFAULT_THREAD_PERIOD), period(DEFAULT_THREAD_PERIOD) {
    rgbVis_p = YARP_NULLPTR;
    rgbVis_p2 = YARP_NULLPTR;
    fgImage = YARP_NULLPTR;
    fgImage2 = YARP_NULLPTR;
    fgImageRaw = YARP_NULLPTR;
    fgImageRaw2 = YARP_NULLPTR;
    fgCtrl_DC1394 = YARP_NULLPTR;
    fgCtrl2_DC1394 = YARP_NULLPTR;
//    fgAv = YARP_NULLPTR;
    fgCtrl = YARP_NULLPTR;
    fgCtrl2 = YARP_NULLPTR;
    fgTimed = YARP_NULLPTR;
    img=YARP_NULLPTR;
    img2=YARP_NULLPTR;
    img_Raw=YARP_NULLPTR;
    img2_Raw=YARP_NULLPTR;
    param.spoke = false;
    param.canDrop = true;
    param.addStamp = false;
    param.active = false;
    param.singleThreaded = false;
    param.hasAudio = false;
    param.twoCameras = false;
    param.split = false;
//    param.cap=AV;
    param.cap=COLOR;
    p2 = YARP_NULLPTR;
    isSubdeviceOwned=false;
    count = 0;
    count2 = 0;

}

bool ServerGrabber::close() {
    if (!param.active) {
        return false;
    }
    param.active = false;
    detachAll();
    rpcPort.interrupt();
    pImg.interrupt();
    rpcPort.close();
    pImg.close();
    poly->close();
    delete poly;
    poly=YARP_NULLPTR;
    if(param.twoCameras)
    {
        if(isSubdeviceOwned)
        {
            poly2->close();
            delete poly2;
            poly2=YARP_NULLPTR;
        }
        pImg2.interrupt();
        pImg2.close();
    }
    isSubdeviceOwned=false;
    if (p2!=YARP_NULLPTR) {
        delete p2;
        p2 =YARP_NULLPTR;
    }
    return true;
}

bool ServerGrabber::open(yarp::os::Searchable& config) {
    if (param.active) {
        yError("Did you just try to open the same ServerGrabber twice?\n");
        return false;
    }

    if(!fromConfig(config))
    {
        yError() << "Device ServerGrabber failed to open, check previous log for error messsages.\n";
        return false;
    }


    if(!initialize_YARP(config) )
    {
        yError() <<"ServerGrabber: Error initializing YARP ports";
        return false;
    }

    param.active = true;
    //ASK/TODO update usage
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

bool ServerGrabber::fromConfig(yarp::os::Searchable &config)
{
    if(config.check("period"))
        period = config.find("period").asInt();
    else if(config.check("framerate"))
        period = (1/(config.find("period").asInt()))*1000;
    else
        yWarning()<<"ServerGrabber: period/framerate parameter not found, using default of"<< DEFAULT_THREAD_PERIOD << "ms";
    if(!config.check("subdevice"))
        param.twoCameras=true;
    if(config.check("split"))
        param.split=config.find("split").asBool();
    if(config.find("capabilities").asString()=="COLOR")
        param.cap=COLOR;
    else if(config.find("capabilities").asString()=="RAW")
        param.cap=RAW;
    else
        yWarning()<<"ServerGrabber: 'capabilities' parameter not found or mispelled, the option available are COLOR(default) and RAW, using default";
    param.canDrop = !config.check("no_drop","if present, use strict policy for sending data");
    param.addStamp = config.check("stamp","if present, add timestamps to data");

    param.singleThreaded =
        config.check("single_threaded",
                     "if present, operate in single threaded mode")!=0;
    //TODO audio part
    yarp::os::ConstString rootName;
    rootName = config.check("name",Value("/grabber"),
                            "name of port to send data on").asString();
    rpcPort_Name  = rootName + "/rpc";
    if(param.twoCameras)
    {
        if(param.split)
        {
            pImg_Name = rootName + "/left:o";
            pImg2_Name = rootName + "/right:o";
        }
        else
            pImg_Name = rootName;

        // check if we need to create subdevice or if they are
        // passed later on thorugh attachAll()
        if(!(config.findGroup("LEFT").isNull()) && !(config.findGroup("RIGHT").isNull()))
        {
            isSubdeviceOwned=true;
            if(! openAndAttachSubDevice(config))
            {
                yError("ServerGrabber: error while opening subdevice\n");
                return false;
            }
        }
        else
        {
            isSubdeviceOwned=false;
            yWarning()<<"ServerGrabber: LEFT and RIGHT group not found...";
            if(!openDeferredAttach(config))
            return false;
        }
    }
    else
    {
        pImg_Name = rootName;
        if(config.check("subdevice"))
        {
            isSubdeviceOwned=true;
            if(! openAndAttachSubDevice(config))
            {
                yError("ServerGrabber: error while opening subdevice\n");
                return false;
            }
        }
        else
        {
            isSubdeviceOwned=false;
            if(!openDeferredAttach(config))
            return false;
        }
    }


    return true;
}

bool ServerGrabber::initialize_YARP(yarp::os::Searchable &params)
{
    // Open ports
    bool bRet;
    bRet = true;
    if(!rpcPort.open(rpcPort_Name.c_str()))
    {
        yError() << "ServerGrabber: unable to open rpc Port" << rpcPort_Name.c_str();
        bRet = false;
    }

    rpcPort.setReader(*this);
    pImg.promiseType(Type::byName("yarp/image"));
    pImg.setWriteOnly();

    if(!pImg.open(pImg_Name.c_str()))
    {
        yError() << "ServerGrabber: unable to open image streaming Port" << pImg_Name.c_str();
        bRet = false;
    }
    if(param.twoCameras && param.split)
    {
        pImg2.promiseType(Type::byName("yarp/image"));
        pImg2.setWriteOnly();
        if(!pImg2.open(pImg2_Name.c_str()))
        {
            yError() << "ServerGrabber: unable to open image streaming Port" << pImg2_Name.c_str();
            bRet = false;
        }
    }

    return bRet;
}

bool ServerGrabber::respond(const yarp::os::Bottle& cmd,
                                 yarp::os::Bottle& response) {
    int code = cmd.get(0).asVocab();
    Bottle response2;
    switch (code)
    {
    // first check if requests are coming from new iFrameGrabberControl2 interface and process them
    case VOCAB_FRAMEGRABBER_CONTROL2:
    {
        if(param.twoCameras)
        {
            bool ret;
            ret=ifgCtrl_Parser.respond(cmd, response);
            ret&=ifgCtrl2_Parser.respond(cmd, response2);
            if(!ret || (response!=response2))
            {
                response.clear();
                response.add(VOCAB_FAILED);
                ret=false;
            }
            return ret;
        }
        else
            return ifgCtrl_Parser.respond(cmd, response);
    } break;
    case VOCAB_RGB_VISUAL_PARAMS:
    {
        if(param.twoCameras)
        {
            bool ret;
            ret=rgbParser.respond(cmd,response);
            ret&=rgbParser2.respond(cmd,response2);
            if(!ret || (response!=response2))
            {
                response.clear();
                response.add(VOCAB_FAILED);
                ret=false;
            }
            return ret;
        }
        else
            return rgbParser.respond(cmd,response);
    } break;
        //////////////////
        // DC1394 COMMANDS
        //////////////////
    default:
    {
        if(param.twoCameras)
        {
            bool ret;
            ret=ifgCtrl_DC1394_Parser.respond(cmd, response);
            ret&=ifgCtrl2_DC1394_Parser.respond(cmd, response2);
            if(!ret || (response!=response2))
            {
                response.clear();

                return DeviceResponder::respond(cmd,response);
            }
            return ret;
        }
        else
            return ifgCtrl_DC1394_Parser.respond(cmd, response);
    } break;
    }
    return DeviceResponder::respond(cmd,response);
}

bool ServerGrabber::attachAll(const PolyDriverList &device2attach)
{
    bool ret=false;
    bool leftDone=false;//for avoiding double left
    bool rightDone=false;//for avoiding double right
    if(param.twoCameras)
    {
        if (device2attach.size() != 2)
        {
            yError("ServerGrabber: expected two devices to be attached");
            return false;
        }
        for(int i=0;i<device2attach.size();i++)
        {
            yarp::dev::PolyDriver * Idevice2attach = device2attach[i]->poly;
            if (!Idevice2attach->isValid())
            {
                yError() << "ServerGrabber: Device " << device2attach[i]->key << " to attach to is not valid ... cannot proceed";
                return false;
            }
            if(device2attach[i]->key == "LEFT" && !leftDone)
            {
                leftDone |= Idevice2attach->view(rgbVis_p);
                leftDone |= Idevice2attach->view(fgImage);
                leftDone |= Idevice2attach->view(fgImageRaw);
                leftDone |= Idevice2attach->view(fgCtrl);
                leftDone |= Idevice2attach->view(fgCtrl_DC1394);
            }
            else if(device2attach[i]->key == "RIGHT" && !rightDone)
            {
                rightDone |= Idevice2attach->view(rgbVis_p2);
                rightDone |= Idevice2attach->view(fgImage2);
                rightDone |= Idevice2attach->view(fgImageRaw2);
                rightDone |= Idevice2attach->view(fgCtrl2);
                rightDone |= Idevice2attach->view(fgCtrl2_DC1394);
            }
            else
            {
                yError() << "ServerGrabber: failed to attach. The two targets must be LEFT RIGHT and devices must implement"
                            " either IFrameGrabberImage or IFrameGrabberImageRaw";
                return false;

            }
        }
        switch(param.cap)
        {
            case COLOR :
            {
                if((fgImage==YARP_NULLPTR) || (fgImage2==YARP_NULLPTR))
                {
                    yError()<<"ServerGrabber: capability required not supported";
                    return false;
                }
            }
            break;
            case RAW :
            {
                if((fgImageRaw==YARP_NULLPTR) || (fgImageRaw2==YARP_NULLPTR))
                {
                    yError()<<"ServerGrabber: capability required not supported";
                    return false;
                }
            }
        }
        if((rgbVis_p == YARP_NULLPTR) || (rgbVis_p2 == YARP_NULLPTR))
        {
            yWarning()<<"ServerGrabber: Targets has not IVisualParamInterface, some features cannot be available";
        }
        //Configuring parsers
        if(rgbVis_p != YARP_NULLPTR && rgbVis_p2 != YARP_NULLPTR)
        {
            if(!(rgbParser.configure(rgbVis_p)) || !(rgbParser2.configure(rgbVis_p2)))
            {
                yError() << "ServerGrabber: error configuring interfaces for parsers";
                return false;
            }
        }
        if(fgCtrl != YARP_NULLPTR && fgCtrl2 != YARP_NULLPTR)
        {
            if(!(ifgCtrl_Parser.configure(fgCtrl)) || !(ifgCtrl2_Parser.configure(fgCtrl2)))
            {
                yError() << "ServerGrabber: error configuring interfaces for parsers";
                return false;
            }
        }
        if(fgCtrl_DC1394 != YARP_NULLPTR && fgCtrl2_DC1394 != YARP_NULLPTR)
        {
            if(!(ifgCtrl_DC1394_Parser.configure(fgCtrl_DC1394)) || !(ifgCtrl2_DC1394_Parser.configure(fgCtrl2_DC1394)))
            {
                yError() << "ServerGrabber: error configuring interfaces for parsers";
                return false;
            }
        }
    }
    else{
        if (device2attach.size() != 1)
        {
            yError("ServerGrabber: expected one device to be attached");
            return false;
        }
        yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;
        Idevice2attach->view(rgbVis_p);
        Idevice2attach->view(fgImage);
        Idevice2attach->view(fgImageRaw);
        Idevice2attach->view(fgCtrl);
        Idevice2attach->view(fgCtrl_DC1394);
        switch(param.cap){
            case COLOR :
            {
                if((fgImage==YARP_NULLPTR))
                {
                    yError()<<"ServerGrabber: capability required not supported";
                    return false;
                }
            }
            break;
            case RAW :
            {
                if((fgImageRaw==YARP_NULLPTR))
                {
                    yError()<<"ServerGrabber: capability required not supported";
                    return false;
                }
            }
        }

        if (!Idevice2attach->isValid())
        {
            yError() << "ServerGrabber: Device " << device2attach[0]->key << " to attach to is not valid ... cannot proceed";
            return false;
        }

        if((rgbVis_p == YARP_NULLPTR))
        {
            yWarning()<<"ServerGrabber: Targets has not IVisualParamInterface, some features cannot be available";
        }

        //Configuring parsers
        if(rgbVis_p != YARP_NULLPTR)
        {
            if(!(rgbParser.configure(rgbVis_p)))
            {
                yError() << "ServerGrabber: error configuring interfaces for parsers";
                return false;
            }
        }
        if(fgCtrl != YARP_NULLPTR)
        {
            if(!(ifgCtrl_Parser.configure(fgCtrl)))
            {
                yError() << "ServerGrabber: error configuring interfaces for parsers";
                return false;
            }
        }

        if(fgCtrl_DC1394 != YARP_NULLPTR)
        {
            if(!(ifgCtrl_DC1394_Parser.configure(fgCtrl_DC1394)))
            {
                yError() << "ServerGrabber: error configuring interfaces for parsers";
                return false;
            }
        }
    }

    RateThread::setRate(period);
    ret = RateThread::start();

    return ret;
}
bool ServerGrabber::detachAll()
{
    if (yarp::os::RateThread::isRunning())
        yarp::os::RateThread::stop();

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
        return false;

    rgbVis_p       = YARP_NULLPTR;
    rgbVis_p2      = YARP_NULLPTR;
    fgImage        = YARP_NULLPTR;
    fgImage2       = YARP_NULLPTR;
    fgImageRaw     = YARP_NULLPTR;
    fgImageRaw2    = YARP_NULLPTR;
    fgCtrl         = YARP_NULLPTR;
    fgCtrl2        = YARP_NULLPTR;
    fgCtrl_DC1394  = YARP_NULLPTR;
    fgCtrl2_DC1394 = YARP_NULLPTR;
    return true;
}

bool ServerGrabber::attach(PolyDriver *poly)
{
    if(param.twoCameras)
    {
        yError()<<"ServerGrabber: server grabber configured for two cameras, but only one provided";
        return false;
    }
    PolyDriverList plist;
    if(poly)
    {
        PolyDriverDescriptor p(poly,"poly");
        plist.push(p);
        return attachAll(plist);
    }
    else
    {
        yError()<<"ServerGrabber: invalid device to be attached";
        return false;
    }
    return true;
}
bool ServerGrabber::detach()
{
    return detachAll();
}

bool ServerGrabber::openDeferredAttach(Searchable &prop){
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool ServerGrabber::openAndAttachSubDevice(Searchable &prop){
    PolyDriverList plist;
    if(param.twoCameras){
        Property p,p2;
        poly  = new PolyDriver;
        poly2 = new PolyDriver;
        p.fromString(prop.findGroup("LEFT").toString().c_str());
        p2.fromString(prop.findGroup("RIGHT").toString().c_str());
        if(param.cap==COLOR){
            p.put("pixelType", VOCAB_PIXEL_RGB);
            p2.put("pixelType", VOCAB_PIXEL_RGB);
        }
        else
        {
            p.put("pixelType", VOCAB_PIXEL_MONO);
            p2.put("pixelType", VOCAB_PIXEL_MONO);
        }
        if(p.find("height").asInt() != p2.find("height").asInt() ||
           p.find("width").asInt() != p2.find("width").asInt())
        {
            yError()<<"ServerGrabber: error in the configuration file, the two images have to have the same dimensions";
            return false;
        }
        //COSA FA? Serve? Guardarci
        //p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
        // if error occour during open, quit here.
        poly->open(p);
        poly2->open(p2);

        if (!(poly->isValid()) || !(poly2->isValid()))
        {
            yError("ServerGrabber: opening devices... FAILED\n");
            return false;
        }
        PolyDriverDescriptor pd(poly,"LEFT");
        PolyDriverDescriptor pd2(poly2,"RIGHT");
        plist.push(pd);
        plist.push(pd2);
        //The thread is started by attachAll()
        if(!attachAll(plist))
            return false;
    }
    else
    {
        Property p;
        poly  = new PolyDriver;
        p.fromString(prop.toString().c_str());
        if(param.cap==COLOR){
            p.put("pixelType", VOCAB_PIXEL_RGB);
        }
        else
        {
            p.put("pixelType", VOCAB_PIXEL_MONO);
        }
        p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
        p.unput("device");
        p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

        // if error occour during open, quit here.
        poly->open(p);

        if (!(poly->isValid()))
        {
            yError("opening controlBoardWrapper2 subdevice... FAILED\n");
            return false;
        }
        PolyDriverDescriptor pd(poly,"poly");
        plist.push(pd);
        //The thread is started by attachAll()
        if(!attachAll(plist))
            return false;
    }
    isSubdeviceOwned = true;
    return true;
}

bool ServerGrabber::threadInit()
{
    if(param.twoCameras && !param.split)
    {
        if(param.cap==COLOR)
        {
            img= new ImageOf<PixelRgb>;
            img->resize(fgImage->width(),fgImage->height());
            img2= new ImageOf<PixelRgb>;
            img2->resize(fgImage2->width(),fgImage2->height());
        }
        else
        {
            img_Raw= new ImageOf<PixelMono>;
            img_Raw->resize(fgImageRaw->width(),fgImageRaw->height());
            img2_Raw= new ImageOf<PixelMono>;
            img2_Raw->resize(fgImageRaw2->width(),fgImageRaw2->height());
        }
    }
    return true;
}

void ServerGrabber::threadRelease(){

    if(param.twoCameras && !param.split)
    {
        if(param.cap==COLOR)
        {
            if(img!=YARP_NULLPTR)
            {
                delete img;
                img=YARP_NULLPTR;
            }
            if(img2!=YARP_NULLPTR)
            {
                delete img2;
                img2=YARP_NULLPTR;
            }
        }
        else
        {
            if(img_Raw!=YARP_NULLPTR)
            {
                delete img_Raw;
                img_Raw=YARP_NULLPTR;
            }
            if(img2_Raw!=YARP_NULLPTR)
            {
                delete img2_Raw;
                img2_Raw=YARP_NULLPTR;
            }
        }
    }

}

void ServerGrabber::run()
{
    if(param.twoCameras)
    {
        if(param.split)
        {
            FlexImage& flex_i=pImg.prepare();
            FlexImage& flex_i2=pImg2.prepare();
            //TODO scrivi su porte
            if(param.cap==COLOR)
            {
                if(fgImage!=YARP_NULLPTR && fgImage2 !=YARP_NULLPTR)
                {
                    flex_i.setPixelCode(VOCAB_PIXEL_RGB);
                    flex_i.resize(fgImage->width(),fgImage->height());
                    flex_i2.setPixelCode(VOCAB_PIXEL_RGB);
                    flex_i2.resize(fgImage2->width(),fgImage2->height());
                    ImageOf<PixelRgb> i;
                    ImageOf<PixelRgb> i2;
                    //ASK ma flex_i.width e height saranno 0 giusto?
                    i.setExternal(flex_i.getRawImage(),flex_i.width(),flex_i.height());
                    fgImage->getImage(i);
                    i2.setExternal(flex_i2.getRawImage(),flex_i2.width(),flex_i2.height());
                    fgImage2->getImage(i2);
                }
                else
                    yError()<<"ServerGrabber: Image not captured.. check hardware configuration";

            }
            if(param.cap==RAW)
            {
                if(fgImageRaw!=YARP_NULLPTR && fgImageRaw2 !=YARP_NULLPTR)
                {
                    flex_i.setPixelCode(VOCAB_PIXEL_MONO);
                    flex_i.resize(fgImageRaw->width(),fgImageRaw->height());
                    flex_i2.setPixelCode(VOCAB_PIXEL_MONO);
                    flex_i2.resize(fgImageRaw2->width(),fgImageRaw2->height());
                    ImageOf<PixelMono> i;
                    ImageOf<PixelMono> i2;
                    i.setExternal(flex_i.getRawImage(),flex_i.width(),flex_i.height());
                    fgImageRaw->getImage(i);
                    i2.setExternal(flex_i2.getRawImage(),flex_i2.width(),flex_i2.height());
                    fgImageRaw2->getImage(i2);
                }
                else
                    yError()<<"ServerGrabber: Image not captured.. check hardware configuration";

            }
            Stamp s = Stamp(count,Time::now());
            pImg.setStrict(!param.canDrop);
            pImg.setEnvelope(s);
            pImg.write();
            pImg2.setStrict(!param.canDrop);
            Stamp s2 = Stamp(count2,Time::now());
            pImg2.setEnvelope(s2);
            pImg2.write();
            count++;
            count2++;

        }
        else
        {
            FlexImage& flex_i=pImg.prepare();
            if(param.cap==COLOR)
            {
                if(fgImage!=YARP_NULLPTR && fgImage2 !=YARP_NULLPTR)
                {
                    flex_i.setPixelCode(VOCAB_PIXEL_RGB);
                    flex_i.resize(fgImage->width()*2,fgImage->height());
                    fgImage->getImage(*img);
                    fgImage2->getImage(*img2);
                    int singleImage_rowSizeByte= img->getRowSize();
                    unsigned char * pixelLeft=img->getRawImage();
                    unsigned char * pixelRight=img2->getRawImage();
                    unsigned char * pixelOutLeft=flex_i.getRawImage();
                    unsigned char * pixelOutRight=flex_i.getRawImage()+ singleImage_rowSizeByte;
                    for(int h=0; h<img->height(); h++)
                    {
                        memcpy(pixelOutLeft, pixelLeft,singleImage_rowSizeByte);
                        memcpy(pixelOutRight, pixelRight, singleImage_rowSizeByte);
                        pixelOutLeft+=2*singleImage_rowSizeByte;
                        pixelOutRight+=2*singleImage_rowSizeByte;
                        pixelLeft+= singleImage_rowSizeByte;
                        pixelRight+= singleImage_rowSizeByte;
                    }
                }
                else
                    yError()<<"ServerGrabber: Image not captured.. check hardware configuration";

            }
            if(param.cap==RAW)
            {
                if(fgImageRaw!=YARP_NULLPTR && fgImageRaw2 !=YARP_NULLPTR)
                {
                    flex_i.setPixelCode(VOCAB_PIXEL_MONO);
                    flex_i.resize(fgImageRaw->width()*2,fgImageRaw->height());
                    fgImageRaw->getImage(*img_Raw);
                    fgImageRaw2->getImage(*img2_Raw);
                    int singleImage_rowSizeByte= img_Raw->getRowSize();
                    unsigned char * pixelLeft=img_Raw->getRawImage();
                    unsigned char * pixelRight=img2_Raw->getRawImage();
                    unsigned char * pixelOutLeft=flex_i.getRawImage();
                    unsigned char * pixelOutRight=flex_i.getRawImage()+ singleImage_rowSizeByte;
                    for(int h=0; h<img_Raw->height(); h++)
                    {
                        memcpy(pixelOutLeft, pixelLeft,singleImage_rowSizeByte);
                        memcpy(pixelOutRight, pixelRight, singleImage_rowSizeByte);
                        pixelOutLeft+=2*singleImage_rowSizeByte;
                        pixelOutRight+=2*singleImage_rowSizeByte;
                        pixelLeft+= singleImage_rowSizeByte;
                        pixelRight+= singleImage_rowSizeByte;
                    }
                }
                else
                    yError()<<"ServerGrabber: Image not captured.. check hardware configuration";

            }

            Stamp s = Stamp(count,Time::now());
            pImg.setStrict(!param.canDrop);
            pImg.setEnvelope(s);
            pImg.write();
            count++;
        }
    }
    else
    {
        FlexImage& flex_i=pImg.prepare();
        if(param.cap==COLOR)
        {
            if(fgImage!=YARP_NULLPTR)
            {
                ImageOf<PixelRgb> i;
                flex_i.setPixelCode(VOCAB_PIXEL_RGB);
                flex_i.resize(fgImage->width(),fgImage->height());
                i.setExternal(flex_i.getRawImage(),flex_i.width(),flex_i.height());
                fgImage->getImage(i);
            }
            else
                yError()<<"ServerGrabber: Image not captured.. check hardware configuration";
        }
        if(param.cap==RAW)
        {
            if(fgImageRaw!=YARP_NULLPTR)
            {
                ImageOf<PixelMono> i;
                flex_i.setPixelCode(VOCAB_PIXEL_MONO);
                flex_i.resize(fgImageRaw->width(),fgImageRaw->height());
                i.setExternal(flex_i.getRawImage(),flex_i.width(),flex_i.height());
                fgImageRaw->getImage(i);
            }
            else
                yError()<<"ServerGrabber: Image not captured.. check hardware configuration";
        }
        Stamp s = Stamp(count,Time::now());
        pImg.setStrict(!param.canDrop);
        pImg.setEnvelope(s);
        pImg.write();
        count++;
    }
}

void ServerGrabber::shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest)
{
    dest.setPixelCode(src.getPixelCode());
    dest.setPixelSize(src.getPixelSize());
    dest.setQuantum(src.getQuantum());
    dest.setExternal(src.getRawImage(), src.width(), src.height());
}
