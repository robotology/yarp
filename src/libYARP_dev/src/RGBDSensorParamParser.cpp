/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/RGBDSensorParamParser.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

static bool checkParam(const Bottle& input, RGBDSensorParamParser::RGBDParam& param, bool& found)
{
    bool ret = false;
    Bottle bt=input.findGroup(param.name).tail(); // the first element is the name of the parameter

    if (!bt.isNull())
    {
        Bottle* b;
        if (param.size>1 && bt.size()==1)
        {
            b = bt.get(0).asList();
        }
        else
            b = &bt;
        if (b->isNull())
        {
            yError()<<"RGBDSensorParamParser: check"<<param.name<<"in config file";
            return false;
        }
        if (b->size() != param.size)
        {
            yError() << "RGBDSensorParamParser: parameter" << param.name << "size should be" << param.size;
            return false;
        }
        param.val.resize(param.size);
        for (size_t i=0;i<b->size();i++)
        {
            ret = true;
            param.val[i] = b->get(i);
            found = true;
        }
    }
    else
    {
        ret   = true;
        found = false;
    }
    return ret;
}

static bool checkParam(const Bottle& settings, const Bottle& description, RGBDSensorParamParser::RGBDParam &param)
{
    bool ret1, ret2, ret3;

    ret3 = true;
    ret1 = checkParam(settings,    param, param.isSetting);    // look for settings
    ret2 = checkParam(description, param, param.isDescription);// look for HW_DESCRIPTION

    if ( (param.isSetting) && (param.isDescription) )
    {
        yError() << "Setting " << param.name << " can either be a 'SETTING' or 'HW_DESCRIPTION', not both. Fix the config file. \
                    Look for documentation online.";
        ret3 = false;
    }
    return (ret1 && ret2 && ret3);
}


static bool parseIntrinsic(const Searchable& config, const string& groupName, yarp::sig::IntrinsicParams &params)
{

    pair<string, double*>          realparam;
    vector<pair<string, double*> > realParams;
    size_t                         i;

    if (!config.check(groupName))
    {
        if (params.isOptional)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    Bottle& intrinsic = config.findGroup(groupName);

    realparam.first = "physFocalLength";    realparam.second = &params.physFocalLength; realParams.push_back(realparam);
    realparam.first = "focalLengthX";       realparam.second = &params.focalLengthX;    realParams.push_back(realparam);
    realparam.first = "focalLengthY";       realparam.second = &params.focalLengthY;    realParams.push_back(realparam);
    realparam.first = "principalPointX";    realparam.second = &params.principalPointX; realParams.push_back(realparam);
    realparam.first = "principalPointY";    realparam.second = &params.principalPointY; realParams.push_back(realparam);

    for(i = 0; i < realParams.size(); i++)
    {
        if (!intrinsic.check(realParams[i].first))
        {
            yError() << "RGBDSensorParamParser: missing" << realParams[i].first << "param in" << groupName << "group in the configuration file";
            return false;
        }

        *(realParams[i].second) = intrinsic.find(realParams[i].first).asFloat64();
    }

    if (!intrinsic.check("distortionModel"))
    {
        yError() << "missing distortionModel param in configuration";
        return false;
    }

    if (!config.check(intrinsic.find("distortionModel").asString()))
    {
        yError() << "missing" << intrinsic.find("distortionModel").asString() << "group in configuration file";
        return false;
    }

    Bottle& distortion = config.findGroup(intrinsic.find("distortionModel").asString());

    if (!distortion.check("name"))
    {
        yError() << "RGBDSensorParamParser: missing name param in" << config.find("distortionModel").asString() << "group in configuration file";
        return false;
    }
    if (distortion.find("name").asString() != "plumb_bob")
    {
        yError() << "RGBDSensorParamParser: only plumb_bob distortion model is supported at the moment";
        return false;
    }

    realParams.clear();
    realparam.first = "k1";    realparam.second = &params.distortionModel.k1; realParams.push_back(realparam);
    realparam.first = "k2";    realparam.second = &params.distortionModel.k2; realParams.push_back(realparam);
    realparam.first = "t1";    realparam.second = &params.distortionModel.t1; realParams.push_back(realparam);
    realparam.first = "t2";    realparam.second = &params.distortionModel.t2; realParams.push_back(realparam);
    realparam.first = "k3";    realparam.second = &params.distortionModel.k3; realParams.push_back(realparam);

    for(i = 0; i < realParams.size(); i++)
    {
        if (!distortion.check(realParams[i].first))
        {
            yError() << "RGBDSensorParamParser: missing" << realParams[i].first << "param in" << intrinsic.find("distortionModel").asString() << "group in the configuration file";
            return false;
        }
        *(realParams[i].second) = distortion.find(realParams[i].first).asFloat64();
    }

    return true;
}

bool RGBDSensorParamParser::parseParam(const Searchable &config, std::vector<RGBDParam*>& params)
{
    bool ret = true;

    if (!config.check("SETTINGS"))
    {
        yError() << "RGBDSensorParamParser: missing SETTINGS section on the configuration file";
        return false;
    }

    Bottle& settings = config.findGroup("SETTINGS");

    if (!config.check("HW_DESCRIPTION"))
    {
        yError() << "RGBDSensorParamParser: missing HW_DESCRIPTION section on the configuration file";
        return false;
    }

    Bottle& description = config.findGroup("HW_DESCRIPTION");


    for (auto& v: params)
    {
        if (!checkParam(settings, description, *v)        ) {ret = false;}
    }

    if (!ret)
    {
        yError() << "depthCamera driver input file not correct, please fix it!";
        return false;
    }

    if (!parseIntrinsic(config, "RGB_INTRINSIC_PARAMETERS", rgbIntrinsic))
    {
        yError() << "RGBDSensorParamParser: incomplete or missing RGB_INTRINSIC_PARAMETERS section on the configuration file";
        return false;
    }


    if (!parseIntrinsic(config, "DEPTH_INTRINSIC_PARAMETERS", depthIntrinsic))
    {
        yError() << "RGBDSensorParamParser: incomplete or missing DEPTH_INTRINSIC_PARAMETERS section on the configuration file";
        return false;
    }

    if (!config.check("EXTRINSIC_PARAMETERS"))
    {
        if (!isOptionalExtrinsic)
        {
            yError() << "RGBDSensorParamParser: missing EXTRINSIC_PARAMETERS section on the configuration file";
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {

        Bottle& extrinsic = config.findGroup("EXTRINSIC_PARAMETERS");

        if (!extrinsic.check("transformation"))
        {
            yError() << "RGBDSensorParamParser: missing transformation parameter under EXTRINSIC_PARAMETERS group in configuration file";
            return false;
        }

        Bottle transformation = extrinsic.findGroup("transformation").tail();
        Bottle* tf;

        if (transformation.size()==1)
        {
            tf = transformation.get(0).asList();
        }
        else
        {
            tf=&transformation;
        }
        if (!(tf->size() == 4*4))
        {
            yError() << "RGBDSensorParamParser: the size of the transformation matrix is wrong";
            return false;
        }

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                int k = i*4+j;
                Value& v = tf->get(k);
                if (!v.isFloat64())
                {
                    yError() << "wrong data format on transformation matrix (position" << k << ")";
                    return false;
                }
                transformationMatrix[i][j] = v.asFloat64();
            }
        }
    }

    return ret;
}

