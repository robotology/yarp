/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vector>

#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/IntrinsicParams.h>
#include <yarp/dev/api.h>

#ifndef YARP_DEV_RGBDSENSORPARAMPARSER_H
#define YARP_DEV_RGBDSENSORPARAMPARSER_H

namespace yarp {
namespace dev {

/**
 * @brief The RGBDSensorParamParser class.
 * This class has been designed to uniform the parsing of RGBD yarp devices.
 */
class YARP_dev_API RGBDSensorParamParser
{
public:

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2.0
    YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::sig::IntrinsicParams instead") yarp::sig::IntrinsicParams IntrinsicParams;
#endif
    /**
     * @brief The RGBDParam struct.
     * A RGBD param has a name, can be a setting or a description for/of the RGBD device.
     * The value(s) is stored in ad vector of yarp::os::Value.
     */
    struct YARP_dev_API RGBDParam
    {
        /**
         * @brief RGBDParam, default constructor.
         */
        RGBDParam() : name("unknown"), isSetting(false), isDescription(false), size(1)
        {
            val.resize(size);
        }

        /**
         * @brief RGBDParam
         * @param _name, name of the parameter.
         * @param _size, dimension of the parameter (e.g. the resolution is represented by 2 values).
         */
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


    /**
     * @brief RGBDSensorParamParser, default constructor.
     */
    RGBDSensorParamParser(): depthIntrinsic(), rgbIntrinsic(),
                             transformationMatrix(4,4), isOptionalExtrinsic(true) {
        transformationMatrix.eye();
    }

    /**
     * @brief parseParam, parse the params stored in a Searchable.
     * @param config[in], Searchable containing the parameters of the RGDB sensor
     * @param params[out], vector containing all the description/settings of the
     * RGBD sensor.
     * @return true on success, false otherwise.
     */
    bool parseParam(const yarp::os::Searchable& config, std::vector<RGBDParam *> &params);

    yarp::sig::IntrinsicParams depthIntrinsic;
    yarp::sig::IntrinsicParams rgbIntrinsic;
    yarp::sig::Matrix transformationMatrix;
    bool isOptionalExtrinsic;
};

} // dev
} // yarp

#endif
