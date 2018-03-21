/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define RETURN_FALSE_STATUS_NOT_OK(s) if (s != STATUS_OK){yError() << OpenNI::getExtendedError(); return false;}



struct yarp::dev::impl::RGBDParam
{
    RGBDParam() : name("unknown"), isSetting(false), isDescription(false), size(1)
    {
        val.resize(size);
    }

    std::string  name;
    bool         isSetting;
    bool         isDescription;
    int          size;

    std::vector<yarp::os::Value> val;
};

struct yarp::dev::impl::CameraParameters
{
    RGBDParam               clipPlanes;
    RGBDParam               accuracy;
    RGBDParam               depthRes;
    RGBDParam               depth_Fov;
    IntrinsicParams         depthIntrinsic;

    RGBDParam               rgbRes;
    RGBDParam               rgb_Fov;
    RGBDParam               rgbMirroring;
    RGBDParam               depthMirroring;
    IntrinsicParams         rgbIntrinsic;
    yarp::sig::Matrix       transformationMatrix;
};
