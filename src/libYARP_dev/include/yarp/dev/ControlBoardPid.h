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
    double kp;     /**< proportional gain */
    double kd;     /**< derivative gain */
    double ki;	   /**< integrative gain */
    double max_int;  /**< saturation threshold for the integrator */
    double scale;    /**< scale for the pid output */
   	double max_output; /**< max output */
    double offset;

public:
    /*! \brief Default Constructor */
    Pid();
    /*! \brief Destructor */
    ~Pid();

    /** Constructor
     * @param kp proportional gain
     * @param kd derivative gain
     * @param ki integrative gain
     * @param int_max  integrator max output
     * @param scale scaling factor
     * @param out_max cap on output
     */
    Pid(double kp, double kd, double ki, 
        double int_max, double scale, double out_max);

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
};

#endif
