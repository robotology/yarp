/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_NAMECONFIG_H
#define YARP_OS_IMPL_NAMECONFIG_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>

namespace yarp {
namespace os {
namespace impl {

#define YARP_CONFIG_FILENAME "yarp.conf"
#define YARP_CONFIG_NAMESPACE_FILENAME "yarp_namespace.conf"

/**
 * Small helper class to help deal with legacy YARP configuration files.
 *
 */
class YARP_os_impl_API NameConfig
{
public:
    static std::string expandFilename(const char* fname);

    std::string getSafeString(const std::string& txt);

    std::string getConfigFileName(const char* stem = nullptr,
                                  const char* ns = nullptr);

    std::string readConfig(const std::string& fileName);

    bool writeConfig(const std::string& fileName, const std::string& text);

    bool fromFile(const char* ns = nullptr);

    bool toFile(bool clean = false);

    void setAddress(const Contact& address);

    static std::string getHostName(bool prefer_loopback = false,
                                   const std::string& seed = "");

    static std::string getIps();

    static yarp::os::Bottle getIpsAsBottle();

    static bool isLocalName(const std::string& name);

    Contact getAddress();

    std::string getMode()
    {
        return mode;
    }

    void setMode(const std::string& mode)
    {
        this->mode = mode;
    }

    std::string getNamespace(bool refresh = false);

    yarp::os::Bottle getNamespaces(bool refresh = false);

    void setNamespace(const std::string& ns);

private:
    Contact address;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) space;
    yarp::os::Bottle spaces;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) mode;

    bool fromString(const std::string& txt);
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_NAMECONFIG_H
