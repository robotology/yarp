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

    /**
     * Initialize the module.  You should override this.
     * @param config is a list of parameters for the module.
     * Which parameters are effective for your module can vary.
     * @return true/false upon success/failure
     */
    virtual bool open(Searchable& config) { return true; }

    /**
     * Shut the module down.  You should override this.
     * @return true/false on success/failure.
     */
    virtual bool close() { return true; }

    /**
     * Reinitialize the module.  By default, this is the same as calling
     * close() and then open() with the new configuration, but particular
     * modules override this to do things more efficiently.
     */
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
