/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H
#define YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H


// ControlBoardWrapper
// A modified version of the remote control board class
// which remaps joints, it can also merge networks into a single part.

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/PolyDriver.h>

#include "RPCMessagesParser.h"
#include "StreamingMessagesParser.h"
#include <string>
#include <vector>

#ifdef MSVC
#    pragma warning(disable : 4355)
#endif

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

class ControlBoardWrapper;

/*
* An Helper class for the controlBoardWrapper
* It maps only a subpart of the underlying device.
*/

class SubDevice
{
public:
    std::string id;
    size_t base {static_cast<size_t>(-1)};
    size_t top {static_cast<size_t>(-1)};
    size_t wbase;     //wrapper base
    size_t wtop;      //wrapper top
    size_t axes {0};  //number of used axis of this subdevice
    size_t totalAxis; //Numeber of total axis that the subdevice can control

    bool configuredF {false};

    std::string parentName;

    yarp::dev::PolyDriver* subdevice {nullptr};
    yarp::dev::IPidControl* pid {nullptr};
    yarp::dev::IPositionControl* pos {nullptr};
    yarp::dev::IVelocityControl* vel {nullptr};
    yarp::dev::IEncodersTimed* iJntEnc {nullptr};
    yarp::dev::IMotorEncoders* iMotEnc {nullptr};
    yarp::dev::IAmplifierControl* amp {nullptr};
    yarp::dev::IControlLimits* lim {nullptr};
    yarp::dev::IControlCalibration* calib {nullptr};
    yarp::dev::IPreciselyTimed* iTimed {nullptr};
    yarp::dev::ITorqueControl* iTorque {nullptr};
    yarp::dev::IImpedanceControl* iImpedance {nullptr};
    yarp::dev::IControlMode* iMode {nullptr};
    yarp::dev::IAxisInfo* info {nullptr};
    yarp::dev::IPositionDirect* posDir {nullptr};
    yarp::dev::IInteractionMode* iInteract {nullptr};
    yarp::dev::IMotor* imotor {nullptr};
    yarp::dev::IRemoteVariables* iVar {nullptr};
    yarp::dev::IPWMControl* iPWM {nullptr};
    yarp::dev::ICurrentControl* iCurr {nullptr};

    yarp::sig::Vector subDev_joint_encoders;
    yarp::sig::Vector jointEncodersTimes;
    yarp::sig::Vector subDev_motor_encoders;
    yarp::sig::Vector motorEncodersTimes;

    SubDevice() = default;

    bool attach(yarp::dev::PolyDriver* d, const std::string& id);
    void detach();

    bool configure(size_t wbase, size_t wtop, size_t base, size_t top, size_t axes, const std::string& id, const std::string& _parentName);

    bool isAttached() const
    {
        return attachedF;
    }

private:
    bool attachedF {false};
};

typedef std::vector<SubDevice> SubDeviceVector;

struct DevicesLutEntry
{
    size_t offset;          //an offset, the device is mapped starting from this joint
    size_t deviceEntry;     //index to the joint corresponding subdevice in the list
    size_t jointIndexInDev; //the index of joint in the numeration inside the device
};


class WrappedDevice
{
public:
    SubDeviceVector subdevices;
    std::vector<DevicesLutEntry> lut;
    size_t maxNumOfJointsInDevices;

    inline SubDevice* getSubdevice(size_t i)
    {
        if (i >= subdevices.size()) {
            return nullptr;
        }

        return &subdevices[i];
    }
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H
