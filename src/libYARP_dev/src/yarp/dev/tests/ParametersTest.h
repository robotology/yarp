/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERSTEST_H
#define PARAMETERSTEST_H

#include <yarp/dev/IDeviceDriverParams.h>
#include <yarp/dev/PolyDriver.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_params_test(yarp::dev::PolyDriver* pdrv)
    {
        REQUIRE(pdrv !=nullptr);

        yarp::dev::IDeviceDriverParams* iparams=nullptr;
        pdrv->view(iparams);
        if (!iparams)
        {
            WARN("The device does not implement IDeviceDriverParams interface");
            return;
        }

        std::string configuration = iparams->getConfiguration();
        CHECK(!configuration.empty());

        std::string class_name = iparams->getDeviceClassName();
        CHECK(!class_name.empty());

        std::string device_name = iparams->getDeviceName();
        CHECK(!device_name.empty());

        std::string doc = iparams->getDocumentationOfDeviceParams();
        CHECK(!doc.empty());

        std::string tmp;
        CHECK(!iparams->getParamValue("not_existing_param", tmp));
        CHECK(tmp.empty());

        auto listofparams = iparams->getListOfParams();
        for (const auto& name : listofparams)
        {
            //let's check that the parameter exists and we can get its value
            std::string value;
            bool b= iparams->getParamValue(name, value);
            CHECK(b);

            //Some string can be empty, and that's ok
            //CHECK(!value.empty());
        }
    }
}

#endif
