/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "AudioRecorderServerImpl.h"

/*! \file AudioRecorderServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;


namespace {
YARP_LOG_COMPONENT(AUDIOGRAB_RPC, "yarp.device.map2D_nws_yarp.IAudioGrabberRPCd")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(AUDIOGRAB_RPC, "Invalid interface"); return false;}}

bool IAudioGrabberRPCd::setHWGain_RPC(const double gain)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_igrab == nullptr) { yCError(AUDIOGRAB_RPC, "Invalid interface"); return false; }}

    if (!m_igrab->setHWGain(gain))
    {
        yCError(AUDIOGRAB_RPC, "Unable to setHWGain");
        return false;
    }
    return true;
}

bool IAudioGrabberRPCd::setSWGain_RPC(const double gain)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_igrab == nullptr) { yCError(AUDIOGRAB_RPC, "Invalid interface"); return false; }}

    if (!m_igrab->setSWGain(gain))
    {
        yCError(AUDIOGRAB_RPC, "Unable to setSWGain");
        return false;
    }
    return true;
}

bool IAudioGrabberRPCd::resetRecordingAudioBuffer_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_igrab == nullptr) { yCError(AUDIOGRAB_RPC, "Invalid interface"); return false; }}

    if (!m_igrab->resetRecordingAudioBuffer())
    {
        yCError(AUDIOGRAB_RPC, "Unable to resetRecordingAudioBuffer");
        return false;
    }
    return true;
}

bool IAudioGrabberRPCd::startRecording_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_igrab == nullptr) { yCError(AUDIOGRAB_RPC, "Invalid interface"); return false; }}

    if (!m_igrab->startRecording())
    {
        yCError(AUDIOGRAB_RPC, "Unable to startRecording");
        return false;
    }
    return true;
}

bool IAudioGrabberRPCd::stopRecording_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_igrab == nullptr) { yCError(AUDIOGRAB_RPC, "Invalid interface"); return false; }}

    if (!m_igrab->stopRecording())
    {
        yCError(AUDIOGRAB_RPC, "Unable to stopRecording");
        return false;
    }
    return true;
}

return_isRecording IAudioGrabberRPCd::isRecording_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_isRecording ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret=false;
        return ret;
    }

    bool recording_enabled;
    if (!m_igrab->isRecording(recording_enabled))
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = false;
        return ret;
    }

    ret.ret = true;
    ret.isRecording= recording_enabled;
    return ret;
}

return_getSound IAudioGrabberRPCd::getSound(const size_t min_number_of_samples, const size_t max_number_of_samples, const double max_samples_timeout_s)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getSound ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = false;
        return ret;
    }

    if (!m_igrab->getSound(ret.sound, min_number_of_samples, max_number_of_samples, max_samples_timeout_s))
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = false;
        return ret;
    }

    ret.ret = true;
    return ret;
}

return_getRecordingAudioBufferMaxSize IAudioGrabberRPCd::getRecordingAudioBufferMaxSize()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getRecordingAudioBufferMaxSize ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = false;
        return ret;
    }

    if (!m_igrab->getRecordingAudioBufferMaxSize(ret.bufsize))
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = false;
        return ret;
    }

    ret.ret = true;
    return ret;
}

return_getRecordingAudioBufferCurrentSize IAudioGrabberRPCd::getRecordingAudioBufferCurrentSize()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getRecordingAudioBufferCurrentSize ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = false;
        return ret;
    }

    if (!m_igrab->getRecordingAudioBufferCurrentSize(ret.bufsize))
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = false;
        return ret;
    }

    ret.ret = true;
    return ret;
}
