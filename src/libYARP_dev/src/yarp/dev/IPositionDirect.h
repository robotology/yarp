/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_IPOSITIONDIRECT_H
#define YARP_IPOSITIONDIRECT_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

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

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax)=0;

    /** Set new position for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool setPosition(int j, double ref)=0;

    /** Set new reference point for all axes.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param refs array, new reference points, one value for each joint, the size is n_joints.
     *        The first value will be the new reference for the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    10 30 40
     * @return true/false on success/failure
     */
    virtual bool setPositions(const int n_joint, const int *joints, const double *refs)=0;

    /** Set new position for a set of axis.
     * @param refs specifies the new reference points
     * @return true/false on success/failure
     */
    virtual bool setPositions(const double *refs)=0;

    /** Get the last position reference for the specified axis.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPosition(const int joint, double *ref) {return false;}

    /** Get the last position reference for all axes.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositions(double *refs) {return false;}

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositions(const int n_joint, const int *joints, double *refs) {return false;}
};



/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control in encoder
 * coordinates.
 * This interface is used to send high frequency streaming commands to the boards, the aim
 * is to reach low level control in firmware bypassing the trajetory generator, raw functions.
 */
class YARP_dev_API yarp::dev::IPositionDirectRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IPositionDirectRaw() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *axes) = 0;

    /** Set new position for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool setPositionRaw(int j, double ref)=0;

    /** Set new reference point for all axes.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param refs array, new reference points, one value for each joint, the size is n_joints. The first value will be the new reference for the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    10 30 40
     * @return true/false on success/failure
     */
    virtual bool setPositionsRaw(const int n_joint, const int *joints, const double *refs)=0;


    /** Set new position for a set of axes.
     * @param refs specifies the new reference points
     * @return true/false on success/failure
     */
    virtual bool setPositionsRaw(const double *refs)=0;

    /** Get the last position reference for the specified axis.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositionRaw(const int joint, double *ref) {return false;}

    /** Get the last position reference for all axes.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositionsRaw(double *refs) {return false;};


    /** Get the last position reference for the specified group of axes.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositionsRaw(const int n_joint, const int *joints, double *refs) {return false;}
};

constexpr yarp::conf::vocab32_t VOCAB_POSITION_DIRECT           = yarp::os::createVocab32('d','p','o');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_DIRECTS          = yarp::os::createVocab32('d','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_DIRECT_GROUP     = yarp::os::createVocab32('d','p','o','g');

#endif // YARP_IPOSITIONDIRECT_H
