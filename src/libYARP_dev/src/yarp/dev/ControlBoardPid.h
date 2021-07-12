/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/api.h>

#ifndef YARP_DEV_CONTROLBOARDPID_H
#define YARP_DEV_CONTROLBOARDPID_H

/*!
 * \file ControlBoardPid.h define control board standard interfaces
 */

namespace yarp{
    namespace dev{
        class Pid;
    }
}

/*!
 * \brief Contains the parameters for a PID
 */
class YARP_dev_API yarp::dev::Pid
{
public:
    double kp;                 //!< proportional gain
    double kd;                 //!< derivative gain
    double ki;                 //!< integrative gain
    double max_int;            //!< saturation threshold for the integrator
    double scale;              //!< scale for the pid output
    double max_output;         //!< max output
    double offset;             //!< pwm offset added to the pid output
    double stiction_up_val;    //!< up stiction offset added to the pid output
    double stiction_down_val;  //!< down stiction offset added to the pid output
    double kff;                //!< feedforward gain

public:
    /*!
     * \brief Default Constructor.
     */
    Pid();

    /*!
     * \brief Destructor.
     */
    ~Pid();

    /*!
     * \brief Basic constructor.
     *
     * \param kp proportional gain
     * \param kd derivative gain
     * \param ki integrative gain
     * \param int_max  integrator max output
     * \param scale scaling factor
     * \param out_max cap on output
     */
    Pid(double kp, double kd, double ki, double int_max, double scale, double out_max);

    /*!
     * \brief Advanced constructor.
     *
     * \param kp proportional gain
     * \param kd derivative gain
     * \param ki integrative gain
     * \param int_max  integrator max output
     * \param scale scaling factor
     * \param out_max cap on output
     * \param st_up up stiction offset
     * \param st_down down stiction offset
     * \param kff feedforward gain
     */
    Pid(double kp, double kd, double ki,
        double int_max, double scale, double out_max, double st_up, double st_down, double kff);

    /*!
     * \brief Set proportional gain.
     *
     * \param p new gain
     */
    void setKp(double p);

    /*!
     * \brief Set integrative gain.
     *
     * \param i new gain
     */
    void setKi(double i);

    /*!
     * \brief Set derivative gain.
     *
     * \param d new gain
     */
    void setKd(double d);

    /*!
     * \brief Set max threshold for the integrative part.
     *
     * \param m new max
     */
    void setMaxInt(double m);

    /*!
     * \brief Set output scale for the pid.
     *
     * \param sc scale value
     */
    void setScale(double sc);

    /*!
     * \brief Set max output value for the pid.
     *
     * \param m new value
     */
    void setMaxOut(double m);

    /*!
     * \brief Set offset value for the pid.
     *
     * \param o new offset value
     */
    void setOffset(double o);

    /*!
     * \brief Set the two stiction values for the pid.
     *
     * \param up_value the new up value
     * \param down_value the new down value
     */
    void setStictionValues(double up_value, double down_value);

    /*!
     * \brief Set the feedforward gain for the pid.
     *
     * \param Kff new gain
     */
    void setKff(double Kff);

    /*!
     * \brief return true if all params are equal
     *
     * \param p pid to be compared
     */
    bool operator==(const yarp::dev::Pid &p) const;

    /*!
    * \brief Set all pid parameters to zero
    *
    */
    void clear();
};

#endif // YARP_DEV_CONTROLBOARDPID_H
