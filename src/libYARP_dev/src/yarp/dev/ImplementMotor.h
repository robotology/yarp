/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTMOTOR_H
#define YARP_DEV_IMPLEMENTMOTOR_H

#include <yarp/dev/IMotor.h>

namespace yarp {
    namespace dev {
        class ImplementMotor;
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


class YARP_dev_API yarp::dev::ImplementMotor: public IMotor
{
protected:
    IMotorRaw *imotor;
    void *helper;

    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

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
    ImplementMotor(yarp::dev::IMotorRaw *y);


    virtual ~ImplementMotor();

    bool getNumberOfMotors(int *num) override;

    bool getTemperature      (int m, double* val) override;
    bool getTemperatures     (double *vals) override;
    bool getTemperatureLimit (int m, double* val) override;
    bool setTemperatureLimit (int m, const double val) override;
    bool getGearboxRatio     (int m, double* val) override;
    bool setGearboxRatio     (int m, const double val) override;
};

#endif // YARP_DEV_IMPLEMENTMOTOR_H
