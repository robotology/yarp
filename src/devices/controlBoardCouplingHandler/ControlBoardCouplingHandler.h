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
#include "ControlBoardCouplingHandler_ParamsParser.h"

#include <string>
#include <vector>


#ifdef MSVC
    #pragma warning(disable:4355)
#endif
class ControlBoardCouplingHandler :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IEncodersTimed,
        public yarp::dev::IMultipleWrapper,
        public yarp::dev::IAxisInfo,
        // public yarp::dev::IControlLimits,
        public yarp::dev::IPreciselyTimed,
        public ControlBoardCouplingHandler_ParamsParser {
private:
    yarp::dev::PolyDriver            jointCouplingHandler;
    yarp::dev::IEncodersTimed        *iJntEnc;
    //yarp::dev::IControlLimits        *lim;
    yarp::dev::IPreciselyTimed       *iTimed;
    yarp::dev::IAxisInfo             *info;
    yarp::dev::IJointCoupling        *iJntCoupling;

    yarp::sig::Vector physJointsPos;
    yarp::sig::Vector physJointsVel;
    yarp::sig::Vector physJointsAcc;
    yarp::sig::Vector actAxesPos;
    yarp::sig::Vector actAxesVel;
    yarp::sig::Vector actAxesAcc;

    /** Verbosity of the class */
    bool _verb{false};

    // to open ports and print more detailed debug messages
    std::string partName;

    /**
     * Configure buffers used by the device
     */
    void configureBuffers();

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
