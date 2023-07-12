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

class InputPortProcessor : public yarp::os::BufferedPort<yarp::sig::Sound>
{
    yarp::sig::Sound lastSound;
    yarp::os::Stamp  lastStamp;
    std::mutex mutex;

public:
    InputPortProcessor();

    using yarp::os::BufferedPort<yarp::sig::Sound>::onRead;
    void onRead(yarp::sig::Sound&v) override;

    inline bool getLast(yarp::sig::Sound& data, yarp::os::Stamp &stmp);
};

/**
* @ingroup dev_impl_network_clients
*
* \brief `audioRecoder_nwc_yarp`: The client side of any IAudioGrabberSound capable device.
* \section AudioRecorder_nwc_yarp_device_parameters Description of input parameters
* Please note that the getSound() method is currently implemented as a RPC call and streaming functions are not yet implemented.
*
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the audioRecoder_nwc_yarp device.                |       |
* | remote         |      -         | string  | -              |   -           | Yes          | Full port name of the port opened on the server side, to which the audioRecoder_nwc_yarp connects to.    |     |
* | carrier        |     -          | string  | -              | tcp           | No           | The carrier used for the streaming connection with the server.    |       |
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/
class AudioRecorder_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IAudioGrabberSound
{
protected:
    InputPortProcessor  m_inputPort;
    yarp::os::Port      m_rpcPort;
    IAudioGrabberMsgs   m_audiograb_RPC;
    std::mutex          m_mutex;
    bool                m_useStreaming = false;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* IAudioGrabberSound */
    virtual bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    virtual bool startRecording() override;
    virtual bool stopRecording() override;
    virtual bool isRecording(bool& recording_enabled) override;
    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetRecordingAudioBuffer() override;
    virtual bool setSWGain(double gain) override;
    virtual bool setHWGain(double gain) override;

};

#endif // YARP_DEV_AUDIORECORDER_NWC_YARP_H
