/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoard_nwc_yarp.h"
#include "ControlBoard_nwc_yarp_LogComponent.h"
#include "stateExtendedReader.h"

#include <cstring>
#include <algorithm>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/QosStyle.h>
#include <yarp/os/Vocab32.h>


#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/IPreciselyTimed.h>

#include <mutex>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace {

constexpr int PROTOCOL_VERSION_MAJOR = 1;
constexpr int PROTOCOL_VERSION_MINOR = 9;
constexpr int PROTOCOL_VERSION_TWEAK = 0;

constexpr double DIAGNOSTIC_THREAD_PERIOD = 1.000;

inline bool getTimeStamp(Bottle &bot, Stamp &st)
{
    if (bot.get(3).asVocab32()==VOCAB_TIMESTAMP)
    {
        //yup! we have a timestamp
        int fr=bot.get(4).asInt32();
        double ts=bot.get(5).asFloat64();
        st=Stamp(fr,ts);
        return true;
    }
    return false;
}

} // namespace


class DiagnosticThread :
        public PeriodicThread
{
    StateExtendedInputPort *owner{nullptr};
    std::string ownerName;

public:
    using PeriodicThread::PeriodicThread;

    void setOwner(StateExtendedInputPort *o)
    {
        owner = o;
        ownerName = owner->getName();
    }

    void run() override
    {
        if (owner != nullptr)
        {
            if (owner->getIterations() > 100)
            {
                int it;
                double av;
                double max;
                double min;
                owner->getEstFrequency(it, av, min, max);
                owner->resetStat();
                yCDebug(CONTROLBOARD_NWC_YARP,
                        "%s: %d msgs av:%.2lf min:%.2lf max:%.2lf [ms]",
                        ownerName.c_str(),
                        it,
                        av,
                        min,
                        max);
            }

        }
    }
};


bool ControlBoard_nwc_yarp::isLive()
{
    if (!m_njIsKnown)
    {
        auto ret = m_RPC.getAxesRPC();
        if (ret.ret)
        {
            if (ret.axes >= 0)
            {
                m_nj = ret.axes;
                m_njIsKnown = true;
            }
        }
    }
    return m_njIsKnown;
}

bool ControlBoard_nwc_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    extendedIntputStatePort.setTimeout(m_timeout);

    //handle local Qos
    yarp::os::QosStyle localQos;
    if (m_local_qos_enable)
    {
        localQos.setThreadPriority(m_local_qos_thread_priority);
        localQos.setThreadPolicy(m_local_qos_thread_policy);
        localQos.setPacketPriority(m_local_qos_packet_priority);
    }

    //handle remote Qos
    yarp::os::QosStyle remoteQos;
    if (m_remote_qos_enable)
    {
        remoteQos.setThreadPriority(m_remote_qos_thread_priority);
        remoteQos.setThreadPolicy(m_remote_qos_thread_policy);
        remoteQos.setPacketPriority(m_remote_qos_packet_priority);
    }

    //handle param writeStrict
    if (m_writeStrict == "on")
    {
        writeStrict_singleJoint = true;
        writeStrict_moreJoints  = true;
        yCInfo(CONTROLBOARD_NWC_YARP, "RemoteControlBoard is ENABLING the writeStrict option for all commands");
    }
    else if(m_writeStrict == "off")
    {
        writeStrict_singleJoint = false;
        writeStrict_moreJoints  = false;
        yCInfo(CONTROLBOARD_NWC_YARP, "RemoteControlBoard is DISABLING the writeStrict option for all commands");
    }
    else if (m_writeStrict.empty())
    {
        //leave the default values
    }
    else
    {
        yCError(CONTROLBOARD_NWC_YARP, "Found writeStrict option with wrong value. Accepted options are 'on' or 'off'");
        return false;
    }

    //open ports
    bool portProblem = false;
    if (m_local != "") {
        std::string s1;
        s1 = m_local;
        s1 += "/command:o";
        if (!command_p.open(s1)) { portProblem = true; }
        s1 = m_local;
        s1 += "/stateExt:i";
        if (!extendedIntputStatePort.open(s1)) { portProblem = true; }
        if (!portProblem)
        {
            extendedIntputStatePort.useCallback();
        }
    }

    bool connectionProblem = false;
    if (m_remote != "" && !portProblem)
    {
        std::string s1 = m_remote;
        s1 += "/nws/rpc:i";
        std::string s2 = m_local;
        s2 += "/rpc:o";
        bool ok = false;
        // RPC port needs to be tcp, therefore no carrier option is added here
        // ok=Network::connect(s2.c_str(), s1.c_str());         //This doesn't take into consideration possible YARP_PORT_PREFIX on local ports
        // ok=Network::connect(rpc_p.getName(), s1.c_str());    //This should work also with YARP_PORT_PREFIX because getting back the name of the port will return the modified name

        s1 = m_remote;
        s1 += "/command:i";
        s2 = m_local;
        s2 += "/command:o";
        //ok = Network::connect(s2.c_str(), s1.c_str(), carrier);
        // ok=Network::connect(command_p.getName(), s1.c_str(), carrier); //doesn't take into consideration possible YARP_PORT_PREFIX on local ports
        ok = command_p.addOutput(s1, m_carrier_cmd);
        if (!ok) {
            yCError(CONTROLBOARD_NWC_YARP, "Problem connecting to %s, is the remote device available?", s1.c_str());
            connectionProblem = true;
        }
        // set the QoS preferences for the 'command' port
        if (m_local_qos_enable || m_remote_qos_enable) {
            NetworkBase::setConnectionQos(command_p.getName(), s1, localQos, remoteQos, false);
        }

        s1 = m_remote;
        s1 += "/stateExt:o";
        s2 = m_local;
        s2 += "/stateExt:i";
        // not checking return value for now since it is wip (different machines can have different compilation flags
        ok = Network::connect(s1, extendedIntputStatePort.getName(), m_carrier);
        if (ok)
        {
            // set the QoS preferences for the 'state' port
            if (m_local_qos_enable || m_remote_qos_enable) {
                NetworkBase::setConnectionQos(s1, extendedIntputStatePort.getName(), remoteQos, localQos, false);
            }
        }
        else
        {
            yCError(CONTROLBOARD_NWC_YARP, "Problem connecting to %s, is the remote device available?", s1.c_str());
            connectionProblem = true;
        }
    }

    if (connectionProblem||portProblem)
    {
        command_p.close();
        extendedIntputStatePort.close();
        return false;
    }

    command_buffer.attach(command_p);

    // open rpc port
    std::string local_rpc_portname = m_local + "/nwc/rpc:o";
    if (!m_rpcPort.open(local_rpc_portname))
    {
        yCError(CONTROLBOARD_NWC_YARP, "open() error could not open rpc port %s, check network\n", local_rpc_portname.c_str());
        command_p.close();
        extendedIntputStatePort.close();
        return false;
    }

    //Attach the ControlBoardMsgs to the port
    if (!m_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(CONTROLBOARD_NWC_YARP, "Error! Cannot attach the port as a client");
        command_p.close();
        extendedIntputStatePort.close();
        m_rpcPort.close();
        return false;
    }

    // connection for the rpc port
    std::string nws_rpc_portname = m_remote + "/nws/rpc:i";
    if (!Network::connect(local_rpc_portname, nws_rpc_portname))
    {
        yCError(CONTROLBOARD_NWC_YARP, "open() error could not connect to %s\n", nws_rpc_portname.c_str());
        command_p.close();
        extendedIntputStatePort.close();
        m_rpcPort.close();
        return false;
    }

    // Check the protocol version
    if (!m_RPC.checkProtocolVersion())
    {
        command_p.close();
        extendedIntputStatePort.close();
        m_rpcPort.close();
        return false;
    }

    if (!isLive())
    {
        if (m_remote!="")
        {
            yCError(CONTROLBOARD_NWC_YARP, "Problems with obtaining the number of controlled axes");
            command_p.close();
            extendedIntputStatePort.close();
            m_rpcPort.close();
            return false;
        }
    }

    if (m_diagnostic)
    {
        diagnosticThread = new DiagnosticThread(DIAGNOSTIC_THREAD_PERIOD);
        diagnosticThread->setOwner(&extendedIntputStatePort);
        diagnosticThread->start();
    }

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

    // whole part  (safe here because we already got the m_nj
    last_wholePart.jointPosition.resize(m_nj);
    last_wholePart.jointVelocity.resize(m_nj);
    last_wholePart.jointAcceleration.resize(m_nj);
    last_wholePart.motorPosition.resize(m_nj);
    last_wholePart.motorVelocity.resize(m_nj);
    last_wholePart.motorAcceleration.resize(m_nj);
    last_wholePart.torque.resize(m_nj);
    last_wholePart.current.resize(m_nj);
    last_wholePart.pwmDutycycle.resize(m_nj);
    last_wholePart.controlMode.resize(m_nj);
    last_wholePart.interactionMode.resize(m_nj);

    //check device capabilities
    auto ret = m_RPC.getDeviceInterfacesRPC();
    if (!ret.ret)
    {
       yCError(CONTROLBOARD_NWC_YARP, "Unable to getDeviceInterfacesRPC");
        command_p.close();
        extendedIntputStatePort.close();
        m_rpcPort.close();
       return false;
    }
    m_device_has_interfaces = ret.interfaces;

    return true;
}

bool ControlBoard_nwc_yarp::close()
{
    if (diagnosticThread!=nullptr)
    {
        diagnosticThread->stop();
        delete diagnosticThread;
    }

    command_buffer.detach(); //TBC

    m_rpcPort.interrupt(); // TBC
    command_p.interrupt(); // TBC
    extendedIntputStatePort.interrupt(); // TBC

    m_rpcPort.close();
    command_p.close();
    extendedIntputStatePort.close();
    return true;
}

#if 0
#define LOCKMUTEX std::lock_guard<std::mutex> lg(m_mutex);
#else
#define LOCKMUTEX
#endif

#define CHECK_INTERFACE(VALUE)                                            \
    {                                                                     \
        if (auto it = m_device_has_interfaces.find(VALUE);                \
            it == m_device_has_interfaces.end() || !it->second)           \
        {                                                                 \
            yCError(CONTROLBOARD_NWC_YARP,                                   \
                    "Device does not implement required interface: %s",   \
                    VALUE);                                               \
            return yarp::dev::ReturnValue::return_code::                  \
                return_value_error_not_implemented_by_device;             \
        }                                                                 \
    }

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getAxes(int *ax)
{
    LOCKMUTEX
    auto ret = m_RPC.getAxesRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAxesRPC");
        return ret.ret;
    }
    *ax = ret.axes;
    return ret.ret;
}

// BEGIN IPidControl

ReturnValue ControlBoard_nwc_yarp::getAvailablePids(int j, std::vector<yarp::dev::PidControlTypeEnum>& avail)
{
    LOCKMUTEX
    auto ret = m_RPC.getAvailablePidsRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAvailablePids");
        return ret.ret;
    }
    avail = ret.avail;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid)
{
    LOCKMUTEX
    auto ret = m_RPC.setPidRPC(pidtype, j, pid);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPidRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::setPids(const PidControlTypeEnum& pidtype, const Pid *pids)
{
    LOCKMUTEX
    std::vector<Pid> pids_vec(pids, pids + m_nj);
    auto ret = m_RPC.setPidsRPC(pidtype, pids_vec);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPidsRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::setPidReference(const PidControlTypeEnum& pidtype, int j, double ref)
{
    LOCKMUTEX
    auto ret = m_RPC.setPidReferenceRPC(pidtype, j, ref);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPidReferenceRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::setPidReferences(const PidControlTypeEnum& pidtype, const double *refs)
{
    LOCKMUTEX
    std::vector<double> pids_refs(refs, refs + m_nj);
    auto ret = m_RPC.setPidReferencesRPC(pidtype, pids_refs);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPidReferencesRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit)
{
    LOCKMUTEX
    auto ret = m_RPC.setPidErrorLimitRPC(pidtype, j, limit);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPidErrorLimitRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits)
{
    LOCKMUTEX
    std::vector<double> pids_lims(limits, limits + m_nj);
    auto ret = m_RPC.setPidErrorLimitsRPC(pidtype, pids_lims);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPidErrorLimitsRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidErrorRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidErrorRPC");
        return ret.ret;
    }
    *err = ret.err;
    return  ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidErrors(const PidControlTypeEnum& pidtype, double *errs)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidErrorsRPC(pidtype);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidErrorsRPC");
        return ret.ret;
    }
    std::copy(ret.errs.begin(), ret.errs.end(), errs);
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidRPC");
        return ret.ret;
    }
    *pid = ret.pid;
    return  ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidsRPC(pidtype);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidsRPC");
        return ret.ret;
    }
    std::copy(ret.pids.begin(), ret.pids.end(), pids);
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidReferenceRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidReferenceRPC");
        return ret.ret;
    }
    *ref = ret.ref;
    return  ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidReferencesRPC(pidtype);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidReferencesRPC");
        return ret.ret;
    }
    std::copy(ret.refs.begin(), ret.refs.end(), refs);
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidErrorLimitRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidErrorLimitRPC");
        return ret.ret;
    }
    *limit = ret.lim;
    return  ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidErrorLimitsRPC(pidtype);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidErrorLimits");
        return ret.ret;
    }
    std::copy(ret.lims.begin(), ret.lims.end(), limits);
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::resetPid(const PidControlTypeEnum& pidtype, int j)
{
    LOCKMUTEX
    auto ret = m_RPC.resetPidRPC(pidtype, j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to resetPidRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::disablePid(const PidControlTypeEnum& pidtype, int j)
{
    LOCKMUTEX
    auto ret = m_RPC.disablePidRPC(pidtype, j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to disablePidRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::enablePid(const PidControlTypeEnum& pidtype, int j)
{
    LOCKMUTEX
    auto ret = m_RPC.enablePidRPC(pidtype, j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to enablePidRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool& enabled)
{
    LOCKMUTEX
    auto ret = m_RPC.isPidEnabledRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to isPidEnabledRPC");
        return ret.ret;
    }
    enabled = ret.isEnabled;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidOutputRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidOutputRPC");
        return ret.ret;
    }
    *out = ret.out;
    return  ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidOutputs(const PidControlTypeEnum& pidtype, double *outs)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidOutputsRPC(pidtype);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidOutputsRPC");
        return ret.ret;
    }
    std::copy(ret.outs.begin(), ret.outs.end(), outs);
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iPidControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_PIDCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_PIDCONTROL_SET_OFFSET);
        c.head.addVocab32(yarp::conf::vocab32_t(pidtype));
        c.head.addInt32(j);
        c.body.resize(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setPidOffsetOneRPC(pidtype,j,v);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to velocityMoveOneRPC");
            return ret;
        }
        return ret;
    }
}

ReturnValue ControlBoard_nwc_yarp::setPidFeedforward(const PidControlTypeEnum& pidtype, int j, double v)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iPidControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_PIDCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_PIDCONTROL_SET_FEEDFORWARD);
        c.head.addVocab32(yarp::conf::vocab32_t(pidtype));
        c.head.addInt32(j);
        c.body.resize(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setPidFeedforwardOneRPC(pidtype,j,v);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to velocityMoveOneRPC");
            return ret;
        }
        return ret;
    }
}

ReturnValue ControlBoard_nwc_yarp::getPidExtraInfo(const PidControlTypeEnum& pidtype, int j, PidExtraInfo& info)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidExtraInfoRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidExtraInfoRPC");
        return ret.ret;
    }
    info = ret.info;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidExtraInfos(const PidControlTypeEnum& pidtype, std::vector<PidExtraInfo>& info)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidExtraInfosRPC(pidtype);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidExtraInfosRPC");
        return ret.ret;
    }
    info = ret.info;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidOffset(const PidControlTypeEnum& pidtype, int j, double& value)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidOffsetRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidOffsetRPC");
        return ret.ret;
    }
    value = ret.offset;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getPidFeedforward(const PidControlTypeEnum& pidtype, int j,  double& value)
{
    LOCKMUTEX
    auto ret = m_RPC.getPidFeedforwardRPC(pidtype,j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPidFeedforwardRPC");
        return ret.ret;
    }
    value = ret.feedforward;
    return  ret.ret;
}

// END IPidControl

// BEGIN IEncoder

yarp::dev::ReturnValue ControlBoard_nwc_yarp::resetEncoder(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.resetEncoderOneRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to resetEncoderOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::resetEncoders()
{
    LOCKMUTEX
    auto ret = m_RPC.resetEncoderAllRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to resetEncoderAllRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setEncoder(int j, double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setEncoderOneRPC(j, val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setEncoderOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setEncoders(const double *vals)
{
    LOCKMUTEX
    std::vector<double> temp(vals, vals + this->m_nj);
    auto ret = m_RPC.setEncoderAllRPC(temp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setEncoderAllRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoder(int j, double *v)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER, v, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoderTimed(int j, double *v, double *t)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER, v, m_lastStamp, localArrivalTime);
    *t=m_lastStamp.getTime();
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoders(double *encs)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODERS, encs, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncodersTimed(double *encs, double *ts)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODERS, encs, m_lastStamp, localArrivalTime);
    std::fill_n(ts, m_nj, m_lastStamp.getTime());
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoderSpeed(int j, double *sp)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER_SPEED, sp, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoderSpeeds(double *spds)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODER_SPEEDS, spds, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoderAcceleration(int j, double *acc)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_ENCODER_ACCELERATION, acc, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getEncoderAccelerations(double *accs)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_ENCODER_ACCELERATIONS, accs, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

// END IEncoder

// BEGIN IRemoteVariable

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRemoteVariable(std::string key, yarp::os::Bottle& val)
{
    LOCKMUTEX
    auto ret = m_RPC.getRemoteVariableRPC(key);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRemoteVariable");
        return ret.ret;
    }
    val = ret.val;
    return  ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    LOCKMUTEX
    auto ret = m_RPC.setRemoteVariableRPC(key, val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setRemoteVariable");
        return ret;
    }
    return ret;
}


yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    LOCKMUTEX
    auto ret = m_RPC.getRemoteVariablesListRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRemoteVariable");
        return ret.ret;
    }
    std::copy(ret.listOfKeys.begin(), ret.listOfKeys.end(), listOfKeys);
    return  ret.ret;
}

// END IRemoteVariable

// BEGIN IMotor

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getNumberOfMotors(int *num)
{
    LOCKMUTEX
    auto ret = m_RPC.getNumberOfMotorsRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getNumberOfMotors");
        return ret.ret;
    }
    *num = ret.val;
    return  ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTemperature      (int m, double* val)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(m, VOCAB_TEMPERATURE, val, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTemperatures     (double *vals)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_TEMPERATURE, vals, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTemperatureLimit (int m, double* val)
{
    LOCKMUTEX
    auto ret = m_RPC.getTemperatureLimitRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTemperatureLimit");
        return ret.ret;
    }
    *val = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTemperatureLimit (int m, const double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setTemperatureLimitRPC(m, val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTemperatureLimit");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getGearboxRatio(int m, double* val)
{
    LOCKMUTEX
    auto ret = m_RPC.getGearboxRatioRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getGearboxRatio");
        return ret.ret;
    }
    *val = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setGearboxRatio(int m, const double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setGearboxRatioRPC(m, val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setGearboxRatio");
        return ret;
    }
    return ret;
}

// END IMotor

// BEGIN IMotorEncoder

yarp::dev::ReturnValue ControlBoard_nwc_yarp::resetMotorEncoder(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.resetMotorEncoderRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to resetMotorEncoder");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::resetMotorEncoders()
{
    LOCKMUTEX
    auto ret = m_RPC.resetMotorEncodersRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to resetMotorEncoders");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setMotorEncoder(int j, const double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setMotorEncoderRPC(j, val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setMotorEncoder");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setMotorEncoderCountsPerRevolution(int m, const double cpr)
{
    LOCKMUTEX
    auto ret = m_RPC.setMotorEncoderCountsPerRevolutionRPC(m, cpr);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setMotorEncoderCountsPerRevolution");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoderCountsPerRevolution(int m, double *cpr)
{
    LOCKMUTEX
    auto ret = m_RPC.getMotorEncoderCountsPerRevolutionRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getMotorEncoderCountsPerRevolution");
        return ret.ret;
    }
    *cpr = ret.cpr;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setMotorEncoders(const double *vals)
{
    LOCKMUTEX
    std::vector<double> temp(vals, vals + this->m_nj);
    auto ret = m_RPC.setMotorEncodersAllRPC(temp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setMotorEncoders");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoder(int j, double *v)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER, v, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoderTimed(int j, double *v, double *t)
{
    double localArrivalTime = 0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER, v, m_lastStamp, localArrivalTime);
    *t=m_lastStamp.getTime();
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoders(double *encs)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODERS, encs, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncodersTimed(double *encs, double *ts)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODERS, encs, m_lastStamp, localArrivalTime);
    std::fill_n(ts, m_nj, m_lastStamp.getTime());
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoderSpeed(int j, double *sp)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER_SPEED, sp, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoderSpeeds(double *spds)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODER_SPEEDS, spds, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoderAcceleration(int j, double *acc)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_MOTOR_ENCODER_ACCELERATION, acc, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorEncoderAccelerations(double *accs)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_MOTOR_ENCODER_SPEEDS, accs, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getNumberOfMotorEncoders(int *num)
{
    LOCKMUTEX
    auto ret = m_RPC.getNumberOfMotorEncodersRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getNumberOfMotorEncodersRPC");
        return ret.ret;
    }
    *num = ret.num;
    return ret.ret;
}

// END IMotorEncoder

// BEGIN IPreciselyTimed

/**
 * Get the time stamp for the last read data
 * @return last time stamp.
 */
Stamp ControlBoard_nwc_yarp::getLastInputStamp()
{
    Stamp ret;
//    mutex.lock();
    ret = m_lastStamp;
//    mutex.unlock();
    return ret;
}

// END IPreciselyTimed

// BEGIN IPositionControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::positionMove(int j, double ref)
{
    LOCKMUTEX
    auto ret = m_RPC.positionMoveOneRPC(j, ref);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to positionMoveOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::positionMove(const int n_joint, const int *joints, const double *refs)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    std::vector<double> tempvals(refs, refs + n_joint);
    auto ret = m_RPC.positionMoveGroupRPC(tempjoints, tempvals);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to positionMoveGroupRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::positionMove(const double *refs)
{
    LOCKMUTEX
    std::vector<double> temp(refs, refs + this->m_nj);
    auto ret = m_RPC.positionMoveAllRPC(temp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to positionMoveAllRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTargetPosition(const int joint, double *ref)
{
    LOCKMUTEX
    auto ret = m_RPC.getTargetPositionOneRPC(joint);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTargetPositionsOneRPC");
        return ret.ret;
    }
    *ref = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTargetPositions(double *refs)
{
    LOCKMUTEX
    auto ret = m_RPC.getTargetPositionAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTargetPositionAllRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), refs);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTargetPositions(const int n_joint, const int *joints, double *refs)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    auto ret = m_RPC.getTargetPositionGroupRPC(tempjoints);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTargetPositionGroupRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), refs);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::relativeMove(int j, double delta)
{
    LOCKMUTEX
    auto ret = m_RPC.relativeMoveOneRPC(j, delta);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to relativeMoveOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::relativeMove(const int n_joint, const int *joints, const double *refs)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    std::vector<double> tempvals(refs, refs + n_joint);
    auto ret = m_RPC.relativeMoveGroupRPC(tempjoints, tempvals);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to relativeMoveGroupRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::relativeMove(const double *deltas)
{
    LOCKMUTEX
    std::vector<double> temp(deltas, deltas + this->m_nj);
    auto ret = m_RPC.relativeMoveAllRPC(temp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to relativeMoveAllRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::checkMotionDone(int j, bool& flag)
{
    LOCKMUTEX
    auto ret = m_RPC.checkMotionDoneOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to checkMotionDoneOneRPC");
        return ret.ret;
    }
    flag = ret.flag;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::checkMotionDone(const std::vector<int>& joints, bool& flag)
{
    LOCKMUTEX
    auto ret = m_RPC.checkMotionDoneGroupRPC(joints);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to checkMotionDoneGroupRPC");
        return ret.ret;
    }
    flag = ret.flag;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::checkMotionDone(bool& flag)
{
    LOCKMUTEX
    auto ret = m_RPC.checkMotionDoneAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to checkMotionDoneAllRPC");
        return ret.ret;
    }
    flag = ret.flag;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTrajSpeed(int j, double sp)
{
    LOCKMUTEX
    auto ret = m_RPC.setTrajSpeedOneRPC(j, sp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTrajSpeedOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTrajSpeeds(const int n_joint, const int *joints, const double *spds)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    std::vector<double> tempvals(spds, spds + n_joint);
    auto ret = m_RPC.setTrajSpeedGroupRPC(tempjoints, tempvals);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTrajSpeedGroupRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTrajSpeeds(const double *spds)
{
    LOCKMUTEX
    std::vector<double> temp(spds, spds + this->m_nj);
    auto ret = m_RPC.setTrajSpeedAllRPC(temp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTrajSpeedAllRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTrajAcceleration(int j, double acc)
{
    LOCKMUTEX
    auto ret = m_RPC.setTrajAccelerationOneRPC(j, acc);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTrajAccelerationOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTrajAccelerations(const int n_joint, const int *joints, const double *accs)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    std::vector<double> tempvals(accs, accs + n_joint);
    auto ret = m_RPC.setTrajAccelerationsGroupRPC(tempjoints, tempvals);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTrajAccelerationsGroupRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setTrajAccelerations(const double *accs)
{
    LOCKMUTEX
    std::vector<double> temp(accs, accs + this->m_nj);
    auto ret = m_RPC.setTrajAccelerationsAllRPC(temp);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setTrajAccelerationsAllRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTrajSpeed(int j, double *ref)
{
    LOCKMUTEX
    auto ret = m_RPC.getTrajSpeedOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTrajSpeedOneRPC");
        return ret.ret;
    }
    *ref = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTrajSpeeds(const int n_joint, const int *joints, double *spds)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    auto ret = m_RPC.getTrajSpeedsGroupRPC(tempjoints);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTrajSpeedsGroupRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), spds);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTrajSpeeds(double *spds)
{
    LOCKMUTEX
    auto ret = m_RPC.getTrajSpeedsAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTrajSpeedsAllRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), spds);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTrajAcceleration(int j, double *acc)
{
    LOCKMUTEX
    auto ret = m_RPC.getTrajAccelerationOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTrajAccelerationOneRPC");
        return ret.ret;
    }
    *acc = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTrajAccelerations(const int n_joint, const int *joints, double *accs)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    auto ret = m_RPC.getTrajAccelerationGroupRPC(tempjoints);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTrajAccelerationsGroupRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), accs);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTrajAccelerations(double *accs)
{
    LOCKMUTEX
    auto ret = m_RPC.getTrajAccelerationAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTrajAccelerationsAllRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), accs);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::stop(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.stopOneRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to stopOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::stop(const int len, const int *joints)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + len);
    auto ret = m_RPC.stopGroupRPC(tempjoints);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to stopGroupRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::stop()
{
    LOCKMUTEX
    auto ret = m_RPC.stopAllRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to stopAllRPC");
        return ret;
    }
    return ret;
}

// END IPositionControl

// BEGIN IJoint Fault
yarp::dev::ReturnValue ControlBoard_nwc_yarp::getLastJointFault(int j, int& fault, std::string& message)
{
    LOCKMUTEX
    auto ret = m_RPC.getLastJointFaultRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getLastJointFaultRPC");
        return ret.ret;
    }
    fault = ret.fault;
    message = ret.message;
    return ret.ret;
}
// END IJointFault

// BEGIN IVelocityControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::velocityMove(int j, double v)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iVelocityControl")
        if (!isLive()) {
           return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_VELOCITYCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_VELOCITY_MOVE);
        c.head.addInt32(j);
        c.body.resize(1);
        memcpy(&(c.body[0]), &v, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.velocityMoveOneRPC(j,v);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to velocityMoveOneRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::velocityMove(const double *v)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iVelocityControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_VELOCITYCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_VELOCITY_MOVES);
        c.body.resize(m_nj);
        memcpy(&(c.body[0]), v, sizeof(double)*m_nj);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<double> temp(v, v + this->m_nj);
        auto ret = m_RPC.velocityMoveAllRPC(temp);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to velocityMoveAllRPC");
            return ret;
        }
        return ret;
    }
}

// END IVelocityControl

// BEGIN IAmplifierControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::enableAmp(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.enableAmpRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to enableAmp");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::disableAmp(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.disableAmpRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to disableAmpRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getAmpStatus(int *st)
{
    LOCKMUTEX
    auto ret = m_RPC.getAmpStatusAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAmpStatusAllRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), st);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getAmpStatus(int j, int *st)
{
    LOCKMUTEX
    auto ret = m_RPC.getAmpStatusOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAmpStatusOneRPC");
        return ret.ret;
    }
    *st = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setMaxCurrent(int j, double v)
{
    LOCKMUTEX
    auto ret = m_RPC.setMaxCurrentRPC(j,v);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setMaxCurrentRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMaxCurrent(int j, double *v)
{
    LOCKMUTEX
    auto ret = m_RPC.getMaxCurrentRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getMaxCurrentRPC");
        return ret.ret;
    }
    *v = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getNominalCurrent(int m, double *val)
{
    LOCKMUTEX
    auto ret = m_RPC.getNominalCurrentRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getNominalCurrentRPC");
        return ret.ret;
    }
    *val = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setNominalCurrent(int m, const double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setNominalCurrentRPC(m,val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setNominalCurrentRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getPeakCurrent(int m, double *val)
{
    LOCKMUTEX
    auto ret = m_RPC.getPeakCurrentRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPeakCurrentRPC");
        return ret.ret;
    }
    *val = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setPeakCurrent(int m, const double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setPeakCurrentRPC(m,val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPeakCurrentRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getPWM(int m, double* val)
{
    double localArrivalTime = 0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(m, VOCAB_PWMCONTROL_PWM_OUTPUT, val, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getPWMLimit(int m, double* val)
{
    LOCKMUTEX
    auto ret = m_RPC.getPWMLimitRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPWMLimitRPC");
        return ret.ret;
    }
    *val = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setPWMLimit(int m, const double val)
{
    LOCKMUTEX
    auto ret = m_RPC.setPWMLimitRPC(m,val);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPWMLimitRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getPowerSupplyVoltage(int m, double* val)
{
    LOCKMUTEX
    auto ret = m_RPC.getPowerSupplyVoltageRPC(m);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPowerSupplyVoltageRPC");
        return ret.ret;
    }
    *val = ret.val;
    return ret.ret;
}

// END IAmplifierControl

// BEGIN IControlLimits

ReturnValue ControlBoard_nwc_yarp::setPosLimits(int axis, double min, double max)
{
    LOCKMUTEX
    auto ret = m_RPC.setPosLimitsRPC(axis, min, max);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setPosLimitsRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::getPosLimits(int axis, double *min, double *max)
{
    LOCKMUTEX
    auto ret = m_RPC.getPosLimitsRPC(axis);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getPosLimitsRPC");
        return ret.ret;
    }
    *min = ret.min;
    *max = ret.max;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::setVelLimits(int axis, double min, double max)
{
    LOCKMUTEX
    auto ret = m_RPC.setVelLimitsRPC(axis, min, max);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setVelLimitsRPC");
        return ret;
    }
    return ret;
}

ReturnValue ControlBoard_nwc_yarp::getVelLimits(int axis, double *min, double *max)
{
    LOCKMUTEX
    auto ret = m_RPC.getVelLimitsRPC(axis);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getVelLimitsRPC");
        return ret.ret;
    }
    *min = ret.min;
    *max = ret.max;
    return ret.ret;
}

// END IControlLimits

// BEGIN IAxisInfo

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getAxisName(int j, std::string& name)
{
    LOCKMUTEX
    auto ret = m_RPC.getAxisNameRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAxisNameRPC");
        return ret.ret;
    }
    name = ret.name;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    LOCKMUTEX
    auto ret = m_RPC.getJointTypeRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getJointTypeRPC");
        return ret.ret;
    }
    type = ret.joint;
    return ret.ret;
}

// END IAxisInfo

// BEGIN IControlCalibration
yarp::dev::ReturnValue ControlBoard_nwc_yarp::calibrateRobot()
{
    LOCKMUTEX
    auto ret = m_RPC.calibrateRobotRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to calibrateRobot");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::abortCalibration()
{
    LOCKMUTEX
    auto ret = m_RPC.abortCalibrationRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to abortCalibration");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::abortPark()
{
    LOCKMUTEX
    auto ret = m_RPC.abortParkRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to abortPark");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::park(bool wait)
{
    LOCKMUTEX
    auto ret = m_RPC.parkRPC(wait);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to park");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3)
{
    LOCKMUTEX
    auto ret = m_RPC.calibrateAxisWithParamsRPC(j, ui, v1, v2, v3);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to calibrateAxisWithParams");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setCalibrationParameters(int j, const CalibrationParameters& params)
{
    LOCKMUTEX
    yCalibrationParameters cal;
    cal.param1 = params.param1;
    cal.param2 = params.param2;
    cal.param3 = params.param3;
    cal.param4 = params.param4;
    cal.param5 = params.param5;
    cal.paramZero  = params.paramZero;
    auto ret = m_RPC.setCalibrationParametersRPC(j,cal);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setCalibrationParameters");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::calibrationDone(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.calibrationDoneRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to calibrationDone");
        return ret;
    }
    return ret;
}

// END IControlCalibration

// BEGIN ITorqueControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefTorque(int j, double *t)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefTorqueOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefTorqueOneRPC");
        return ret.ret;
    }
    *t = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefTorques(double *t)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefTorqueAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefTorqueAllRPC");
        return ret.ret;
    }

    std::copy(ret.val.begin(), ret.val.end(), t);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefTorques(const double *t)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iTorqueControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_TORQUECONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_TORQUES_DIRECTS);

        c.body.resize(m_nj);

        memcpy(c.body.data(), t, sizeof(double) * m_nj);

        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<double> temp(t, t + this->m_nj);
        auto ret = m_RPC.setRefTorqueAllRPC(temp);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefTorqueAllRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefTorque(int j, double v)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iTorqueControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_TORQUECONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_TORQUES_DIRECT);
        c.head.addInt32(j);

        c.body.clear();
        c.body.resize(1);
        c.body[0] = v;
        command_buffer.write(writeStrict_singleJoint);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setRefTorqueOneRPC(j,v);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefTorqueOneRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefTorques(const int n_joint, const int *joints, const double *t)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iTorqueControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_TORQUECONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_TORQUES_DIRECT_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++) {
            jointList.addInt32(joints[i]);
        }
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), t, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<int> tempjoints(joints, joints + n_joint);
        std::vector<double> tempdata(t, t + n_joint);
        auto ret = m_RPC.setRefTorqueGroupRPC(tempjoints, tempdata);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefTorqueGroupRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setMotorTorqueParams(int j, const MotorTorqueParameters params)
{
    LOCKMUTEX
    yMotorTorqueParameters mparams;
    mparams.bemf = params.bemf;
    mparams.bemf_scale = params.bemf_scale;
    mparams.coulombNeg = params.coulombNeg;
    mparams.coulombPos = params.coulombPos;
    mparams.ktau = params.ktau;
    mparams.ktau_scale = params.ktau_scale;
    mparams.velocityThres = params.velocityThres;
    mparams.viscousNeg = params.viscousNeg;
    mparams.viscousPos = params.viscousPos;
    auto ret = m_RPC.setMotorTorqueParamsRPC(j, mparams);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setMotorTorqueParamsRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getMotorTorqueParams(int j, MotorTorqueParameters *params)
{
    LOCKMUTEX
    auto ret = m_RPC.getMotorTorqueParamsRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getMotorTorqueParamsRPC");
        return ret.ret;
    }
    (*params).bemf = ret.params.bemf;
    (*params).bemf_scale = ret.params.bemf_scale;
    (*params).coulombNeg = ret.params.coulombNeg;
    (*params).coulombPos = ret.params.coulombPos;
    (*params).ktau = ret.params.ktau;
    (*params).ktau_scale = ret.params.ktau_scale;
    (*params).velocityThres = ret.params.velocityThres;
    (*params).viscousNeg = ret.params.viscousNeg;
    (*params).viscousPos = ret.params.viscousPos;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTorque(int j, double *t)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_TRQ, t, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTorques(double *t)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_TRQS, t, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTorqueRange(int j, double *min, double* max)
{
    LOCKMUTEX
    auto ret = m_RPC.getTorqueRangeOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTorqueRangeOneRPC");
        return ret.ret;
    }
    *min = ret.min;
    *max = ret.max;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTorqueRanges(double *min, double *max)
{
    LOCKMUTEX
    auto ret = m_RPC.getTorqueRangeAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTorqueRangeAllRPC");
        return ret.ret;
    }
    std::copy(ret.mins.begin(), ret.mins.end(), min);
    std::copy(ret.maxs.begin(), ret.maxs.end(), max);
    return ret.ret;
}

// END ITorqueControl

// BEGIN IImpedanceControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getImpedance(int j, double *stiffness, double *damping)
{
    LOCKMUTEX
    auto ret = m_RPC.getImpedanceRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getImpedanceRPC");
        return ret.ret;
    }
    *stiffness = ret.stiffness;
    *damping = ret.damping;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getImpedanceOffset(int j, double *offset)
{
    LOCKMUTEX
    auto ret = m_RPC.getImpedanceOffsetRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getImpedanceOffsetRPC");
        return ret.ret;
    }
    *offset = ret.offset;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setImpedance(int j, double stiffness, double damping)
{
    LOCKMUTEX
    auto ret = m_RPC.setImpedanceRPC(j,stiffness, damping);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setImpedance");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setImpedanceOffset(int j, double offset)
{
    LOCKMUTEX
    auto ret = m_RPC.setImpedanceOffsetRPC(j,offset);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setImpedanceOffset");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    LOCKMUTEX
    auto ret = m_RPC.getCurrentImpedanceLimitRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getCurrentImpedanceLimitRPC");
        return ret.ret;
    }
    *min_stiff = ret.min_stiffness;
    *max_stiff = ret.max_stiffness;
    *min_damp = ret.min_damping;
    *max_damp = ret.max_damping;
    return ret.ret;
}

// END IImpedanceControl

// BEGIN IControlMode

ReturnValue ControlBoard_nwc_yarp::getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)
{
    LOCKMUTEX
    auto ret = m_RPC.getAvailableControlModesRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAvailableControlModes");
        return ret.ret;
    }
    avail =ret.avail;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getControlMode(int j, yarp::dev::ControlModeEnum& mode)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    int mode_tmp;
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_CM_CONTROL_MODE, &mode_tmp, m_lastStamp, localArrivalTime);
    mode = (yarp::dev::ControlModeEnum) mode_tmp;
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getControlModes(std::vector<yarp::dev::ControlModeEnum>& modes)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_CM_CONTROL_MODES, last_wholePart.controlMode.data(), m_lastStamp, localArrivalTime);
    if(ret)
    {
        for (int i = 0; i < modes.size(); i++) {
            modes[i] = (yarp::dev::ControlModeEnum) last_wholePart.controlMode[i];
        }
    } else {
        ret = false;
    }

    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getControlModes(const std::vector<int>& joints, std::vector<yarp::dev::ControlModeEnum>& modes)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_CM_CONTROL_MODES, last_wholePart.controlMode.data(), m_lastStamp, localArrivalTime);
    if(ret)
    {
        for (int i = 0; i < joints.size(); i++) {
            modes[i] = (yarp::dev::ControlModeEnum)last_wholePart.controlMode[joints[i]];
        }
    } else {
        ret = false;
    }

    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setControlMode(int j, yarp::dev::SelectableControlModeEnum mode)
{
    LOCKMUTEX
    auto ret = m_RPC.setControlModeOneRPC(j, mode);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setControlMode");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setControlModes(const std::vector<int>& j, const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    LOCKMUTEX
    auto ret = m_RPC.setControlModeGroupRPC(j, modes);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setControlModes");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setControlModes(const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    LOCKMUTEX
    auto ret = m_RPC.setControlModeAllRPC(modes);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setControlModes");
        return ret;
    }
    return ret;
}

// END IControlMode

// BEGIN IPositionDirect

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setPosition(int j, double ref)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iPositionDirect")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_POSITIONDIRECTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_POSITION_DIRECT);
        c.head.addInt32(j);
        c.body.resize(1);
        memcpy(&(c.body[0]), &ref, sizeof(double));
        command_buffer.write(writeStrict_singleJoint);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setRefPositionOneRPC(j, ref);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setPositionOneRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setPositions(const int n_joint, const int *joints, const double *refs)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iPositionDirect")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_POSITIONDIRECTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_POSITION_DIRECT_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++) {
            jointList.addInt32(joints[i]);
        }
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), refs, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<int> tempjoints(joints, joints + n_joint);
        std::vector<double> tempdata(refs, refs + n_joint);
        auto ret = m_RPC.setRefPositionGroupRPC(tempjoints, tempdata);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setPositionGroupRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setPositions(const double *refs)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iPositionDirect")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_POSITIONDIRECTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_POSITION_DIRECTS);
        c.body.resize(m_nj);
        memcpy(&(c.body[0]), refs, sizeof(double)*m_nj);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<double> temp(refs, refs + m_nj);
        auto ret = m_RPC.setRefPositionAllRPC(temp);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setPositionAllRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefPosition(const int joint, double* ref)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefPositionOneRPC(joint);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefPositionOneRPC");
        return ret.ret;
    }
    *ref = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefPositions(double* refs)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefPositionAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefPositionAllRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), refs);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefPositions(const int n_joint, const int* joints, double* refs)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    auto ret = m_RPC.getRefPositionGroupRPC(tempjoints);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefPositionGroupRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), refs);
    return ret.ret;
}

// END IPositionDirect

// BEGIN IVelocityControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iVelocityControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_VELOCITYCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_VELOCITY_MOVE_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++) {
            jointList.addInt32(joints[i]);
        }
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), spds, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<int> tempjoints(joints, joints + n_joint);
        std::vector<double> tempdata(spds, spds + n_joint);
        auto ret = m_RPC.velocityMoveGroupRPC(tempjoints, tempdata);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to velocityMoveGroupRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTargetVelocity(const int joint, double* vel)
{
    LOCKMUTEX
    auto ret = m_RPC.getTargetVelocityOneRPC(joint);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTargetVelocityOneRPC");
        return ret.ret;
    }
    *vel = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTargetVelocities(double* vels)
{
    LOCKMUTEX
    auto ret = m_RPC.getTargetVelocityAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTargetVelocityAllRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), vels);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getTargetVelocities(const int n_joint, const int* joints, double* vels)
{
    LOCKMUTEX
    std::vector<int> tempjoints(joints, joints + n_joint);
    auto ret = m_RPC.getTargetVelocityGroupRPC(tempjoints);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getTargetVelocityGroupRPC");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), vels);
    return ret.ret;
}

// END IVelocityControl

// BEGIN IInteractionMode

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getInteractionMode(int axis, yarp::dev::InteractionModeEnum& mode)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    int mode_tmp;
    bool ret = extendedIntputStatePort.getLastSingle(axis, VOCAB_INTERACTION_MODE, &mode_tmp, m_lastStamp, localArrivalTime);
    mode = (yarp::dev::InteractionModeEnum)mode_tmp;
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getInteractionModes(const std::vector<int>& joints, std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    double localArrivalTime=0.0;

    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_INTERACTION_MODES, last_wholePart.interactionMode.data(), m_lastStamp, localArrivalTime);
    if(ret)
    {
        for (int i = 0; i < joints.size(); i++) {
            modes[i] = (yarp::dev::InteractionModeEnum)last_wholePart.interactionMode[joints[i]];
        }
    } else {
        ret = false;
    }

    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getInteractionModes(std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    double localArrivalTime=0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_INTERACTION_MODES, last_wholePart.interactionMode.data(), m_lastStamp, localArrivalTime);
    if(ret)
    {
        for (int i = 0; i < modes.size(); i++) {
            modes[i] = (yarp::dev::InteractionModeEnum) last_wholePart.interactionMode[i];
        }
    } else {
        ret = false;
    }
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)
{
    LOCKMUTEX
    auto ret = m_RPC.setInteractionModeOneRPC(axis,mode);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setInteractionModeRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setInteractionModes(const std::vector<int>& joints, const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    LOCKMUTEX
    auto ret = m_RPC.setInteractionModesGroupRPC(joints, modes);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setInteractionModesGroupRPC");
        return ret;
    }
    return ret;}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setInteractionModes(const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    LOCKMUTEX
    auto ret = m_RPC.setInteractionModesAllRPC(modes);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setInteractionModesAllRPC");
        return ret;
    }
    return ret;}

// END IInteractionMode

// BEGIN IRemoteCalibrator

yarp::dev::ReturnValue ControlBoard_nwc_yarp::isCalibratorDevicePresent(bool *isCalib)
{
    LOCKMUTEX
    auto ret = m_RPC.isCalibratorDevicePresentRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to isCalibratorDevicePresentRPC");
        return ret.ret;
    }
    *isCalib = ret.isPresent;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::calibrateSingleJoint(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.calibrateSingleJointRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to calibrateSingleJoint");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::calibrateWholePart()
{
    LOCKMUTEX
    auto ret = m_RPC.calibrateWholePartRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to calibrateWholePart");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::homingSingleJoint(int j)
{
    LOCKMUTEX
    auto ret = m_RPC.homingSingleJointRPC(j);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to homingSingleJoint");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::homingWholePart()
{
    LOCKMUTEX
    auto ret = m_RPC.homingWholePartRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to homingWholePart");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::parkSingleJoint(int j, bool _wait)
{
    LOCKMUTEX
    auto ret = m_RPC.parkSingleJointRPC(j,_wait);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to parkSingleJoint");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::parkWholePart()
{
    LOCKMUTEX
    auto ret = m_RPC.parkWholePartRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to parkWholePart");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::quitCalibrate()
{
    LOCKMUTEX
    auto ret = m_RPC.quitCalibrateRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to quitCalibrate");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::quitPark()
{
    LOCKMUTEX
    auto ret = m_RPC.quitParkRPC();
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to quitPark");
        return ret;
    }
    return ret;
}

// END IRemoteCalibrator

// BEGIN ICurrentControl

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefCurrents(double *t)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefCurrentAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefCurrentAllRPC");
        return ret.ret;
    }
    std::copy(ret.refs.begin(), ret.refs.end(), t);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefCurrent(int j, double *t)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefCurrentOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefCurrentOneRPC");
        return ret.ret;
    }
    *t = ret.ref;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefCurrents(const double *refs)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iCurrentControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_CURRENTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_CURRENT_REFS);
        c.body.resize(m_nj);
        memcpy(&(c.body[0]), refs, sizeof(double)*m_nj);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<double> temp(refs, refs + this->m_nj);
        auto ret = m_RPC.setRefCurrentAllRPC(temp);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefCurrentAllRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefCurrent(int j, double ref)
{
    LOCKMUTEX
    auto ret = m_RPC.setRefCurrentOneRPC(j,ref);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefCurrentOneRPC");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefCurrents(const int n_joint, const int *joints, const double *refs)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iCurrentControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_CURRENTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_CURRENT_REF_GROUP);
        c.head.addInt32(n_joint);
        Bottle &jointList = c.head.addList();
        for (int i = 0; i < n_joint; i++) {
            jointList.addInt32(joints[i]);
        }
        c.body.resize(n_joint);
        memcpy(&(c.body[0]), refs, sizeof(double)*n_joint);
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<int> tempjoints(joints, joints + n_joint);
        std::vector<double> tempdata(refs, refs + n_joint);
        auto ret = m_RPC.setRefCurrentGroupRPC(tempjoints, tempdata);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefCurrentGroupRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getCurrents(double *vals)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iCurrentControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        double localArrivalTime=0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastVector(VOCAB_AMP_CURRENTS, vals, m_lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.getCurrentAllRPC();
        if (!ret.ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to getCurrentAllRPC");
            return ret.ret;
        }
        std::copy(ret.currs.begin(), ret.currs.end(), vals);
        return ret.ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getCurrent(int j, double *val)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iCurrentControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        double localArrivalTime = 0.0;
        extendedPortMutex.lock();
        bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_AMP_CURRENT, val, m_lastStamp, localArrivalTime);
        extendedPortMutex.unlock();
        return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.getCurrentOneRPC(j);
        if (!ret.ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to getCurrentOneRPC");
            return ret.ret;
        }
        *val = ret.curr;
        return ret.ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getCurrentRange(int j, double *min, double *max)
{
    LOCKMUTEX
    auto ret = m_RPC.getCurrentRangeOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getCurrentRangeOneRPC");
        return ret.ret;
    }
    *max = ret.max;
    *min = ret.min;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getCurrentRanges(double *min, double *max)
{
    LOCKMUTEX
    auto ret = m_RPC.getCurrentRangeAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getCurrentRangeOneRPC");
        return ret.ret;
    }
    std::copy(ret.mins.begin(), ret.mins.end(), min);
    std::copy(ret.maxs.begin(), ret.maxs.end(), max);
    return ret.ret;
}

// END ICurrentControl

// BEGIN IPWMControl
yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefDutyCycle(int j, double v)
{
    LOCKMUTEX
    auto ret = m_RPC.setRefDutyCycleOneRPC(j,v);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefDutyCycle");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setRefDutyCycles(const double *v)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iPwmControl")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_PWMCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_PWMCONTROL_REF_PWMS);

        c.body.resize(m_nj);

        memcpy(&(c.body[0]), v, sizeof(double)*m_nj);

        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        std::vector<double> temp(v, v + this->m_nj);
        auto ret = m_RPC.setRefDutyCycleAllRPC(temp);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefDutyCycleAllRPC");
            return ret;
        }
        return ret;
    }
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefDutyCycle(int j, double *ref)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefDutyCycleOneRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefDutyCycle");
        return ret.ret;
    }
    *ref = ret.val;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getRefDutyCycles(double *refs)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefDutyCycleAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefDutyCycle");
        return ret.ret;
    }
    std::copy(ret.val.begin(), ret.val.end(), refs);
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getDutyCycle(int j, double *out)
{
    double localArrivalTime = 0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastSingle(j, VOCAB_PWMCONTROL_PWM_OUTPUT, out, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getDutyCycles(double *outs)
{
    double localArrivalTime = 0.0;
    extendedPortMutex.lock();
    bool ret = extendedIntputStatePort.getLastVector(VOCAB_PWMCONTROL_PWM_OUTPUTS, outs, m_lastStamp, localArrivalTime);
    extendedPortMutex.unlock();
    return ret?ReturnValue_ok:ReturnValue::return_code::return_value_error_not_ready;
}
// END IPWMControl

// BEGIN IJointBrake
yarp::dev::ReturnValue ControlBoard_nwc_yarp::isJointBraked(int j, bool& braked) const
{
    LOCKMUTEX
    auto ret = m_RPC.isJointBrakedRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to isJointBraked");
        return ret.ret;
    }
    braked = ret.isBraked;
    return ret.ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setManualBrakeActive(int j, bool active)
{
    LOCKMUTEX
    auto ret = m_RPC.setManualBrakeActiveRPC(j,active);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setManualBrakeActive");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::setAutoBrakeEnabled(int j, bool enabled)
{
    LOCKMUTEX
    auto ret = m_RPC.setAutoBrakeEnabledRPC(j,enabled);
    if (!ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to setAutoBrakeEnabled");
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoard_nwc_yarp::getAutoBrakeEnabled(int j, bool& enabled) const
{
    LOCKMUTEX
    auto ret = m_RPC.getAutoBrakeEnabledRPC(j);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAutoBrakeEnabled");
        return ret.ret;
    }
    enabled = ret.enabled;
    return ret.ret;
}
// END IJointBrake

// IVelocityDirect
ReturnValue ControlBoard_nwc_yarp::getAxes(size_t& axes)
{
    LOCKMUTEX
    auto ret = m_RPC.getAxesRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getAxes");
        return ret.ret;
    }
    axes = ret.axes;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::setRefVelocity(int jnt, double vel)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iVelocityDirect")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_VELOCITYDIRECTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_VELOCITY_DIRECT_SET_ONE);
        c.head.addInt32(jnt);
        c.body.resize(1);
        c.body[0] = vel;
        command_buffer.write(writeStrict_singleJoint);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setRefVelocityOneRPC(jnt,vel);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefVelocityOneRPC");
            return ret;
        }
        return ret;
    }
}

ReturnValue ControlBoard_nwc_yarp::setRefVelocity(const std::vector<double>& vels)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iVelocityDirect")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_VELOCITYDIRECTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_VELOCITY_DIRECT_SET_ALL);
        c.body.resize(m_nj);
        for (size_t i = 0; i < vels.size(); i++) {
            c.body[i]=vels[i];
        }
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setRefVelocityAllRPC(vels);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefVelocityAllRPC");
            return ret;
        }
        return ret;
    }
}

ReturnValue ControlBoard_nwc_yarp::setRefVelocity(const std::vector<int>& jnts, const std::vector<double>& vels)
{
    if (m_use_streaming)
    {
        CHECK_INTERFACE("iVelocityDirect")
        if (!isLive()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        CommandMessage& c = command_buffer.get();
        c.head.clear();
        c.head.addVocab32(VOCAB_VELOCITYDIRECTCONTROL_INTERFACE);
        c.head.addVocab32(VOCAB_VELOCITY_DIRECT_SET_GROUP);
        c.head.addInt32(jnts.size());
        Bottle &jointList = c.head.addList();
        for (size_t i = 0; i < jnts.size(); i++) {
            jointList.addInt32(jnts[i]);
        }
        c.body.resize(jnts.size());
        for (size_t i = 0; i < jnts.size(); i++) {
            c.body[i]=vels[i];
        }
        command_buffer.write(writeStrict_moreJoints);
        return ReturnValue_ok;
    }
    else
    {
        LOCKMUTEX
        auto ret = m_RPC.setRefVelocityGroupRPC(jnts,vels);
        if (!ret) {
            yCError(CONTROLBOARD_NWC_YARP, "Unable to setRefVelocityGroupRPC");
            return ret;
        }
        return ret;
    }
}

ReturnValue ControlBoard_nwc_yarp::getRefVelocity(const int jnt, double& vel)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefVelocityOneRPC(jnt);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefVelocityOneRPC");
        return ret.ret;
    }
    vel = ret.vel;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getRefVelocity(std::vector<double>& vels)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefVelocityAllRPC();
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefVelocityAllRPC");
        return ret.ret;
    }
    vels = ret.vel;
    return ret.ret;
}

ReturnValue ControlBoard_nwc_yarp::getRefVelocity(const std::vector<int>& jnts, std::vector<double>& vels)
{
    LOCKMUTEX
    auto ret = m_RPC.getRefVelocityGroupRPC(jnts);
    if (!ret.ret) {
        yCError(CONTROLBOARD_NWC_YARP, "Unable to getRefVelocityGroupRPC");
        return ret.ret;
    }
    vels = ret.vel;
    return ret.ret;
}
