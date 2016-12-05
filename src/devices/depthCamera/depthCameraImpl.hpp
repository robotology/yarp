/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
*         Alberto Cardellino <alberto.cardellino@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*
* Private header file.
*/


#define RETURN_FALSE_STATUS_NOT_OK(s) if(s != STATUS_OK){yError() << OpenNI::getExtendedError(); return false;}

struct yarp::dev::impl::plum_bob
{
    double k1;
    double k2;
    double t1;
    double t2;
    double k3;
};

struct yarp::dev::impl::IntrinsicParams
{
    double   principalPointX;
    double   principalPointY;
    double   focalLengthX;
    double   focalLengthY;
    plum_bob distortionModel;
};

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

