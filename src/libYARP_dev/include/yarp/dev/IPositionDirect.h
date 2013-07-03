// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Alberto Cardellino
* email:   alberto.cardellino@iit.it
* website: www.robotcub.org
* Permission is granted to copy, distribute, and/or modify this program
* under the terms of the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*
* A copy of the license can be found at
* http://www.robotcub.org/icub/license/gpl.txt
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details
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
 * Interface for a generic control board device implementing position control in encoder
 * coordinates.
 * This interfaceis used to send high frequency streaming commands to the boards, the aim
 * is to reach low level control in firmware bypassing the trajetory generator.
 */

class yarp::dev::IPositionDirect
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

        /** Set new position for a set of axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool setPositions(const int n_joint, const int *joints, double *refs)=0;

    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    virtual bool setPositions(const double *refs)=0;
};



/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control in encoder
 * coordinates.
 * This interfaceis used to send high frequency streaming commands to the boards, the aim
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

        /** Set new position for a set of axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool setPositionsRaw(const int n_joint, const int *joints, const double *refs)=0;

    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    virtual bool setPositionsRaw(const double *refs)=0;
};

#define VOCAB_DIRECT_POSITION_          VOCAB4('d','p','o')
#define VOCAB_DIRECT_POSITIONS          VOCAB4('d','p','o','s')
#define VOCAB_DIRECT_POSITION_GROUP     VOCAB4('d','p','o','g')

#endif
