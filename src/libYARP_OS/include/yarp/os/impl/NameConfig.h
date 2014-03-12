// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NAMECONFIG_
#define _YARP2_NAMECONFIG_

#include <yarp/os/impl/String.h>
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

    static String expandFilename(const char *name);

    String getConfigFileName(const char *stem = NULL,
                             const char *ns = NULL);

    static bool createPath(const String& fileName, int ignoreLevel = 1);

    String readConfig(const String& fileName);

    bool writeConfig(const String& fileName, const String& text);

    bool fromFile(const char *ns = 0 /*NULL*/);

    bool toFile(bool clean = false);

    void setAddress(const Contact& address);

    static String getHostName(bool prefer_loopback = false,
                              String seed = "");

    static String getIps();

    static yarp::os::Bottle getIpsAsBottle();

    static bool isLocalName(const String& name);

    Contact getAddress();

    String getMode() {
        return mode;
    }

    void setMode(const String& mode) {
        this->mode = mode;
    }

    String getNamespace(bool refresh = false);

    yarp::os::Bottle getNamespaces(bool refresh = false);

    void setNamespace(const String& ns);

private:
    Contact address;
    String space;
    yarp::os::Bottle spaces;
    String mode;

    bool fromString(const String& txt);
};

#endif
