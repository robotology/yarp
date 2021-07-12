/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_IPOSITIONDIRECTIMPL_H
#define YARP_IPOSITIONDIRECTIMPL_H


#include <yarp/dev/IPositionDirect.h>

namespace yarp{
    namespace dev {
        class ImplementPositionDirect;
        class StubImplPositionDirectRaw;
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

/**
 * Default implementation of the IPositionDirect interface. This class can
 * be used to easily provide an implementation of IPositionDirect.
 *
 */
class YARP_dev_API yarp::dev::ImplementPositionDirect : public yarp::dev::IPositionDirect
{
protected:
    IPositionDirectRaw *iPDirect;
    void    *helper;
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;

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
    bool uninitialize ();

public:
    /**
     * Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementPositionDirect(yarp::dev::IPositionDirectRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementPositionDirect();

    bool getAxes(int *axes) override;
    bool setPosition(int j, double ref) override;
    bool setPositions(const int n_joint, const int *joints, const double *refs) override;
    bool setPositions(const double *refs) override;
    bool getRefPosition(const int joint, double *ref) override;
    bool getRefPositions(double *refs) override;
    bool getRefPositions(const int n_joint, const int *joints, double *refs) override;
};


/**
 * Stub implementation of IPositionControl2Raw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IPositionControlRaw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 *
 */
class YARP_dev_API yarp::dev::StubImplPositionDirectRaw: public IPositionDirect
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplPositionDirectRaw(){}

    bool getAxes(int *axis) override
    {return NOT_YET_IMPLEMENTED("getAxes");}

    bool setPosition(int j, double ref) override
    {return NOT_YET_IMPLEMENTED("setPosition");}

    bool setPositions(const int n_joint, const int *joints, const double *refs) override
    {return NOT_YET_IMPLEMENTED("setPositions");}

    bool setPositions(const double *refs) override
    {return NOT_YET_IMPLEMENTED("setPositions");}
};

#endif // YARP_IPOSITIONDIRECTIMPL_H
