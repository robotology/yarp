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

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(AUDIOGRAB_RPC, "Invalid interface"); return ReturnValue::return_code::return_value_error_not_ready;}}

ReturnValue IAudioGrabberRPCd::setHWGain_RPC(const double gain)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    CHECK_POINTER(m_igrab)

    auto ret = m_igrab->setHWGain(gain);
    if (!ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to setHWGain");
        return ret;
    }
    return ret;
}

ReturnValue IAudioGrabberRPCd::setSWGain_RPC(const double gain)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    CHECK_POINTER(m_igrab)

    auto ret = m_igrab->setSWGain(gain);
    if (!ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to setSWGain");
        return ret;
    }
    return ret;
}

ReturnValue IAudioGrabberRPCd::resetRecordingAudioBuffer_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    CHECK_POINTER(m_igrab)

    auto ret = m_igrab->resetRecordingAudioBuffer();
    if (!ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to resetRecordingAudioBuffer");
        return ret;
    }
    return ret;
}

ReturnValue IAudioGrabberRPCd::startRecording_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    CHECK_POINTER(m_igrab)

    auto ret = m_igrab->startRecording();
    if (!ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to startRecording");
        return ret;
    }
    return ret;
}

ReturnValue IAudioGrabberRPCd::stopRecording_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    CHECK_POINTER(m_igrab)

    auto ret = m_igrab->stopRecording();
    if (!ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to stopRecording");
        return ret;
    }
    return ret;
}

return_isRecording IAudioGrabberRPCd::isRecording_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_isRecording ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    bool recording_enabled=false;
    auto rec_ret = m_igrab->isRecording(recording_enabled);
    if (!rec_ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = rec_ret;
        return ret;
    }

    ret.ret = ReturnValue_ok;
    ret.isRecording= recording_enabled;
    return ret;
}

return_getSound IAudioGrabberRPCd::getSound_RPC(const size_t min_number_of_samples, const size_t max_number_of_samples, const double max_samples_timeout_s)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getSound ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_igrab->getSound(ret.sound, min_number_of_samples, max_number_of_samples, max_samples_timeout_s);
    if (!ret.ret)
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        return ret;
    }

    ret.ret = ReturnValue_ok;
    return ret;
}

return_getRecordingAudioBufferMaxSize IAudioGrabberRPCd::getRecordingAudioBufferMaxSize_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getRecordingAudioBufferMaxSize ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ReturnValue r2 = m_igrab->getRecordingAudioBufferMaxSize(ret.bufsize);
    if (!r2)
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = r2;
        return ret;
    }

    ret.ret = ReturnValue_ok;
    return ret;
}

return_getRecordingAudioBufferCurrentSize IAudioGrabberRPCd::getRecordingAudioBufferCurrentSize_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getRecordingAudioBufferCurrentSize ret;
    if (m_igrab == nullptr)
    {
        yCError(AUDIOGRAB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ReturnValue r2 = m_igrab->getRecordingAudioBufferCurrentSize(ret.bufsize);
    if (!r2)
    {
        yCError(AUDIOGRAB_RPC, "Unable to evaluate isRecording");
        ret.ret = r2;
        return ret;
    }

    ret.ret = ReturnValue_ok;
    return ret;
}
