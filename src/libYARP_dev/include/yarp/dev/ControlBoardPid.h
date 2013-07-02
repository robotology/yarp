// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Lorenzo Natale, Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/dev/api.h>

#ifndef __YARPCONTROLBOARDPID__
#define __YARPCONTROLBOARDPID__

/*! \file ControlBoardPid.h define control board standard interfaces*/

namespace yarp{
    namespace dev{
        class Pid;
    }
}

/**
 * Contains the parameters for a PID
 */
class YARP_dev_API yarp::dev::Pid
{
public:
    double kp;                 /**< proportional gain */
    double kd;                 /**< derivative gain */
    double ki;                 /**< integrative gain */
    double max_int;            /**< saturation threshold for the integrator */
    double scale;              /**< scale for the pid output */
    double max_output;         /**< max output */
    double offset;             /**< pwm offset added to the pid output */
    double stiction_up_val;    /**< up stiction offset added to the pid output */
    double stiction_down_val;  /**< down stiction offset added to the pid output */
    double kff;                /**< feedforward gain */

public:
    /*! \brief Default Constructor */
    Pid();
    /*! \brief Destructor */
    ~Pid();

   /** Basic constructor
     * @param kp proportional gain
     * @param kd derivative gain
     * @param ki integrative gain
     * @param int_max  integrator max output
     * @param scale scaling factor
     * @param out_max cap on output
     */
    Pid(double p, double d, double i, double intm, double sc, double omax);

    /** Advanced constructor
     * @param kp proportional gain
     * @param kd derivative gain
     * @param ki integrative gain
     * @param int_max  integrator max output
     * @param scale scaling factor
     * @param out_max cap on output
     * @param st_up up stiction offset
     * @param st_down down stiction offset
     * @param kff feedforward gain
     */
    Pid(double kp, double kd, double ki, 
        double int_max, double scale, double out_max, double st_up, double st_down, double kff);

    /** Set proportional gain 
     * @param p new gain
     */
    void setKp(double p);

    /** Set integrative gain 
     * @param i new gain
     */
    void setKi(double i);

    /** Set derivative gain 
     * @param d new gain
     */
    void setKd(double d);

    /** Set max threshold for the integrative part
     * @param m new max
     */
    void setMaxInt(double m);

    /** Set output scale for the pid.
     * @param sc scale value
     */
    void setScale(double sc);

    /** Set max output value for the pid.
     * @param m new value
     */
    void setMaxOut(double m);

    /** Set offset value for the pid.
     * @param o new offset value
     */
    void setOffset(double o);

    /** Set the two stiction values for the pid.
     * @param up_value the new up value
     * @param down_value the new down value
     */
    void setStictionValues(double up_value, double down_value);

    /** Set the feedforward gain for the pid.
     * @param Kff new gain
     */
    void setKff(double ff);
};

#endif
