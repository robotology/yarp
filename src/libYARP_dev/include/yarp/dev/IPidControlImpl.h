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
    bool initialize(int size, const int *amap, const double *enc, const double *zos, const double* newtons, const double* amps);

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

    virtual bool setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid);
    virtual bool setPids(const PidControlTypeEnum& pidtype, const Pid *pids);
    virtual bool setPidReference(const PidControlTypeEnum& pidtype, int j, double ref);
    virtual bool setPidReferences(const PidControlTypeEnum& pidtype, const double *refs);
    virtual bool setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit);
    virtual bool setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits);
    virtual bool getPidError(const PidControlTypeEnum& pidtype, int j, double *err);
    virtual bool getPidErrors(const PidControlTypeEnum& pidtype, double *errs);
    virtual bool getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out);
    virtual bool getPidOutputs(const PidControlTypeEnum& pidtype, double *outs);
    virtual bool getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid);
    virtual bool getPids(const PidControlTypeEnum& pidtype, Pid *pids);
    virtual bool getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref);
    virtual bool getPidReferences(const PidControlTypeEnum& pidtype, double *refs);
    virtual bool getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *ref);
    virtual bool getPidErrorLimits(const PidControlTypeEnum& pidtype, double *refs);
    virtual bool resetPid(const PidControlTypeEnum& pidtype, int j);
    virtual bool enablePid(const PidControlTypeEnum& pidtype, int j);
    virtual bool disablePid(const PidControlTypeEnum& pidtype, int j);
    virtual bool setPidOffset(const PidControlTypeEnum& pidtype, int j, double v);
    virtual bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled);

    void convert_units_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, double userval, int j, double &machineval, int &k);
    void convert_units_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, const double* userval, double* machineval);
    void convert_units_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double machineval, double* userval, int k);
    void convert_units_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double* machineval, double* userval);

    virtual ~ImplementPidControl();
};

#endif // YARP_DEV_IMPLEMENTPIDCONTROL_H
