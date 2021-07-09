/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTPIDCONTROL_H
#define YARP_DEV_IMPLEMENTPIDCONTROL_H

#include <yarp/dev/IPidControl.h>

namespace yarp {
    namespace dev {
        class ImplementPidControl;
    }
}

namespace yarp {
namespace dev {
namespace impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace impl
} // namespace dev
} // namespace yarp

class YARP_dev_API yarp::dev::ImplementPidControl : public IPidControl
{
protected:
    IPidControlRaw *iPid;
    void *helper;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<yarp::dev::Pid> *pidBuffManager;

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

    bool setConversionUnits(const PidControlTypeEnum& pidtype, const PidFeedbackUnitsEnum fbk_conv_units, const PidOutputUnitsEnum out_conv_units);

public:
    /* Constructor.
    * @param y is the pointer to the class instance inheriting from this
    *  implementation.
    */
    ImplementPidControl(yarp::dev::IPidControlRaw *y);

    /* Destructor.
    */
    virtual ~ImplementPidControl();

    bool setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) override;
    bool setPids(const PidControlTypeEnum& pidtype, const Pid *pids) override;
    bool setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) override;
    bool setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) override;
    bool setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) override;
    bool setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) override;
    bool getPidError(const PidControlTypeEnum& pidtype, int j, double *err) override;
    bool getPidErrors(const PidControlTypeEnum& pidtype, double *errs) override;
    bool getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) override;
    bool getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) override;
    bool getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) override;
    bool getPids(const PidControlTypeEnum& pidtype, Pid *pids) override;
    bool getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) override;
    bool getPidReferences(const PidControlTypeEnum& pidtype, double *refs) override;
    bool getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *ref) override;
    bool getPidErrorLimits(const PidControlTypeEnum& pidtype, double *refs) override;
    bool resetPid(const PidControlTypeEnum& pidtype, int j) override;
    bool enablePid(const PidControlTypeEnum& pidtype, int j) override;
    bool disablePid(const PidControlTypeEnum& pidtype, int j) override;
    bool setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) override;
    bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled) override;
    void b();
};

#endif // YARP_DEV_IMPLEMENTPIDCONTROL_H
