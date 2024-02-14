/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
#define YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H

#include <yarp/os/Network.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IJointCoupling.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <string>
#include <vector>


#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/**
 * @ingroup dev_impl_remappers
 * FIXME !!!!
 * @brief `controlBoardCouplingHandler` : device that takes a list of axes from multiple controlboards and expose them as a single controlboard.
 *
 * | YARP device name |
 * |:-----------------:|
 * | `controlBoardCouplingHandler` |
 *
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | axesNames     |      -         | vector of strings  | -      |   -           | Yes     | Ordered list of the axes that are part of the remapped device. |  |
 *
 * The axes are then mapped to the wrapped controlboard in the attachAll method, using the
 * values returned by the getAxisName method of the controlboard. If different axes
 * in two attached controlboard have the same name, the behaviour of this device is undefined.
 *
 * Configuration file using .ini format.
 *
 * \code{.unparsed}
 *  device controlBoardCouplingHandler
 *  axesNames (joint1 joint2 joint3)
 *
 * ...
 * \endcode
 *
 * For compatibility with the controlBoard_nws_yarp, the
 * networks keyword can also be used to select the desired joints.
 * For more information on the syntax of the networks, see the
 * controlBoard_nws_yarp class.
 *
 * \code{.unparsed}
 *  networks (net_larm net_lhand)
 *  joints 16
 *  net_larm    0 3  0 3
 *  net_lhand   4 6  0 2
 * \endcode
 *
 */

class ControlBoardCouplingHandler :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IEncodersTimed,
        public yarp::dev::IMultipleWrapper,
        public yarp::dev::IAxisInfo,
        // public yarp::dev::IControlLimits,
        public yarp::dev::IPreciselyTimed{
private:
    yarp::dev::PolyDriver            jointCouplingHandler;
    yarp::dev::IEncodersTimed        *iJntEnc;
    //yarp::dev::IControlLimits        *lim;
    yarp::dev::IPreciselyTimed       *iTimed;
    yarp::dev::IAxisInfo             *info;

    std::vector<std::string> axesNames;

    /** number of axes controlled by this controlboard */
    int controlledJoints{0};

    /** Verbosity of the class */
    bool _verb{false};

    // to open ports and print more detailed debug messages
    std::string partName;

    /**
     * Set the number of controlled axes, resizing appropriately
     * all the necessary buffers.
     */
    void setNrOfControlledAxes(const size_t nrOfControlledAxes);

    /**
     * If the class was configured using the networks format,
     * call this method to update the vector containing the
     * axesName .
     */
    bool updateAxesName();
    // TODO I am keeping it just for compiling, but it should be removed

    /**
     * Configure buffers used by the device
     */
    void configureBuffers();

    // Parse device options
    bool parseOptions(yarp::os::Property &prop);

    bool usingAxesNamesForAttachAll{false};
    bool usingNetworksForAttachAll{false};

    /***
     * Parse device options if networks option is passed
     *
     * This will fill the axesNames and controlledJoints attributes, while it
     * leaves empty the remappedDevices attribute that will be then
     * filled only at the attachAll method.
     */
    bool parseAxesNames(const yarp::os::Property &prop);

    /***
     * Parse device options if networks option is passed
     *
     * This will fill the remappedDevices and controlledJoints attributes, while it
     * leaves empty the axesNames attribute that will be then
     * filled only at the attachAll method.
     */
    bool parseNetworks(const yarp::os::Property &prop);

    /**
     * attachAll if the networks option is used for configuration.
     */
    bool attachAllUsingNetworks(const yarp::dev::PolyDriverList &l);

    /**
     * attachAll if the axesNames option is used for configuration.
     */
    bool attachAllUsingAxesNames(const yarp::dev::PolyDriverList &l);


public:
    ControlBoardCouplingHandler() = default;
    ControlBoardCouplingHandler(const ControlBoardCouplingHandler&) = delete;
    ControlBoardCouplingHandler(ControlBoardCouplingHandler&&) = delete;
    ControlBoardCouplingHandler& operator=(const ControlBoardCouplingHandler&) = delete;
    ControlBoardCouplingHandler& operator=(ControlBoardCouplingHandler&&) = delete;
    ~ControlBoardCouplingHandler() override = default;

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return _verb; }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    bool close() override;


    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are described in the class documentation.
    */
    bool open(yarp::os::Searchable &prop) override;

    bool detachAll() override;

    bool attachAll(const yarp::dev::PolyDriverList &l) override;

    /* IEncoders */
    bool resetEncoder(int j) override;

    bool resetEncoders() override;

    bool setEncoder(int j, double val) override;

    bool setEncoders(const double *vals) override;

    bool getEncoder(int j, double *v) override;

    bool getEncoders(double *encs) override;

    bool getEncodersTimed(double *encs, double *t) override;

    bool getEncoderTimed(int j, double *v, double *t) override;

    bool getEncoderSpeed(int j, double *sp) override;

    bool getEncoderSpeeds(double *spds) override;

    bool getEncoderAcceleration(int j, double *acc) override;

    bool getEncoderAccelerations(double *accs) override;

    /* IAxisInfo */
    bool getAxes(int *ax) override;

    bool getAxisName(int j, std::string &name) override;

    bool getJointType(int j, yarp::dev::JointTypeEnum &type) override;

    // /* IControlLimits */
    // bool setLimits(int j, double min, double max) override;

    // bool getLimits(int j, double *min, double *max) override;

    // bool setVelLimits(int j, double min, double max) override;

    // bool getVelLimits(int j, double *min, double *max) override;


    yarp::os::Stamp getLastInputStamp() override;
};

#endif // YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
