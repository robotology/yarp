// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPTORQUECONTROLIMPL__
#define __YARPTORQUECONTROLIMPL__

#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementTorqueControl;
    }
}

class YARP_dev_API yarp::dev::ImplementTorqueControl: public ITorqueControl 
{
protected:
    yarp::dev::ITorqueControlRaw *iTorqueRaw;
    void *helper;
    double *temp;
	double *temp2;
    Pid *tmpPids;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos, const double *nw);
          
    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this 
     *  implementation.
     */
    ImplementTorqueControl(yarp::dev::ITorqueControlRaw *y);
        
    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementTorqueControl();

    virtual bool getAxes(int *ax);
    virtual bool setTorqueMode();
    virtual bool getRefTorque(int j, double *);
    virtual bool getRefTorques(double *t);
    virtual bool setRefTorques(const double *t);
    virtual bool setRefTorque(int j, double t);
    virtual bool getTorques(double *t);
    virtual bool getTorque(int j, double *t);
    virtual bool getBemfParam(int j, double *bemf);
    virtual bool setBemfParam(int j, double bemf);
    virtual bool getTorqueRange(int j, double *min, double *max);
    virtual bool getTorqueRanges(double *min, double *max);
    virtual bool setTorquePid(int j, const Pid &pid);
    virtual bool setTorquePids(const Pid *pids);
    virtual bool setTorqueErrorLimit(int j, double limit);
    virtual bool setTorqueErrorLimits(const double *limits);
    virtual bool getTorqueError(int j, double *err);
    virtual bool getTorqueErrors(double *errs);
    virtual bool getTorquePidOutput(int j, double *out);
    virtual bool getTorquePidOutputs(double *outs);
    virtual bool getTorquePid(int j, Pid *pid);
    virtual bool getTorquePids(Pid *pids);
    virtual bool getTorqueErrorLimit(int j, double *limit);
    virtual bool getTorqueErrorLimits(double *limits);
    virtual bool resetTorquePid(int j);
    virtual bool disableTorquePid(int j);
    virtual bool enableTorquePid(int j);
    virtual bool setTorqueOffset(int j, double v);
};

#endif


