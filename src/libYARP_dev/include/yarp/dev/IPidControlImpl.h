/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale <lorenzo.natale@iit.it>, Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTPIDCONTROL_H
#define YARP_DEV_IMPLEMENTPIDCONTROL_H

#include <yarp/dev/IPidControl.h>

namespace yarp {
    namespace dev {
        class ImplementPidControl;
    }
}

class YARP_dev_API yarp::dev::ImplementPidControl : public IPidControl
{
protected:
    IPidControlRaw *iPid;
    void *helper;
    double *temp;
    yarp::dev::Pid *tmpPids;


    /**
    * Initialize the internal data and alloc memory.
    * @param size is the number of controlled axes the driver deals with.
    * @param amap is a lookup table mapping axes onto physical drivers.
    * @return true if initialized succeeded, false if it wasn't executed, or assert.
    */
    bool initialize(int size, const int *amap, const double *enc, const double *zos);

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
    ImplementPidControl(yarp::dev::IPidControlRaw *y);

    virtual bool setPid(int j, const Pid &pid);
    virtual bool setPids(const Pid *pids);
    virtual bool setReference(int j, double ref);
    virtual bool setReferences(const double *refs);
    virtual bool setErrorLimit(int j, double limit);
    virtual bool setErrorLimits(const double *limits);
    virtual bool getError(int j, double *err);
    virtual bool getErrors(double *errs);
    virtual bool getOutput(int j, double *out);
    virtual bool getOutputs(double *outs);
    virtual bool getPid(int j, Pid *pid);
    virtual bool getPids(Pid *pids);
    virtual bool getReference(int j, double *ref);
    virtual bool getReferences(double *refs);
    virtual bool getErrorLimit(int j, double *ref);
    virtual bool getErrorLimits(double *refs);
    virtual bool resetPid(int j);
    virtual bool enablePid(int j);
    virtual bool disablePid(int j);
    virtual bool setOffset(int j, double v);

    virtual ~ImplementPidControl();
};

#endif // YARP_DEV_IMPLEMENTPIDCONTROL_H
