/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IENCODERSTIMED_H
#define YARP_DEV_IENCODERSTIMED_H

#include <yarp/dev/IEncoders.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
class IEncodersTimed;
class IEncodersTimedRaw;
}

/**
 * \ingroup dev_iface_motor_raw
 *
 * \brief Control board, extend encoder raw interface adding timestamps.
 */
class yarp::dev::IEncodersTimedRaw: public IEncodersRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IEncodersTimedRaw() {}

     /**
     * Read the instantaneous acceleration of all axes.
     * \param encs pointer to the array that will contain the output
     * \param stamps pointer to the array that will contain individual timestamps
     * \return return true if all goes well, false if anything bad happens.
     */
    virtual yarp::dev::ReturnValue getEncodersTimedRaw(double *encs, double *stamps)=0;

     /**
     * Read the instantaneous acceleration of all axes.
     * \param j axis index
     * \param encs encoder value
     * \param stamp corresponding timestamp
     * \return true if all goes well, false if anything bad happens.
     */
    virtual yarp::dev::ReturnValue getEncoderTimedRaw(int j, double *encs, double *stamp)=0;
};

/**
 * \ingroup dev_iface_motor
 *
 * \brief Control board, extend encoder interface with timestamps.
 */
class YARP_dev_API yarp::dev::IEncodersTimed: public IEncoders
{
public:
     /**
     * Destructor.
     */
    virtual ~IEncodersTimed() {}

     /**
     * Read the instantaneous acceleration of all axes.
     * \param encs pointer to the array that will contain the output
     * \param time pointer to the array that will contain individual timestamps
     * \return true if all goes well, false if anything bad happens.
     */
    virtual yarp::dev::ReturnValue getEncodersTimed(double *encs, double *time)=0;

    /**
    * Read the instantaneous acceleration of all axes.
    * \param j axis index
    * \param encs encoder value (pointer to)
    * \param time corresponding timestamp (pointer to)
    * \return true if all goes well, false if anything bad happens.
    */
    virtual yarp::dev::ReturnValue getEncoderTimed(int j, double *encs, double *time)=0;
};


#endif // YARP_DEV_IENCODERSTIMED_H
