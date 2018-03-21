/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_ICONFIG_H
#define YARP_OS_ICONFIG_H


#include <yarp/os/Searchable.h>

namespace yarp {
    namespace os {
        class IConfig;
    }
}

/**
 *
 * An object that can be configured.  When possible, we separate
 * out the configuration for modules and devices into external
 * files, command line options, or GUIs.
 *
 */
class YARP_OS_API yarp::os::IConfig {
public:

    /**
     * Destructor.
     */
    virtual ~IConfig();

    /**
     * Initialize the object.  You should override this.
     * @param config is a list of parameters for the object.
     * Which parameters are effective for your object can vary.
     * @return true/false upon success/failure
     */
    virtual bool open(Searchable& config);

    /**
     * Shut the object down.  You should override this.
     * @return true/false on success/failure.
     */
    virtual bool close();

    /**
     * Change online parameters.
     * The parameters that can be changed online (in other words,
     * without closing and reopening) will vary between objects.
     *
     * @param config A list of parameters for the object.
     * @return true/false on success/failure.
     */
    virtual bool configure(Searchable& config);
};

#endif // YARP_OS_ICONFIG_H
