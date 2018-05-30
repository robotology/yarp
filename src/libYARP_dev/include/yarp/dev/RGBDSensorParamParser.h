/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Searchable.h>
#include <yarp/sig/Matrix.h>
#include <yarp/dev/api.h>

#include <vector>

#ifndef YARP_DEV_RGBDSENSORPARAMPARSER_H
#define YARP_DEV_RGBDSENSORPARAMPARSER_H

namespace yarp {
namespace dev {

class YARP_dev_API RGBDSensorParamParser
{
public:
    struct YARP_dev_API IntrinsicParams
    {
        struct YARP_dev_API plum_bob
        {
            double k1;
            double k2;
            double t1;
            double t2;
            double k3;
            plum_bob(): k1(0.0), k2(0.0),
                        t1(0.0), t2(0.0),
                        k3(0.0) {}
        };
        double   principalPointX;
        double   principalPointY;
        double   focalLengthX;
        double   focalLengthY;
        plum_bob distortionModel;
        bool     isOptional;
        IntrinsicParams(): principalPointX(0.0), principalPointY(0.0),
                           focalLengthX(0.0), focalLengthY(0.0),
                           distortionModel(), isOptional(false) {}
    };
    struct YARP_dev_API RGBDParam
    {
        RGBDParam() : name("unknown"), isSetting(false), isDescription(false), size(1)
        {
            val.resize(size);
        }

        RGBDParam(const std::string& _name, const int _size) : name(_name), isSetting(false),
                                              isDescription(false), size(_size)
        {
            val.resize(size);
        }


        YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;
        bool         isSetting;
        bool         isDescription;
        size_t          size;

        YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<yarp::os::Value>) val;
    };

    IntrinsicParams         depthIntrinsic;
    IntrinsicParams         rgbIntrinsic;
    yarp::sig::Matrix       transformationMatrix;
    bool                    isOptionalExtrinsic;

    RGBDSensorParamParser(): depthIntrinsic(), rgbIntrinsic(),
                             transformationMatrix(4,4), isOptionalExtrinsic(true) {
        transformationMatrix.eye();
    }
    bool parseParam(yarp::os::Searchable& config, std::vector<RGBDParam *> &params);
};

} // dev
} // yarp

#endif




