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

#include <yarp/sig/IntrinsicParams.h>

#include <vector>

#ifndef YARP_DEV_RGBDSENSORPARAMPARSER_H
#define YARP_DEV_RGBDSENSORPARAMPARSER_H

namespace yarp {
namespace dev {


class YARP_dev_API RGBDSensorParamParser
{
public:

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2.0
    YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::sig::IntrinsicParams instead") yarp::sig::IntrinsicParams IntrinsicParams;
#endif
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

    yarp::sig::IntrinsicParams         depthIntrinsic;
    yarp::sig::IntrinsicParams         rgbIntrinsic;
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




