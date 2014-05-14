// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __YARPP_DIRECTOSITION_INTERFACE__
#define __YARPP_DIRECTOSITION_INTERFACE__

namespace yarp {
    namespace dev {
        class IPositionDirect;
        class IPositionDirectRaw;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control.
 * This interface is used to send high frequency streaming commands to the boards, the aim
 * is to reach low level control in firmware bypassing the trajetory generator.
 */

class YARP_dev_API yarp::dev::IPositionDirect
{

public:
    /**
     * Destructor.
     */
    virtual ~IPositionDirect() {}

    /** Set new position for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool setPosition(int j, double ref)=0;


    /** Set new reference point for all axes.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param refs array, new reference points, one value for each joint, the size is n_joints. The first value will be the new reference fot the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    10 30 40
     * @return true/false on success/failure
     */
    virtual bool setPositions(const int n_joint, const int *joints, double *refs)=0;

    /** Set new position for a set of axis.
     * @param refs specifies the new reference points
     * @return true/false on success/failure
     */
    virtual bool setPositions(const double *refs)=0;
};



/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control in encoder
 * coordinates.
 * This interface is used to send high frequency streaming commands to the boards, the aim
 * is to reach low level control in firmware bypassing the trajetory generator, raw functions.
 */

class yarp::dev::IPositionDirectRaw
{

public:
    /**
     * Destructor.
     */
    virtual ~IPositionDirectRaw() {}

    /** Set new position for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool setPositionRaw(int j, double ref)=0;

    /** Set new reference point for all axes.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param refs array, new reference points, one value for each joint, the size is n_joints. The first value will be the new reference fot the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    10 30 40
     * @return true/false on success/failure
     */
    virtual bool setPositionsRaw(const int n_joint, const int *joints, double *refs)=0;


    /** Set new position for a set of axis.
     * @param refs specifies the new reference points
     * @return true/false on success/failure
     */
    virtual bool setPositionsRaw(const double *refs)=0;
};

#define VOCAB_POSITION_DIRECT           VOCAB3('d','p','o')
#define VOCAB_POSITION_DIRECTS          VOCAB4('d','p','o','s')
#define VOCAB_POSITION_DIRECT_GROUP     VOCAB4('d','p','o','g')

#endif
