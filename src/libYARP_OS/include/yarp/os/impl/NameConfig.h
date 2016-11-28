/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_NAMECONFIG
#define YARP2_NAMECONFIG

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
class YARP_OS_impl_API yarp::os::impl::NameConfig {
public:

    static ConstString expandFilename(const char *fname);

    ConstString getSafeString(const ConstString& txt);

    ConstString getConfigFileName(const char *stem = YARP_NULLPTR,
                             const char *ns = YARP_NULLPTR);

    static bool createPath(const ConstString& fileName, int ignoreLevel = 1);

    ConstString readConfig(const ConstString& fileName);

    bool writeConfig(const ConstString& fileName, const ConstString& text);

    bool fromFile(const char *ns = YARP_NULLPTR);

    bool toFile(bool clean = false);

    void setAddress(const Contact& address);

    static ConstString getHostName(bool prefer_loopback = false,
                              ConstString seed = "");

    static ConstString getIps();

    static yarp::os::Bottle getIpsAsBottle();

    static bool isLocalName(const ConstString& name);

    Contact getAddress();

    ConstString getMode() {
        return mode;
    }

    void setMode(const ConstString& mode) {
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

#endif
