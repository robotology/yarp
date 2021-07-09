/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLDEBUG_H
#define YARP_DEV_ICONTROLDEBUG_H

#include <yarp/dev/api.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, debug commands.
 */
class YARP_dev_API IControlDebug
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlDebug() = default;

    /* Set the print function, pass here a pointer to your own function
     * to print. This function should implement "printf" like parameters.
     * @param a pointer to the print function
     * @return I don't see good reasons why it should return false.
     */
    virtual bool setPrintFunction(int (*f)(const char* fmt, ...)) = 0;

    /* Read the content of the board internal memory, this is usually done
     * at boot time, but can be forced by calling this method.
     * @return true/false on success failure
     */
    virtual bool loadBootMemory() = 0;

    /* Save the current board configuration to the internal memory,
     * this values are read at boot time or if loadBootMemory() is called.
     * @return true/false on success/failure
     */
    virtual bool saveBootMemory() = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_ICONTROLDEBUG_H
