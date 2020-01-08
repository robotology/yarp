/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LogStream.h>
#include <yarp/dev/IVisualParamsImpl.h>
#include <yarp/dev/GenericVocabs.h>
#include <cstring>

using namespace yarp::os;
using namespace yarp::dev;

//
// Depth helper implementation
//

// RGB Sender
Implement_RgbVisualParams_Sender::Implement_RgbVisualParams_Sender(yarp::os::Port &port) : _port(port) {};

int Implement_RgbVisualParams_Sender::getRgbHeight()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_HEIGHT);
    _port.write(cmd, response);
    return response.get(3).asInt32();
}

int Implement_RgbVisualParams_Sender::getRgbWidth()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_WIDTH);
    _port.write(cmd, response);
    return response.get(3).asInt32();
}
bool Implement_RgbVisualParams_Sender::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations){
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_SUPPORTED_CONF);
    _port.write(cmd, response);

    if((response.get(0).asVocab())== VOCAB_FAILED)
    {
        configurations.clear();
        return false;
    }
    configurations.resize(response.get(3).asInt32());
    for(int i=0; i<response.get(3).asInt32(); i++){
        configurations[i].width=response.get(4 + i*4).asInt32();
        configurations[i].height=response.get(4 + i*4 + 1).asInt32();
        configurations[i].framerate=response.get(4 + i*4 + 2).asFloat64();
        configurations[i].pixelCoding=static_cast<YarpVocabPixelTypesEnum>(response.get(4 + i*4 + 3).asVocab());
    }
    return true;
}
bool Implement_RgbVisualParams_Sender::getRgbResolution(int &width, int &height){
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_RESOLUTION);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        width = 0;
        height= 0;
        return false;
    }
    width  = response.get(3).asInt32();
    height = response.get(4).asInt32();
    return true;

}

bool Implement_RgbVisualParams_Sender::setRgbResolution(int width, int height)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_RESOLUTION);
    cmd.addInt32(width);
    cmd.addInt32(height);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

bool Implement_RgbVisualParams_Sender::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FOV);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        horizontalFov = 0;
        verticalFov = 0;
        return false;
    }
    horizontalFov = response.get(3).asFloat64();
    verticalFov   = response.get(4).asFloat64();
    return true;
}

bool Implement_RgbVisualParams_Sender::setRgbFOV(double horizontalFov, double verticalFov)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FOV);
    cmd.addFloat64(horizontalFov);
    cmd.addFloat64(verticalFov);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

bool Implement_RgbVisualParams_Sender::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_INTRINSIC_PARAM);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        intrinsic.clear();
        return false;
    }
    bool ret;
    ret=Property::copyPortable(response.get(3), intrinsic);
    if(!response.get(4).isNull())
    {
        Property& p=intrinsic.addGroup("right");
        ret &= Property::copyPortable(response.get(4),p);
        return ret;
    }
    return ret;

}

bool Implement_RgbVisualParams_Sender::getRgbMirroring(bool& mirror)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_MIRROR);
    _port.write(cmd, response);
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        return false;
    }
    mirror = response.get(3).asBool();
    return true;
}

bool Implement_RgbVisualParams_Sender::setRgbMirroring(bool mirror)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_MIRROR);
    cmd.addInt32(mirror);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

// RGB Parser
Implement_RgbVisualParams_Parser::Implement_RgbVisualParams_Parser() : iRgbVisual(nullptr) {}

// Implement_RgbVisualParams_Parser::~Implement_RgbVisualParams_Parser() { }

bool Implement_RgbVisualParams_Parser::configure(IRgbVisualParams *interface)
{
    bool ret = false;
    if(interface)
    {
        iRgbVisual = interface;
        ret = true;
    }
    else
    {
        iRgbVisual = nullptr;
        ret = false;
    }
    return ret;
}

bool Implement_RgbVisualParams_Parser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret = false;
    response.clear();
    if(!iRgbVisual)
    {
        yError() << "Rgb Visual parameter Parser has not been correctly configures. IRgbVisualParams interface is not valid";
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    int code = cmd.get(0).asVocab();
    if(code != VOCAB_RGB_VISUAL_PARAMS)
    {
        yError() << "Rgb Visual Params Parser received a command not belonging to this interface. Required interface is " << yarp::os::Vocab::decode(code);
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    switch (cmd.get(1).asVocab())
    {
        case VOCAB_GET:
        {
            switch(cmd.get(2).asVocab())
            {
                case VOCAB_HEIGHT:
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_HEIGHT);
                    response.addVocab(VOCAB_IS);
                    response.addInt32(iRgbVisual->getRgbHeight());
                    ret=true;
                break;

                case VOCAB_WIDTH:
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_WIDTH);
                    response.addVocab(VOCAB_IS);
                    response.addInt32(iRgbVisual->getRgbWidth());
                    ret=true;
                break;

                case VOCAB_SUPPORTED_CONF:
                {
                    yarp::sig::VectorOf<CameraConfig> conf;
                    ret = iRgbVisual->getRgbSupportedConfigurations(conf);
                    if(ret){
                        response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                        response.addVocab(VOCAB_SUPPORTED_CONF);
                        response.addVocab(VOCAB_IS);
                        response.addInt32(conf.size());
                        for(size_t i=0; i<conf.size(); i++){
                            response.addInt32(conf[i].width);
                            response.addInt32(conf[i].height);
                            response.addFloat64(conf[i].framerate);
                            response.addVocab(conf[i].pixelCoding);
                        }
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;


                case VOCAB_RESOLUTION:
                {
                    int width, height;
                    ret = iRgbVisual->getRgbResolution(width, height);
                    if(ret)
                    {
                        response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                        response.addVocab(VOCAB_RESOLUTION);
                        response.addVocab(VOCAB_IS);
                        response.addInt32(width);
                        response.addInt32(height);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                case VOCAB_FOV:
                {
                    double hFov, vFov;
                    ret = iRgbVisual->getRgbFOV(hFov, vFov);
                    if(ret)
                    {
                        response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                        response.addVocab(VOCAB_FOV);
                        response.addVocab(VOCAB_IS);
                        response.addFloat64(hFov);
                        response.addFloat64(vFov);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                case VOCAB_INTRINSIC_PARAM:
                {
                    yarp::os::Property params;
                    ret = iRgbVisual->getRgbIntrinsicParam(params);
                    if(ret)
                    {
                        response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                        response.addVocab(VOCAB_INTRINSIC_PARAM);
                        response.addVocab(VOCAB_IS);
                        yarp::os::Bottle& tmp=response.addList();
                        ret &= Property::copyPortable(params, tmp);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                case VOCAB_MIRROR:
                {
                    bool mirror;
                    ret = iRgbVisual->getRgbMirroring(mirror);
                    if(ret)
                    {
                        response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                        response.addVocab(VOCAB_MIRROR);
                        response.addVocab(VOCAB_IS);
                        response.addInt32(mirror);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received an unknown GET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                    ret = false;
                }
                break;
            }
        }
        break;

        case VOCAB_SET:
        {
            switch(cmd.get(2).asVocab())
            {
                case VOCAB_RESOLUTION:
                    ret = iRgbVisual->setRgbResolution(cmd.get(3).asInt32(), cmd.get(4).asInt32());
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                break;

                case VOCAB_FOV:
                    ret = iRgbVisual->setRgbFOV(cmd.get(3).asFloat64(), cmd.get(4).asFloat64());
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                break;

                case VOCAB_MIRROR:
                {
                    ret = iRgbVisual->setRgbMirroring(cmd.get(3).asBool());
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                }
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received am unknown SET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                    ret = false;
                }
                break;
            }
        }  // end VOCAB
        break;

        default:
        {
            yError() << "Rgb Visual parameter interface Parser received a malformed request. Command should either be 'set' or 'get', received " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
            ret = false;
        }
        break;
    }
    return ret;
}

//
// Depth helper implementation
//

// Sender

Implement_DepthVisualParams_Sender::Implement_DepthVisualParams_Sender(yarp::os::Port &port) : _port(port) {};

// Implement_DepthVisualParams_Sender::~Implement_DepthVisualParams_Sender() { };

int Implement_DepthVisualParams_Sender::getDepthHeight()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_HEIGHT);
    _port.write(cmd, response);
    return response.get(3).asInt32();
}

int Implement_DepthVisualParams_Sender::getDepthWidth()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_WIDTH);
    _port.write(cmd, response);
    return response.get(3).asInt32();
}

bool Implement_DepthVisualParams_Sender::setDepthResolution(int _width, int _height)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_RESOLUTION);
    cmd.addInt32(_width);
    cmd.addInt32(_height);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

bool Implement_DepthVisualParams_Sender::getDepthFOV(double &horizontalFov, double &verticalFov)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FOV);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        horizontalFov = 0;
        verticalFov = 0;
        return false;
    }
    horizontalFov = response.get(3).asFloat64();
    verticalFov   = response.get(4).asFloat64();
    return true;
}

bool Implement_DepthVisualParams_Sender::setDepthFOV(double horizontalFov, double verticalFov)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FOV);
    cmd.addFloat64(horizontalFov);
    cmd.addFloat64(verticalFov);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

double Implement_DepthVisualParams_Sender::getDepthAccuracy()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ACCURACY);
    _port.write(cmd, response);
    return response.get(3).asFloat64();
}

bool Implement_DepthVisualParams_Sender::setDepthAccuracy(double accuracy)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ACCURACY);
    cmd.addFloat64(accuracy);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

bool Implement_DepthVisualParams_Sender::getDepthClipPlanes(double &nearPlane, double &farPlane)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CLIP_PLANES);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        nearPlane = 0;
        farPlane  = 0;
        return false;
    }
    nearPlane = response.get(3).asFloat64();
    farPlane  = response.get(4).asFloat64();
    return true;
}

bool Implement_DepthVisualParams_Sender::setDepthClipPlanes(double nearPlane, double farPlane)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_CLIP_PLANES);
    cmd.addFloat64(nearPlane);
    cmd.addFloat64(farPlane);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

bool Implement_DepthVisualParams_Sender::getDepthIntrinsicParam(yarp::os::Property &intrinsic)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_INTRINSIC_PARAM);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        intrinsic.clear();
        return false;
    }

    Property::copyPortable(response.get(3), intrinsic);  // will it really work??
    return true;
}

bool Implement_DepthVisualParams_Sender::getDepthMirroring(bool& mirror)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_MIRROR);
    _port.write(cmd, response);
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        return false;
    }
    mirror = response.get(3).asBool();
    return true;
}

bool Implement_DepthVisualParams_Sender::setDepthMirroring(bool mirror)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_MIRROR);
    cmd.addInt32(mirror);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

Implement_DepthVisualParams_Parser::Implement_DepthVisualParams_Parser() : iDepthVisual(nullptr) { }
// Implement_DepthVisualParams_Parser::~Implement_DepthVisualParams_Parser() { }

bool Implement_DepthVisualParams_Parser::configure(IDepthVisualParams *interface)
{
    bool ret = false;
    if(interface)
    {
        iDepthVisual = interface;
        ret = true;
    }
    else
    {
        iDepthVisual = nullptr;
        ret = false;
    }
    return ret;
}

bool Implement_DepthVisualParams_Parser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret = false;
    response.clear();
    if(!iDepthVisual)
    {
        yError() << "Depth Visual parameter Parser has not been correctly configured. IDepthVisualParams interface is not valid";
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    int code = cmd.get(0).asVocab();
    if(code != VOCAB_DEPTH_VISUAL_PARAMS)
    {
        yError() << "Depth Visual Params Parser received a command not belonging to this interface. Required interface was " << yarp::os::Vocab::decode(code);
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    switch (cmd.get(1).asVocab())
    {
        case VOCAB_GET:
        {
            switch(cmd.get(2).asVocab())
            {
                case VOCAB_HEIGHT:
                {
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_HEIGHT);
                    response.addVocab(VOCAB_IS);
                    response.addInt32(iDepthVisual->getDepthHeight());
                }
                break;

                case VOCAB_WIDTH:
                {
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_WIDTH);
                    response.addVocab(VOCAB_IS);
                    response.addInt32(iDepthVisual->getDepthWidth());
                }
                break;

                case VOCAB_FOV:
                {
                    double hFov, vFov;
                    ret = iDepthVisual->getDepthFOV(hFov, vFov);
                    if(ret)
                    {
                        response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                        response.addVocab(VOCAB_FOV);
                        response.addVocab(VOCAB_IS);
                        response.addFloat64(hFov);
                        response.addFloat64(vFov);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                case VOCAB_INTRINSIC_PARAM:
                {
                    yarp::os::Property params;
                    ret = iDepthVisual->getDepthIntrinsicParam(params);
                    if(ret)
                    {
                        response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                        response.addVocab(VOCAB_INTRINSIC_PARAM);
                        response.addVocab(VOCAB_IS);
                        yarp::os::Bottle& tmp=response.addList();
                        ret &= Property::copyPortable(params, tmp);
                    }
                    else
                    {
                        response.addVocab(VOCAB_FAILED);
                    }
                }
                break;

                case VOCAB_ACCURACY:
                {
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_ACCURACY);
                    response.addVocab(VOCAB_IS);
                    response.addFloat64(iDepthVisual->getDepthAccuracy());
                }
                break;

                case VOCAB_CLIP_PLANES:
                {
                    double nearPlane, farPlane;
                    iDepthVisual->getDepthClipPlanes(nearPlane, farPlane);
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_CLIP_PLANES);
                    response.addVocab(VOCAB_IS);
                    response.addFloat64(nearPlane);
                    response.addFloat64(farPlane);
                }
                break;

                case VOCAB_MIRROR:
                {
                    bool mirror;
                    ret = iDepthVisual->getDepthMirroring(mirror);
                    if(ret)
                    {
                        response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                        response.addVocab(VOCAB_MIRROR);
                        response.addVocab(VOCAB_IS);
                        response.addInt32(mirror);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                default:
                {
                    yError() << "Depth Visual Parameter interface parser received am unknown GET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                    ret = false;
                }
                break;
            }
        }
        break;

        case VOCAB_SET:
        {
            switch(cmd.get(2).asVocab())
            {
                case VOCAB_RESOLUTION:
                {
                    ret = iDepthVisual->setDepthResolution(cmd.get(3).asInt32(), cmd.get(4).asInt32());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                }
                break;

                case VOCAB_FOV:
                {
                    ret = iDepthVisual->setDepthFOV(cmd.get(3).asFloat64(), cmd.get(4).asFloat64());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                }
                break;

                case VOCAB_ACCURACY:
                {
                    ret = iDepthVisual->setDepthAccuracy(cmd.get(3).asFloat64());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                }
                break;

                case VOCAB_CLIP_PLANES:
                {
                    ret = iDepthVisual->setDepthClipPlanes(cmd.get(3).asFloat64(), cmd.get(4).asFloat64());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                }
                break;

                case VOCAB_MIRROR:
                {
                    ret = iDepthVisual->setDepthMirroring(cmd.get(3).asBool());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt32(ret);
                }
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received am unknown SET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                    ret = false;
                }
                break;
            }
        }
        break;

        default:
        {
            yError() << "Rgb Visual parameter interface Parser received a malformed request. Command should either be 'set' or 'get', received " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
            ret = false;
        }
        break;
    }
    return ret;
}
