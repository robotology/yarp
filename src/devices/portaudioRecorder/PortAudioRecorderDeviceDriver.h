/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef PortAudioRecorderDeviceDriverh
#define PortAudioRecorderDeviceDriverh

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <portaudio.h>

#include "PortAudioRecorderDeviceDriver_ParamsParser.h"

/**
* @ingroup dev_impl_media
*
* \brief `portaudioRecorder`: A device driver for an audio source wrapped by PortAudio library.
* Requires the PortAudio library (http://www.portaudio.com), at least v19.
* Only 16bits sample format is currently supported by this device.
* This device driver derives from AudioRecorderDeviceBase base class. Please check its documentation for additional details.
*
* Parameters required by this device are shown in class: PortAudioRecorderDeviceDriver_ParamsParser and AudioRecorderDeviceBase
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class PortAudioRecorderDeviceDriver :
        public yarp::dev::AudioRecorderDeviceBase,
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread,
        public PortAudioRecorderDeviceDriver_ParamsParser
{
private:
    PaStreamParameters  m_inputParameters;
    PaStream*           m_stream;
    PaError             m_err;

public:
    PortAudioRecorderDeviceDriver();
    PortAudioRecorderDeviceDriver(const PortAudioRecorderDeviceDriver&) = delete;
    PortAudioRecorderDeviceDriver(PortAudioRecorderDeviceDriver&&) = delete;
    PortAudioRecorderDeviceDriver& operator=(const PortAudioRecorderDeviceDriver&) = delete;
    PortAudioRecorderDeviceDriver& operator=(PortAudioRecorderDeviceDriver&&) = delete;
    ~PortAudioRecorderDeviceDriver() override;

public: //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

public: //AudioRecorderDeviceBase(IAudioGrabberSound)
    yarp::dev::ReturnValue startRecording() override;
    yarp::dev::ReturnValue stopRecording() override;
    yarp::dev::ReturnValue setHWGain(double gain) override;

public: //Thread
    void threadRelease() override;
    bool threadInit() override;
    void run() override;

protected:
    void*   m_system_resource;
    void handleError();
};

#endif
