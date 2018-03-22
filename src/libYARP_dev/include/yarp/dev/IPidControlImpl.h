/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTPIDCONTROL_H
#define YARP_DEV_IMPLEMENTPIDCONTROL_H

#include <yarp/dev/IPidControl.h>

namespace yarp {
    namespace dev {
        class ImplementPidControl;
    }
}

enum PidUnitsEnum
{
    MACHINE = 0,
    METRIC = 1,
    UNKNOWN = 2
};

class YARP_dev_API yarp::dev::ImplementPidControl : public IPidControl
{
    struct PidUnits
    {
        PidUnitsEnum fbk_units;
        PidUnitsEnum out_units;
        PidUnits() {
            fbk_units = MACHINE;
            out_units = MACHINE;
        }
    };

private:
    class PrivateUnitsHandler;
    PrivateUnitsHandler* mPriv;

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
    bool initialize(int size, const int *amap, const double *enc, const double *zos, const double* newtons, const double* amps, const double* dutys);

    /**
    * Clean up internal data and memory.
    * @return true if uninitialization is executed, false otherwise.
    */
    bool uninitialize();

    bool setConversion(const PidControlTypeEnum& pidtype, const PidUnitsEnum fbk_conv_units, const PidUnitsEnum out_conv_units);

public:
    /* Constructor.
    * @param y is the pointer to the class instance inheriting from this
    *  implementation.
    */
    ImplementPidControl(yarp::dev::IPidControlRaw *y);

    /* Destructor.
    */
    virtual ~ImplementPidControl();

    virtual bool setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) override;
    virtual bool setPids(const PidControlTypeEnum& pidtype, const Pid *pids) override;
    virtual bool setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) override;
    virtual bool setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) override;
    virtual bool setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) override;
    virtual bool setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) override;
    virtual bool getPidError(const PidControlTypeEnum& pidtype, int j, double *err) override;
    virtual bool getPidErrors(const PidControlTypeEnum& pidtype, double *errs) override;
    virtual bool getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) override;
    virtual bool getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) override;
    virtual bool getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) override;
    virtual bool getPids(const PidControlTypeEnum& pidtype, Pid *pids) override;
    virtual bool getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) override;
    virtual bool getPidReferences(const PidControlTypeEnum& pidtype, double *refs) override;
    virtual bool getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *ref) override;
    virtual bool getPidErrorLimits(const PidControlTypeEnum& pidtype, double *refs) override;
    virtual bool resetPid(const PidControlTypeEnum& pidtype, int j) override;
    virtual bool enablePid(const PidControlTypeEnum& pidtype, int j) override;
    virtual bool disablePid(const PidControlTypeEnum& pidtype, int j) override;
    virtual bool setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) override;
    virtual bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled) override;

private:
    void convert_units_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, double userval, int j, double &machineval, int &k);
    void convert_units_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, const double* userval, double* machineval);
    void convert_units_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double machineval, double* userval, int k);
    void convert_units_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double* machineval, double* userval);

    void convert_pid_to_user    (const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_raw, int j_raw, Pid &out_usr, int &k_usr);
    void convert_pid_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_usr, int j_usr, Pid &out_raw, int &k_raw);
};

#endif // YARP_DEV_IMPLEMENTPIDCONTROL_H
