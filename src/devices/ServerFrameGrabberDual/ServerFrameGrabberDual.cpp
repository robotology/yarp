/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ServerFrameGrabberDual.h"

#include <yarp/os/Log.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/ImageUtils.h>
#include <yarp/os/PortablePair.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/GenericVocabs.h>

#include <cstring>
#include <algorithm> // std::for_each

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(SERVERGRABBER, "yarp.device.grabberDual")
}


bool DC1394Parser::configure(IFrameGrabberControlsDC1394 *interface)
{
    fgCtrl_DC1394 = interface;
    return true;
}

bool DC1394Parser::respond(const Bottle& cmd, Bottle& response)
{
    int code = cmd.get(1).asVocab();
    if (fgCtrl_DC1394)
    {
        switch(code)
        {
        case VOCAB_DRGETMSK: // VOCAB_DRGETMSK 12
            response.addInt32(int(fgCtrl_DC1394->getVideoModeMaskDC1394()));
            return true;
        case VOCAB_DRGETVMD: // VOCAB_DRGETVMD 13
            response.addInt32(int(fgCtrl_DC1394->getVideoModeDC1394()));
            return true;
        case VOCAB_DRSETVMD: // VOCAB_DRSETVMD 14
            response.addInt32(int(fgCtrl_DC1394->setVideoModeDC1394(cmd.get(1).asInt32())));
            return true;
        case VOCAB_DRGETFPM: // VOCAB_DRGETFPM 15
            response.addInt32(int(fgCtrl_DC1394->getFPSMaskDC1394()));
            return true;
        case VOCAB_DRGETFPS: // VOCAB_DRGETFPS 16
            response.addInt32(int(fgCtrl_DC1394->getFPSDC1394()));
            return true;
        case VOCAB_DRSETFPS: // VOCAB_DRSETFPS 17
            response.addInt32(int(fgCtrl_DC1394->setFPSDC1394(cmd.get(1).asInt32())));
            return true;

        case VOCAB_DRGETISO: // VOCAB_DRGETISO 18
            response.addInt32(int(fgCtrl_DC1394->getISOSpeedDC1394()));
            return true;
        case VOCAB_DRSETISO: // VOCAB_DRSETISO 19
            response.addInt32(int(fgCtrl_DC1394->setISOSpeedDC1394(cmd.get(1).asInt32())));
            return true;

        case VOCAB_DRGETCCM: // VOCAB_DRGETCCM 20
            response.addInt32(int(fgCtrl_DC1394->getColorCodingMaskDC1394(cmd.get(1).asInt32())));
            return true;
        case VOCAB_DRGETCOD: // VOCAB_DRGETCOD 21
            response.addInt32(int(fgCtrl_DC1394->getColorCodingDC1394()));
            return true;
        case VOCAB_DRSETCOD: // VOCAB_DRSETCOD 22
            response.addInt32(int(fgCtrl_DC1394->setColorCodingDC1394(cmd.get(1).asInt32())));
            return true;

        case VOCAB_DRGETF7M: // VOCAB_DRGETF7M 25
            {
                unsigned int xstep,ystep,xdim,ydim,xoffstep,yoffstep;
                fgCtrl_DC1394->getFormat7MaxWindowDC1394(xdim,ydim,xstep,ystep,xoffstep,yoffstep);
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
                fgCtrl_DC1394->getFormat7WindowDC1394(xdim,ydim,x0,y0);
                response.addInt32(xdim);
                response.addInt32(ydim);
                response.addInt32(x0);
                response.addInt32(y0);
            }
            return true;
        case VOCAB_DRSETWF7: // VOCAB_DRSETWF7 27
            response.addInt32(int(fgCtrl_DC1394->setFormat7WindowDC1394(cmd.get(1).asInt32(),cmd.get(2).asInt32(),cmd.get(3).asInt32(),cmd.get(4).asInt32())));
            return true;
        case VOCAB_DRSETOPM: // VOCAB_DRSETOPM 28
            response.addInt32(int(fgCtrl_DC1394->setOperationModeDC1394(cmd.get(1).asInt32()!=0)));
            return true;
        case VOCAB_DRGETOPM: // VOCAB_DRGETOPM 29
            response.addInt32(fgCtrl_DC1394->getOperationModeDC1394());
            return true;

        case VOCAB_DRSETTXM: // VOCAB_DRSETTXM 30
            response.addInt32(int(fgCtrl_DC1394->setTransmissionDC1394(cmd.get(1).asInt32()!=0)));
            return true;
        case VOCAB_DRGETTXM: // VOCAB_DRGETTXM 31
            response.addInt32(fgCtrl_DC1394->getTransmissionDC1394());
            return true;
        case VOCAB_DRSETBCS: // VOCAB_DRSETBCS 34
            response.addInt32(int(fgCtrl_DC1394->setBroadcastDC1394(cmd.get(1).asInt32()!=0)));
            return true;
        case VOCAB_DRSETDEF: // VOCAB_DRSETDEF 35
            response.addInt32(int(fgCtrl_DC1394->setDefaultsDC1394()));
            return true;
        case VOCAB_DRSETRST: // VOCAB_DRSETRST 36
            response.addInt32(int(fgCtrl_DC1394->setResetDC1394()));
            return true;
        case VOCAB_DRSETPWR: // VOCAB_DRSETPWR 37
            response.addInt32(int(fgCtrl_DC1394->setPowerDC1394(cmd.get(1).asInt32()!=0)));
            return true;
        case VOCAB_DRSETCAP: // VOCAB_DRSETCAP 38
            response.addInt32(int(fgCtrl_DC1394->setCaptureDC1394(cmd.get(1).asInt32()!=0)));
            return true;
        case VOCAB_DRSETBPP: // VOCAB_DRSETCAP 39
            response.addInt32(int(fgCtrl_DC1394->setBytesPerPacketDC1394(cmd.get(1).asInt32())));
            return true;
        case VOCAB_DRGETBPP: // VOCAB_DRGETTXM 40
            response.addInt32(fgCtrl_DC1394->getBytesPerPacketDC1394());
            return true;
        }
    }
    else
    {
        yCWarning(SERVERGRABBER) << "DC1394Parser:  firewire interface not implemented in subdevice, some features could not be available";
//        response.clear();
//        response.addVocab(VOCAB_FAILED);
        return DeviceResponder::respond(cmd,response);
    }

    return true;
}

// **********ServerGrabberResponder**********

ServerGrabberResponder::ServerGrabberResponder(bool _left) :
        left(_left)
{
}

bool ServerGrabberResponder::configure(ServerGrabber* _server)
{
    if(_server)
    {
        server=_server;
        return true;
    }
    yCError(SERVERGRABBER) << "ServerGrabberResponder: invalid server pointer";
    return false;
}

bool ServerGrabberResponder::respond(const yarp::os::Bottle &command, yarp::os::Bottle &reply){
    if(server)
    {
        if(server->respond(command,reply,left,false))
        {
            return true;
        }
        else
        {
            return DeviceResponder::respond(command, reply);
        }
    }
    else
        return false;
}

// **********ServerGrabber**********

ServerGrabber::ServerGrabber() :
        PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

ServerGrabber::~ServerGrabber()
{
    if(param.active)
        close();
}

bool ServerGrabber::close() {
    if (!param.active) {
        return false;
    }
    stopThread();

    param.active = false;
    pImg.interrupt();
    pImg.close();
    rpcPort.interrupt();
    rpcPort.close();

    if(responder){
        delete responder;
        responder=nullptr;
    }

    if(param.split)
    {
        pImg2.interrupt();
        pImg2.close();
    }


    if(param.twoCameras)
    {
        rpcPort2.interrupt();
        rpcPort2.close();
    }

    cleanUp();
    if(poly)
    {
        poly->close();
        delete poly;
        poly=nullptr;
    }

    if(responder2)
    {
        delete responder2;
        responder=nullptr;
    }

    if(isSubdeviceOwned && poly2)
    {
        poly2->close();
        delete poly2;
        poly2=nullptr;
    }
    isSubdeviceOwned=false;
    if (p2!=nullptr) {
        delete p2;
        p2 =nullptr;
    }
    return true;
}

bool ServerGrabber::open(yarp::os::Searchable& config) {
    if (param.active) {
        yCError(SERVERGRABBER, "Did you just try to open the same ServerGrabber twice?");
        return false;
    }

    if(!fromConfig(config))
    {
        yCError(SERVERGRABBER) << "Device ServerGrabber failed to open, check previous log for error messages.";
        return false;
    }

    if(!initialize_YARP(config))
    {
        yCError(SERVERGRABBER) <<"Error initializing YARP ports";
        return false;
    }

    if(isSubdeviceOwned){
        if(! openAndAttachSubDevice(config))
        {
            yCError(SERVERGRABBER, "Error while opening subdevice");
            return false;
        }
    }
    else
    {
        yCInfo(SERVERGRABBER) << "Running, waiting for attach...";
        if(!openDeferredAttach(config))
        return false;
    }


    param.active = true;
//    //ASK/TODO update usage and see if we need to add DeviceResponder as dependency
//    DeviceResponder::makeUsage();
//    addUsage("[set] [bri] $fBrightness", "set brightness");
//    addUsage("[set] [expo] $fExposure", "set exposure");
//    addUsage("[set] [shar] $fSharpness", "set sharpness");
//    addUsage("[set] [whit] $fBlue $fRed", "set white balance");
//    addUsage("[set] [hue] $fHue", "set hue");
//    addUsage("[set] [satu] $fSaturation", "set saturation");
//    addUsage("[set] [gamm] $fGamma", "set gamma");
//    addUsage("[set] [shut] $fShutter", "set shutter");
//    addUsage("[set] [gain] $fGain", "set gain");
//    addUsage("[set] [iris] $fIris", "set iris");

//    addUsage("[get] [bri]",  "get brightness");
//    addUsage("[get] [expo]", "get exposure");
//    addUsage("[get] [shar]", "get sharpness");
//    addUsage("[get] [whit]", "get white balance");
//    addUsage("[get] [hue]",  "get hue");
//    addUsage("[get] [satu]", "get saturation");
//    addUsage("[get] [gamm]", "get gamma");
//    addUsage("[get] [shut]", "get shutter");
//    addUsage("[get] [gain]", "get gain");
//    addUsage("[get] [iris]", "get iris");

//    addUsage("[get] [w]", "get width of image");
//    addUsage("[get] [h]", "get height of image");


    return true;
}

bool ServerGrabber::fromConfig(yarp::os::Searchable &config)
{
    if(config.check("period","refresh period(in ms) of the broadcasted values through yarp ports")
            && config.find("period").isInt32())
        period = config.find("period").asInt32() / 1000.0;
    else
        yCWarning(SERVERGRABBER) << "Period parameter not found, using default of"<< DEFAULT_THREAD_PERIOD << "s";
    if((config.check("subdevice")) && (config.check("left_config") || config.check("right_config")))
    {
        yCError(SERVERGRABBER) << "Found both 'subdevice' and 'left_config/right_config' parameters...";
        return false;
    }
    if(!config.check("subdevice", "name of the subdevice to use as a data source")
            && config.check("left_config","name of the ini file containing the configuration of one of two subdevices to use as a data source")
            && config.check("right_config" , "name of the ini file containing the configuration of one of two subdevices to use as a data source"))
        param.twoCameras=true;
    if(config.check("twoCameras", "if true ServerGrabber will open and handle two devices, if false only one"))//extra conf parameter for the yarprobotinterface
        param.twoCameras=config.find("twoCameras").asBool();
    if(config.check("split", "set 'true' to split the streaming on two different ports"))
        param.split=config.find("split").asBool();
    if(config.check("capabilities","two capabilities supported, COLOR and RAW respectively for rgb and raw streaming"))
    {
        if(config.find("capabilities").asString()=="COLOR")
            param.cap=COLOR;
        else if(config.find("capabilities").asString()=="RAW")
            param.cap=RAW;
    }
    else
        yCWarning(SERVERGRABBER) << "'capabilities' parameter not found or misspelled, the option available are COLOR(default) and RAW, using default";
    param.canDrop = !config.check("no_drop","if present, use strict policy for sending data");
    param.addStamp = config.check("stamp","if present, add timestamps to data");

    param.singleThreaded =
        config.check("single_threaded",
                     "if present, operate in single threaded mode")!=0;
    //TODO audio part
    std::string rootName;
    rootName = config.check("name",Value("/grabber"),
                            "name of port to send data on").asString();
    if(!param.twoCameras && param.split)
        param.splitterMode = true;

    responder = new ServerGrabberResponder(true);
    if(!responder->configure(this))
        return false;
    if(param.twoCameras)
    {
        responder2 = new ServerGrabberResponder(false);
        if(!responder2->configure(this))
            return false;

        rpcPort_Name  = rootName + "/left/rpc";
        rpcPort2_Name  = rootName + "/right/rpc";
        if(param.split)
        {
            pImg_Name = rootName + "/left";
            pImg2_Name = rootName + "/right";
        }
        else
            pImg_Name = rootName;

        // check if we need to create subdevice or if they are
        // passed later on thorugh attachAll()
        if(config.check("left_config") && config.check("right_config"))
        {
            isSubdeviceOwned=true;
        }
        else
        {
            isSubdeviceOwned=false;
        }
    }
    else
    {
        if(param.split)
        {
            responder2 = new ServerGrabberResponder(false);
            if(!responder2->configure(this))
                return false;
            pImg_Name = rootName + "/left";
            pImg2_Name = rootName + "/right";
        }
        else
        {
            pImg_Name = rootName;
        }
        rpcPort_Name  = rootName + "/rpc";
        if(config.check("subdevice"))
        {
            isSubdeviceOwned=true;
        }
        else
        {
            isSubdeviceOwned=false;
        }
    }


    return true;
}

bool ServerGrabber::initialize_YARP(yarp::os::Searchable &params)
{
    // Open ports
    bool bRet;
    bRet = true;
    if(!rpcPort.open(rpcPort_Name))
    {
        yCError(SERVERGRABBER) << "Unable to open rpc Port" << rpcPort_Name.c_str();
        bRet = false;
    }
    rpcPort.setReader(*responder);

    pImg.promiseType(Type::byName("yarp/image"));
    pImg.setWriteOnly();
    if(!pImg.open(pImg_Name))
    {
        yCError(SERVERGRABBER) << "Unable to open image streaming Port" << pImg_Name.c_str();
        bRet = false;
    }
    pImg.setReader(*responder);

    if(param.twoCameras)
    {
        if(!rpcPort2.open(rpcPort2_Name))
        {
            yCError(SERVERGRABBER) << "Unable to open rpc Port" << rpcPort2_Name.c_str();
            bRet = false;
        }
        rpcPort2.setReader(*responder2);
    }
    if(param.split)
    {
        pImg2.promiseType(Type::byName("yarp/image"));
        pImg2.setWriteOnly();

        if(!pImg2.open(pImg2_Name))
        {
            yCError(SERVERGRABBER) << "Unable to open image streaming Port" << pImg2_Name.c_str();
            bRet = false;
        }
        pImg2.setReader(*responder2);
    }

    return bRet;
}

bool ServerGrabber::respond(const yarp::os::Bottle& cmd,
                                  yarp::os::Bottle& response, bool left, bool both=false) {
    int code = cmd.get(0).asVocab();
    Bottle response2;
    switch (code)
    {
    case VOCAB_FRAMEGRABBER_IMAGE:
    {
        switch (cmd.get(1).asVocab())
        {
            case VOCAB_GET:
            {
                switch (cmd.get(2).asVocab())
                {
                    case VOCAB_CROP:
                    {
                        response.clear();
                        // If the device driver support it, use the device implementation, because it may be more efficient.
                        // If not, acquire the whole image and crop it here before sending it.

                        Bottle *list = cmd.get(4).asList();
                        int nPoints = list->size() /2;          //  divided by 2 because each pixel is identified by 2 numbers (u,v)

                        yarp::sig::VectorOf<std::pair<int, int> > vertices;
                        vertices.resize(nPoints);

                        for(int i=0; i<nPoints; i++)
                        {
                            vertices[i].first = list->get(i*2).asInt32();
                            vertices[i].second = list->get(i*2 +1).asInt32();
                        }

                        ImageOf< PixelRgb > cropped;

                        // Choose the interface and eventual offset depending on case.

                        /* HW/SW configurations here:  (1a, 1b, 2a, 2b), for each one the user can request a crop on left or right image
                         * 1) single HW camera as a source
                         *  1a) split false:   a single image to handle
                         *  1b) split true :   2 images, I have to handle left or right image. If user request a crop in the right side,
                         *                      of the image, then add an offset
                         *
                         * 2) two HW sources
                         *  2a) split true:   choose appropriate image source based on left/right request
                         *  2b) split false:  choose appropriate image source based on crop position. Crop request have to belong to a
                         *                      single frame, either left or right. Example: 2 cameras with 320x240 pixels each placed
                         *                      one after the other, generates a single stitched image of 640x240.
                         *                      Anyway a crop request like (200,100)(400,200) shall be rejected, even if it could be
                         *                      considered as a part of the image resulting from the stitch.
                         *                      Right now the decision is took based on the first point of vector 'vertices', since all
                         *                      points are expected to belong to the same frame (left/right)
                         *
                         */

                        // Default values here are valid for cases 1a and `left` side of 2a
                        IFrameGrabberImage *imageInterface = fgImage;

                        if(param.twoCameras == false)   // a single HW source of images
                        {
                            imageInterface = fgImage;
                            if(left == false)                               // if left is false, implicitly split is true
                            {
                                std::for_each(vertices.begin(), vertices.end(), [=](auto &pt) { pt.first += imageInterface->width() / 2; }); // 1b
                            }

                        }
                        else
                        {
                            if(param.split)                                 // 2a, right image
                            {
                                if(left == false)
                                {
                                    imageInterface = fgImage2;
                                    // no offset
                                }
                            }
                            else
                            {
                                if(vertices[0].first >= fgImage->width())    // 2b, right image
                                {
                                    imageInterface = fgImage2;
                                    std::for_each(vertices.begin(), vertices.end(), [=](auto &pt) { pt.first -= fgImage->width(); });
                                }
                            }

                        }

                        if(imageInterface != nullptr)
                        {
                            if(imageInterface->getImageCrop((cropType_id_t) cmd.get(3).asVocab(), vertices, cropped) )
                            {
                                // use the device output
                            }
                            else
                            {
                                // In case the device has not yet implemented this feature, do it here (less efficient)
                                if(cmd.get(3).asVocab() == YARP_CROP_RECT)
                                {
                                    if(nPoints != 2)
                                    {
                                        response.addString("GetImageCrop failed: RECT mode requires 2 vertices.");
                                        yCError(SERVERGRABBER) << "GetImageCrop failed: RECT mode requires 2 vertices, got " << nPoints;
                                        return false;
                                    }
                                    ImageOf< PixelRgb > full;
                                    imageInterface->getImage(full);

                                    if(!utils::cropRect(full, vertices[0], vertices[1], cropped))
                                    {
                                        response.addString("GetImageCrop failed: utils::cropRect error.");
                                        yCError(SERVERGRABBER) << "GetImageCrop failed: utils::cropRect error";
                                        return false;
                                    }
                                }
                                else if(cmd.get(3).asVocab() == YARP_CROP_LIST)
                                {
                                    response.addString("List type not yet implemented");
                                }
                                else
                                {
                                    response.addString("Crop type unknown");
                                }
                            }
                        }

                        response.addVocab(VOCAB_CROP);
                        response.addVocab(VOCAB_IS);
                        response.addInt32(cropped.width());                       // Actual width  of image in pixels, to check everything is ok
                        response.addInt32(cropped.height());                      // Actual height of image in pixels, to check everything is ok

                        response.add(Value(cropped.getRawImage(), cropped.getRawImageSize()));
                        return true;
                    } break;
                } break;

            } break;

            case VOCAB_SET:   // Nothing to do here yet
            default:
            {
                yCError(SERVERGRABBER) << "FrameGrabberImage interface received an unknown command " << cmd.toString();
            } break;

        }

    } break;

    // first check if requests are coming from new iFrameGrabberControl2 interface and process them
    case VOCAB_FRAMEGRABBER_CONTROL:
    {
        if(param.twoCameras)
        {
            bool ret;
            if(both){
                ret=ifgCtrl_Parser.respond(cmd, response);
                ret&=ifgCtrl2_Parser.respond(cmd, response2);
                if(!ret || (response!=response2))
                {
                    response.clear();
                    response.addVocab(VOCAB_FAILED);
                    ret=false;
                    yCWarning(SERVERGRABBER) << "Response different among cameras or failed";
                }
            }
            else
            {
                if(left)
                {
                    ret=ifgCtrl_Parser.respond(cmd, response);
                }
                else
                {
                    ret=ifgCtrl2_Parser.respond(cmd, response);
                }
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
            if(ret)
            {
                switch (cmd.get(2).asVocab())
                {
                    //Only the intrinsic parameters are allowed to be different among the two cameras
                    // so we give both responses appending one to the other.
                    case VOCAB_INTRINSIC_PARAM:
                    {
                        Bottle& newResponse = response.addList();
                        newResponse.append(*response2.get(3).asList());
                        break;
                    }
                    //In general if the two response are different we send a FAIL vocab
                    default:
                    {
                        if(response!=response2)
                        {
                            response.clear();
                            response.addVocab(VOCAB_FAILED);
                            ret=false;
                            yCWarning(SERVERGRABBER) << "Response different among cameras or failed";
                        }
                        break;
                    }
                }
            }

            return ret;
        }
        else
            return rgbParser.respond(cmd,response);
    } break;
        //////////////////
        // DC1394 COMMANDS
        //////////////////
    case VOCAB_FRAMEGRABBER_CONTROL_DC1394:
    {
        if(param.twoCameras)
        {
            bool ret;
            if(both)
            {
                ret=ifgCtrl_DC1394_Parser.respond(cmd, response);
                ret&=ifgCtrl2_DC1394_Parser.respond(cmd, response2);
                if(!ret || (response!=response2))
                {
                    response.clear();
                    response.addString("command not recognized");
                    ret=false;
                    yCWarning(SERVERGRABBER) << "Responses different among cameras or failed";

                }
            }
            else
            {
                if(left)
                {
                    ret=ifgCtrl_DC1394_Parser.respond(cmd, response);
                }
                else
                {
                    ret=ifgCtrl2_DC1394_Parser.respond(cmd, response);
                }
            }
            return ret;
        }
        else
            return ifgCtrl_DC1394_Parser.respond(cmd, response);
    } break;
    }
    yCError(SERVERGRABBER) << "Command not recognized" << cmd.toString();
    return false;
}

bool ServerGrabber::attachAll(const PolyDriverList &device2attach)
{
    bool ret=false;
    if(param.twoCameras)
    {
        bool leftDone=false;//for avoiding double left
        bool rightDone=false;//for avoiding double right
        if (device2attach.size() != 2)
        {
            yCError(SERVERGRABBER, "Expected two devices to be attached");
            return false;
        }
        for(int i=0;i<device2attach.size();i++)
        {
            yarp::dev::PolyDriver * Idevice2attach = device2attach[i]->poly;
            if (!Idevice2attach->isValid())
            {
                yCError(SERVERGRABBER) << "Device " << device2attach[i]->key << " to attach to is not valid ... cannot proceed";
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
                yCError(SERVERGRABBER) << "Failed to attach. The two targets must be LEFT RIGHT and devices must implement"
                            " either IFrameGrabberImage or IFrameGrabberImageRaw";
                return false;

            }
        }
        switch(param.cap)
        {
            case COLOR :
            {
                if((fgImage==nullptr) || (fgImage2==nullptr))
                {
                    yCError(SERVERGRABBER) << "Capability required not supported";
                    return false;
                }
            }
            break;
            case RAW :
            {
                if((fgImageRaw==nullptr) || (fgImageRaw2==nullptr))
                {
                    yCError(SERVERGRABBER) << "Capability required not supported";
                    return false;
                }
            }
        }
        if((rgbVis_p == nullptr) || (rgbVis_p2 == nullptr))
        {
            yCWarning(SERVERGRABBER) << "Targets has not IVisualParamInterface, some features cannot be available";
        }
        //Configuring parsers
        if(rgbVis_p != nullptr && rgbVis_p2 != nullptr)
        {
            if(!(rgbParser.configure(rgbVis_p)) || !(rgbParser2.configure(rgbVis_p2)))
            {
                yCError(SERVERGRABBER) << "Error configuring interfaces for parsers";
                return false;
            }
        }
        if(fgCtrl != nullptr && fgCtrl2 != nullptr)
        {
            if(!(ifgCtrl_Parser.configure(fgCtrl)) || !(ifgCtrl2_Parser.configure(fgCtrl2)))
            {
                yCError(SERVERGRABBER) << "Error configuring interfaces for parsers";
                return false;
            }
        }
        if(fgCtrl_DC1394 != nullptr && fgCtrl2_DC1394 != nullptr)
        {
            if(!(ifgCtrl_DC1394_Parser.configure(fgCtrl_DC1394)) || !(ifgCtrl2_DC1394_Parser.configure(fgCtrl2_DC1394)))
            {
                yCError(SERVERGRABBER) << "Error configuring interfaces for parsers";
                return false;
            }
        }
    }
    else{
        if (device2attach.size() != 1)
        {
            yCError(SERVERGRABBER, "Expected one device to be attached");
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
                if(fgImage==nullptr)
                {
                    yCError(SERVERGRABBER) << "Capability required not supported";
                    return false;
                }
            }
            break;
            case RAW :
            {
                if(fgImageRaw==nullptr)
                {
                    yCError(SERVERGRABBER) << "Capability required not supported";
                    return false;
                }
            }
        }

        if (!Idevice2attach->isValid())
        {
            yCError(SERVERGRABBER) << "Device " << device2attach[0]->key << " to attach to is not valid ... cannot proceed";
            return false;
        }

        if(rgbVis_p == nullptr)
        {
            yCWarning(SERVERGRABBER) << "Targets has not IVisualParamInterface, some features cannot be available";
        }

        //Configuring parsers
        if(rgbVis_p != nullptr)
        {
            if(!(rgbParser.configure(rgbVis_p)))
            {
                yCError(SERVERGRABBER) << "Error configuring interfaces for parsers";
                return false;
            }
        }
        if(fgCtrl != nullptr)
        {
            if(!(ifgCtrl_Parser.configure(fgCtrl)))
            {
                yCError(SERVERGRABBER) << "Error configuring interfaces for parsers";
                return false;
            }
        }

        if(fgCtrl_DC1394 != nullptr)
        {
            if(!(ifgCtrl_DC1394_Parser.configure(fgCtrl_DC1394)))
            {
                yCError(SERVERGRABBER) << "Error configuring interfaces for parsers";
                return false;
            }
        }
    }

    PeriodicThread::setPeriod(period);
    ret = PeriodicThread::start();

    return ret;
}
bool ServerGrabber::detachAll()
{
    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
        return false;
    stopThread();
    return true;

}
void ServerGrabber::stopThread()
{
    if (yarp::os::PeriodicThread::isRunning())
        yarp::os::PeriodicThread::stop();

    rgbVis_p       = nullptr;
    rgbVis_p2      = nullptr;
    fgImage        = nullptr;
    fgImage2       = nullptr;
    fgImageRaw     = nullptr;
    fgImageRaw2    = nullptr;
    fgCtrl         = nullptr;
    fgCtrl2        = nullptr;
    fgCtrl_DC1394  = nullptr;
    fgCtrl2_DC1394 = nullptr;
}

void ServerGrabber::setupFlexImage(const Image &_img, FlexImage &flex_i)
{
    flex_i.setPixelCode(_img.getPixelCode());
    flex_i.setQuantum(_img.getQuantum());
    flex_i.setExternal(_img.getRawImage(), _img.width(),_img.height());

}

bool ServerGrabber::attach(PolyDriver *poly)
{
    if(param.twoCameras)
    {
        yCError(SERVERGRABBER) << "Server grabber configured for two cameras, but only one provided";
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
        yCError(SERVERGRABBER) << "Invalid device to be attached";
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
        std::string file, file2;
        if(!prop.check("left_config") || !prop.check("right_config"))
        {
            yCError(SERVERGRABBER) << "Missing 'left_config' or 'right_config' filename... ";
            return false;
        }
        ResourceFinder rf, rf2;
        if(prop.check("context"))
        {
            rf.setDefaultContext(prop.find("context").asString().c_str());
            rf2.setDefaultContext(prop.find("context").asString().c_str());
        }
        file=prop.find("left_config").toString();
        file2=prop.find("right_config").toString();
        p.fromConfigFile(rf.findFileByName(file));
        p2.fromConfigFile(rf2.findFileByName(file2));
        if(p.isNull() || p2.isNull())
        {
            yCError(SERVERGRABBER) << "Unable to find files specified in 'left_config' and/or 'right_config'";
            return false;
        }
//        p.fromString(prop.findGroup("LEFT").toString().c_str());
//        p2.fromString(prop.findGroup("RIGHT").toString().c_str());
        if(param.cap==COLOR){
            p.put("pixelType", VOCAB_PIXEL_RGB);
            p2.put("pixelType", VOCAB_PIXEL_RGB);
        }
        else
        {
            p.put("pixelType", VOCAB_PIXEL_MONO);
            p2.put("pixelType", VOCAB_PIXEL_MONO);
        }
        if(p.find("height").asInt32() != p2.find("height").asInt32() ||
           p.find("width").asInt32() != p2.find("width").asInt32())
        {
            yCError(SERVERGRABBER) << "Error in the configuration file, the two images have to have the same dimensions";
            return false;
        }
        //COSA FA? Serve? Guardarci
        //p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
        // if errors occurred during open, quit here.
        poly->open(p);
        poly2->open(p2);

        if (!(poly->isValid()) || !(poly2->isValid()))
        {
            yCError(SERVERGRABBER, "Opening devices... FAILED");
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
        p.fromString(prop.toString());
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

        // if errors occurred during open, quit here.
        poly->open(p);

        if (!(poly->isValid()))
        {
            yCError(SERVERGRABBER, "opening  subdevice... FAILED");
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
    if(param.twoCameras)
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
    else
    {
        if(param.cap==COLOR)
        {
            img= new ImageOf<PixelRgb>;
            if(param.splitterMode)
            {
                img->resize(fgImage->width()/2,fgImage->height());

                img2= new ImageOf<PixelRgb>;
                img2->resize(fgImage->width()/2,fgImage->height());
            }
            else
            {
                img->resize(fgImage->width(),fgImage->height());
            }
        }
        else
        {
            img_Raw= new ImageOf<PixelMono>;
            if(param.splitterMode)
            {
                img_Raw->resize(fgImageRaw->width()/2,fgImageRaw->height());

                img2_Raw= new ImageOf<PixelMono>;
                img2_Raw->resize(fgImageRaw->width()/2,fgImageRaw->height());
            }
            else
            {
                img_Raw->resize(fgImageRaw->width(), fgImageRaw->height());
            }
        }
    }
    return true;
}

void ServerGrabber::threadRelease(){



}

void ServerGrabber::run()
{
    if(param.twoCameras)
    {
        if(param.split)
        {
            FlexImage& flex_i=pImg.prepare();
            FlexImage& flex_i2=pImg2.prepare();
            if(param.cap==COLOR)
            {
                if(fgImage!=nullptr && fgImage2 !=nullptr)
                {
                    fgImage->getImage(*img);
                    setupFlexImage(*img,flex_i);
                    fgImage2->getImage(*img2);
                    setupFlexImage(*img2,flex_i2);
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";

            }
            if(param.cap==RAW)
            {
                if(fgImageRaw!=nullptr && fgImageRaw2 !=nullptr)
                {
                    fgImageRaw->getImage(*img_Raw);
                    setupFlexImage(*img_Raw,flex_i);
                    fgImageRaw2->getImage(*img2_Raw);
                    setupFlexImage(*img2_Raw,flex_i2);
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";

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
                if(fgImage!=nullptr && fgImage2 !=nullptr)
                {
                    flex_i.setPixelCode(VOCAB_PIXEL_RGB);
                    flex_i.resize(fgImage->width()*2,fgImage->height());
                    fgImage->getImage(*img);
                    fgImage2->getImage(*img2);

                    bool ok = utils::horzConcat(*img, *img2, flex_i);
                    if (!ok)
                    {
                        yCError(SERVERGRABBER) << "Failed to concatenate images";
                        return;
                    }
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";

            }
            if(param.cap==RAW)
            {
                if(fgImageRaw!=nullptr && fgImageRaw2 !=nullptr)
                {
                    flex_i.setPixelCode(VOCAB_PIXEL_MONO);
                    flex_i.resize(fgImageRaw->width()*2,fgImageRaw->height());
                    fgImageRaw->getImage(*img_Raw);
                    fgImageRaw2->getImage(*img2_Raw);
                    bool ok = utils::horzConcat(*img_Raw, *img2_Raw, flex_i);
                    if (!ok)
                    {
                        yCError(SERVERGRABBER) << "Failed to concatenate images";
                        return;
                    }
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";

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
        if(param.splitterMode)
        {
            FlexImage& flex_i=pImg.prepare();
            FlexImage& flex_i2=pImg2.prepare();

            if(param.cap==COLOR)
            {
                if(fgImage!=nullptr)
                {
                    yarp::sig::ImageOf<yarp::sig::PixelRgb> inputImage;
                    fgImage->getImage(inputImage);

                    bool ok = utils::vertSplit(inputImage,*img,*img2);
                    if (!ok)
                    {
                        yCError(SERVERGRABBER) << "Failed to split the image";
                        return;
                    }

                    setupFlexImage(*img,flex_i);
                    setupFlexImage(*img2,flex_i2);
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";
            }
            if(param.cap==RAW)
            {
                if(fgImageRaw!=nullptr)
                {
                    yarp::sig::ImageOf<yarp::sig::PixelMono> inputImage;
                    fgImageRaw->getImage(inputImage);

                    bool ok = utils::vertSplit(inputImage,*img_Raw,*img2_Raw);
                    if (!ok)
                    {
                        yCError(SERVERGRABBER) << "Failed to split the image";
                        return;
                    }

                    setupFlexImage(*img_Raw,flex_i);
                    setupFlexImage(*img2_Raw,flex_i2);

                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";
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
                if(fgImage!=nullptr)
                {
                    fgImage->getImage(*img);
                    setupFlexImage(*img,flex_i);
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";
            }
            if(param.cap==RAW)
            {
                if(fgImageRaw!=nullptr)
                {
                    fgImageRaw->getImage(*img_Raw);
                    setupFlexImage(*img_Raw,flex_i);
                }
                else
                    yCError(SERVERGRABBER) << "Image not captured.. check hardware configuration";
            }
            Stamp s = Stamp(count,Time::now());
            pImg.setStrict(!param.canDrop);
            pImg.setEnvelope(s);
            pImg.write();
            count++;
        }
    }
}

void ServerGrabber::shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest)
{
    dest.setPixelCode(src.getPixelCode());
    dest.setQuantum(src.getQuantum());
    dest.setExternal(src.getRawImage(), src.width(), src.height());
}
void ServerGrabber::cleanUp()
{
    if(param.cap==COLOR)
    {
        if(img!=nullptr)
        {
            delete img;
            img=nullptr;
        }
        if(img2!=nullptr)
        {
            delete img2;
            img2=nullptr;
        }
    }
    else
    {
        if(img_Raw!=nullptr)
        {
            delete img_Raw;
            img_Raw=nullptr;
        }
        if(img2_Raw!=nullptr)
        {
            delete img2_Raw;
            img2_Raw=nullptr;
        }
    }
}
