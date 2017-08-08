/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
#include <yarp/dev/PreciselyTimed.h>
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
    int axes;

    bool configuredF;

    yarp::dev::ControlBoardWrapper   *parent;

    yarp::dev::PolyDriver            *subdevice;
    yarp::dev::IPidControl           *pid;
    yarp::dev::IPositionControl      *pos;
    yarp::dev::IPositionControl2     *pos2;
    yarp::dev::IVelocityControl      *vel;
    yarp::dev::IVelocityControl2     *vel2;
    yarp::dev::IEncodersTimed        *iJntEnc;
    yarp::dev::IMotorEncoders        *iMotEnc;
    yarp::dev::IAmplifierControl     *amp;
    yarp::dev::IControlLimits2       *lim2;
    yarp::dev::IControlCalibration   *calib;
    yarp::dev::IControlCalibration2  *calib2;
    yarp::dev::IPreciselyTimed       *iTimed;
    yarp::dev::ITorqueControl        *iTorque;
    yarp::dev::IImpedanceControl     *iImpedance;
    yarp::dev::IControlMode          *iMode;
    yarp::dev::IControlMode2         *iMode2;
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

    bool configure(int base, int top, int axes, const std::string &id, yarp::dev::ControlBoardWrapper *_parent);

    inline void refreshJointEncoders()
    {
        for(int j=base, idx=0; j<(base+axes); j++, idx++)
        {
            if(iJntEnc)
                iJntEnc->getEncoderTimed(j, &subDev_joint_encoders[idx], &jointEncodersTimes[idx]);
        }
    }

    inline void refreshMotorEncoders()
    {
        for(int j=base, idx=0; j<(base+axes); j++, idx++)
        {
            if(iMotEnc)
                iMotEnc->getMotorEncoderTimed(j, &subDev_motor_encoders[idx], &motorEncodersTimes[idx]);
        }
    }



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
};


class yarp::dev::impl::WrappedDevice
{
public:
    SubDeviceVector subdevices;
    std::vector<DevicesLutEntry> lut;

    inline yarp::dev::impl::SubDevice *getSubdevice(unsigned int i)
    {
        if (i>=subdevices.size())
            return 0;

        return &subdevices[i];
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // YARP_DEV_CONTROLBOARDWRAPPER_SUBDEVICE_H
