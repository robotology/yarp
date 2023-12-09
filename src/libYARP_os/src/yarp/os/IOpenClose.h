/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IOPENCLOSE_H
#define YARP_OS_IOPENCLOSE_H


#include <yarp/os/Searchable.h>

namespace yarp::os {

/**
 *
 * An object that can be opened/closed.
 *
 */
class YARP_os_API IOpenClose
{
public:
    /**
     * Destructor.
     */
    virtual ~IOpenClose() {}

    /**
     * Initialize the object.  You should override this.
     * @param config is a list of parameters for the object.
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

#endif // YARP_OS_IOPENCLOSE_H
