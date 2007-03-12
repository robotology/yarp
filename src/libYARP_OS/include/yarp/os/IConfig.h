// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef _YARP2_ICONFIG_
#define _YARP2_ICONFIG_


#include <yarp/os/Searchable.h>

namespace yarp {
    namespace os {
        class IConfig;
    }
};

/**
 *
 * An object that can be configured.  When possible, we separate
 * out the configuration for modules and devices into external
 * files, command line options, or GUIs.
 *
 */
class yarp::os::IConfig {
public:

    virtual bool openFromCommand(int argc, char *argv[], 
                                 bool skipFirst = true);

    virtual bool open(Searchable& config) { return true; }

    virtual bool close() { return true; }

    virtual bool configure(Searchable& config) {
        close();
        return open(config);
    }

    /**
     * Destructor.
     */
    virtual ~IConfig() {}
};

#endif
