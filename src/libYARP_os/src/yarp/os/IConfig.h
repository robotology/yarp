/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ICONFIG_H
#define YARP_OS_ICONFIG_H


#include <yarp/os/Searchable.h>

namespace yarp::os {

/**
 *
 * An object that can be configured.  When possible, we separate
 * out the configuration for modules and devices into external
 * files, command line options, or GUIs.
 *
 */
class YARP_os_API IConfig
{
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
    virtual bool open(Searchable& config) = 0;

    /**
     * Shut the object down.  You should override this.
     * @return true/false on success/failure.
     */
    virtual bool close() = 0;
};

} // namespace yarp::os

#endif // YARP_OS_ICONFIG_H
