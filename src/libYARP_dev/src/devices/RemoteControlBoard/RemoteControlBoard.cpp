/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstring>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/QosStyle.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/PreciselyTimed.h>

#include <mutex>

#include "stateExtendedReader.h"

#define PROTOCOL_VERSION_MAJOR 1
#define PROTOCOL_VERSION_MINOR 9
#define PROTOCOL_VERSION_TWEAK 0

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace yarp{
    namespace dev {
        class RemoteControlBoard;
        struct ProtocolVersion;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

const double DIAGNOSTIC_THREAD_PERIOD=1.000;

inline bool getTimeStamp(Bottle &bot, Stamp &st)
{
    if (bot.get(3).asVocab()==VOCAB_TIMESTAMP)
    {
        //yup! we have a timestamp
        int fr=bot.get(4).asInt32();
        double ts=bot.get(5).asFloat64();
        st=Stamp(fr,ts);
        return true;
    }
    return false;
}

struct yarp::dev::ProtocolVersion
{
    int major;
    int minor;
    int tweak;
};


class DiagnosticThread: public PeriodicThread
{
    StateExtendedInputPort *owner;
    std::string ownerName;

public:
    DiagnosticThread(double r): PeriodicThread(r)
    { owner=nullptr; }

    void setOwner(StateExtendedInputPort *o)
    {
        owner=o;
        ownerName=owner->getName();
    }

    void run() override
    {
        if (owner!=nullptr)
        {
            if (owner->getIterations()>100)
            {
                int it;
                double av;
                double max;
                double min;
                owner->getEstFrequency(it, av, min, max);
                owner->resetStat();
                yDebug("%s: %d msgs av:%.2lf min:%.2lf max:%.2lf [ms]\n",
                    ownerName.c_str(),
                    it,
                    av,
                    min,
                    max);
            }

        }
    }

};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


/**
* @ingroup dev_impl_network_clients
*
* The client side of the control board, connects to a remote controlboard using the YARP network.
*
* This device communicates using the YARP ports opened the yarp::dev::ControlBoardWrapper device
* to use a device exposing controlboard method even from a different process (or even computer)
* from the one that opened the controlboard device.
*
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units | Default Value | Required     | Description                                    | Notes |
* |:--------------:|:--------------:|:-------:|:-----:|:-------------:|:-----------: |:----------------------------------------------:|:-----:|
* | remote         |       -        | string  | -     |   -           | Yes          | Prefix of the port to which to connect.        |       |
* | local          |       -        | string  | -     |   -           | Yes          | Port prefix of the port opened by this device. |       |
* | writeStrict    |       -        | string  | -     | See note      | No           |                                                |       |
*
*/
class yarp::dev::RemoteControlBoard :
    public IPidControl,
    public IPositionControl,
    public IVelocityControl,
    public IEncodersTimed,
    public IMotorEncoders,
    public IMotor,
    public IAmplifierControl,
    public IControlLimits,
    public IAxisInfo,
    public IPreciselyTimed,
    public IControlCalibration,
    public ITorqueControl,
    public IImpedanceControl,
    public IControlMode,
    public DeviceDriver,
    public IPositionDirect,
    public IInteractionMode,
    public IRemoteCalibrator,
    public IRemoteVariables,
    public IPWMControl,
    public ICurrentControl
{

#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
    Port rpc_p;
    Port command_p;
    DiagnosticThread *diagnosticThread{nullptr};

    PortReaderBuffer<yarp::sig::Vector> state_buffer;
    PortWriterBuffer<CommandMessage> command_buffer;
    bool writeStrict_singleJoint{true};
    bool writeStrict_moreJoints{false};

    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
//  yarp::os::PortReaderBuffer<jointData>           extendedInputState_buffer;  // Buffer storing new data
    StateExtendedInputPort                          extendedIntputStatePort;  // Buffered port storing new data
    std::mutex extendedPortMutex;
    yarp::dev::impl::jointData last_singleJoint;     // tmp to store last received data for a particular joint
//    yarp::os::Port extendedIntputStatePort;         // Port /stateExt:i reading the state of the joints
    yarp::dev::impl::jointData last_wholePart;         // tmp to store last received data for whole part

    std::string remote;
    std::string local;
    mutable Stamp lastStamp;  //this is shared among all calls that read encoders
    int nj{0};
    bool njIsKnown{false};

    ProtocolVersion protocolVersion;

    // Check for number of joints, if needed.
    // This is to allow for delayed connection to the remote control board.
    bool isLive() {
        if (!njIsKnown) {
            bool ok = get1V1I(VOCAB_AXES, nj);
            if (nj!=0 && ok) {
                njIsKnown = true;
            }
        }
        return njIsKnown;
    }

    bool send1V(int v)
    {
        Bottle cmd, response;
        cmd.addVocab(v);
        bool ok=rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool send2V(int v1, int v2)
    {
        Bottle cmd, response;
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        bool ok=rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool send2V1I(int v1, int v2, int axis)
    {
        Bottle cmd, response;
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt32(axis);
        bool ok=rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool send1V1I(int v, int axis)
    {
        Bottle cmd, response;
        cmd.addVocab(v);
        cmd.addInt32(axis);
        bool ok=rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool send3V1I(int v1, int v2, int v3, int j)
    {
        Bottle cmd, response;
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addVocab(v3);
        cmd.addInt32(j);
        bool ok=rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }
    /**
     * Send a SET command without parameters and wait for a reply.
     * @param code is the command Vocab identifier.
     * @return true/false on success/failure.
     */
    bool set1V(int code) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Send a SET command and an additional double valued variable
     * and then wait for a reply.
     * @param code is the command to send.
     * @param v is a double valued parameter.
     * @return true/false on success/failure.
     */
    bool set1V2D(int code, double v)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addFloat64(v);

        bool ok = rpc_p.write(cmd, response);

        return CHECK_FAIL(ok, response);
    }

    /**
     * Send a SET command with an additional integer valued variable
     * and then wait for a reply.
     * @param code is the command to send.
     * @param v is an integer valued parameter.
     * @return true/false on success/failure.
     */
    bool set1V1I(int code, int v) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt32(v);

        bool ok = rpc_p.write(cmd, response);

        return CHECK_FAIL(ok, response);
    }

    /**
     * Send a GET command expecting a double value in return.
     * @param code is the Vocab code of the GET command.
     * @param v is a reference to the return variable.
     * @return true/false on success/failure.
     */
    bool get1V1D(int code, double& v) const {
        Bottle cmd;
        Bottle response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            // response should be [cmd] [name] value
            v = response.get(2).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }

        return false;
    }

    /**
     * Send a GET command expecting an integer value in return.
     * @param code is the Vocab code of the GET command.
     * @param v is a reference to the return variable.
     * @return true/false on success/failure.
     */
    bool get1V1I(int code, int& v) const {
        Bottle cmd;
        Bottle response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            // response should be [cmd] [name] value
            v = response.get(2).asInt32();

            getTimeStamp(response, lastStamp);
            return true;
        }

        return false;
    }

    /**
     * Helper method to set a double value to a single axis.
     * @param code is the name of the command to be transmitted
     * @param j is the axis
     * @param val is the double value
     * @return true/false on success/failure
     */
    bool set1V1I1D(int code, int j, double val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt32(j);
        cmd.addFloat64(val);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set1V1I2D(int code, int j, double val1, double val2)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt32(j);
        cmd.addFloat64(val1);
        cmd.addFloat64(val2);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Helper method used to set an array of double to all axes.
     * @param v is the command to set
     * @param val is the double array (of length nj)
     * @return true/false on success/failure
     */
    bool set1VDA(int v, const double *val) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++)
            l.addFloat64(val[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set2V1DA(int v1, int v2, const double *val) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++)
            l.addFloat64(val[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set2V2DA(int v1, int v2, const double *val1, const double *val2) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < nj; i++)
            l1.addFloat64(val1[i]);
        Bottle& l2 = cmd.addList();
        for (i = 0; i < nj; i++)
            l2.addFloat64(val2[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set1V1I1IA1DA(int v, const int len, const int *val1, const double *val2) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v);
        cmd.addInt32(len);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < len; i++)
            l1.addInt32(val1[i]);
        Bottle& l2 = cmd.addList();
        for (i = 0; i < len; i++)
            l2.addFloat64(val2[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set2V1I1D(int v1, int v2, int axis, double val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt32(axis);
        cmd.addFloat64(val);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

     bool setValWithPidType(int voc, PidControlTypeEnum type, int axis, double val)
     {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(voc);
        cmd.addVocab(type);
        cmd.addInt32(axis);
        cmd.addFloat64(val);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setValWithPidType(int voc, PidControlTypeEnum type, const double* val_arr)
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(voc);
        cmd.addVocab(type);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++)
            l.addFloat64(val_arr[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getValWithPidType(int voc, PidControlTypeEnum type, int j, double *val)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(voc);
        cmd.addVocab(type);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response))
        {
            *val = response.get(2).asFloat64();
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool getValWithPidType(int voc, PidControlTypeEnum type, double *val)
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(voc);
        cmd.addVocab(type);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response))
        {
            int i;
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            yAssert(nj == l.size());
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asFloat64();
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool set2V1I(int v1, int v2, int axis) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt32(axis);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Helper method used to get a double value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool get1V1I1D(int v, int j, double *val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }


    /**
     * Helper method used to get an integer value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool get1V1I1I(int v, int j, int *val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asInt32();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get2V1I1D(int v1, int v2, int j, double *val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get2V1I2D(int v1, int v2, int j, double *val1, double *val2) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // ok
            *val1 = response.get(2).asFloat64();
            *val2 = response.get(3).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get1V1I2D(int code, int axis, double *v1, double *v2)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        cmd.addInt32(axis);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            *v1 = response.get(2).asFloat64();
            *v2 = response.get(3).asFloat64();
            return true;
        }
        return false;
    }

    /**
     * Helper method used to get a double value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool get1V1I1B(int v, int j, bool &val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            val = (response.get(2).asInt32()!=0);
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get1V1I1IA1B(int v,  const int len, const int *val1, bool &retVal ) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt32(len);
        Bottle& l1 = cmd.addList();
        for (int i = 0; i < len; i++)
            l1.addInt32(val1[i]);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            retVal = (response.get(2).asInt32()!=0);
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get2V1I1IA1DA(int v1, int v2, const int n_joints, const int *joints, double *retVals, std::string functionName = "")
    {
        Bottle cmd, response;
        if (!isLive()) return false;

        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt32(n_joints);

        Bottle& l1 = cmd.addList();
        for (int i = 0; i < n_joints; i++)
            l1.addInt32(joints[i]);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response))
        {
            int i;
            Bottle& list = *(response.get(0).asList());
            yAssert(list.size() >= (size_t) n_joints)

            if (list.size() != (size_t )n_joints)
            {
                yError("%s length of response does not match: expected %d, received %zu\n ", functionName.c_str(), n_joints , list.size() );
                return false;
            }
            else
            {
                for (i = 0; i < n_joints; i++)
                {
                    retVals[i] = (double) list.get(i).asFloat64();
                }
                return true;
            }
        }
        return false;
    }

    bool get1V1B(int v, bool &val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            val = (response.get(2).asInt32()!=0);
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    /**
     * Helper method to get an array of integers from the remote peer.
     * @param v is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get1VIA(int v, int *val) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            yAssert(nj == l.size());
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asInt32();

            getTimeStamp(response, lastStamp);

            return true;
        }
        return false;
    }

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get1VDA(int v, double *val) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            yAssert(nj == l.size());
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asFloat64();

            getTimeStamp(response, lastStamp);

            return true;
        }
        return false;
    }

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v1 is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get1V1DA(int v1, double *val) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v1);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            yAssert(nj == l.size());
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v1 is the name of the command
     * @param v2 v2
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get2V1DA(int v1, int v2, double *val) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            yAssert(nj == l.size());
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get2V2DA(int v1, int v2, double *val1, double *val2) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle* lp1 = response.get(2).asList();
            if (lp1 == nullptr)
                return false;
            Bottle& l1 = *lp1;
            Bottle* lp2 = response.get(3).asList();
            if (lp2 == nullptr)
                return false;
            Bottle& l2 = *lp2;

            int nj1 = l1.size();
            int nj2 = l2.size();
           // yAssert(nj == nj1);
           // yAssert(nj == nj2);

            for (i = 0; i < nj1; i++)
                val1[i] = l1.get(i).asFloat64();
            for (i = 0; i < nj2; i++)
                val2[i] = l2.get(i).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get1V1I1S(int code, int j, std::string &name)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            name = response.get(2).asString();
            return true;
        }
        return false;
    }


    bool get1V1I1IA1DA(int v, const int len, const int *val1, double *val2)
    {
        if(!isLive()) return false;

        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt32(len);
        Bottle &l1 = cmd.addList();
        for(int i = 0; i < len; i++)
            l1.addInt32(val1[i]);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle* lp2 = response.get(2).asList();
            if (lp2 == nullptr)
                return false;
            Bottle& l2 = *lp2;

            int nj2 = l2.size();
            if(nj2 != len)
            {
                yError("received an answer with an unexpected number of entries!\n");
                return false;
            }
            for (i = 0; i < nj2; i++)
                val2[i] = l2.get(i).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:
    /**
     * Constructor.
     */
    RemoteControlBoard() :
        protocolVersion(ProtocolVersion{0,0,0})
    {}

    /**
     * Destructor.
     */
    virtual ~RemoteControlBoard() = default;


    /**
     * Default open.
     * @return always true.
     */
    virtual bool open() {
        return true;
    }

    bool open(Searchable& config) override {
        remote = config.find("remote").asString();
        local = config.find("local").asString();

        if (config.check("timeout"))
        {
            extendedIntputStatePort.setTimeout(config.find("timeout").asFloat64());
        }
        // check the Qos perefernces if available (local and remote)
        yarp::os::QosStyle localQos;
        if (config.check("local_qos")) {
            Bottle& qos = config.findGroup("local_qos");
            if(qos.check("thread_priority"))
                localQos.setThreadPriority(qos.find("thread_priority").asInt32());
            if(qos.check("thread_policy"))
                localQos.setThreadPolicy(qos.find("thread_policy").asInt32());
            if(qos.check("packet_priority"))
                localQos.setPacketPriority(qos.find("packet_priority").asString());
        }

        yarp::os::QosStyle remoteQos;
        if (config.check("remote_qos")) {
            Bottle& qos = config.findGroup("remote_qos");
            if(qos.check("thread_priority"))
                remoteQos.setThreadPriority(qos.find("thread_priority").asInt32());
            if(qos.check("thread_policy"))
                remoteQos.setThreadPolicy(qos.find("thread_policy").asInt32());
            if(qos.check("packet_priority"))
                remoteQos.setPacketPriority(qos.find("packet_priority").asString());
        }

        bool writeStrict_isFound = config.check("writeStrict");
        if(writeStrict_isFound)
        {
            Value &gotStrictVal = config.find("writeStrict");
            if(gotStrictVal.asString() == "on")
            {
                writeStrict_singleJoint = true;
                writeStrict_moreJoints  = true;
                yInfo("RemoteControlBoard is ENABLING the writeStrict option for all commands\n");
            }
            else if(gotStrictVal.asString() == "off")
            {
                writeStrict_singleJoint = false;
                writeStrict_moreJoints  = false;
                yInfo("RemoteControlBoard is DISABLING the writeStrict opition for all commands\n");
            }
            else
                yError("Found writeStrict opition with wrong value. Accepted options are 'on' or 'off'\n");
        }

        if (local=="") {
            yError("Problem connecting to remote controlboard, 'local' port prefix not given\n");
            return false;
        }

        if (remote=="") {
            yError("Problem connecting to remote controlboard, 'remote' port name not given\n");
            return false;
        }

        std::string carrier =
            config.check("carrier",
            Value("udp"),
            "default carrier for streaming robot state").asString();

        bool portProblem = false;
        if (local != "") {
            std::string s1 = local;
            s1 += "/rpc:o";
            if (!rpc_p.open(s1)) { portProblem = true; }
            s1 = local;
            s1 += "/command:o";
            if (!command_p.open(s1)) { portProblem = true; }
            s1 = local;
            s1 += "/stateExt:i";
            if (!extendedIntputStatePort.open(s1)) { portProblem = true; }
            if (!portProblem)
            {
                extendedIntputStatePort.useCallback();
            }
        }

        bool connectionProblem = false;
        if (remote != "" && !portProblem)
        {
            std::string s1 = remote;
            s1 += "/rpc:i";
            std::string s2 = local;
            s2 += "/rpc:o";
            bool ok = false;
            // RPC port needs to be tcp, therefore no carrier option is added here
            // ok=Network::connect(s2.c_str(), s1.c_str());         //This doesn't take into consideration possible YARP_PORT_PREFIX on local ports
            // ok=Network::connect(rpc_p.getName(), s1.c_str());    //This should work also with YARP_PORT_PREFIX because getting back the name of the port will return the modified name
            ok=rpc_p.addOutput(s1);                         //This works because we are manipulating only remote side and let yarp handle the local side
            if (!ok) {
                yError("Problem connecting to %s, is the remote device available?\n", s1.c_str());
                connectionProblem = true;
            }

            s1 = remote;
            s1 += "/command:i";
            s2 = local;
            s2 += "/command:o";
            //ok = Network::connect(s2.c_str(), s1.c_str(), carrier);
            // ok=Network::connect(command_p.getName(), s1.c_str(), carrier); //doesn't take into consideration possible YARP_PORT_PREFIX on local ports
            ok = command_p.addOutput(s1, carrier);
            if (!ok) {
                yError("Problem connecting to %s, is the remote device available?\n", s1.c_str());
                connectionProblem = true;
            }
            // set the QoS preferences for the 'command' port
            if (config.check("local_qos") || config.check("remote_qos"))
                NetworkBase::setConnectionQos(command_p.getName(), s1, localQos, remoteQos, false);

            s1 = remote;
            s1 += "/stateExt:o";
            s2 = local;
            s2 += "/stateExt:i";
            // not checking return value for now since it is wip (different machines can have different compilation flags
            ok = Network::connect(s1, extendedIntputStatePort.getName(), carrier);
            if (ok)
            {
                // set the QoS preferences for the 'state' port
                if (config.check("local_qos") || config.check("remote_qos"))
                    NetworkBase::setConnectionQos(s1, extendedIntputStatePort.getName(), remoteQos, localQos, false);
            }
            else
            {
                yError("Problem connecting to %s, is the remote device available?\n", s1.c_str());
                connectionProblem = true;
            }
        }

        if (connectionProblem||portProblem) {

            rpc_p.close();
            command_p.close();
            extendedIntputStatePort.close();
            return false;
        }

        state_buffer.setStrict(false);
        command_buffer.attach(command_p);

        if (!checkProtocolVersion(config.check("ignoreProtocolCheck")))
        {
            yError() << "checkProtocolVersion failed";
            command_buffer.detach();
            rpc_p.close();
            command_p.close();
            extendedIntputStatePort.close();
            return false;
        }

        if (!isLive()) {
            if (remote!="") {
                yError("Problems with obtaining the number of controlled axes\n");
                command_buffer.detach();
                rpc_p.close();
                command_p.close();
                extendedIntputStatePort.close();
                return false;
            }
        }

        if (config.check("diagnostic"))
        {
            diagnosticThread = new DiagnosticThread(DIAGNOSTIC_THREAD_PERIOD);
            diagnosticThread->setOwner(&extendedIntputStatePort);
            diagnosticThread->start();
        }
        else
            diagnosticThread=nullptr;

        // allocate memory for helper struct
        // single joint
        last_singleJoint.jointPosition.resize(1);
        last_singleJoint.jointVelocity.resize(1);
        last_singleJoint.jointAcceleration.resize(1);
        last_singleJoint.motorPosition.resize(1);
        last_singleJoint.motorVelocity.resize(1);
        last_singleJoint.motorAcceleration.resize(1);
        last_singleJoint.torque.resize(1);
        last_singleJoint.pwmDutycycle.resize(1);
        last_singleJoint.current.resize(1);
        last_singleJoint.controlMode.resize(1);
        last_singleJoint.interactionMode.resize(1);

        // whole part  (safe here because we already got the nj
        last_wholePart.jointPosition.resize(nj);
        last_wholePart.jointVelocity.resize(nj);
        last_wholePart.jointAcceleration.resize(nj);
        last_wholePart.motorPosition.resize(nj);
        last_wholePart.motorVelocity.resize(nj);
        last_wholePart.motorAcceleration.resize(nj);
        last_wholePart.torque.resize(nj);
        last_wholePart.current.resize(nj);
        last_wholePart.pwmDutycycle.resize(nj);
        last_wholePart.controlMode.resize(nj);
        last_wholePart.interactionMode.resize(nj);
        return true;
    }

    /**
     * Close the device driver and stop the port connections.
     * @return true/false on success/failure.
     */
    bool close() override {

        if (diagnosticThread!=nullptr)
        {
            diagnosticThread->stop();
            delete diagnosticThread;
        }

        rpc_p.interrupt();
        command_p.interrupt();
        extendedIntputStatePort.interrupt();

        rpc_p.close();
        command_p.close();
        extendedIntputStatePort.close();
        return true;
    }

    bool setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) override {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(pidtype);
        cmd.addInt32(j);
        Bottle& l = cmd.addList();
        l.addFloat64(pid.kp);
        l.addFloat64(pid.kd);
        l.addFloat64(pid.ki);
        l.addFloat64(pid.max_int);
        l.addFloat64(pid.max_output);
        l.addFloat64(pid.offset);
        l.addFloat64(pid.scale);
        l.addFloat64(pid.stiction_up_val);
        l.addFloat64(pid.stiction_down_val);
        l.addFloat64(pid.kff);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setPids(const PidControlTypeEnum& pidtype, const Pid *pids) override {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_PIDS);
        cmd.addVocab(pidtype);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++) {
            Bottle& m = l.addList();
            m.addFloat64(pids[i].kp);
            m.addFloat64(pids[i].kd);
            m.addFloat64(pids[i].ki);
            m.addFloat64(pids[i].max_int);
            m.addFloat64(pids[i].max_output);
            m.addFloat64(pids[i].offset);
            m.addFloat64(pids[i].scale);
            m.addFloat64(pids[i].stiction_up_val);
            m.addFloat64(pids[i].stiction_down_val);
            m.addFloat64(pids[i].kff);
        }

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) override
    {
        return setValWithPidType(VOCAB_REF, pidtype, j, ref);
    }

    bool setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) override {
        return setValWithPidType(VOCAB_REFS, pidtype, refs);
    }

    bool setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) override {
        return setValWithPidType(VOCAB_LIM, pidtype, j, limit);
    }

    bool setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) override {
        return setValWithPidType(VOCAB_LIMS, pidtype, limits);
    }

    bool getPidError(const PidControlTypeEnum& pidtype, int j, double *err) override {
        return getValWithPidType(VOCAB_ERR, pidtype, j, err);
    }

    bool getPidErrors(const PidControlTypeEnum& pidtype, double *errs) override {
        return getValWithPidType(VOCAB_ERRS, pidtype, errs);
    }

    bool getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) override {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(pidtype);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            pid->kp = l.get(0).asFloat64();
            pid->kd = l.get(1).asFloat64();
            pid->ki = l.get(2).asFloat64();
            pid->max_int = l.get(3).asFloat64();
            pid->max_output = l.get(4).asFloat64();
            pid->offset = l.get(5).asFloat64();
            pid->scale = l.get(6).asFloat64();
            pid->stiction_up_val = l.get(7).asFloat64();
            pid->stiction_down_val = l.get(8).asFloat64();
            pid->kff = l.get(9).asFloat64();
            return true;
        }
        return false;
    }

    bool getPids(const PidControlTypeEnum& pidtype, Pid *pids) override {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_PIDS);
        cmd.addVocab(pidtype);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response))
        {
            int i;
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            yAssert(nj == l.size());
            for (i = 0; i < nj; i++)
            {
                Bottle* mp = l.get(i).asList();
                if (mp == nullptr)
                    return false;
                pids[i].kp = mp->get(0).asFloat64();
                pids[i].kd = mp->get(1).asFloat64();
                pids[i].ki = mp->get(2).asFloat64();
                pids[i].max_int = mp->get(3).asFloat64();
                pids[i].max_output = mp->get(4).asFloat64();
                pids[i].offset = mp->get(5).asFloat64();
                pids[i].scale = mp->get(6).asFloat64();
                pids[i].stiction_up_val = mp->get(7).asFloat64();
                pids[i].stiction_down_val = mp->get(8).asFloat64();
                pids[i].kff = mp->get(9).asFloat64();
            }
            return true;
        }
        return false;
    }

    bool getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) override {
        return getValWithPidType(VOCAB_REF, pidtype, j, ref);
    }

    bool getPidReferences(const PidControlTypeEnum& pidtype, double *refs) override {
        return getValWithPidType(VOCAB_REFS, pidtype, refs);
    }

    bool getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit) override {
        return getValWithPidType(VOCAB_LIM, pidtype, j, limit);
    }

    bool getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits) override {
        return getValWithPidType(VOCAB_LIMS, pidtype, limits);
    }

    bool resetPid(const PidControlTypeEnum& pidtype, int j) override {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_RESET);
        cmd.addVocab(pidtype);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool disablePid(const PidControlTypeEnum& pidtype, int j) override {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_DISABLE);
        cmd.addVocab(pidtype);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool enablePid(const PidControlTypeEnum& pidtype, int j) override {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_ENABLE);
        cmd.addVocab(pidtype);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addVocab(VOCAB_ENABLE);
        cmd.addVocab(pidtype);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response))
        {
            *enabled = response.get(2).asBool();
            return true;
        }
        return false;
    }

    bool getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) override
    {
        return getValWithPidType(VOCAB_OUTPUT, pidtype, j, out);
    }

    bool getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) override
    {
        return getValWithPidType(VOCAB_OUTPUTS, pidtype, outs);
    }

    bool setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) override
    {
        return setValWithPidType(VOCAB_OFFSET, pidtype, j, v);
    }

    /* IEncoder */

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j is the axis number
     * @return true/false on success/failure
     */
    bool resetEncoder(int j) override {
        return set1V1I(VOCAB_E_RESET, j);
    }

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    bool resetEncoders() override {
        return set1V(VOCAB_E_RESETS);
    }

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false on success/failure
     */
    bool setEncoder(int j, double val) override {
        return set1V1I1D(VOCAB_ENCODER, j, val);
    }

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    bool setEncoders(const double *vals) override {
        return set1VDA(VOCAB_ENCODERS, vals);
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    bool getEncoder(int j, double *v) override
    {
        double localArrivalTime = 0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER, v, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    bool getEncoderTimed(int j, double *v, double *t) override
    {
        double localArrivalTime = 0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER, v, lastStamp, localArrivalTime);
        *t=lastStamp.getTime();
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the position of all axes. This object receives encoders periodically
     * from a YARP port. You should check the return value of the function to
     * make sure that encoders have been received at least once and with the expected
     * rate.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure. Failure means encoders have not been received
     * from the server or that they are not being streamed with the expected rate.
     */
    bool getEncoders(double *encs) override {
        double localArrivalTime = 0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODERS, encs, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();

        return ret;
    }

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @param ts pointer to the array that will contain timestamps
     * @return true/false on success/failure
     */
    bool getEncodersTimed(double *encs, double *ts) override {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODERS, encs, lastStamp, localArrivalTime);
        std::fill_n(ts, nj, lastStamp.getTime());
        extendedPortMutex.unlock();
        return ret;
    }
    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    bool getEncoderSpeed(int j, double *sp) override
    {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER_SPEED, sp, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }


    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    bool getEncoderSpeeds(double *spds) override
    {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODER_SPEEDS, spds, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param acc pointer to the array that will contain the output
     */

    bool getEncoderAcceleration(int j, double *acc) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER_ACCELERATION, acc, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    bool getEncoderAccelerations(double *accs) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODER_ACCELERATIONS, accs, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /* IRemoteVariable */
    bool getRemoteVariable(std::string key, yarp::os::Bottle& val) override {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_REMOTE_VARIABILE_INTERFACE);
        cmd.addVocab(VOCAB_VARIABLE);
        cmd.addString(key);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response))
        {
            val = *(response.get(2).asList());
            return true;
        }
        return false;
    }

    bool setRemoteVariable(std::string key, const yarp::os::Bottle& val) override {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_VARIABILE_INTERFACE);
        cmd.addVocab(VOCAB_VARIABLE);
        cmd.addString(key);
        cmd.append(val);
        //std::string s = cmd.toString();
        bool ok = rpc_p.write(cmd, response);

        return CHECK_FAIL(ok, response);
    }


    bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) override {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_REMOTE_VARIABILE_INTERFACE);
        cmd.addVocab(VOCAB_LIST_VARIABLES);
        bool ok = rpc_p.write(cmd, response);
        //std::string s = response.toString();
        if (CHECK_FAIL(ok, response))
        {
            *listOfKeys = *(response.get(2).asList());
            //std::string s = listOfKeys->toString();
            return true;
        }
        return false;
    }

    /* IMotor */
    bool getNumberOfMotors(int *num) override {
        return get1V1I(VOCAB_MOTORS_NUMBER, *num);
    }

    bool getTemperature      (int m, double* val) override {
        return get1V1I1D(VOCAB_TEMPERATURE, m, val);
    }

    bool getTemperatures     (double *vals) override {
        return get1VDA(VOCAB_TEMPERATURES, vals);
    }

    bool getTemperatureLimit (int m, double* val) override {
        return get1V1I1D(VOCAB_TEMPERATURE_LIMIT, m, val);
    }

    bool setTemperatureLimit (int m, const double val) override {
        return set1V1I1D(VOCAB_TEMPERATURE_LIMIT, m, val);
    }

    bool getGearboxRatio(int m, double* val) override {
        return get1V1I1D(VOCAB_GEARBOX_RATIO, m, val);
    }

    virtual bool getGearboxRatio(int m, const double val) {
        return set1V1I1D(VOCAB_GEARBOX_RATIO, m, val);
    }
    /* IMotorEncoder */

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j is the axis number
     * @return true/false on success/failure
     */
    bool resetMotorEncoder(int j) override {
        return set1V1I(VOCAB_MOTOR_E_RESET, j);
    }

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    bool resetMotorEncoders() override {
        return set1V(VOCAB_MOTOR_E_RESETS);
    }

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false on success/failure
     */
    bool setMotorEncoder(int j, const double val) override {
        return set1V1I1D(VOCAB_MOTOR_ENCODER, j, val);
    }

    /**
     * Sets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr new parameter
     * @return true/false
     */
    bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override {
        return set1V1I1D(VOCAB_MOTOR_CPR, m, cpr);
    }

    /**
     * gets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr pointer to storage for the return value
     * @return true/false
     */
    bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override {
         return get1V1I1D(VOCAB_MOTOR_CPR, m, cpr);
    }

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    bool setMotorEncoders(const double *vals) override {
        return set1VDA(VOCAB_MOTOR_ENCODERS, vals);
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    bool getMotorEncoder(int j, double *v) override
    {
        double localArrivalTime = 0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER, v, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    bool getMotorEncoderTimed(int j, double *v, double *t) override
    {
        double localArrivalTime = 0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER, v, lastStamp, localArrivalTime);
        *t=lastStamp.getTime();
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the position of all axes. This object receives encoders periodically
     * from a YARP port. You should check the return value of the function to
     * make sure that encoders have been received at least once and with the expected
     * rate.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure. Failure means encoders have not been received
     * from the server or that they are not being streamed with the expected rate.
     */
    bool getMotorEncoders(double *encs) override
    {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODERS, encs, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();

        return ret;
    }

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @param ts pointer to the array that will contain timestamps
     * @return true/false on success/failure
     */
    bool getMotorEncodersTimed(double *encs, double *ts) override
    {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODERS, encs, lastStamp, localArrivalTime);
        std::fill_n(ts, nj, lastStamp.getTime());
        extendedPortMutex.unlock();
        return ret;
    }
    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    bool getMotorEncoderSpeed(int j, double *sp) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER_SPEED, sp, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    bool getMotorEncoderSpeeds(double *spds) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODER_SPEEDS, spds, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param acc pointer to the array that will contain the output
     */

    bool getMotorEncoderAcceleration(int j, double *acc) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER_ACCELERATION, acc, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    bool getMotorEncoderAccelerations(double *accs) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODER_SPEEDS, accs, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /* IPreciselyTimed */
    /**
     * Get the time stamp for the last read data
     * @return last time stamp.
     */
    Stamp getLastInputStamp() override {
        Stamp ret;
//        mutex.lock();
        ret = lastStamp;
//        mutex.unlock();
        return ret;
    }

    /**
     * Get the number of available motor encoders.
     * @param m pointer to a value representing the number of available motor encoders.
     * @return true/false
     */
    bool getNumberOfMotorEncoders(int *num) override {
        return get1V1I(VOCAB_MOTOR_ENCODER_NUMBER, *num);
    }

    /* IPositionControl */

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    bool getAxes(int *ax) override {
        return get1V1I(VOCAB_AXES, *ax);
    }

    /**
     * Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    bool positionMove(int j, double ref) override {
        return set1V1I1D(VOCAB_POSITION_MOVE, j, ref);
    }

    /**
     * Set new reference point for a group of axis.
     * @param n_joint number of joints specified in the list
     * @param joints list of joints controlled
     * @param refs specifies the new reference points
     * @return true/false on success/failure
     */
    bool positionMove(const int n_joint, const int *joints, const double *refs) override {
        return set1V1I1IA1DA(VOCAB_POSITION_MOVE_GROUP, n_joint, joints, refs);
    }

    /**
     * Set new reference point for all axes.
     * @param refs array, new reference points
     * @return true/false on success/failure
     */
    bool positionMove(const double *refs) override {
        return set1VDA(VOCAB_POSITION_MOVES, refs);
    }

    /** Get the last position reference for the specified axis.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    bool getTargetPosition(const int joint, double *ref) override
    {
        return get1V1I1D(VOCAB_POSITION_MOVE, joint, ref);
    }

    /** Get the last position reference for all axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    bool getTargetPositions(double *refs) override
    {
        return get1V1DA(VOCAB_POSITION_MOVES, refs);
    }

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override
    {
        return get1V1I1IA1DA(VOCAB_POSITION_MOVE_GROUP, n_joint, joints, refs);
    }

    /**
     * Set relative position. The command is relative to the
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    bool relativeMove(int j, double delta) override {
        return set1V1I1D(VOCAB_RELATIVE_MOVE, j, delta);
    }

    /** Set relative position for a subset of joints.
     * @param n_joint number of joints specified in the list
     * @param joints pointer to the array of joint numbers
     * @param refs pointer to the array of relative commands
     * @return true/false on success/failure
     */
    bool relativeMove(const int n_joint, const int *joints, const double *refs) override {
        return set1V1I1IA1DA(VOCAB_RELATIVE_MOVE_GROUP, n_joint, joints, refs);
    }

    /**
     * Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    bool relativeMove(const double *deltas) override {
        return set1VDA(VOCAB_RELATIVE_MOVES, deltas);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param j joint id
     * @param flag true/false if trajectory is terminated or not.
     * @return true on success/failure.
     */
    bool checkMotionDone(int j, bool *flag) override {
        return get1V1I1B(VOCAB_MOTION_DONE, j, *flag);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true/false if network communication went well.
     */
    bool checkMotionDone(const int n_joint, const int *joints, bool *flag) override {
        return get1V1I1IA1B(VOCAB_MOTION_DONE_GROUP, n_joint, joints, *flag);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true on success/failure.
     */
    bool checkMotionDone(bool *flag) override {
        return get1V1B(VOCAB_MOTION_DONES, *flag);
    }

    /**
     * Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    bool setRefSpeed(int j, double sp) override {
        return set1V1I1D(VOCAB_REF_SPEED, j, sp);
    }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override {
        return set1V1I1IA1DA(VOCAB_REF_SPEED_GROUP, n_joint, joints, spds);
    }

    /**
     * Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    bool setRefSpeeds(const double *spds) override {
        return set1VDA(VOCAB_REF_SPEEDS, spds);
    }

    /**
     * Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    bool setRefAcceleration(int j, double acc) override {
        return set1V1I1D(VOCAB_REF_ACCELERATION, j, acc);
    }

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override {
        return set1V1I1IA1DA(VOCAB_REF_ACCELERATION_GROUP, n_joint, joints, accs);
    }

    /**
     * Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    bool setRefAccelerations(const double *accs) override {
        return set1VDA(VOCAB_REF_ACCELERATIONS, accs);
    }

    /**
     * Get reference speed for a joint. Returns the speed used to
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    bool getRefSpeed(int j, double *ref) override {
        return get1V1I1D(VOCAB_REF_SPEED, j, ref);
    }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override {
        return get1V1I1IA1DA(VOCAB_REF_SPEED_GROUP, n_joint, joints, spds);
    }

    /**
     * Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    bool getRefSpeeds(double *spds) override {
        return get1VDA(VOCAB_REF_SPEEDS, spds);
    }

    /**
     * Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    bool getRefAcceleration(int j, double *acc) override {
        return get1V1I1D(VOCAB_REF_ACCELERATION, j, acc);
    }

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override {
        return get1V1I1IA1DA(VOCAB_REF_ACCELERATION_GROUP, n_joint, joints, accs);
    }

    /**
     * Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    bool getRefAccelerations(double *accs) override {
        return get1VDA(VOCAB_REF_ACCELERATIONS, accs);
    }

    /**
     * Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    bool stop(int j) override {
        return set1V1I(VOCAB_STOP, j);
    }

    /** Stop motion for subset of joints
     * @param len size of joint list
     * @param val1 list of joints
     * @return true/false on success/failure
     */
    bool stop(const int len, const int *val1) override
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_STOP_GROUP);
        cmd.addInt32(len);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < len; i++)
            l1.addInt32(val1[i]);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Stop motion, multiple joints
     * @return true/false on success/failure
     */
    bool stop() override {
        return set1V(VOCAB_STOPS);
    }

    /* IVelocityControl */
    /**
     * Set new reference speed for a single axis.
     * @param j joint number
     * @param v specifies the new ref speed
     * @return true/false on success/failure
     */
    bool velocityMove(int j, double v) override {
     //   return set1V1I1D(VOCAB_VELOCITY_MOVE, j, v);
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVE);
        c.head.addInt32(j);
        c.body.resize(1);
        memcpy(&(c.body[0]), &v, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    /**
     * Set a new reference speed for all axes.
     * @param v is a vector of double representing the requested speed.
     * @return true/false on success/failure.
     */
    bool velocityMove(const double *v) override {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVES);
        c.body.resize(nj);
        memcpy(&(c.body[0]), v, sizeof(double)*nj);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    /* IAmplifierControl */

    /**
     * Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    bool enableAmp(int j) override
    { return set1V1I(VOCAB_AMP_ENABLE, j); }

    /**
     * Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */

    bool disableAmp(int j) override {
        return set1V1I(VOCAB_AMP_DISABLE, j);
    }

    /**
     * Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    bool getAmpStatus(int *st) override {
        return get1VIA(VOCAB_AMP_STATUS, st);
    }

    /* Get the status of a single amplifier, coded in a 32 bits integer
     * @param m motor number
     * @param st storage for return value
     * @return true/false success failure.
     */
    bool getAmpStatus(int j, int *st) override
    {
        return get1V1I1I(VOCAB_AMP_STATUS_SINGLE, j, st);
    }

    /**
     * Set the maximum electric current going to a given motor. The behavior
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param m motor number
     * @param v the new value
     * @return probably true, might return false in bad time
     */
    bool setMaxCurrent(int j, double v) override {
        return set1V1I1D(VOCAB_AMP_MAXCURRENT, j, v);
    }

    /**
    * Returns the maximum electric current allowed for a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
     * @param m motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    bool getMaxCurrent(int j, double *v) override {
        return get1V1I1D(VOCAB_AMP_MAXCURRENT, j, v);
    }

    /* Get the the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its datasheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    bool getNominalCurrent(int m, double *val) override
    {
        return get1V1I1D(VOCAB_AMP_NOMINAL_CURRENT, m, val);
    }

    /* Set the the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its datasheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param m motor number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    bool setNominalCurrent(int m, const double val) override
    {
        return set1V1I1D(VOCAB_AMP_NOMINAL_CURRENT, m, val);
    }

    /* Get the the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    bool getPeakCurrent(int m, double *val) override
    {
        return get1V1I1D(VOCAB_AMP_PEAK_CURRENT, m, val);
    }


    /* Set the the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    bool setPeakCurrent(int m, const double val) override
    {
        return set1V1I1D(VOCAB_AMP_PEAK_CURRENT, m, val);
    }

    /* Get the the current PWM value used to control the motor.
     * @param m motor number
     * @param val filled with PWM value [DutyCycle]
     * @return true/false success failure.
     */
    bool getPWM(int m, double* val) override
    {
        double localArrivalTime = 0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(m, VOCAB_PWMCONTROL_PWM_OUTPUT, val, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /* Get the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    bool getPWMLimit(int m, double* val) override
    {
        return get1V1I1D(VOCAB_AMP_PWM_LIMIT, m, val);
    }

    /* Set the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val new value for the PWM limit.
     * @return true/false success failure.
     */
    bool setPWMLimit(int m, const double val) override
    {
        return set1V1I1D(VOCAB_AMP_PWM_LIMIT, m, val);
    }

    /* Get the power source voltage for the given motor in Volt.
     * @param m motor number
     * @param val filled with return value.
     * @return true/false success failure.
     */
    bool getPowerSupplyVoltage(int m, double* val) override
    {
        return get1V1I1D(VOCAB_AMP_VOLTAGE_SUPPLY, m, val);
    }

    /* IControlLimits */

    /**
     * Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    bool setLimits(int axis, double min, double max) override {
        return set1V1I2D(VOCAB_LIMITS, axis, min, max);
    }

    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false if something bad happens
     */
    bool getLimits(int axis, double *min, double *max) override {
        return get1V1I2D(VOCAB_LIMITS, axis, min, max);
    }

    bool setVelLimits(int axis, double min, double max) override
    {
        return set1V1I2D(VOCAB_VEL_LIMITS, axis, min, max);
    }

    bool getVelLimits(int axis, double *min, double *max) override
    {
        return get1V1I2D(VOCAB_VEL_LIMITS, axis, min, max);
    }

    /* IAxisInfo */
    bool getAxisName(int j, std::string& name) override {
        return get1V1I1S(VOCAB_INFO_NAME, j, name);
    }

    bool getJointType(int j, yarp::dev::JointTypeEnum& type) override {
        return get1V1I1I(VOCAB_INFO_TYPE, j, (int*)&type);
    }

    /* IControlCalibration */
    bool calibrateRobot() override
    { return send1V(VOCAB_CALIBRATE); }

    bool abortCalibration() override
    { return send1V(VOCAB_ABORTCALIB); }

    bool abortPark() override
    { return send1V(VOCAB_ABORTPARK); }

    bool park(bool wait=true) override
    { return send1V(VOCAB_PARK); }

    bool calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_CALIBRATE_JOINT);
        cmd.addInt32(j);
        cmd.addInt32(ui);
        cmd.addFloat64(v1);
        cmd.addFloat64(v2);
        cmd.addFloat64(v3);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool setCalibrationParameters(int j, const CalibrationParameters& params) override
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_CALIBRATE_JOINT_PARAMS);
        cmd.addInt32(j);
        cmd.addInt32(params.type);
        cmd.addFloat64(params.param1);
        cmd.addFloat64(params.param2);
        cmd.addFloat64(params.param3);
        cmd.addFloat64(params.param4);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool calibrationDone(int j) override
    { return send1V1I(VOCAB_CALIBRATE_DONE, j); }

    bool getRefTorque(int j, double *t) override
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_REF, j, t); }

    bool getRefTorques(double *t) override
    { return get2V1DA(VOCAB_TORQUE, VOCAB_REFS, t); }

    bool setRefTorques(const double *t) override
    {
        //Now we use streaming instead of rpc
        //return set2V1DA(VOCAB_TORQUE, VOCAB_REFS, t);
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_TORQUES_DIRECTS);

        c.body.resize(nj);

        memcpy(c.body.data(), t, sizeof(double) * nj);

        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool setRefTorque(int j, double v) override
    {
        //return set2V1I1D(VOCAB_TORQUE, VOCAB_REF, j, v);
        // use the streaming port!
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        // in streaming port only SET command can be sent, so it is implicit
        c.head.addVocab(VOCAB_TORQUES_DIRECT);
        c.head.addInt32(j);

        c.body.clear();
        c.body.resize(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool setRefTorques(const int n_joint, const int *joints, const double *t) override
    {
        //return set2V1I1D(VOCAB_TORQUE, VOCAB_REF, j, v);
        // use the streaming port!
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        // in streaming port only SET command can be sent, so it is implicit
        c.head.addVocab(VOCAB_TORQUES_DIRECT_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++)
            jointList.addInt32(joints[i]);
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), t, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool setMotorTorqueParams(int j, const MotorTorqueParameters params) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_TORQUE);
        cmd.addVocab(VOCAB_MOTOR_PARAMS);
        cmd.addInt32(j);
        Bottle& b = cmd.addList();
        b.addFloat64(params.bemf);
        b.addFloat64(params.bemf_scale);
        b.addFloat64(params.ktau);
        b.addFloat64(params.ktau_scale);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getMotorTorqueParams(int j, MotorTorqueParameters *params) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_TORQUE);
        cmd.addVocab(VOCAB_MOTOR_PARAMS);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            if (l.size() != 4)
            {
                yError("getMotorTorqueParams return value not understood, size!=4");
                return false;
            }
            params->bemf        = l.get(0).asFloat64();
            params->bemf_scale  = l.get(1).asFloat64();
            params->ktau        = l.get(2).asFloat64();
            params->ktau_scale  = l.get(3).asFloat64();
            return true;
        }
        return false;
    }

    bool getTorque(int j, double *t) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_TRQ, t, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    bool getTorques(double *t) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_TRQS, t, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    bool getTorqueRange(int j, double *min, double* max) override
    { return get2V1I2D(VOCAB_TORQUE, VOCAB_RANGE, j, min, max); }

    bool getTorqueRanges(double *min, double *max) override
    { return get2V2DA(VOCAB_TORQUE, VOCAB_RANGES, min, max); }

    bool getImpedance(int j, double *stiffness, double *damping) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_PARAM);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            *stiffness = l.get(0).asFloat64();
            *damping   = l.get(1).asFloat64();
            return true;
        }
        return false;
    }

    bool getImpedanceOffset(int j, double *offset) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_OFFSET);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            *offset    = l.get(0).asFloat64();
            return true;
        }
        return false;
    }

    bool setImpedance(int j, double stiffness, double damping) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_PARAM);
        cmd.addInt32(j);

        Bottle& b = cmd.addList();
        b.addFloat64(stiffness);
        b.addFloat64(damping);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setImpedanceOffset(int j, double offset) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_OFFSET);
        cmd.addInt32(j);

        Bottle& b = cmd.addList();
        b.addFloat64(offset);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_LIMITS);
        cmd.addInt32(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle* lp = response.get(2).asList();
            if (lp == nullptr)
                return false;
            Bottle& l = *lp;
            *min_stiff    = l.get(0).asFloat64();
            *max_stiff    = l.get(1).asFloat64();
            *min_damp     = l.get(2).asFloat64();
            *max_damp     = l.get(3).asFloat64();
            return true;
        }
        return false;
    }

    // IControlMode
    bool getControlMode(int j, int *mode) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_CM_CONTROL_MODE, mode, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    // IControlMode2
    bool getControlModes(const int n_joint, const int *joints, int *modes) override
    {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_CM_CONTROL_MODES, last_wholePart.controlMode.data(), lastStamp, localArrivalTime);
        if(ret)
        {
            for (int i = 0; i < n_joint; i++)
                modes[i] = last_wholePart.controlMode[joints[i]];
        }
        else
            ret = false;

        extendedPortMutex.unlock();
        return ret;
    }

    // IControlMode
    bool getControlModes(int *modes) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_CM_CONTROL_MODES, modes, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    // IControlMode2
    bool setControlMode(const int j, const int mode) override
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODE);
        cmd.addInt32(j);
        cmd.addVocab(mode);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setControlModes(const int n_joint, const int *joints, int *modes) override
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODE_GROUP);
        cmd.addInt32(n_joint);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < n_joint; i++)
            l1.addInt32(joints[i]);

        Bottle& l2 = cmd.addList();
        for (i = 0; i < n_joint; i++)
            l2.addVocab(modes[i]);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setControlModes(int *modes) override
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODES);

        int i;
        Bottle& l2 = cmd.addList();
        for (i = 0; i < nj; i++)
            l2.addVocab(modes[i]);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    //
    // IPositionDirect Interface
    //
    bool setPosition(int j, double ref) override
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_POSITION_DIRECT);
        c.head.addInt32(j);
        c.body.resize(1);
        memcpy(&(c.body[0]), &ref, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool setPositions(const int n_joint, const int *joints, const double *refs) override
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_POSITION_DIRECT_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++)
            jointList.addInt32(joints[i]);
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), refs, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool setPositions(const double *refs) override
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_POSITION_DIRECTS);
        c.body.resize(nj);
        memcpy(&(c.body[0]), refs, sizeof(double)*nj);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool getRefPosition(const int joint, double* ref) override
    {
        return get1V1I1D(VOCAB_POSITION_DIRECT, joint, ref);
    }

    bool getRefPositions(double* refs) override
    {
        return get1V1DA(VOCAB_POSITION_DIRECTS, refs);
    }

    bool getRefPositions(const int n_joint, const int* joints, double* refs) override
    {
        return get1V1I1IA1DA(VOCAB_POSITION_DIRECT_GROUP, n_joint, joints, refs);
    }

    //
    // IVelocityControl2 Interface
    //
    bool velocityMove(const int n_joint, const int *joints, const double *spds) override
    {
        // streaming port
        if (!isLive())
            return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVE_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++)
            jointList.addInt32(joints[i]);
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), spds, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool getRefVelocity(const int joint, double* vel) override
    {
        return get1V1I1D(VOCAB_VELOCITY_MOVE, joint, vel);
    }

    bool getRefVelocities(double* vels) override
    {
        return get1VDA(VOCAB_VELOCITY_MOVES, vels);
    }

    bool getRefVelocities(const int n_joint, const int* joints, double* vels) override
    {
        return get1V1I1IA1DA(VOCAB_VELOCITY_MOVE_GROUP, n_joint, joints, vels);
    }

    // Interaction Mode interface
    bool getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(axis, VOCAB_INTERACTION_MODE, (int*) mode, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override
    {
        double localArrivalTime=0.0;

        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_CM_CONTROL_MODES, last_wholePart.interactionMode.data(), lastStamp, localArrivalTime);
        if(ret)
        {
            for (int i = 0; i < n_joints; i++)
                modes[i] = (yarp::dev::InteractionModeEnum)last_wholePart.interactionMode[joints[i]];
        }
        else
            ret = false;

        extendedPortMutex.unlock();
        return ret;
    }

    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_INTERACTION_MODES, (int*) modes, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    bool setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode) override
    {
        Bottle cmd, response;
        if (!isLive()) return false;

        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
        cmd.addVocab(VOCAB_INTERACTION_MODE);
        cmd.addInt32(axis);
        cmd.addVocab(mode);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override
    {
        Bottle cmd, response;
        if (!isLive()) return false;

        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
        cmd.addVocab(VOCAB_INTERACTION_MODE_GROUP);
        cmd.addInt32(n_joints);

        Bottle& l1 = cmd.addList();
        for (int i = 0; i < n_joints; i++)
            l1.addInt32(joints[i]);

        Bottle& l2 = cmd.addList();
        for (int i = 0; i < n_joints; i++)
        {
            l2.addVocab(modes[i]);
        }
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes) override
    {
        Bottle cmd, response;
        if (!isLive()) return false;

        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
        cmd.addVocab(VOCAB_INTERACTION_MODES);

        Bottle& l1 = cmd.addList();
        for (int i = 0; i < nj; i++)
            l1.addVocab(modes[i]);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool checkProtocolVersion(bool ignore)
    {
        bool error=false;
        // verify protocol
        Bottle cmd, reply;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PROTOCOL_VERSION);
        rpc_p.write(cmd, reply);

        // check size and format of messages, expected [prot] int int int [ok]
        if (reply.size()!=5)
           error=true;

        if (reply.get(0).asVocab()!=VOCAB_PROTOCOL_VERSION)
           error=true;

        if (!error)
        {
            protocolVersion.major=reply.get(1).asInt32();
            protocolVersion.minor=reply.get(2).asInt32();
            protocolVersion.tweak=reply.get(3).asInt32();

            //verify protocol
            if (protocolVersion.major!=PROTOCOL_VERSION_MAJOR)
                error=true;

            if (protocolVersion.minor!=PROTOCOL_VERSION_MINOR)
                error=true;
        }

        if (!error)
            return true;

        // protocol did not match
        yError("expecting protocol %d %d %d, but the device we are connecting to has protocol version %d %d %d\n",
                        PROTOCOL_VERSION_MAJOR, PROTOCOL_VERSION_MINOR, PROTOCOL_VERSION_TWEAK,
                        protocolVersion.major, protocolVersion.minor, protocolVersion.tweak);

        bool ret;
        if (ignore)
        {
            yWarning(" ignoring error but please update YARP or the remotecontrolboard implementation\n");
            ret = true;
        }
        else
        {
            yError(" please update YARP or the remotecontrolboard implementation\n");
            ret = false;
        }

        return ret;
    }

    /* IRemoteCalibrator interface */

    /**
     * @brief isCalibratorDevicePresent: check if a calibrator device has been set
     * @return true if a valid calibrator device has been found
     */
    bool isCalibratorDevicePresent(bool *isCalib) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_IS_CALIBRATOR_PRESENT);
        bool ok = rpc_p.write(cmd, response);
        if(ok)
        {
            *isCalib = response.get(2).asInt32()!=0;
        }
        else
        {
            *isCalib = false;
        }
        return CHECK_FAIL(ok, response);
    }

    /**
     * @brief calibrateSingleJoint: call the calibration procedure for the single joint
     * @param j: joint to be calibrated
     * @return true if calibration was successful
     */
    bool calibrateSingleJoint(int j) override
    {
        return set2V1I(VOCAB_REMOTE_CALIBRATOR_INTERFACE, VOCAB_CALIBRATE_SINGLE_JOINT, j);
    }

    /**
     * @brief calibrateWholePart: call the procedure for calibrating the whole device
     * @return true if calibration was successful
     */
    bool calibrateWholePart() override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_CALIBRATE_WHOLE_PART);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * @brief homingSingleJoint: call the homing procedure for a single joint
     * @param j: joint to be calibrated
     * @return true if homing was successful, false otherwise
     */
    bool homingSingleJoint(int j) override
    {
        return set2V1I(VOCAB_REMOTE_CALIBRATOR_INTERFACE, VOCAB_HOMING_SINGLE_JOINT, j);
    }

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was successful, false otherwise
     */
    bool homingWholePart() override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_HOMING_WHOLE_PART);
        bool ok = rpc_p.write(cmd, response);
        yDebug() << "Sent homing whole part message";
        return CHECK_FAIL(ok, response);
    }

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if successful
     */
    bool parkSingleJoint(int j, bool _wait=true) override
    {
        return set2V1I(VOCAB_REMOTE_CALIBRATOR_INTERFACE, VOCAB_PARK_SINGLE_JOINT, j);
    }

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if successful
     */
    bool parkWholePart() override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_PARK_WHOLE_PART);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * @brief quitCalibrate: interrupt the calibration procedure
     * @return true if successful
     */
    bool quitCalibrate() override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_QUIT_CALIBRATE);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * @brief quitPark: interrupt the park procedure
     * @return true if successful
     */
    bool quitPark() override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_QUIT_PARK);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

//    virtual bool getAxes(int *ax)
//    {
//    }

    bool getRefCurrents(double *t) override
    {
        return get2V1DA(VOCAB_CURRENTCONTROL_INTERFACE, VOCAB_CURRENT_REFS, t);
    }

    bool getRefCurrent(int j, double *t) override
    {
        return get2V1I1D(VOCAB_CURRENTCONTROL_INTERFACE, VOCAB_CURRENT_REF, j, t);
    }

    bool setRefCurrents(const double *refs) override
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_CURRENTCONTROL_INTERFACE);
        c.head.addVocab(VOCAB_CURRENT_REFS);
        c.body.resize(nj);
        memcpy(&(c.body[0]), refs, sizeof(double)*nj);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool setRefCurrent(int j, double ref) override
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_CURRENTCONTROL_INTERFACE);
        c.head.addVocab(VOCAB_CURRENT_REF);
        c.head.addInt32(j);
        c.body.resize(1);
        memcpy(&(c.body[0]), &ref, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool setRefCurrents(const int n_joint, const int *joints, const double *refs) override
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_CURRENTCONTROL_INTERFACE);
        c.head.addVocab(VOCAB_CURRENT_REF_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++)
            jointList.addInt32(joints[i]);
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), refs, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    /**
     * Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    bool getCurrents(double *vals) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_AMP_CURRENTS, vals, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    /**
     * Read the electric current going to a given motor.
     * @param m motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad time
     */
    bool getCurrent(int j, double *val) override
    {
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_AMP_CURRENT, val, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    bool getCurrentRange(int j, double *min, double *max) override
    {
        return get2V1I2D(VOCAB_CURRENTCONTROL_INTERFACE, VOCAB_CURRENT_RANGE, j, min, max);
    }

    bool getCurrentRanges(double *min, double *max) override
    {
        return get2V2DA(VOCAB_CURRENTCONTROL_INTERFACE, VOCAB_CURRENT_RANGES, min, max);
    }

    //iPWMControl
    bool setRefDutyCycle(int j, double v) override
    {
        // using the streaming port
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        // in streaming port only SET command can be sent, so it is implicit
        c.head.addVocab(VOCAB_PWMCONTROL_INTERFACE);
        c.head.addVocab(VOCAB_PWMCONTROL_REF_PWM);
        c.head.addInt32(j);

        c.body.clear();
        c.body.resize(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool setRefDutyCycles(const double *v) override
    {
        // using the streaming port
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_PWMCONTROL_INTERFACE);
        c.head.addVocab(VOCAB_PWMCONTROL_REF_PWMS);

        c.body.resize(nj);

        memcpy(&(c.body[0]), v, sizeof(double)*nj);

        command_buffer.write(writeStrict_moreJoints);

        return true;
    }

    bool getRefDutyCycle(int j, double *ref) override
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PWMCONTROL_INTERFACE);
        cmd.addVocab(VOCAB_PWMCONTROL_REF_PWM);
        cmd.addInt32(j);
        response.clear();

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response))
        {
            // ok
            *ref = response.get(2).asFloat64();

            getTimeStamp(response, lastStamp);
            return true;
        }
        else
            return false;
    }

    bool getRefDutyCycles(double *refs) override
    {
        return get2V1DA(VOCAB_PWMCONTROL_INTERFACE, VOCAB_PWMCONTROL_REF_PWMS, refs);
    }

    bool getDutyCycle(int j, double *out) override
    {
        double localArrivalTime = 0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_PWMCONTROL_PWM_OUTPUT, out, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

    bool getDutyCycles(double *outs) override
    {
        double localArrivalTime = 0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_PWMCONTROL_PWM_OUTPUTS, outs, lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret;
    }

};


// implementation of CommandsHelper

yarp::dev::DriverCreator *createRemoteControlBoard() {
    return new DriverCreatorOf<RemoteControlBoard>("remote_controlboard",
        "controlboardwrapper2",
        "yarp::dev::RemoteControlBoard");
}
