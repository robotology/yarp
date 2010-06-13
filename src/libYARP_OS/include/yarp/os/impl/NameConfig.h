// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NAMECONFIG_
#define _YARP2_NAMECONFIG_

#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Address.h>

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
class yarp::os::impl::NameConfig {
public:

    String getConfigFileName(const char *stem = NULL);

    static bool createPath(const String& fileName, int ignoreLevel = 1);

    String readConfig(const String& fileName);

    bool writeConfig(const String& fileName, const String& text);

    bool fromFile();

    bool toFile(bool clean = false);

    void setAddress(const Address& address);

    String getHostName();

    static String getIps();

    bool isLocalName(const String& name);

    Address getAddress();

    String getNamespace(bool refresh = false);

    static String getEnv(const String& key);

private:
    Address address;
    String space;

    bool fromString(const String& txt);
};

#endif

