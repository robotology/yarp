/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDHELPERS_H
#define YARP_DEV_CONTROLBOARDHELPERS_H

#include <yarp/os/PortablePair.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/GenericVocabs.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* the control command message type
 * head is a Bottle which contains the specification of the message type
 * body is a Vector which move the robot accordingly
 */
typedef yarp::os::PortablePair <yarp::os::Bottle, yarp::sig::Vector> CommandMessage;

/* check whether the last command failed */
inline bool CHECK_FAIL(bool ok, yarp::os::Bottle& response)
{
    if (ok) {
        if (response.get(0).isVocab32() && response.get(0).asVocab32() == VOCAB_FAILED) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // YARP_DEV_CONTROLBOARDHELPERS_H_H
