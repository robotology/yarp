/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_NAMECONFIG_H
#define YARP_OS_IMPL_NAMECONFIG_H

#include <yarp/os/Contact.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        namespace impl {
            class NameConfig;
        }
    }
}

#define YARP_CONFIG_FILENAME "yarp.conf"
#define YARP_CONFIG_NAMESPACE_FILENAME "yarp_namespace.conf"

/**
 * Small helper class to help deal with legacy YARP configuration files.
 *
 */
class YARP_OS_impl_API yarp::os::impl::NameConfig
{
public:

    static ConstString expandFilename(const char *fname);

    ConstString getSafeString(const ConstString& txt);

    ConstString getConfigFileName(const char *stem = nullptr,
                                  const char *ns = nullptr);

    static bool createPath(const ConstString& fileName, int ignoreLevel = 1);

    ConstString readConfig(const ConstString& fileName);

    bool writeConfig(const ConstString& fileName, const ConstString& text);

    bool fromFile(const char *ns = nullptr);

    bool toFile(bool clean = false);

    void setAddress(const Contact& address);

    static ConstString getHostName(bool prefer_loopback = false,
                                   const ConstString& seed = "");

    static ConstString getIps();

    static yarp::os::Bottle getIpsAsBottle();

    static bool isLocalName(const ConstString& name);

    Contact getAddress();

    ConstString getMode()
    {
        return mode;
    }

    void setMode(const ConstString& mode)
    {
        this->mode = mode;
    }

    ConstString getNamespace(bool refresh = false);

    yarp::os::Bottle getNamespaces(bool refresh = false);

    void setNamespace(const ConstString& ns);

private:
    Contact address;
    ConstString space;
    yarp::os::Bottle spaces;
    ConstString mode;

    bool fromString(const ConstString& txt);
};

#endif // YARP_OS_IMPL_NAMECONFIG_H
