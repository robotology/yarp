/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/LogStream.h>
#include <yarp/dev/IVisualParamsImpl.h>

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
    return response.get(3).asInt();
}

int Implement_RgbVisualParams_Sender::getRgbWidth()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_WIDTH);
    _port.write(cmd, response);
    return response.get(3).asInt();
}

bool Implement_RgbVisualParams_Sender::setRgbResolution(int width, int height)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_RESOLUTION);
    cmd.addInt(width);
    cmd.addInt(height);
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
    horizontalFov = response.get(3).asDouble();
    verticalFov   = response.get(4).asDouble();
    return true;
}

bool Implement_RgbVisualParams_Sender::setRgbFOV(double horizontalFov, double verticalFov)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FOV);
    cmd.addDouble(horizontalFov);
    cmd.addDouble(verticalFov);
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

    return Property::copyPortable(response.get(3), intrinsic);  // will it really work??
}

// RGB Parser
Implement_RgbVisualParams_Parser::Implement_RgbVisualParams_Parser() : iRgbVisual(YARP_NULLPTR) {}

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
        iRgbVisual = YARP_NULLPTR;
        ret = false;
    }
    return ret;
}

bool Implement_RgbVisualParams_Parser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret;
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
                    response.addInt(iRgbVisual->getRgbHeight());
                break;

                case VOCAB_WIDTH:
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_WIDTH);
                    response.addVocab(VOCAB_IS);
                    response.addInt(iRgbVisual->getRgbWidth());
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
                        response.addDouble(hFov);
                        response.addDouble(vFov);
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
                        yarp::os::Bottle params_b;
                        response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                        response.addVocab(VOCAB_INTRINSIC_PARAM);
                        response.addVocab(VOCAB_IS);
                        ret &= Property::copyPortable(params, params_b);  // will it really work??
                        response.append(params_b);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received am unknown GET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
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
                    ret = iRgbVisual->setRgbResolution(cmd.get(3).asInt(), cmd.get(4).asInt());
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt(ret);
                break;

                case VOCAB_FOV:
                    ret = iRgbVisual->setRgbFOV(cmd.get(3).asDouble(), cmd.get(4).asDouble());
                    response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt(ret);
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received am unknown SET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                }
                break;
            }
        }
        break;

        default:
        {
            yError() << "Rgb Visual parameter interface Parser received a malformed request. Command should either be 'set' or 'get', received " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
        }
        break;
    }
    return ret;
}

//
// Depth helper implementation
//

// Sender

Implement_DepthVisualParams_Sender::Implement_DepthVisualParams_Sender(yarp::os::Port port) : _port(port) {};

// Implement_DepthVisualParams_Sender::~Implement_DepthVisualParams_Sender() { };

int Implement_DepthVisualParams_Sender::getDepthHeight()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_HEIGHT);
    _port.write(cmd, response);
    return response.get(3).asInt();
}

int Implement_DepthVisualParams_Sender::getDepthWidth()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_WIDTH);
    _port.write(cmd, response);
    return response.get(3).asInt();
}

bool Implement_DepthVisualParams_Sender::setDepthResolution(int _width, int _height)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_RESOLUTION);
    cmd.addInt(_width);
    cmd.addInt(_height);
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
    horizontalFov = response.get(3).asDouble();
    verticalFov   = response.get(4).asDouble();
    return true;
}

bool Implement_DepthVisualParams_Sender::setDepthFOV(double horizontalFov, double verticalFov)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FOV);
    cmd.addDouble(horizontalFov);
    cmd.addDouble(verticalFov);
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
    return response.get(3).asInt();
}

bool Implement_DepthVisualParams_Sender::setDepthAccuracy(double accuracy)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_RESOLUTION);
    cmd.addInt(accuracy);
    _port.write(cmd, response);
    return response.get(2).asBool();
}

bool Implement_DepthVisualParams_Sender::getDepthClipPlanes(double &near, double &far)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CLIP_PLANES);
    _port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        near = 0;
        far  = 0;
        return false;
    }
    near = response.get(3).asDouble();
    far  = response.get(4).asDouble();
    return true;
}

bool Implement_DepthVisualParams_Sender::setDepthClipPlanes(double near, double far)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_CLIP_PLANES);
    cmd.addInt(near);
    cmd.addInt(far);
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


Implement_DepthVisualParams_Parser::Implement_DepthVisualParams_Parser() : iDepthVisual(YARP_NULLPTR) { }
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
        iDepthVisual = YARP_NULLPTR;
        ret = false;
    }
    return ret;
}

bool Implement_DepthVisualParams_Parser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret;
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
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_HEIGHT);
                    response.addVocab(VOCAB_IS);
                    response.addInt(iDepthVisual->getDepthHeight());
                break;

                case VOCAB_WIDTH:
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_WIDTH);
                    response.addVocab(VOCAB_IS);
                    response.addInt(iDepthVisual->getDepthWidth());
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
                        response.addDouble(hFov);
                        response.addDouble(vFov);
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
                        yarp::os::Bottle params_b;
                        response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                        response.addVocab(VOCAB_INTRINSIC_PARAM);
                        response.addVocab(VOCAB_IS);
                        Property::copyPortable(params, params_b);  // will it really work??
                        response.append(params_b);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                case VOCAB_ACCURACY:
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_ACCURACY);
                    response.addVocab(VOCAB_IS);
                    response.addInt(iDepthVisual->getDepthAccuracy());
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received am unknown GET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
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
                    ret = iDepthVisual->setDepthResolution(cmd.get(3).asInt(), cmd.get(4).asInt());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt(ret);
                break;

                case VOCAB_FOV:
                    ret = iDepthVisual->setDepthFOV(cmd.get(3).asDouble(), cmd.get(4).asDouble());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt(ret);
                break;

                case VOCAB_ACCURACY:
                    ret = iDepthVisual->setDepthAccuracy(cmd.get(3).asDouble());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt(ret);
                break;

                case VOCAB_CLIP_PLANES:
                    ret = iDepthVisual->setDepthClipPlanes(cmd.get(3).asDouble(), cmd.get(4).asDouble());
                    response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                    response.addVocab(VOCAB_SET);
                    response.addInt(ret);
                break;

                case VOCAB_INTRINSIC_PARAM:
                {
                    yarp::os::Property params;
                    ret = iDepthVisual->getDepthIntrinsicParam(params);
                    if(ret)
                    {
                        yarp::os::Bottle params_b;
                        response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                        response.addVocab(VOCAB_INTRINSIC_PARAM);
                        response.addVocab(VOCAB_IS);
                        Property::copyPortable(params, params_b);  // will it really work??
                        response.append(params_b);
                    }
                    else
                        response.addVocab(VOCAB_FAILED);
                }
                break;

                default:
                {
                    yError() << "Rgb Visual Parameter interface parser received am unknown SET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                }
                break;
            }
        }
        break;

        default:
        {
            yError() << "Rgb Visual parameter interface Parser received a malformed request. Command should either be 'set' or 'get', received " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
        }
        break;
    }
    return ret;
}
