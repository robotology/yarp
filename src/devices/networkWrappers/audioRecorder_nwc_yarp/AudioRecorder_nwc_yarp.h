/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_AUDIORECORDER_NWC_YARP_H
#define YARP_DEV_AUDIORECORDER_NWC_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IAudioGrabberSound.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/PolyDriver.h>

#include "IAudioGrabberMsgs.h"
#include <mutex>
#include "AudioRecorder_nwc_yarp_ParamsParser.h"

class AudioRecorder_InputPortProcessor : public yarp::os::BufferedPort<yarp::sig::Sound>
{
    yarp::sig::Sound lastSound;
    yarp::os::Stamp lastStamp;
    std::mutex mutex;

public:
    AudioRecorder_InputPortProcessor();

    using yarp::os::BufferedPort<yarp::sig::Sound>::onRead;
    void onRead(yarp::sig::Sound& v) override;

    inline bool getLast(yarp::sig::Sound& data, yarp::os::Stamp& stmp);
};

/**
* @ingroup dev_impl_nwc_yarp
*
* \brief `audioRecoder_nwc_yarp`: The client side of any IAudioGrabberSound capable device.
* \section AudioRecorder_nwc_yarp_device_parameters Description of input parameters
* Please note that the getSound() method is currently implemented as a RPC call and streaming functions are not yet implemented.
*
* Parameters required by this device are shown in class: AudioRecorder_nwc_yarp_ParamsParser
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/
class AudioRecorder_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IAudioGrabberSound,
        public AudioRecorder_nwc_yarp_ParamsParser
{
protected:
    AudioRecorder_InputPortProcessor  m_inputPort;
    yarp::os::Port      m_rpcPort;
    IAudioGrabberMsgs   m_audiograb_RPC;
    std::mutex          m_mutex;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IAudioGrabberSound */
    virtual yarp::dev::ReturnValue getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    virtual yarp::dev::ReturnValue startRecording() override;
    virtual yarp::dev::ReturnValue stopRecording() override;
    virtual yarp::dev::ReturnValue isRecording(bool& recording_enabled) override;
    virtual yarp::dev::ReturnValue getRecordingAudioBufferMaxSize(yarp::sig::AudioBufferSize& size) override;
    virtual yarp::dev::ReturnValue getRecordingAudioBufferCurrentSize(yarp::sig::AudioBufferSize& size) override;
    virtual yarp::dev::ReturnValue resetRecordingAudioBuffer() override;
    virtual yarp::dev::ReturnValue setSWGain(double gain) override;
    virtual yarp::dev::ReturnValue setHWGain(double gain) override;

};

#endif // YARP_DEV_AUDIORECORDER_NWC_YARP_H
