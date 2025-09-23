/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IVELOCITYDIRECTIMPL_H
#define YARP_DEV_IVELOCITYDIRECTIMPL_H


#include <yarp/dev/IVelocityDirect.h>
#include <yarp/os/Log.h>

namespace yarp::dev {
class ImplementVelocityDirect;
}

class YARP_dev_API yarp::dev::ImplementVelocityDirect : public IVelocityDirect
{
protected:
    IVelocityDirectRaw *m_iVelocityDirectRaw=nullptr;
    void    *m_helper=nullptr;
    std::vector<int> m_buffer_ints;
    std::vector<double> m_buffer_doubles;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     *  respect to the control/output values of the driver.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize();

public:
    /**
     * Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementVelocityDirect(yarp::dev::IVelocityDirectRaw* y);

    /**
     * Destructor.
     */
    virtual ~ImplementVelocityDirect();

    yarp::dev::ReturnValue getAxes(size_t& axes) override;
    yarp::dev::ReturnValue setRefVelocity(int jnt, double vel) override;
    yarp::dev::ReturnValue setRefVelocity(const std::vector<double>& vels) override;
    yarp::dev::ReturnValue setRefVelocity(const std::vector<int>& jnts, const std::vector<double>& vels) override;
    yarp::dev::ReturnValue getRefVelocity(const int jnt, double& vel) override;
    yarp::dev::ReturnValue getRefVelocity(std::vector<double>& vels) override;
    yarp::dev::ReturnValue getRefVelocity(const std::vector<int>& jnts, std::vector<double>& vels) override;
};

#endif // YARP_DEV_IVELOCITYDIRECTIMPL_H
