// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_NAMECONFIG_
#define _YARP2_NAMECONFIG_

#include <yarp/String.h>
#include <yarp/Address.h>

namespace yarp {
    class NameConfig;
}

#define YARP_CONFIG_FILENAME "yarp.conf"
#define YARP_CONFIG_NAMESPACE_FILENAME "yarp_namespace.conf"

/**
 * Small helper class to help deal with legacy YARP configuration files. 
 *
 */
class yarp::NameConfig {
public:

    String getConfigFileName(const char *stem = NULL);

    bool createPath(const String& fileName, int ignoreLevel = 1);

    String readConfig(const String& fileName);

    bool writeConfig(const String& fileName, const String& text);

    bool fromFile();

    bool toFile();

    void setAddress(const Address& address);

    String getHostName();

    static String getIps();

    bool isLocalName(const String& name);

    Address getAddress();

    String getNamespace();

    static String getEnv(const String& key);

private:
    Address address;
    String space;

    bool fromString(const String& txt);
};

#endif

