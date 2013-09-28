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

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/PreciselyTimed.h>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace yarp{
    namespace dev {
        class RemoteControlBoard;
    }
}

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

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

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


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
                printf("%s: %d msgs av:%.2lf min:%.2lf max:%.2lf [ms]\n",
                    ownerName.c_str(),
                    it,
                    av,
                    min,
                    max);
            }

        }
    }

};

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
    public IAmplifierControl,
    public IControlLimits2,
    public IAxisInfo,
    public IPreciselyTimed,
    public IControlCalibration2,
    public ITorqueControl,
    public IImpedanceControl,
    public IControlMode,
    public IOpenLoopControl,
    public DeviceDriver,
    public IPositionDirect
{
protected:
    Port rpc_p;
    Port command_p;
    DiagnosticThread *diagnosticThread;
    // Port state_p;

    PortReaderBuffer<yarp::sig::Vector> state_buffer;
    // BufferedPort<yarp::sig::Vector> state_p;
    StateInputPort state_p;
    PortWriterBuffer<CommandMessage> command_buffer;
    bool writeStrict;

    ConstString remote;
    ConstString local;
    mutable Stamp lastStamp;  //this is shared among all calls that read encoders
    // Semaphore mutex;
    int nj;
    bool njIsKnown;

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
     * @param v is the name of the command
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
                printf("received an answer with an unexpected number of entries!\n");
                return false;
            }
            for (i = 0; i < nj2; i++)
                val2[i] = l2.get(i).asDouble();

            getTimeStamp(response, lastStamp);
            return true;
        }
        return false;
    }

public:
    /**
     * Constructor.
     */
    RemoteControlBoard() {
        nj = 0;
        njIsKnown = false;
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

        Value strict = config.check("writeStrict", Value("off"), "Do I use writeStrict?");

        if(strict.asString() == "on")
        {
            writeStrict = true;
            printf("RemoteControlBoard is using the writeStrict option\n");
        }
        else
            writeStrict = false;

        if (local=="") {
            fprintf(stderr,"Problem connecting to remote controlboard, 'local' port prefix not given\n");
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

            //new code
            if (!portProblem) {
                state_p.useCallback();
            }
        }

        bool connectionProblem = false;
        if (remote != "" && !portProblem) {
            ConstString s1 = remote;
            s1 += "/rpc:i";
            ConstString s2 = local;
            s2 += "/rpc:o";
            bool ok = false;
            // ok=Network::connect(s2.c_str(), s1.c_str()); //doesn't take into consideration possible YARP_PORT_PREFIX on local ports
            ok=rpc_p.addOutput(s1.c_str());
            if (!ok) {
                printf("Problem connecting to %s, is the remote device available?\n", s1.c_str());
                connectionProblem = true;
            }
            s1 = remote;
            s1 += "/command:i";
            s2 = local;
            s2 += "/command:o";
            //ok = Network::connect(s2.c_str(), s1.c_str(), carrier);
            ok = command_p.addOutput(s1.c_str());
            if (!ok) {
                printf("Problem connecting to %s, is the remote device available?\n", s1.c_str());
                connectionProblem = true;
            }

            s1 = remote;
            s1 += "/state:o";
            s2 = local;
            s2 += "/state:i";

            ok = Network::connect(s1, state_p.getName(), carrier);

            if (!ok) {
                printf("Problem connecting to %s from %s, is the remote device available?\n", s1.c_str(), state_p.getName().c_str());
                connectionProblem = true;
            }
        }

        if (connectionProblem||portProblem) {

            rpc_p.close();
            command_p.close();
            state_p.close();

            return false;
        }

        //        state_buffer.attach(state_p);
        state_buffer.setStrict(false);
        command_buffer.attach(command_p);

        if (!isLive()) {
            if (remote!="") {
                printf("Problems with obtaining the number of controlled axes\n");
                rpc_p.close();
                command_p.close();
                state_p.close();
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

    /**
     * Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutput(int j, double *out) {
        return get1V1I1D(VOCAB_OUTPUT, j, out);
    }

    /**
     * Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getOutputs(double *outs) {
        return get1VDA(VOCAB_OUTPUTS, outs);
    }

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
        bool ret=state_p.getLast(j, *v, lastStamp, localArrivalTime);

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
        bool ret=state_p.getLast(j, *v, lastStamp, localArrivalTime);

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
        Vector tmp(nj);
        double localArrivalTime=0.0;

        // mutex.wait();
        bool ret=state_p.getLast(tmp,lastStamp,localArrivalTime);
        // mutex.post();

        if (ret)
        {
            if (tmp.size() != (size_t)nj)
                fprintf(stderr, "tmp.size: %d  nj %d\n", (int)tmp.size(), nj);

            YARP_ASSERT (tmp.size() == (size_t)nj);
            memcpy (encs, &(tmp.operator [](0)), sizeof(double)*nj);

            ////////////////////////// HANDLE TIMEOUT
            // fill the vector anyway
            if (Time::now()-localArrivalTime>TIMEOUT)
                ret=false;
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
        Vector tmp(nj);
        double localArrivalTime=0.0;

 //       mutex.wait();
        bool ret=state_p.getLast(tmp,lastStamp,localArrivalTime);
 //       mutex.post();

        if (ret)
        {
            if (tmp.size() != (size_t)nj)
                fprintf(stderr, "tmp.size: %d  nj %d\n", (int)tmp.size(), nj);

            YARP_ASSERT (tmp.size() == (size_t)nj);
            for(int j=0; j<nj; j++)
            {
                encs[j]=tmp[j];
                ts[j]=lastStamp.getTime();
            }
            ////////////////////////// HANDLE TIMEOUT
            // fill the vector anyway
            if (Time::now()-localArrivalTime>TIMEOUT)
                ret=false;
        }

        return ret;
    }
    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getEncoderSpeed(int j, double *sp) {
        return get1V1I1D(VOCAB_ENCODER_SPEED, j, sp);
    }


    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getEncoderSpeeds(double *spds) {
        return get1VDA(VOCAB_ENCODER_SPEEDS, spds);
    }

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param acc pointer to the array that will contain the output
     */

    virtual bool getEncoderAcceleration(int j, double *acc) {
        return get1V1I1D(VOCAB_ENCODER_ACCELERATION, j, acc);
    }

    /**
     * Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    virtual bool getEncoderAccelerations(double *accs) {
        return get1VDA(VOCAB_ENCODER_ACCELERATIONS, accs);
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
     * @param j joint number
     * @param ref specifies the new ref point
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
     * @param joints pointer to the array of joint numbers
     * @param deltas pointer to the array of relative commands
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
     * @param flag: true/false if trajectory is terminated or not.
     * @return true on success/failure.
     */
    virtual bool checkMotionDone(int j, bool *flag) {
        return get1V1I1B(VOCAB_MOTION_DONE, j, *flag);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flag  pointer to the array that will store the actual value of the checkMotionDone
     * @return true/false if network communication went well.
     */
    virtual bool checkMotionDone(const int n_joint, const int *joints, bool *flag) {
        return get1V1I1IA1B(VOCAB_MOTION_DONE_GROUP, n_joint, joints, *flag);
    }

    /** Check if the current trajectory is terminated. Non blocking.
     * @param flag: true/false if trajectories for all controlled joints are terminated.
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
     * @param joints pointer to the array of joint numbers
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
        return set1V1I1D(VOCAB_VELOCITY_MOVE, j, v);
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
        command_buffer.write(writeStrict);
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
    { return set2V1DA(VOCAB_TORQUE, VOCAB_REFS, t); }

    bool setRefTorque(int j, double v)
    { return set2V1I1D(VOCAB_TORQUE, VOCAB_REF, j, v); }

    bool getBemfParam(int j, double *t)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_BEMF, j, t); }

    bool setBemfParam(int j, double v)
    { return set2V1I1D(VOCAB_TORQUE, VOCAB_BEMF, j, v); }

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

        bool ok = rpc_p.write(cmd, response);
        return CHECK_FAIL(ok, response);
    }

    bool getTorque(int j, double *t)
    { return get2V1I1D(VOCAB_TORQUE, VOCAB_TRQ, j, t); }

    bool getTorques(double *t)
    { return get2V1DA(VOCAB_TORQUE, VOCAB_TRQS, t); }

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

    bool setPositionMode(int j)
    { return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_POSITION, j); }

    bool setVelocityMode(int j)
    { return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_VELOCITY, j); }

    bool setTorqueMode(int j)
    { return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_TORQUE, j); }

    bool setImpedancePositionMode(int j)
    { return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_IMPEDANCE_POS, j); }

    bool setImpedanceVelocityMode(int j)
    { return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_IMPEDANCE_VEL, j); }

    bool setOpenLoopMode()
    { return set1V(VOCAB_OPENLOOP_MODE); }

    bool setOpenLoopMode(int j)
    {  return send3V1I(VOCAB_SET, VOCAB_ICONTROLMODE, VOCAB_CM_OPENLOOP, j); }

    bool getControlMode(int j, int *mode)
    {
        Bottle cmd, resp;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODE);
        cmd.addInt(j);

        bool ok = rpc_p.write(cmd, resp);
        if (CHECK_FAIL(ok, resp)) {
            *mode=resp.get(2).asVocab();
            return true;
        }

        return ok;
    }

    bool getControlModes(int *modes)
    {
        if (!isLive()) return false;
        Bottle cmd, resp;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_ICONTROLMODE);
        cmd.addVocab(VOCAB_CM_CONTROL_MODES);

        bool ok = rpc_p.write(cmd, resp);
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

        return ok;
    }

    bool setOutput(int j, double v)
    { return set1V1I1D(VOCAB_OUTPUT, j, v); }

    bool setOutputs(const double *v)
    {
        if (!isLive()) return false;
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab(VOCAB_OUTPUTS);

        c.body.size(nj);

        memcpy(&(c.body[0]), v, sizeof(double)*nj);

        command_buffer.write(writeStrict);

        return true;

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
        command_buffer.write(writeStrict);
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
        command_buffer.write(writeStrict);
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
        command_buffer.write(writeStrict);
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
        command_buffer.write(writeStrict);
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
            }
        }
        return true;
    }
};

// implementation of CommandsHelper

yarp::dev::DriverCreator *createRemoteControlBoard() {
    return new DriverCreatorOf<RemoteControlBoard>("remote_controlboard",
        "controlboard",
        "RemoteControlBoard");
}

