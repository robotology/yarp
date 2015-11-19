// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Giorgio Metta and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __CONTROLBOARDHELPERS__
#define __CONTROLBOARDHELPERS__

#include <yarp/os/PortablePair.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* the control command message type
 * head is a Bottle which contains the specification of the message type
 * body is a Vector which move the robot accordingly
 */
typedef yarp::os::PortablePair <yarp::os::Bottle, yarp::sig::Vector> CommandMessage;

/* check whether the last command failed */
inline bool CHECK_FAIL(bool ok, yarp::os::Bottle& response) {

    if (ok) {
        if (response.get(0).isVocab() && response.get(0).asVocab() == VOCAB_FAILED) {
            return false;
        }
    }
    else
        return false;

    return true;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif
