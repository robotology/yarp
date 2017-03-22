/*
 * Copyright (C) 2016 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTCURRENTCONTROL_H
#define YARP_DEV_IMPLEMENTCURRENTCONTROL_H

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementCurrentControl;
    }
}

class YARP_dev_API yarp::dev::ImplementCurrentControl: public ICurrentControl
{
protected:
    yarp::dev::ICurrentControlRaw *iCurrentRaw;
    void *helper;
    double *temp;
    double *temp2;
    int    *temp_int;
    yarp::dev::Pid *tmpPids;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize(int size, const int *amap, const double* ampsToSens);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementCurrentControl(yarp::dev::ICurrentControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementCurrentControl();

    virtual bool getAxes(int *ax);
    virtual bool getRefCurrent(int j, double *);
    virtual bool getRefCurrents(double *t);
    virtual bool setRefCurrents(const double *t);
    virtual bool setRefCurrent(int j, double t);
    virtual bool setRefCurrents(const int n_joint, const int *joints, const double *t);
    virtual bool getCurrents(double *t);
    virtual bool getCurrent(int j, double *t);
    virtual bool getCurrentRange(int j, double *min, double *max);
    virtual bool getCurrentRanges(double *min, double *max);
    virtual bool setCurrentPid(int j, const Pid &pid);
    virtual bool setCurrentPids(const Pid *pids);
    virtual bool getCurrentError(int j, double *err);
    virtual bool getCurrentErrors(double *errs);
    virtual bool getCurrentPidOutput(int j, double *out);
    virtual bool getCurrentPidOutputs(double *outs);
    virtual bool getCurrentPid(int j, Pid *pid);
    virtual bool getCurrentPids(Pid *pids);
    virtual bool resetCurrentPid(int j);
    virtual bool disableCurrentPid(int j);
    virtual bool enableCurrentPid(int j);
};

#endif // YARP_DEV_IMPLEMENTCURRENTCONTROL_H
