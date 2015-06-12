// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2006 RobotCub Consortium
* Authors: Giorgio Metta, Lorenzo Natale, Paul Fitzpatrick
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <string.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/PreciselyTimed.h>

#include <stateExtendedReader.hpp>

#define PROTOCOL_VERSION_MAJOR 1
#define PROTOCOL_VERSION_MINOR 3
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

inline bool getTimeStamp(Bottle &bot, Stamp &st)
{
    if (bot.get(3).asVocab()==VOCAB_TIMESTAMP)
    {
        //yup! we have a timestamp
        int fr=bot.get(4).asInt();
        double ts=bot.get(5).asDouble();
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


// encoders should arrive at least every 0.5s to be considered valide
// getEncoders will return false otherwise.
const double TIMEOUT=0.5;

class StateInputPort:public BufferedPort<yarp::sig::Vector>
{
    yarp::sig::Vector last;
    Semaphore mutex;
    Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    bool valid;
    int count;
public:

    inline void resetStat()
    {
        mutex.wait();
        count=0;
        deltaT=0;
        deltaTMax=0;
        deltaTMin=1e22;
        now=Time::now();
        prev=now;
        mutex.post();
    }

    StateInputPort()
    {
        valid=false;
        resetStat();
    }

    virtual void onRead(yarp::sig::Vector &v)
    {
        now=Time::now();
        mutex.wait();

        if (count>0)
        {
            double tmpDT=now-prev;
            deltaT+=tmpDT;
            if (tmpDT>deltaTMax)
                deltaTMax=tmpDT;
            if (tmpDT<deltaTMin)
                deltaTMin=tmpDT;
        }

        prev=now;
        count++;

        valid=true;
        last=v;
        getEnvelope(lastStamp);
        //check that timestamp are available
        if (!lastStamp.isValid())
            lastStamp.update(now);
        mutex.post();
    }

    inline bool getLast(int j, double &v, Stamp &stamp, double &localArrivalTime)
    {
        if ((size_t) j>=last.size())
            return false;

        mutex.wait();
        bool ret=valid;
        if (ret)
        {
            v=last[j];
            stamp=lastStamp;
            localArrivalTime=now;
        }
        mutex.post();
        return ret;
    }

    inline bool getLast(yarp::sig::Vector &n, Stamp &stmp, double &localArrivalTime)
    {
        mutex.wait();
        bool ret=valid;
        if (ret)
        {
            n=last;
            stmp=lastStamp;
            localArrivalTime=now;
        }
        mutex.post();

        return ret;
    }

    inline int getIterations()
    {
        mutex.wait();
        int ret=count;
        mutex.post();
        return ret;
    }

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max)
    {
        mutex.wait();
        ite=count;
        min=deltaTMin*1000;
        max=deltaTMax*1000;
        if (count<1)
        {
            av=0;
        }
        else
        {
            av=deltaT/count;
        }
        av=av*1000;
        mutex.post();
    }

};



#include <yarp/os/RateThread.h>

using namespace yarp::os;

const int DIAGNOSTIC_THREAD_RATE=1000;

class DiagnosticThread: public RateThread
{
    StateInputPort *owner;
    ConstString ownerName;

public:
    DiagnosticThread(int r): RateThread(r)
    { owner=0; }

    void setOwner(StateInputPort *o)
    {
        owner=o;
        ownerName=owner->getName().c_str();
    }

    void run()
    {
        if (owner!=0)
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
* @ingroup dev_impl_wrapper
*
* The client side of the control board, connects to a ServerControlBoard.
*/
class yarp::dev::RemoteControlBoard :
    public IPidControl,
    public IPositionControl2,
    public IVelocityControl2,
    public IEncodersTimed,
    public IMotorEncoders,
    public IMotor,
    public IAmplifierControl,
    public IControlLimits2,
    public IAxisInfo,
    public IPreciselyTimed,
    public IControlCalibration2,
    public ITorqueControl,
    public IImpedanceControl,
//    public IControlMode,
    public IControlMode2,
    public IOpenLoopControl,
    public DeviceDriver,
    public IPositionDirect,
    public IInteractionMode,
    public IRemoteCalibrator,
    public IRemoteVariables
{

#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
    Port rpc_p;
    Port command_p;
    DiagnosticThread *diagnosticThread;

    PortReaderBuffer<yarp::sig::Vector> state_buffer;
    StateInputPort state_p;
    PortWriterBuffer<CommandMessage> command_buffer;
    bool writeStrict_singleJoint;
    bool writeStrict_moreJoints;

    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
//  yarp::os::PortReaderBuffer<jointData>           extendedInputState_buffer;  // Buffer storing new data
    StateExtendedInputPort                          extendedIntputStatePort;  // Buffered port storing new data
    Semaphore extendedPortMutex;
    jointData last_singleJoint;     // tmp to store last received data for a particular joint
//    yarp::os::Port extendedIntputStatePort;         // Port /stateExt:i reading the state of the joints
    jointData last_wholePart;         // tmp to store last received data for whole part

    bool controlBoardWrapper1_compatibility;
    ConstString remote;
    ConstString local;
    mutable Stamp lastStamp;  //this is shared among all calls that read encoders
    // Semaphore mutex;
    int nj;
    bool njIsKnown;

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
        cmd.addInt(axis);
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
        cmd.addInt(axis);
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
        cmd.addInt(j);
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
        cmd.addDouble(v);

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
        cmd.addInt(v);

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
            v = response.get(2).asDouble();

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
            v = response.get(2).asInt();

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
        cmd.addInt(j);
        cmd.addDouble(val);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set1V1I2D(int code, int j, double val1, double val2)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt(j);
        cmd.addDouble(val1);
        cmd.addDouble(val2);

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
            l.addDouble(val[i]);
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
            l.addDouble(val[i]);
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
            l1.addDouble(val1[i]);
        Bottle& l2 = cmd.addList();
        for (i = 0; i < nj; i++)
            l2.addDouble(val2[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set1V1I1IA1DA(int v, const int len, const int *val1, const double *val2) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v);
        cmd.addInt(len);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < len; i++)
            l1.addInt(val1[i]);
        Bottle& l2 = cmd.addList();
        for (i = 0; i < len; i++)
            l2.addDouble(val2[i]);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set2V1I1D(int v1, int v2, int axis, double val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt(axis);
        cmd.addDouble(val);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool set2V1I(int v1, int v2, int axis) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(v1);
        cmd.addVocab(v2);
        cmd.addInt(axis);
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
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asDouble();

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
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asInt();

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
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            // ok
            *val = response.get(2).asDouble();

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
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            // ok
            *val1 = response.get(2).asDouble();
            *val2 = response.get(3).asDouble();

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
        cmd.addInt(axis);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            *v1 = response.get(2).asDouble();
            *v2 = response.get(3).asDouble();
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
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            val = (response.get(2).asInt()!=0);
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get1V1I1IA1B(int v,  const int len, const int *val1, bool &retVal ) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        cmd.addInt(len);
        Bottle& l1 = cmd.addList();
        for (int i = 0; i < len; i++)
            l1.addInt(val1[i]);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            retVal = (response.get(2).asInt()!=0);
            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

    bool get1V1B(int v, bool &val) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(v);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            val = (response.get(2).asInt()!=0);
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
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;

            int njs = l.size();
            YARP_ASSERT (nj == njs);
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asInt();

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
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;

            int njs = l.size();
            YARP_ASSERT (nj == njs);
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asDouble();

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
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;

            int njs = l.size();
            YARP_ASSERT (nj == njs);
            for (i = 0; i < nj; i++)
                val[i] = l.get(i).asDouble();

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
            Bottle& l1 = *(response.get(2).asList());
            if (&l1 == 0)
                return false;
            Bottle& l2 = *(response.get(3).asList());
            if (&l2 == 0)
                return false;

            int nj1 = l1.size();
            int nj2 = l2.size();
           // ACE_ASSERT (nj == nj1);
           // ACE_ASSERT (nj == nj2);

            for (i = 0; i < nj1; i++)
                val1[i] = l1.get(i).asDouble();
            for (i = 0; i < nj2; i++)
                val2[i] = l2.get(i).asDouble();

            getTimeStamp(response, lastStamp);

            return true;
        }
        return false;
    }

    bool get1V1I1S(int code, int j, yarp::os::ConstString &name)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        cmd.addInt(j);
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
        cmd.addInt(len);
        Bottle &l1 = cmd.addList();
        for(int i = 0; i < len; i++)
            l1.addInt(val1[i]);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle& l2 = *(response.get(2).asList());
            if (&l2 == 0)
                return false;

            int nj2 = l2.size();
            if(nj2 != len)
            {
                yError("received an answer with an unexpected number of entries!\n");
                return false;
            }
            for (i = 0; i < nj2; i++)
                val2[i] = l2.get(i).asDouble();

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
    RemoteControlBoard() {
        nj = 0;
        njIsKnown = false;
        writeStrict_singleJoint = true;
        writeStrict_moreJoints  = false;
        controlBoardWrapper1_compatibility = false;
    }

    /**
     * Destructor.
     */
    virtual ~RemoteControlBoard() {
    }


    /**
     * Default open.
     * @return always true.
     */
    virtual bool open() {
        return true;
    }

    virtual bool open(Searchable& config) {
        remote = config.find("remote").asString().c_str();
        local = config.find("local").asString().c_str();

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

        ConstString carrier =
            config.check("carrier",
            Value("udp"),
            "default carrier for streaming robot state").asString().c_str();

        bool portProblem = false;
        if (local != "") {
            ConstString s1 = local;
            s1 += "/rpc:o";
            if (!rpc_p.open(s1.c_str())) { portProblem = true; }
            s1 = local;
            s1 += "/command:o";
            if (!command_p.open(s1.c_str())) { portProblem = true; }
            s1 = local;
            s1 += "/state:i";
            if (!state_p.open(s1.c_str())) { portProblem = true; }

            s1 = local;
            s1 += "/stateExt:i";
            if (!extendedIntputStatePort.open(s1.c_str())) { portProblem = true; }
            //new code
            if (!portProblem)
            {
                state_p.useCallback();
                extendedIntputStatePort.useCallback();
            }
        }

        bool connectionProblem = false;
        if (remote != "" && !portProblem)
        {
            ConstString s1 = remote;
            s1 += "/rpc:i";
            ConstString s2 = local;
            s2 += "/rpc:o";
            bool ok = false;
            // RPC port needs to be tcp, therefore no carrier option is added here
            // ok=Network::connect(s2.c_str(), s1.c_str());         //This doesn't take into consideration possible YARP_PORT_PREFIX on local ports
            // ok=Network::connect(rpc_p.getName(), s1.c_str());    //This should work also with YARP_PORT_PREFIX because getting back the name of the port will return the modified name
            ok=rpc_p.addOutput(s1.c_str());                         //This works because we are manipulating only remote side and let yarp handle the local side
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
            ok = command_p.addOutput(s1.c_str(), carrier);
            if (!ok) {
                yError("Problem connecting to %s, is the remote device available?\n", s1.c_str());
                connectionProblem = true;
            }

            s1 = remote;
            s1 += "/state:o";
            s2 = local;
            s2 += "/state:i";
            ok = Network::connect(s1, state_p.getName(), carrier);

            if (!ok) {
                yError("Problem connecting to %s from %s, is the remote device available?\n", s1.c_str(), state_p.getName().c_str());
                connectionProblem = true;
            }

            s1 = remote;
            s1 += "/stateExt:o";
            s2 = local;
            s2 += "/stateExt:i";
            // not checking return value for now since it is wip (different machines can have different compilation flags
            ok = Network::connect(s1, extendedIntputStatePort.getName(), carrier);
            if (ok)
            {
                controlBoardWrapper1_compatibility = false;
            }
            else
            {
                yError("*** Extended port %s was not found on the controlBoardWrapper I'm connecting to. Falling back to compatibility behaviour\n", s1.c_str());
                yWarning("Updating to newer yarp and the usage of controlBoardWrapper2 is suggested***\n");
                //connectionProblem = true;     // for compatibility
                controlBoardWrapper1_compatibility = true;
            }
        }

        if (connectionProblem||portProblem) {

            rpc_p.close();
            command_p.close();
            state_p.close();
            extendedIntputStatePort.close();
            return false;
        }

        state_buffer.setStrict(false);
        command_buffer.attach(command_p);

        if (!checkProtocolVersion(config.check("ignoreProtocolCheck")))
        {
            std::cout << "checkProtocolVersion failed" << std::endl;
            command_buffer.detach();
            rpc_p.close();
            command_p.close();
            state_p.close();
            extendedIntputStatePort.close();
            return false;
        }

        if (!isLive()) {
            if (remote!="") {
                yError("Problems with obtaining the number of controlled axes\n");
                command_buffer.detach();
                rpc_p.close();
                command_p.close();
                state_p.close();
                extendedIntputStatePort.close();
                return false;
            }
        }

        if (config.check("diagnostic"))
        {
            diagnosticThread = new DiagnosticThread(DIAGNOSTIC_THREAD_RATE);
            diagnosticThread->setOwner(&state_p);
            diagnosticThread->start();
        }
        else
            diagnosticThread=0;

        // allocate memory for helper struct
        // single joint
        last_singleJoint.jointPosition.resize(1);
        last_singleJoint.jointVelocity.resize(1);
        last_singleJoint.jointAcceleration.resize(1);
        last_singleJoint.motorPosition.resize(1);
        last_singleJoint.motorVelocity.resize(1);
        last_singleJoint.motorAcceleration.resize(1);
        last_singleJoint.torque.resize(1);
        last_singleJoint.pidOutput.resize(1);
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
        last_wholePart.pidOutput.resize(nj);
        last_wholePart.controlMode.resize(nj);
        last_wholePart.interactionMode.resize(nj);
        return true;
    }

    /**
     * Close the device driver and stop the port connections.
     * @return true/false on success/failure.
     */
    virtual bool close() {

        if (diagnosticThread!=0)
        {
            diagnosticThread->stop();
            delete diagnosticThread;
        }

        rpc_p.close();
        command_p.close();
        state_p.close();
        extendedIntputStatePort.close();
        return true;
    }

    /**
     * Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */

    virtual bool setPid(int j, const Pid &pid) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PID);
        cmd.addInt(j);
        Bottle& l = cmd.addList();
        l.addDouble(pid.kp);
        l.addDouble(pid.kd);
        l.addDouble(pid.ki);
        l.addDouble(pid.max_int);
        l.addDouble(pid.max_output);
        l.addDouble(pid.offset);
        l.addDouble(pid.scale);
        l.addDouble(pid.stiction_up_val);
        l.addDouble(pid.stiction_down_val);
        l.addDouble(pid.kff);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setPids(const Pid *pids) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_PIDS);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++) {
            Bottle& m = l.addList();
            m.addDouble(pids[i].kp);
            m.addDouble(pids[i].kd);
            m.addDouble(pids[i].ki);
            m.addDouble(pids[i].max_int);
            m.addDouble(pids[i].max_output);
            m.addDouble(pids[i].offset);
            m.addDouble(pids[i].scale);
            m.addDouble(pids[i].stiction_up_val);
            m.addDouble(pids[i].stiction_down_val);
            m.addDouble(pids[i].kff);
        }

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }


    /**
     * Set the controller reference point for a given axis.
     * Warning this method can result in very large torques
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method.
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     */
    virtual bool setReference(int j, double ref) {
        return set1V1I1D(VOCAB_REF, j, ref);
    }


    /**
     * Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method.
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     */
    virtual bool setReferences(const double *refs) {
        return set1VDA(VOCAB_REFS, refs);
    }


    /**
     * Set the error limit for the controller on a specifi joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setErrorLimit(int j, double limit) {
        return set1V1I1D(VOCAB_LIM, j, limit);
    }

    /**
     * Get the error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setErrorLimits(const double *limits) {
        return set1VDA(VOCAB_LIMS, limits);
    }

    /**
     * Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getError(int j, double *err) {
        return get1V1I1D(VOCAB_ERR, j, err);
    }

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getErrors(double *errs) {
        return get1VDA(VOCAB_ERRS, errs);
    }

//    /**
//     * Get the output of the controller (e.g. pwm value)
//     * @param j joint number
//     * @param out pointer to storage for return value
//     * @return success/failure
//     */
//    virtual bool getOutput(int j, double *out) {
//        return get1V1I1D(VOCAB_OUTPUT, j, out);
//    }
//
//    /**
//     * Get the output of the controllers (e.g. pwm value)
//     * @param outs pinter to the vector that will store the output values
//     * @return true/false on success/failure
//     */
//    virtual bool getOutputs(double *outs) {
//        return get1VDA(VOCAB_OUTPUTS, outs);
//    }

    /**
     * Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getPid(int j, Pid *pid) {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PID);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            pid->kp = l.get(0).asDouble();
            pid->kd = l.get(1).asDouble();
            pid->ki = l.get(2).asDouble();
            pid->max_int = l.get(3).asDouble();
            pid->max_output = l.get(4).asDouble();
            pid->offset = l.get(5).asDouble();
            pid->scale = l.get(6).asDouble();
            pid->stiction_up_val = l.get(7).asDouble();
            pid->stiction_down_val = l.get(8).asDouble();
            pid->kff = l.get(9).asDouble();
            return true;
        }
        return false;
    }

    /**
     * Get current pid value for all controlled axes.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getPids(Pid *pids) {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_PIDS);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            int i;
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            const int njs = l.size();
            YARP_ASSERT (njs == nj);
            for (i = 0; i < nj; i++)
            {
                Bottle& m = *(l.get(i).asList());
                if (&m == 0)
                    return false;
                pids[i].kp = m.get(0).asDouble();
                pids[i].kd = m.get(1).asDouble();
                pids[i].ki = m.get(2).asDouble();
                pids[i].max_int = m.get(3).asDouble();
                pids[i].max_output = m.get(4).asDouble();
                pids[i].offset = m.get(5).asDouble();
                pids[i].scale = m.get(6).asDouble();
                pids[i].stiction_up_val = m.get(7).asDouble();
                pids[i].stiction_down_val = m.get(8).asDouble();
                pids[i].kff = m.get(9).asDouble();
            }
            return true;
        }
        return false;
    }

    /**
     * Get the current reference position of the controller for a specific joint.
     * @param j is joint number
     * @param ref pointer to storage for return value
     * @return true/false on success/failure
     */
    virtual bool getReference(int j, double *ref) {
        return get1V1I1D(VOCAB_REF, j, ref);
    }

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     */
    virtual bool getReferences(double *refs) {
        return get1VDA(VOCAB_REFS, refs);
    }

    /**
     * Get the error limit for the controller on a specific joint
     * @param j is the joint number
     * @param limit pointer to storage
     * @return true/false on success/failure
     */
    virtual bool getErrorLimit(int j, double *limit) {
        return get1V1I1D(VOCAB_LIM, j, limit);
    }

    /**
     * Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getErrorLimits(double *limits) {
        return get1VDA(VOCAB_LIMS, limits);
    }

    /**
     * Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     */
    virtual bool resetPid(int j) {
        return set1V1I(VOCAB_RESET, j);
    }

    /**
     * Disable the pid computation for a joint
     * @param j is the joint number
     * @return true/false on success/failure
     */
    virtual bool disablePid(int j) {
        return set1V1I(VOCAB_DISABLE, j);
    }

    /**
     * Enable the pid computation for a joint
     * @param j is the joint number
     * @return true/false on success/failure
     */
    virtual bool enablePid(int j) {
        return set1V1I(VOCAB_ENABLE, j);
    }

    /* IEncoder */

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j is the axis number
     * @return true/false on success/failure
     */
    virtual bool resetEncoder(int j) {
        return set1V1I(VOCAB_E_RESET, j);
    }

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    virtual bool resetEncoders() {
        return set1V(VOCAB_E_RESETS);
    }

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false on success/failure
     */
    virtual bool setEncoder(int j, double val) {
        return set1V1I1D(VOCAB_ENCODER, j, val);
    }

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setEncoders(const double *vals) {
        return set1VDA(VOCAB_ENCODERS, vals);
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoder(int j, double *v) {
        // return get1V1I1D(VOCAB_ENCODER, j, v);
        double localArrivalTime = 0.0;
        bool ret;

        if(controlBoardWrapper1_compatibility)
        {
            ret=state_p.getLast(j, *v, lastStamp, localArrivalTime);
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.jointPosition_isValid)
                *v = last_singleJoint.jointPosition[0];
            else
                ret = false;
        }
        if (ret && Time::now()-localArrivalTime>TIMEOUT)
            ret=false;

        return ret;
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoderTimed(int j, double *v, double *t) {
        // return get1V1I1D(VOCAB_ENCODER, j, v);
        double localArrivalTime = 0.0;
        bool ret = false;
        if(controlBoardWrapper1_compatibility)
        {
            ret=state_p.getLast(j, *v, lastStamp, localArrivalTime);
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.jointPosition_isValid)
                *v = last_singleJoint.jointPosition[0];
            else
                ret = false;
        }
        *t=lastStamp.getTime();

        if (ret && Time::now()-localArrivalTime>TIMEOUT)
            ret=false;

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
    virtual bool getEncoders(double *encs) {
        if (!isLive()) return false;
        // particular case, does not use RPC
        bool ret = false;
        double localArrivalTime=0.0;

        if(controlBoardWrapper1_compatibility)
        {
            Vector tmp(nj);

            // mutex.wait();
            ret=state_p.getLast(tmp,lastStamp,localArrivalTime);
            // mutex.post();

            if (ret)
            {
                if (tmp.size() != (size_t)nj)
                    yWarning("tmp.size: %d  nj %d\n", (int)tmp.size(), nj);

                YARP_ASSERT (tmp.size() == (size_t)nj);
                memcpy (encs, &(tmp.operator [](0)), sizeof(double)*nj);

                ////////////////////////// HANDLE TIMEOUT
                // fill the vector anyway
                if (Time::now()-localArrivalTime>TIMEOUT)
                    ret=false;
            }
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.jointPosition_isValid)
                std::copy(last_wholePart.jointPosition.begin(), last_wholePart.jointPosition.end(), encs);
            else
                ret = false;
        }
        return ret;
    }

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @param ts pointer to the array that will contain timestamps
     * @return true/false on success/failure
     */
    virtual bool getEncodersTimed(double *encs, double *ts) {
        if (!isLive()) return false;
        // particular case, does not use RPC
        double localArrivalTime=0.0;

        bool ret=false;
        if(controlBoardWrapper1_compatibility)
        {
            Vector tmp(nj);
     //       mutex.wait();
            ret=state_p.getLast(tmp,lastStamp,localArrivalTime);
     //       mutex.post();

            if (ret)
            {
                if (tmp.size() != (size_t)nj)
                    yWarning("tmp.size: %d  nj %d\n", (int)tmp.size(), nj);

                YARP_ASSERT (tmp.size() == (size_t)nj);
                for(int j=0; j<nj; j++)
                {
                    encs[j]=tmp[j];
                    ts[j]=lastStamp.getTime();
                }
            }
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.jointPosition_isValid)
            {
                std::copy(last_wholePart.jointPosition.begin(), last_wholePart.jointPosition.end(), encs);
                std::fill_n(ts, nj, lastStamp.getTime());
            }
            else
                ret = false;
        }

        ////////////////////////// HANDLE TIMEOUT
        if (Time::now()-localArrivalTime>TIMEOUT)
            ret=false;

        return ret;
    }
    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getEncoderSpeed(int j, double *sp)
    {
        if(controlBoardWrapper1_compatibility)
        {
            return get1V1I1D(VOCAB_ENCODER_SPEED, j, sp);
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.jointVelocity_isValid)
            {
                *sp = last_singleJoint.jointVelocity[0];
            }
            else
                ret = false;

            return ret;
        }
    }


    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getEncoderSpeeds(double *spds)
    {
        if(controlBoardWrapper1_compatibility)
        {
            return get1VDA(VOCAB_ENCODER_SPEEDS, spds);
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.jointVelocity_isValid)
            {
                std::copy(last_wholePart.jointVelocity.begin(), last_wholePart.jointVelocity.end(), spds);
            }
            else
                ret = false;

            return ret;
        }
    }

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param acc pointer to the array that will contain the output
     */

    virtual bool getEncoderAcceleration(int j, double *acc)
    {
        if(controlBoardWrapper1_compatibility)
        {
            return get1V1I1D(VOCAB_ENCODER_ACCELERATION, j, acc);
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.jointAcceleration_isValid)
            {
                *acc = last_singleJoint.jointAcceleration[0];
            }
            else
                ret = false;
            return ret;
        }
    }

    /**
     * Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    virtual bool getEncoderAccelerations(double *accs)
    {
        if(controlBoardWrapper1_compatibility)
        {
            return get1VDA(VOCAB_ENCODER_ACCELERATIONS, accs);
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.jointAcceleration_isValid)
            {
                std::copy(last_wholePart.jointAcceleration.begin(), last_wholePart.jointAcceleration.end(), accs);
            }
            else
                ret = false;

            return ret;
        }
    }

    /* IRemoteVariable */
    virtual bool getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val) {
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

    virtual bool setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val) {
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


    virtual bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) {
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
    virtual bool getNumberOfMotors(int *num) {
        return get1V1I(VOCAB_MOTORS_NUMBER, *num);
    }

    virtual bool getTemperature      (int m, double* val) {
        return get1V1I1D(VOCAB_TEMPERATURE, m, val);
    }

    virtual bool getTemperatures     (double *vals) {
        return get1VDA(VOCAB_TEMPERATURES, vals);
    }

    virtual bool getTemperatureLimit (int m, double* val) {
        return get1V1I1D(VOCAB_TEMPERATURE_LIMIT, m, val);
    }

    virtual bool setTemperatureLimit (int m, const double val) {
        return set1V1I1D(VOCAB_TEMPERATURE_LIMIT, m, val);
    }

    virtual bool getMotorOutputLimit (int m, double* val) {
        return get1V1I1D(VOCAB_MOTOR_OUTPUT_LIMIT, m, val);
    }

    virtual bool setMotorOutputLimit (int m, const double val) {
        return set1V1I1D(VOCAB_MOTOR_OUTPUT_LIMIT, m, val);
    }


    /* IMotorEncoder */

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j is the axis number
     * @return true/false on success/failure
     */
    virtual bool resetMotorEncoder(int j) {
        return set1V1I(VOCAB_MOTOR_E_RESET, j);
    }

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    virtual bool resetMotorEncoders() {
        return set1V(VOCAB_MOTOR_E_RESETS);
    }

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false on success/failure
     */
    virtual bool setMotorEncoder(int j, const double val) {
        return set1V1I1D(VOCAB_MOTOR_ENCODER, j, val);
    }

    /**
     * Sets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr new parameter
     * @return true/false
     */
    virtual bool setMotorEncoderCountsPerRevolution(int m, const double cpr) {
        return set1V1I1D(VOCAB_MOTOR_CPR, m, cpr);
    }

    /**
     * gets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr pointer to storage for the return value
     * @return true/false
     */ 
    virtual bool getMotorEncoderCountsPerRevolution(int m, double *cpr) {
         return get1V1I1D(VOCAB_MOTOR_CPR, m, cpr);
    }

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setMotorEncoders(const double *vals) {
        return set1VDA(VOCAB_MOTOR_ENCODERS, vals);
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getMotorEncoder(int j, double *v) {
        double localArrivalTime = 0.0;
        bool ret;
        if(controlBoardWrapper1_compatibility)
        {
             return get1V1I1D(VOCAB_MOTOR_ENCODER, j, v);
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.motorPosition_isValid)
            {
                *v = last_singleJoint.motorPosition[0];
            }
            else
                ret = false;
        }
        if (ret && Time::now()-localArrivalTime>TIMEOUT)
            ret=false;

        return ret;
    }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getMotorEncoderTimed(int j, double *v, double *t) {
        double localArrivalTime = 0.0;
        bool ret = false;
        if(controlBoardWrapper1_compatibility)
        {
             return get1V1I1D(VOCAB_MOTOR_ENCODER, j, v);
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.motorPosition_isValid)
            {
                *v = last_singleJoint.motorPosition[0];
            }
            else
                ret = false;
        }

        *t=lastStamp.getTime();

        if (ret && Time::now()-localArrivalTime>TIMEOUT)
            ret=false;

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
    virtual bool getMotorEncoders(double *encs) {
        if (!isLive()) return false;
        // particular case, does not use RPC
        bool ret = false;
        double localArrivalTime=0.0;


        if(controlBoardWrapper1_compatibility)
        {
            Vector tmp(nj);

            // mutex.wait();
            return get1VDA(VOCAB_MOTOR_ENCODERS, tmp.data());
            // mutex.post();

            if (ret)
            {
                if (tmp.size() != (size_t)nj)
                    yWarning("tmp.size: %d  nj %d\n", (int)tmp.size(), nj);

                YARP_ASSERT (tmp.size() == (size_t)nj);
                memcpy (encs, &(tmp.operator [](0)), sizeof(double)*nj);

                ////////////////////////// HANDLE TIMEOUT
                // fill the vector anyway
                if (Time::now()-localArrivalTime>TIMEOUT)
                    ret=false;
            }
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.motorPosition_isValid)
            {
                std::copy(last_wholePart.motorPosition.begin(), last_wholePart.motorPosition.end(), encs);
            }
            else
                ret = false;
        }
        return ret;
    }

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @param ts pointer to the array that will contain timestamps
     * @return true/false on success/failure
     */
    virtual bool getMotorEncodersTimed(double *encs, double *ts) {
        if (!isLive()) return false;
        // particular case, does not use RPC
        double localArrivalTime=0.0;

        bool ret=false;
        double time = yarp::os::Time::now();
        if(controlBoardWrapper1_compatibility)
        {
            Vector tmp(nj);
     //       mutex.wait();
            return get1VDA(VOCAB_MOTOR_ENCODERS, tmp.data());
     //       mutex.post();

            if (ret)
            {
                if (tmp.size() != (size_t)nj)
                    yWarning("tmp.size: %d  nj %d\n", (int)tmp.size(), nj);

                YARP_ASSERT (tmp.size() == (size_t)nj);
                for(int j=0; j<nj; j++)
                {
                    encs[j]=tmp[j];
                    ts[j]=time;
                }
            }
        }
        else
        {
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.motorPosition_isValid)
            {
                std::copy(last_wholePart.motorPosition.begin(), last_wholePart.motorPosition.end(), encs);
                std::fill_n(ts, nj, lastStamp.getTime());
            }
            else
                ret = false;
        }

        ////////////////////////// HANDLE TIMEOUT
        if (Time::now()-localArrivalTime>TIMEOUT)
            ret=false;

        return ret;
    }
    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getMotorEncoderSpeed(int j, double *sp)
    {
        if(controlBoardWrapper1_compatibility)
            return get1V1I1D(VOCAB_MOTOR_ENCODER_SPEED, j, sp);
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.motorVelocity_isValid)
            {
                *sp = last_singleJoint.motorVelocity[0];
            }
            else
                ret = false;
            return ret;
        }
    }


    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getMotorEncoderSpeeds(double *spds)
    {
        if(!controlBoardWrapper1_compatibility)
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.motorVelocity_isValid)
            {
                std::copy(last_wholePart.motorVelocity.begin(), last_wholePart.motorVelocity.end(), spds);
            }
            else
                ret = false;
            return ret;
        }
        else
        {
            return get1VDA(VOCAB_MOTOR_ENCODER_SPEEDS, spds);
        }
    }

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param acc pointer to the array that will contain the output
     */

    virtual bool getMotorEncoderAcceleration(int j, double *acc)
    {
        if(!controlBoardWrapper1_compatibility)
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.motorAcceleration_isValid)
            {
                *acc = last_singleJoint.motorAcceleration[0];
            }
            else
                ret = false;
            return ret;
        }
        else
        {
            return get1V1I1D(VOCAB_MOTOR_ENCODER_ACCELERATION, j, acc);
        }
    }

    /**
     * Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    virtual bool getMotorEncoderAccelerations(double *accs)
    {
        if(!controlBoardWrapper1_compatibility)
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.motorAcceleration_isValid)
            {
                std::copy(last_wholePart.motorAcceleration.begin(), last_wholePart.motorAcceleration.end(), accs);
            }
            else
                ret = false;
            return ret;
        }
        else
        {
            return get1VDA(VOCAB_MOTOR_ENCODER_ACCELERATIONS, accs);
        }
    }

    /* IPreciselyTimed */
    /**
     * Get the time stamp for the last read data
     * @return last time stamp.
     */
    virtual Stamp getLastInputStamp() {
        Stamp ret;
//        mutex.wait();
        ret = lastStamp;
//        mutex.post();
        return ret;
    }

    /**
     * Get the number of available motor encoders.
     * @param m pointer to a value representing the number of available motor encoders.
     * @return true/false
     */
    virtual bool getNumberOfMotorEncoders(int *num) {
        return get1V1I(VOCAB_MOTOR_ENCODER_NUMBER, *num);
    }

    /* IPositionControl */

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    virtual bool getAxes(int *ax) {
        return get1V1I(VOCAB_AXES, *ax);
    }

    /**
     * Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * return true/false on success/failure
     */
    virtual bool setPositionMode() {
        return set1V(VOCAB_POSITION_MODE);
    }

    /**
     * Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool positionMove(int j, double ref) {
        return set1V1I1D(VOCAB_POSITION_MOVE, j, ref);
    }

    /**
     * Set new reference point for a group of axis.
     * @param n_joint number of joints specified in the list
     * @param joints list of joints controlled
     * @param refs specifies the new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMove(const int n_joint, const int *joints, const double *refs) {
        return set1V1I1IA1DA(VOCAB_POSITION_MOVE_GROUP, n_joint, joints, refs);
    }

    /**
     * Set new reference point for all axes.
     * @param refs array, new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMove(const double *refs) {
        return set1VDA(VOCAB_POSITION_MOVES, refs);
    }


    /**
     * Set relative position. The command is relative to the
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    virtual bool relativeMove(int j, double delta) {
        return set1V1I1D(VOCAB_RELATIVE_MOVE, j, delta);
    }

    /** Set relative position for a subset of joints.
     * @param n_joint number of joints specified in the list
     * @param joints pointer to the array of joint numbers
     * @param refs pointer to the array of relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const int n_joint, const int *joints, const double *refs) {
        return set1V1I1IA1DA(VOCAB_RELATIVE_MOVE_GROUP, n_joint, joints, refs);
    }

    /**
     * Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const double *deltas) {
        return set1VDA(VOCAB_RELATIVE_MOVES, deltas);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param j joint id
     * @param flag true/false if trajectory is terminated or not.
     * @return true on success/failure.
     */
    virtual bool checkMotionDone(int j, bool *flag) {
        return get1V1I1B(VOCAB_MOTION_DONE, j, *flag);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true/false if network communication went well.
     */
    virtual bool checkMotionDone(const int n_joint, const int *joints, bool *flag) {
        return get1V1I1IA1B(VOCAB_MOTION_DONE_GROUP, n_joint, joints, *flag);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true on success/failure.
     */
    virtual bool checkMotionDone(bool *flag) {
        return get1V1B(VOCAB_MOTION_DONES, *flag);
    }

    /**
     * Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(int j, double sp) {
        return set1V1I1D(VOCAB_REF_SPEED, j, sp);
    }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) {
        return set1V1I1IA1DA(VOCAB_REF_SPEED_GROUP, n_joint, joints, spds);
    }

    /**
     * Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const double *spds) {
        return set1VDA(VOCAB_REF_SPEEDS, spds);
    }

    /**
     * Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(int j, double acc) {
        return set1V1I1D(VOCAB_REF_ACCELERATION, j, acc);
    }

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) {
        return set1V1I1IA1DA(VOCAB_REF_ACCELERATION_GROUP, n_joint, joints, accs);
    }

    /**
     * Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const double *accs) {
        return set1VDA(VOCAB_REF_ACCELERATIONS, accs);
    }

    /**
     * Get reference speed for a joint. Returns the speed used to
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    virtual bool getRefSpeed(int j, double *ref) {
        return get1V1I1D(VOCAB_REF_SPEED, j, ref);
    }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    virtual bool getRefSpeeds(const int n_joint, const int *joints, double *spds) {
        return get1V1I1IA1DA(VOCAB_REF_SPEED_GROUP, n_joint, joints, spds);
    }

    /**
     * Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    virtual bool getRefSpeeds(double *spds) {
        return get1VDA(VOCAB_REF_SPEEDS, spds);
    }

    /**
     * Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual bool getRefAcceleration(int j, double *acc) {
        return get1V1I1D(VOCAB_REF_ACCELERATION, j, acc);
    }

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs){
        return get1V1I1IA1DA(VOCAB_REF_ACCELERATION_GROUP, n_joint, joints, accs);
    }

    /**
     * Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual bool getRefAccelerations(double *accs) {
        return get1VDA(VOCAB_REF_ACCELERATIONS, accs);
    }

    /**
     * Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool stop(int j) {
        return set1V1I(VOCAB_STOP, j);
    }

    /** Stop motion for subset of joints
     * @param len size of joint list
     * @param val1 list of joints 
     * @return true/false on success/failure
     */
    virtual bool stop(const int len, const int *val1)
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_STOP_GROUP);
        cmd.addInt(len);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < len; i++)
            l1.addInt(val1[i]);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    /**
     * Stop motion, multiple joints
     * @return true/false on success/failure
     */
    virtual bool stop() {
        return set1V(VOCAB_STOPS);
    }

    /* IVelocityControl */
    /**
     * Set new reference speed for a single axis.
     * @param j joint number
     * @param v specifies the new ref speed
     * @return true/false on success/failure
     */
    virtual bool velocityMove(int j, double v) {
     //   return set1V1I1D(VOCAB_VELOCITY_MOVE, j, v);
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVE);
        c.head.addInt(j);
        c.body.size(1);
        memcpy(&(c.body[0]), &v, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    /**
     * Set a new reference speed for all axes.
     * @param v is a vector of double representing the requested speed.
     * @return true/false on success/failure.
     */
    virtual bool velocityMove(const double *v) {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVES);
        c.body.size(nj);
        memcpy(&(c.body[0]), v, sizeof(double)*nj);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    /**
     * Set the controller to velocity mode.
     * @return true/false on success/failure.
     */
    virtual bool setVelocityMode() {
        return set1V(VOCAB_VELOCITY_MODE);
    }

    /* IAmplifierControl */

    /**
     * Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j)
    { return set1V1I(VOCAB_AMP_ENABLE, j); }

    virtual bool setOffset(int j, double v)
    { return set1V1I1D(VOCAB_OFFSET, j, v); }

    /**
     * Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */

    virtual bool disableAmp(int j) {
        return set1V1I(VOCAB_AMP_DISABLE, j);
    }

    /**
     * Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrents(double *vals) {
        return get1VDA(VOCAB_AMP_CURRENTS, vals);
    }

    /**
     * Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad time
     */
    virtual bool getCurrent(int j, double *val) {
        return get1V1I1D(VOCAB_AMP_CURRENT, j, val);
    }

    /**
     * Set the maximum electric current going to a given motor. The behavior
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad time
     */
    virtual bool setMaxCurrent(int j, double v) {
        return set1V1I1D(VOCAB_AMP_MAXCURRENT, j, v);
    }

    /**
    * Returns the maximum electric current allowed for a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    virtual bool getMaxCurrent(int j, double *v) {
        return get1V1I1D(VOCAB_AMP_MAXCURRENT, j, v);
    }

    /**
     * Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatus(int *st) {
        return get1VIA(VOCAB_AMP_STATUS, st);
    }

    /* Get the status of a single amplifier, coded in a 32 bits integer
     * @param j joint number
     * @param st storage for return value
     * @return true/false success failure.
     */
    virtual bool getAmpStatus(int j, int *st)
    {
        return get1V1I1I(VOCAB_AMP_STATUS_SINGLE, j, st);
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
    virtual bool setLimits(int axis, double min, double max) {
        return set1V1I2D(VOCAB_LIMITS, axis, min, max);
    }

    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false if something bad happens
     */
    virtual bool getLimits(int axis, double *min, double *max) {
        return get1V1I2D(VOCAB_LIMITS, axis, min, max);
    }

    virtual bool setVelLimits(int axis, double min, double max)
    {
        return set1V1I2D(VOCAB_VEL_LIMITS, axis, min, max);
    }

    virtual bool getVelLimits(int axis, double *min, double *max)
    {
        return get1V1I2D(VOCAB_VEL_LIMITS, axis, min, max);
    }

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString& name) {
        return get1V1I1S(VOCAB_INFO_NAME, j, name);
    }

    /* IControlCalibration */
    bool virtual calibrate()
    { return send1V(VOCAB_CALIBRATE); }

    bool virtual abortCalibration()
    { return send1V(VOCAB_ABORTCALIB); }

    bool virtual abortPark()
    { return send1V(VOCAB_ABORTPARK); }

    bool virtual park(bool wait=true)
    { return send1V(VOCAB_PARK); }

    bool virtual calibrate2(int j, unsigned int ui, double v1, double v2, double v3)
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_CALIBRATE_JOINT);
        cmd.addInt(j);
        cmd.addInt(ui);
        cmd.addDouble(v1);
        cmd.addDouble(v2);
        cmd.addDouble(v3);

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response)) {
            return true;
        }
        return false;
    }

    bool virtual done(int j)
    { return send1V1I(VOCAB_CALIBRATE_DONE, j); }

    bool setTorqueMode()
    { return set1V(VOCAB_TORQUE_MODE); }

    bool getRefTorque(int j, double *t)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_REF, j, t); }

    bool getRefTorques(double *t)
    { return get2V1DA(VOCAB_TORQUE, VOCAB_REFS, t); }

    bool setRefTorques(const double *t)
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

    bool setRefTorque(int j, double v)
    {
        //return set2V1I1D(VOCAB_TORQUE, VOCAB_REF, j, v);
        // use the streaming port!
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        // in streaming port only SET command can be sent, so it is implicit
        c.head.addVocab(VOCAB_TORQUES_DIRECT);
        c.head.addInt(j);

        c.body.clear();
        c.body.resize(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool getBemfParam(int j, double *t)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_BEMF, j, t); }

    bool setBemfParam(int j, double v)
    { return set2V1I1D(VOCAB_TORQUE, VOCAB_BEMF, j, v); }

    bool setMotorTorqueParams(int j, const MotorTorqueParameters params)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_TORQUE);
        cmd.addVocab(VOCAB_MOTOR_PARAMS);
        cmd.addInt(j);
        Bottle& b = cmd.addList();
        b.addDouble(params.bemf);
        b.addDouble(params.bemf_scale);
        b.addDouble(params.ktau);
        b.addDouble(params.ktau_scale);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getMotorTorqueParams(int j, MotorTorqueParameters *params)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_TORQUE);
        cmd.addVocab(VOCAB_MOTOR_PARAMS);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            if (l.size() != 4)
            {
                yError("getMotorTorqueParams return value not understood, size!=4");
                return false;
            }
            params->bemf        = l.get(0).asDouble();
            params->bemf_scale  = l.get(1).asDouble();
            params->ktau        = l.get(2).asDouble();
            params->ktau_scale  = l.get(3).asDouble();
            return true;
        }
        return false;
    }

    bool setTorquePid(int j, const Pid &pid)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_TORQUE);
        cmd.addVocab(VOCAB_PID);
        cmd.addInt(j);

        Bottle& b = cmd.addList();
        b.addDouble(pid.kp);
        b.addDouble(pid.kd);
        b.addDouble(pid.ki);
        b.addDouble(pid.max_int);
        b.addDouble(pid.max_output);
        b.addDouble(pid.offset);
        b.addDouble(pid.scale);
        b.addDouble(pid.stiction_up_val);
        b.addDouble(pid.stiction_down_val);
        b.addDouble(pid.kff);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getTorque(int j, double *t)
    {
        if(controlBoardWrapper1_compatibility)
        {
            return get2V1I1D(VOCAB_TORQUE, VOCAB_TRQ, j, t);
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.torque_isValid)
            {
                *t = last_singleJoint.torque[0];
            }
            else
                ret = false;

            return ret;
        }
    }

    bool getTorques(double *t)
    {
        if(controlBoardWrapper1_compatibility)
        {
            return get2V1DA(VOCAB_TORQUE, VOCAB_TRQS, t);
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.torque_isValid)
            {
                std::copy(last_wholePart.torque.begin(), last_wholePart.torque.end(), t);
            }
            else
                ret = false;
            return ret;
        }
    }

    bool getTorqueRange(int j, double *min, double* max)
    { return get2V1I2D(VOCAB_TORQUE, VOCAB_RANGE, j, min, max); }

    bool getTorqueRanges(double *min, double *max)
    { return get2V2DA(VOCAB_TORQUE, VOCAB_RANGES, min, max); }

    bool setTorquePids(const Pid *pids)
    {
        if (!isLive()) return false;
        bool ret=true;
        for (int j=0;j<nj;j++)
        {
            ret=ret&&setTorquePid(j, pids[j]);
        }
        return ret;
    }

    bool setTorqueErrorLimit(int j, double limit)
    { return set2V1I1D(VOCAB_TORQUE, VOCAB_LIM, j, limit); }

    bool setTorqueErrorLimits(const double *limits)
    { return set2V1DA(VOCAB_TORQUE, VOCAB_LIM, limits); }

    bool getTorqueError(int j, double *err)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_ERR, j, err); }

    bool getTorqueErrors(double *errs)
    { return get2V1DA(VOCAB_TORQUE, VOCAB_ERRS, errs); }

    bool getTorquePidOutput(int j, double *out)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_OUTPUT, j, out); }

    bool getTorquePidOutputs(double *out)
    { return get2V1DA(VOCAB_TORQUE, VOCAB_OUTPUTS, out); }

    bool getTorquePid(int j, Pid *pid)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_TORQUE);
        cmd.addVocab(VOCAB_PID);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            pid->kp = l.get(0).asDouble();
            pid->kd = l.get(1).asDouble();
            pid->ki = l.get(2).asDouble();
            pid->max_int = l.get(3).asDouble();
            pid->max_output = l.get(4).asDouble();
            pid->offset = l.get(5).asDouble();
            pid->scale = l.get(6).asDouble();
            pid->stiction_up_val = l.get(7).asDouble();
            pid->stiction_down_val = l.get(8).asDouble();
            pid->kff = l.get(9).asDouble();
            return true;
        }
        return false;
    }

    bool getImpedance(int j, double *stiffness, double *damping)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_PARAM);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            *stiffness = l.get(0).asDouble();
            *damping   = l.get(1).asDouble();
            return true;
        }
        return false;
    }

    bool getImpedanceOffset(int j, double *offset)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_OFFSET);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            *offset    = l.get(0).asDouble();
            return true;
        }
        return false;
    }

    bool setImpedance(int j, double stiffness, double damping)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_PARAM);
        cmd.addInt(j);

        Bottle& b = cmd.addList();
        b.addDouble(stiffness);
        b.addDouble(damping);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setImpedanceOffset(int j, double offset)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_IMP_OFFSET);
        cmd.addInt(j);

        Bottle& b = cmd.addList();
        b.addDouble(offset);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_IMPEDANCE);
        cmd.addVocab(VOCAB_LIMITS);
        cmd.addInt(j);
        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            *min_stiff    = l.get(0).asDouble();
            *max_stiff    = l.get(1).asDouble();
            *min_damp     = l.get(2).asDouble();
            *max_damp     = l.get(3).asDouble();
            return true;
        }
        return false;
    }

    bool getTorquePids(Pid *pids)
    {
        if (!isLive()) return false;
        bool ret=true;
        for(int j=0; j<nj; j++)
        {
            ret=ret&&getTorquePid(j,pids+j);
        }
        return ret;
    }

    bool getTorqueErrorLimit(int j, double *limit)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_LIM, j, limit); }

    bool getTorqueErrorLimits(double *limits)
    { return get2V1DA(VOCAB_TORQUE, VOCAB_LIM, limits); }

    bool resetTorquePid(int j)
    { return set2V1I(VOCAB_TORQUE, VOCAB_RESET, j); }

    bool disableTorquePid(int j)
    { return set2V1I(VOCAB_TORQUE, VOCAB_DISABLE, j);}

    bool enableTorquePid(int j)
    { return set2V1I(VOCAB_TORQUE, VOCAB_ENABLE, j); }

    bool setTorqueOffset(int j, double o)
    { return set2V1I1D(VOCAB_TORQUE, VOCAB_ENABLE, j, o); }

    // IControlMode
    bool setPositionMode(int j)
    {
        return setControlMode(j,VOCAB_CM_POSITION);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_POSITION, j);
    }

    bool setPositionDirectMode(int j)
    {
        return setControlMode(j,VOCAB_CM_POSITION_DIRECT);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_POSITION_DIRECT, j);
    }

    bool setVelocityMode(int j)
    {
        return setControlMode(j,VOCAB_CM_VELOCITY);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_VELOCITY, j);
    }

    bool setTorqueMode(int j)
    {
        return setControlMode(j,VOCAB_CM_TORQUE);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_TORQUE, j);
    }

    bool setImpedancePositionMode(int j)
    {
        return setControlMode(j,VOCAB_CM_IMPEDANCE_POS);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_IMPEDANCE_POS, j);
    }

    bool setImpedanceVelocityMode(int j)
    {
        return setControlMode(j,VOCAB_CM_IMPEDANCE_VEL);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_IMPEDANCE_VEL, j);
    }

    bool setOpenLoopMode()
    {
//        return setControlModes(VOCAB_OPENLOOP_MODE);
        return set1V(VOCAB_OPENLOOP_MODE);
    }

    bool setOpenLoopMode(int j)
    {
        return setControlMode(j,VOCAB_CM_OPENLOOP);
//        return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_OPENLOOP, j);
    }

    bool getControlMode(int j, int *mode)
    {
        bool ok = false;
        if(controlBoardWrapper1_compatibility)
        {
            Bottle cmd, resp;
            cmd.addVocab(VOCAB_GET);
            cmd.addVocab(VOCAB_ICONTROLMODE);
            cmd.addVocab(VOCAB_CM_CONTROL_MODE);
            cmd.addInt(j);

            ok = rpc_p.write(cmd, resp);
            if (CHECK_FAIL(ok, resp))
            {
                *mode=resp.get(2).asVocab();
            }
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            ok = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ok && last_singleJoint.controlMode_isValid)
            {
                *mode = last_singleJoint.controlMode[0];
            }
            else
                ok = false;
        }
        return ok;

    }

    // IControlMode2
    bool getControlModes(const int n_joint, const int *joints, int *modes)
    {
        bool ok = false;      
        if(controlBoardWrapper1_compatibility)
        {
            Bottle cmd, resp;
            cmd.addVocab(VOCAB_GET);
            cmd.addVocab(VOCAB_ICONTROLMODE);
            cmd.addVocab(VOCAB_CM_CONTROL_MODE_GROUP);
            cmd.addInt(n_joint);
            Bottle& l1 = cmd.addList();
            for (int i = 0; i < n_joint; i++)
                l1.addInt(joints[i]);

            ok = rpc_p.write(cmd, resp);

            if (CHECK_FAIL(ok, resp))
            {
                Bottle& l = *(resp.get(2).asList());
                if (&l == 0)
                    return false;

                if (n_joint != l.size())
                {
                    yError("getControlModes group received an answer of wrong length. expected %d, actual size is %d", n_joint, l.size());
                    return false;
                }

                for (int i = 0; i < n_joint; i++)
                    modes[i] = l.get(i).asInt();
            }
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            ok = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ok && last_wholePart.controlMode_isValid)
            {
                for (int i = 0; i < n_joint; i++)
                    modes[i] = last_wholePart.controlMode[joints[i]];
            }
            else
                ok = false;
        }
        return ok;
    }

    // IControlMode
    bool getControlModes(int *modes)
    {
        bool ok = false;
        if(controlBoardWrapper1_compatibility)
        {
            if (!isLive()) return false;
            Bottle cmd, resp;
            cmd.addVocab(VOCAB_GET);
            cmd.addVocab(VOCAB_ICONTROLMODE);
            cmd.addVocab(VOCAB_CM_CONTROL_MODES);

            ok = rpc_p.write(cmd, resp);
            if (CHECK_FAIL(ok, resp)) {
                Bottle& l = *(resp.get(2).asList());
                if (&l == 0)
                    return false;
                int njs = l.size();
                YARP_ASSERT (nj == njs);
                for (int i = 0; i < nj; i++)
                    modes[i] = l.get(i).asInt();
                return true;
            }
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            ok = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ok && last_wholePart.controlMode_isValid)
            {
                std::copy(last_wholePart.controlMode.begin(), last_wholePart.controlMode.end(), modes);
            }
            else
                ok = false;
        }
        return ok;
    }

    // IControlMode2
    bool setControlMode(const int j, const int mode)
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODE);
        cmd.addInt(j);
        cmd.addVocab(mode);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setControlModes(const int n_joint, const int *joints, int *modes)
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODE_GROUP);
        cmd.addInt(n_joint);
        int i;
        Bottle& l1 = cmd.addList();
        for (i = 0; i < n_joint; i++)
            l1.addInt(joints[i]);

        Bottle& l2 = cmd.addList();
        for (i = 0; i < n_joint; i++)
            l2.addVocab(modes[i]);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setControlModes(int *modes)
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

    bool setPositionDirectMode()
    {
        return set1V(VOCAB_POSITION_DIRECT);
    }

    bool setPosition(int j, double ref)
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_POSITION_DIRECT);
        c.head.addInt(j);
        c.body.size(1);
        memcpy(&(c.body[0]), &ref, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool setPositions(const int n_joint, const int *joints, double *refs)
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_POSITION_DIRECT_GROUP);
        c.head.addInt(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++)
            jointList.addInt(joints[i]);
        c.body.size(n_joint);
        memcpy(&(c.body[0]), refs, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool setPositions(const double *refs)
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_POSITION_DIRECTS);
        c.body.size(nj);
        memcpy(&(c.body[0]), refs, sizeof(double)*nj);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    //
    // IVelocityControl2 Interface
    //
    bool velocityMove(const int n_joint, const int *joints, const double *spds)
    {
        // streaming port
        if (!isLive())
            return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_VELOCITY_MOVE_GROUP);
        c.head.addInt(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++)
            jointList.addInt(joints[i]);
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), spds, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return true;
    }

    bool setVelPid(int j, const Pid &pid)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_VEL_PID);
        cmd.addInt(j);
        Bottle& l = cmd.addList();
        l.addDouble(pid.kp);
        l.addDouble(pid.kd);
        l.addDouble(pid.ki);
        l.addDouble(pid.max_int);
        l.addDouble(pid.max_output);
        l.addDouble(pid.offset);
        l.addDouble(pid.scale);
        l.addDouble(pid.stiction_up_val);
        l.addDouble(pid.stiction_down_val);
        l.addDouble(pid.kff);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setVelPids(const Pid *pids)
    {
        if (!isLive()) return false;
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_VEL_PIDS);
        Bottle& l = cmd.addList();
        int i;
        for (i = 0; i < nj; i++) {
            Bottle& m = l.addList();
            m.addDouble(pids[i].kp);
            m.addDouble(pids[i].kd);
            m.addDouble(pids[i].ki);
            m.addDouble(pids[i].max_int);
            m.addDouble(pids[i].max_output);
            m.addDouble(pids[i].offset);
            m.addDouble(pids[i].scale);
            m.addDouble(pids[i].stiction_up_val);
            m.addDouble(pids[i].stiction_down_val);
            m.addDouble(pids[i].kff);
        }

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getVelPid(int j, Pid *pid)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_VEL_PID);
        cmd.addInt(j);

        bool ok = rpc_p.write(cmd, response);
        if (CHECK_FAIL(ok, response)) {
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            pid->kp = l.get(0).asDouble();
            pid->kd = l.get(1).asDouble();
            pid->ki = l.get(2).asDouble();
            pid->max_int = l.get(3).asDouble();
            pid->max_output = l.get(4).asDouble();
            pid->offset = l.get(5).asDouble();
            pid->scale = l.get(6).asDouble();
            pid->stiction_up_val = l.get(7).asDouble();
            pid->stiction_down_val = l.get(8).asDouble();
            pid->kff = l.get(9).asDouble();
            return true;
        }
        return false;
    }

    bool getVelPids(Pid *pids)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_VEL_PIDS);
        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response))
        {
            int i;
            Bottle& l = *(response.get(2).asList());
            if (&l == 0)
                return false;
            const int njs = l.size();
            YARP_ASSERT (njs == nj);
            for (i = 0; i < nj; i++)
            {
                Bottle& m = *(l.get(i).asList());
                if (&m == 0)
                    return false;
                pids[i].kp = m.get(0).asDouble();
                pids[i].kd = m.get(1).asDouble();
                pids[i].ki = m.get(2).asDouble();
                pids[i].max_int = m.get(3).asDouble();
                pids[i].max_output = m.get(4).asDouble();
                pids[i].offset = m.get(5).asDouble();
                pids[i].scale = m.get(6).asDouble();
                pids[i].stiction_up_val = m.get(7).asDouble();
                pids[i].stiction_down_val = m.get(8).asDouble();
                pids[i].kff = m.get(9).asDouble();
            }
            return true;
        }
        return false;
    }

    // Interaction Mode interface
    bool getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)
    {
        bool ok = false;
        if(controlBoardWrapper1_compatibility)
        {
            Bottle cmd, response;

            if (!isLive()) return false;

            cmd.addVocab(VOCAB_GET);
            cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
            cmd.addVocab(VOCAB_INTERACTION_MODE);
            cmd.addInt(axis);

            ok = rpc_p.write(cmd, response);

            if (CHECK_FAIL(ok, response))
            {
                YARP_ASSERT (response.size()>=1);
                *mode = (InteractionModeEnum) response.get(0).asVocab();
            }
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            ok = extendedIntputStatePort.getLast(axis, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ok && last_singleJoint.interactionMode_isValid)
            {
                *mode = (yarp::dev::InteractionModeEnum)last_singleJoint.interactionMode[0];
            }
            else
                ok = false;
        }
        return ok;
    }

    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
    {
        bool ok = false;
        if(controlBoardWrapper1_compatibility)
        {
            Bottle cmd, response;
            if (!isLive()) return false;

            cmd.addVocab(VOCAB_GET);
            cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
            cmd.addVocab(VOCAB_INTERACTION_MODE_GROUP);

            cmd.addInt(n_joints);

            Bottle& l1 = cmd.addList();
            for (int i = 0; i < n_joints; i++)
                l1.addInt(joints[i]);

            ok = rpc_p.write(cmd, response);

            if (CHECK_FAIL(ok, response))
            {
                int i;
                Bottle& list = *(response.get(0).asList());
                YARP_ASSERT(list.size() >= n_joints)

                if (list.size() != n_joints)
                {
                    yError("getInteractionModes, length of response does not match: expected %d, received %d\n ", n_joints , list.size() );
                    return false;
                }
                else
                {
                    for (i = 0; i < n_joints; i++)
                    {
                        modes[i] = (yarp::dev::InteractionModeEnum) list.get(i).asVocab();
                    }
                    return true;
                }
            }
        }
        else
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            ok = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ok && last_wholePart.interactionMode_isValid)
            {
                for (int i = 0; i < n_joints; i++)
                    modes[i] = (yarp::dev::InteractionModeEnum)last_wholePart.interactionMode[joints[i]];
            }
            else
                ok = false;
        }
        return ok;
    }

    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes)
    {
        bool ret = false;
        if(!controlBoardWrapper1_compatibility)
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.interactionMode_isValid)
            {
                std::copy(last_wholePart.interactionMode.begin(), last_wholePart.interactionMode.end(), (int*)modes);
            }
            else
                ret = false;
        }
        else
        {
            Bottle cmd, response;
            if (!isLive()) return false;

            cmd.addVocab(VOCAB_GET);
            cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
            cmd.addVocab(VOCAB_INTERACTION_MODES);

            bool ok = rpc_p.write(cmd, response);

            if (CHECK_FAIL(ok, response))
            {
                int i;
                Bottle& list = *(response.get(0).asList());
                YARP_ASSERT(list.size() >= nj)

                if (list.size() != nj)
                {
                    yError("getInteractionModes, length of response does not match: expected %d, received %d\n ", nj , list.size() );
                    return false;

                }
                else
                {
                    for (i = 0; i < nj; i++)
                    {
                        modes[i] = (yarp::dev::InteractionModeEnum) list.get(i).asVocab();
                    }
                    ret = true;
                }
            }
            else
            {
                ret = false;
            }
        }
        return ret;
    }

    bool setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)
    {
        Bottle cmd, response;
        if (!isLive()) return false;

        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
        cmd.addVocab(VOCAB_INTERACTION_MODE);
        cmd.addInt(axis);
        cmd.addVocab(mode);

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
    {
        Bottle cmd, response;
        if (!isLive()) return false;

        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_INTERFACE_INTERACTION_MODE);
        cmd.addVocab(VOCAB_INTERACTION_MODE_GROUP);
        cmd.addInt(n_joints);

        Bottle& l1 = cmd.addList();
        for (int i = 0; i < n_joints; i++)
            l1.addInt(joints[i]);

        Bottle& l2 = cmd.addList();
        for (int i = 0; i < n_joints; i++)
        {
            l2.addVocab(modes[i]);
        }
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes)
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

    //
    // OPENLOOP interface
    //

    bool setRefOutput(int j, double v)
    {
        //return set2V1I1D(VOCAB_OPENLOOP_INTERFACE, VOCAB_OUTPUT, j, v);
        // use the streaming port!
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        // in streaming port only SET command can be sent, so it is implicit
        c.head.addVocab(VOCAB_OPENLOOP_INTERFACE);
        c.head.addVocab(VOCAB_OPENLOOP_REF_OUTPUT);
        c.head.addInt(j);

        c.body.clear();
        c.body.size(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return true;
    }

    bool setRefOutputs(const double *v)
    {
//        return set2V1DA(VOCAB_OPENLOOP_INTERFACE, VOCAB_OUTPUTS, v);
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_OPENLOOP_INTERFACE);
        c.head.addVocab(VOCAB_OPENLOOP_REF_OUTPUTS);

        c.body.size(nj);

        memcpy(&(c.body[0]), v, sizeof(double)*nj);

        command_buffer.write(writeStrict_moreJoints);

        return true;
    }

    /**
     * Get the last reference sent using the setOutput function
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getRefOutput(int j, double *out)
    {
        //return get2V1D(VOCAB_OPENLOOP_INTERFACE, VOCAB_OPENLOOP_REF_OUTPUTS, out);

        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_OPENLOOP_INTERFACE);
        cmd.addVocab(VOCAB_OPENLOOP_REF_OUTPUT);
        cmd.addInt(j);
        response.clear();

        bool ok = rpc_p.write(cmd, response);

        if (CHECK_FAIL(ok, response))
        {
            // ok
            *out = response.get(2).asDouble();

            getTimeStamp(response, lastStamp);
            return true;
        }
        else
            return false;
    }

    /**
     * Get the last reference sent using the setOutputs function
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefOutputs(double *outs) {
        return get2V1DA(VOCAB_OPENLOOP_INTERFACE, VOCAB_OPENLOOP_REF_OUTPUTS, outs);
    }

    virtual bool getOutput(int j, double *out)
    {
        if(!controlBoardWrapper1_compatibility)
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(j, last_singleJoint, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_singleJoint.pidOutput_isValid)
            {
                *out = last_singleJoint.pidOutput[0];
            }
            else
                ret = false;
            return ret;
        }
        else
        {
            // both iOpenLoop and iPid getOutputs will pass here and use the VOCAB_OPENLOOP_INTERFACE
            Bottle cmd, response;
            cmd.addVocab(VOCAB_GET);
    //        cmd.addVocab(VOCAB_OPENLOOP_INTERFACE);
            cmd.addVocab(VOCAB_OUTPUT);
            cmd.addInt(j);
            bool ok = rpc_p.write(cmd, response);
    
            if (CHECK_FAIL(ok, response))
            {
                // ok
                *out = response.get(2).asDouble();
    
                getTimeStamp(response, lastStamp);
                return true;
            }
            else
                return false;
        }
    }

    /**
     * Get the last reference sent using the setOutputs function
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getOutputs(double *outs)
    {
        if(!controlBoardWrapper1_compatibility)
        {
            double localArrivalTime=0.0;
            extendedPortMutex.wait();
            bool ret = extendedIntputStatePort.getLast(last_wholePart, lastStamp, localArrivalTime);
            extendedPortMutex.post();

            if(ret && last_wholePart.pidOutput_isValid)
            {
                std::copy(last_wholePart.pidOutput.begin(), last_wholePart.pidOutput.end(), outs);
            }
            else
                ret = false;
            return ret;
        }
        else
            return get1VDA(VOCAB_OUTPUTS, outs);
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
            protocolVersion.major=reply.get(1).asInt();
            protocolVersion.minor=reply.get(2).asInt();
            protocolVersion.tweak=reply.get(3).asInt();

            //verify protocol
            if (protocolVersion.major!=PROTOCOL_VERSION_MAJOR)
                error=true;

            if (protocolVersion.minor!=PROTOCOL_VERSION_MINOR)
                error=true;
        }

        if (!error)
            return true;

        // protocol did not match
        yError("expecting protocol %d %d %d, but remotecontrolboard returned protocol version %d %d %d\n",
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
    bool isCalibratorDevicePresent(bool *isCalib)
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_IS_CALIBRATOR_PRESENT);
        bool ok = rpc_p.write(cmd, response);
        if(ok)
        {
            *isCalib = response.get(2).asInt()!=0;
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
    bool calibrateSingleJoint(int j)
    {
        return set2V1I(VOCAB_REMOTE_CALIBRATOR_INTERFACE, VOCAB_CALIBRATE_SINGLE_JOINT, j);
    }

    /**
     * @brief calibrateWholePart: call the procedure for calibrating the whole device
     * @return true if calibration was successful
     */
    bool calibrateWholePart()
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
     * @return true if homing was succesful, false otherwise
     */
    bool homingSingleJoint(int j)
    {
        return set2V1I(VOCAB_REMOTE_CALIBRATOR_INTERFACE, VOCAB_HOMING_SINGLE_JOINT, j);
    }

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was succesful, false otherwise
     */
    bool homingWholePart()
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_HOMING_WHOLE_PART);
        bool ok = rpc_p.write(cmd, response);
        std::cout << "Sent homing whole part message" << std::endl;
        return CHECK_FAIL(ok, response);
    }

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if succesful
     */
    bool parkSingleJoint(int j, bool _wait=true)
    {
        return set2V1I(VOCAB_REMOTE_CALIBRATOR_INTERFACE, VOCAB_PARK_SINGLE_JOINT, j);
    }

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if succesful
     */
    bool parkWholePart()
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
     * @return true if succesful
     */
    bool quitCalibrate()
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
     * @return true if succesful
     */
    bool quitPark()
    {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(VOCAB_REMOTE_CALIBRATOR_INTERFACE);
        cmd.addVocab(VOCAB_QUIT_PARK);
        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }
};

// implementation of CommandsHelper

yarp::dev::DriverCreator *createRemoteControlBoard() {
    return new DriverCreatorOf<RemoteControlBoard>("remote_controlboard",
        "controlboard",
        "yarp::dev::RemoteControlBoard");
}

