/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RFPLUGINFACTORY_H
#define YARP_OS_RFPLUGINFACTORY_H

#include <yarp/os/RFModule.h>

#include <string>

namespace yarp {
namespace os {

class YARP_os_API RFModuleFactory
{
private:
    RFModuleFactory();

public:
    static RFModuleFactory& GetInstance();
    static void AddModule(const std::string& name, RFModule* (*moduleCreate)(void));
    RFModule* GetModule(const std::string& name);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS

};

}
}

#endif // YARP_OS_RFPLUGINFACTORY_H
