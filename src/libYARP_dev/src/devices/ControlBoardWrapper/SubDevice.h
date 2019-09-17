/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H
#define YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H


// ControlBoardWrapper
// A modified version of the remote control board class
// which remaps joints, it can also merge networks into a single part.
//

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

#include <string>
#include <vector>

#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp {
    namespace dev {
        class ControlBoardWrapper;
        namespace impl {
            class SubDevice;
            class WrappedDevice;
        }
    }
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*
* An Helper class for the controlBoardWrapper
* It maps only a subpart of the underlying device.
*/

class  yarp::dev::impl::SubDevice
{
public:
    std::string id;
    int base;
    int top;
    int wbase; //wrapper base
    int wtop; //wrapper top
    int axes; //number of used axis of this subdevice
    int totalAxis; //Numeber of total axis that the subdevice can control

    bool configuredF;

    yarp::dev::ControlBoardWrapper   *parent;

    yarp::dev::PolyDriver            *subdevice;
    yarp::dev::IPidControl           *pid;
    yarp::dev::IPositionControl      *pos;
    yarp::dev::IVelocityControl      *vel;
    yarp::dev::IEncodersTimed        *iJntEnc;
    yarp::dev::IMotorEncoders        *iMotEnc;
    yarp::dev::IAmplifierControl     *amp;
    yarp::dev::IControlLimits        *lim;
    yarp::dev::IControlCalibration   *calib;
    yarp::dev::IPreciselyTimed       *iTimed;
    yarp::dev::ITorqueControl        *iTorque;
    yarp::dev::IImpedanceControl     *iImpedance;
    yarp::dev::IControlMode          *iMode;
    yarp::dev::IAxisInfo             *info;
    yarp::dev::IPositionDirect       *posDir;
    yarp::dev::IInteractionMode      *iInteract;
    yarp::dev::IMotor                *imotor;
    yarp::dev::IRemoteVariables      *iVar;
    yarp::dev::IPWMControl           *iPWM;
    yarp::dev::ICurrentControl       *iCurr;

    yarp::sig::Vector subDev_joint_encoders;
    yarp::sig::Vector jointEncodersTimes;
    yarp::sig::Vector subDev_motor_encoders;
    yarp::sig::Vector motorEncodersTimes;

    SubDevice();

    bool attach(yarp::dev::PolyDriver *d, const std::string &id);
    void detach();
    inline void setVerbose(bool _verbose) {_subDevVerbose = _verbose; }

    bool configure(int wbase, int wtop, int base, int top, int axes, const std::string &id, yarp::dev::ControlBoardWrapper *_parent);

    bool isAttached()
    { return attachedF; }

private:
    bool _subDevVerbose;
    bool attachedF;
};

typedef std::vector<yarp::dev::impl::SubDevice> SubDeviceVector;

struct DevicesLutEntry
{
    int offset; //an offset, the device is mapped starting from this joint
    int deviceEntry; //index to the joint corresponding subdevice in the list
    int jointIndexInDev; //the index of joint in the numeration inside the device
};


class yarp::dev::impl::WrappedDevice
{
public:
    SubDeviceVector subdevices;
    std::vector<DevicesLutEntry> lut;
    int maxNumOfJointsInDevices;

    inline yarp::dev::impl::SubDevice *getSubdevice(unsigned int i)
    {
        if (i>=subdevices.size())
            return 0;

        return &subdevices[i];
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H
