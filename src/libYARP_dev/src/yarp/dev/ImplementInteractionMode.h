/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IINTERACTIONMODEIMPL_H
#define YARP_DEV_IINTERACTIONMODEIMPL_H

#include <yarp/dev/api.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
        class ImplementInteractionMode;
        class StubImplInteractionModeRaw;
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
 * @ingroup dev_iface_motor
 *
 * Interface settings the way the robot interacts with the environment: basic interaction types are Stiff and Compliant.
 * This setting is intended to work in conjunction with other settings like position or velocity control.
 */
class YARP_dev_API yarp::dev::ImplementInteractionMode : public yarp::dev::IInteractionMode
{
protected:
    yarp::dev::IInteractionModeRaw *iInteraction;
    void    *helper;                                // class controlBoardHelper, to handle axis map and conversion unit, where needed

    yarp::dev::impl::FixedSizeBuffersManager<yarp::dev::InteractionModeEnum> *imodeBuffManager; //This Buffer Manager handles temporary buffers of type yarp::dev::InteractionModeEnum
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager; //This Buffer Manager handles temporary buffers  of type int
    /**
     * Initialize the internal data and alloc memory, smaller version.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

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
     * @param Class_p is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementInteractionMode(yarp::dev::IInteractionModeRaw *Class_p);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementInteractionMode();

    // Methods are documented in the <yarp/dev/IInteractionMode.h> file
    bool getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode) override;
    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes) override;

    bool setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode) override;
    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes) override;
};


/**
 * Stub implementation of IInteractionMode interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IInteractionModeRaw. This class allows to
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
class yarp::dev::StubImplInteractionModeRaw : public yarp::dev::IInteractionModeRaw
{
protected:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func = 0)
    {
        if (func)
            yError("%s: not yet implemented\n", func);
        else
            yError("Function not yet implemented\n");

        return false;
    }


public:
    /**
     * Destructor.
     */
    virtual ~StubImplInteractionModeRaw() {}

    bool getInteractionModeRaw(int axis, yarp::dev::InteractionModeEnum* mode) override
    {
        return NOT_YET_IMPLEMENTED("getInteractionModeRaw");
    }

    bool getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override
    {
        return NOT_YET_IMPLEMENTED("getInteractionModesRaw");
    }

    bool getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override
    {
        return NOT_YET_IMPLEMENTED("getInteractionModesRaw");
    }

    bool setInteractionModeRaw(int axis, yarp::dev::InteractionModeEnum mode) override
    {
        return NOT_YET_IMPLEMENTED("setInteractionModeRaw");
    }

    bool setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override
    {
        return NOT_YET_IMPLEMENTED("setInteractionModesRaw");
    }

    bool setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override
    {
        return NOT_YET_IMPLEMENTED("setInteractionModesRaw");
    }
};

#endif // YARP_DEV_IINTERACTIONMODEIMPL_H
