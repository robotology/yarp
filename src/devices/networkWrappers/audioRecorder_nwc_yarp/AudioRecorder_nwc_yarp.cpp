/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "AudioRecorder_nwc_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(AUDIORECORDER_NWC, "yarp.device.audioRecorder_nwc_yarp")
} // namespace

AudioRecorder_InputPortProcessor::AudioRecorder_InputPortProcessor()
{
}

void AudioRecorder_InputPortProcessor::onRead(yarp::sig::Sound& b)
{
    mutex.lock();

    lastSound = b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialization (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    lastStamp = newStamp;

    mutex.unlock();
}

inline bool AudioRecorder_InputPortProcessor::getLast(yarp::sig::Sound& data, Stamp &stmp)
{
    mutex.lock();
      data = lastSound;
      stmp = lastStamp;
    mutex.unlock();
    return true;
}

bool AudioRecorder_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) { return false; }

    std::string local_stream = m_local;
    local_stream += "/data:i";
    std::string local_rpc = m_local;
    local_rpc += "/rpc";
    std::string remote_stream = m_remote;
    remote_stream += "/data:o";
    std::string remote_rpc = m_remote;
    remote_rpc += "/rpc";

    if (m_useStream)
    {
        if (!m_inputPort.open(local_stream))
        {
            yCError(AUDIORECORDER_NWC, "open(): Could not open port %s, check network", local_stream.c_str());
            return false;
        }
       m_inputPort.useCallback();
    }

    if (!m_rpcPort.open(local_rpc))
    {
        yCError(AUDIORECORDER_NWC, "open(): Could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok;
    if (m_useStream)
    {
        ok=Network::connect(remote_stream.c_str(), local_stream.c_str(), m_carrier);
        if (!ok)
        {
            yCError(AUDIORECORDER_NWC, "open(): Could not connect %s -> %s", remote_stream.c_str(), local_stream.c_str());
            return false;
        }
    }

    ok=Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
       yCError(AUDIORECORDER_NWC, "open() Could not connect %s -> %s", remote_rpc.c_str(), local_rpc.c_str());
       return false;
    }

    if (!m_audiograb_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(AUDIORECORDER_NWC, "Error! Cannot attach the port as a client");
        return false;
    }

    //Check the protocol version
    if (!m_audiograb_RPC.checkProtocolVersion()) { return false; }

    yCInfo(AUDIORECORDER_NWC) << "Opening of NWC successful";
    return true;
}

bool AudioRecorder_nwc_yarp::close()
{
    m_rpcPort.close();
    if (m_useStream)
    {
        m_inputPort.close();
    }
    return true;
}

ReturnValue AudioRecorder_nwc_yarp::setSWGain(double gain)
{
    ReturnValue b = m_audiograb_RPC.setSWGain_RPC(gain);
    return b;
}

ReturnValue AudioRecorder_nwc_yarp::setHWGain(double gain)
{
    ReturnValue b = m_audiograb_RPC.setHWGain_RPC(gain);
    return b;
}

ReturnValue AudioRecorder_nwc_yarp::startRecording()
{
    ReturnValue b = m_audiograb_RPC.startRecording_RPC();
    return b;
}

ReturnValue AudioRecorder_nwc_yarp::stopRecording()
{
    ReturnValue b = m_audiograb_RPC.stopRecording_RPC();
    return b;
}

ReturnValue AudioRecorder_nwc_yarp::resetRecordingAudioBuffer()
{
    ReturnValue b = m_audiograb_RPC.resetRecordingAudioBuffer_RPC();
    return b;
}

ReturnValue AudioRecorder_nwc_yarp::isRecording(bool& recording_enabled)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.isRecording_RPC();
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: isRecording()");
        return ret.ret;
    }
    recording_enabled = ret.isRecording;
    return ret.ret;
}

ReturnValue AudioRecorder_nwc_yarp::getRecordingAudioBufferMaxSize(yarp::sig::AudioBufferSize& size)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.getRecordingAudioBufferMaxSize_RPC();
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getRecordingAudioBufferMaxSize()");
        return ret.ret;
    }
    size = ret.bufsize;
    return ret.ret;
}

ReturnValue AudioRecorder_nwc_yarp::getRecordingAudioBufferCurrentSize(yarp::sig::AudioBufferSize& size)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.getRecordingAudioBufferCurrentSize_RPC();
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getRecordingAudioBufferCurrentSize()");
        return ret.ret;
    }
    size = ret.bufsize;
    return ret.ret;
}

ReturnValue AudioRecorder_nwc_yarp::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    if (m_useStream)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getSound() streaming version not yet implemented");
        return ReturnValue::return_code::return_value_error_not_implemented_by_device;
    }
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_audiograb_RPC.getSound_RPC(min_number_of_samples,max_number_of_samples, max_samples_timeout_s);
    if (!ret.ret)
    {
        yCError(AUDIORECORDER_NWC, "Unable to: getSound()");
        return ret.ret;
    }
    sound = ret.sound;
    return ret.ret;
}
