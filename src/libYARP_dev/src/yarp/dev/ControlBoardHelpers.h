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
#include <yarp/dev/ReturnValue.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* the control command message type
 * head is a Bottle which contains the specification of the message type
 * body is a Vector which move the robot accordingly
 */
typedef yarp::os::PortablePair <yarp::os::Bottle, yarp::sig::Vector> CommandMessage;

/* check whether the last command failed */
inline yarp::dev::ReturnValue CHECK_FAIL(bool ok, yarp::os::Bottle& response)
{
    if (ok) {
        if (response.get(0).isVocab32() && response.get(0).asVocab32() == VOCAB_FAILED) {
            return yarp::dev::ReturnValue::return_code::return_value_error_generic;
        }
    } else {
        return yarp::dev::ReturnValue::return_code::return_value_error_generic;
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

#define POINTERCHECK(vec)                 if (vec==nullptr)                                            { yError("vec is nullptr"); return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;}
#define JOINTIDCHECK(j)                   if (!castToMapper(m_helper)->checkAxisId(j))                 { yError("joint id out of bound"); return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;}
#define JOINTSIDCHECK(n_joints, joints)   if (!castToMapper(m_helper)->checkAxesIds(n_joints, joints)) { yError("joint id out of bound"); return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;}

#define JOINTSIDVECCHECK(joints)          if (!castToMapper(m_helper)->checkAxesIds(joints))           { yError("joint id out of bound"); return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;}
#define VECCHECK_SET_ALL(vec)             if (vec.size() != castToMapper(m_helper)->axes())            { yError("vector size does not match number of joints"); return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;}
#define VECCHECK_GET_ALL(vec)             if (vec.size() != castToMapper(m_helper)->axes())            { vec.resize(castToMapper(m_helper)->axes());}
#define VECCHECK_SET_SOME(jointsvec,datavec)   JOINTSIDVECCHECK(jointsvec) if (datavec.size() != jointsvec.size())                 { yError("vector size does not match number of joints"); return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;}
#define VECCHECK_GET_SOME(jointsvec,datavec)   JOINTSIDVECCHECK(jointsvec) if (datavec.size() != jointsvec.size())                 { datavec.resize(jointsvec.size());}

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // YARP_DEV_CONTROLBOARDHELPERS_H_H
