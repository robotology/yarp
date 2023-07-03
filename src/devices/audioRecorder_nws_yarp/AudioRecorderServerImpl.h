/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_AUDIORECORDERSERVERIMPL_H
#define YARP_DEV_AUDIORECORDERSERVERIMPL_H

#include "IAudioGrabberMsgs.h"
#include <yarp/dev/IAudioGrabberSound.h>
#include <yarp/os/Stamp.h>

class IAudioGrabberRPCd : public IAudioGrabberMsgs
{
    private:
    yarp::dev::IAudioGrabberSound* m_igrab = nullptr;
    std::mutex                     m_mutex;

    public:
    void setInterface(yarp::dev::IAudioGrabberSound* _iaudiograb) { m_igrab = _iaudiograb; }

    virtual bool setHWGain_RPC(const double gain) override;
    virtual bool setSWGain_RPC(const double gain) override;
    virtual bool resetRecordingAudioBuffer_RPC() override;
    virtual bool startRecording_RPC() override;
    virtual bool stopRecording_RPC() override;
    virtual return_isRecording isRecording_RPC() override;
    virtual return_getSound getSound(const size_t min_number_of_samples, const size_t max_number_of_samples, const double max_samples_timeout_s) override;
    virtual return_getRecordingAudioBufferMaxSize getRecordingAudioBufferMaxSize() override;
    virtual return_getRecordingAudioBufferCurrentSize getRecordingAudioBufferCurrentSize() override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_AUDIORECORDERSERVERIMPL_H
