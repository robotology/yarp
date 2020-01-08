/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLDEBUG_H
#define YARP_DEV_ICONTROLDEBUG_H

#include <yarp/dev/api.h>

/*! \file ControlDebug.h define control board standard interfaces*/

namespace yarp
{
    namespace dev
    {
        class IControlDebug;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, debug commands.
 */
class YARP_dev_API yarp::dev::IControlDebug
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlDebug() {}

    /* Set the print function, pass here a pointer to your own function
     * to print. This function should implement "printf" like parameters.
     * @param a pointer to the print function
     * @return I don't see good reasons why it should return false.
     */
    virtual bool setPrintFunction(int (*f) (const char *fmt, ...))=0;

    /* Read the content of the board internal memory, this is usually done
     * at boot time, but can be forced by calling this method.
     * @return true/false on success failure
     */
    virtual bool loadBootMemory()=0;

    /* Save the current board configuration to the internal memory,
     * this values are read at boot time or if loadBootMemory() is called.
     * @return true/false on success/failure
     */
    virtual bool saveBootMemory()=0;
};

#endif // YARP_DEV_ICONTROLDEBUG_H
