/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_RFPLUGINFACTORY_H
#define YARP_OS_RFPLUGINFACTORY_H

#include <yarp/os/RFModule.h>
#include <string>

namespace yarp {
namespace os {

class YARP_OS_API RFModuleFactory
{
private:
    struct Private;
    Private* impl;
    RFModuleFactory();
public:

    static RFModuleFactory& GetInstance();
    static void AddModule(const std::string& name, RFModule*(*moduleCreate)(void));
    RFModule* GetModule(const std::string name);
};

}
}

#endif // YARP_OS_RFPLUGINFACTORY_H
