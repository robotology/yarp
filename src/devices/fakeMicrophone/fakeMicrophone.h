/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

namespace yarp{
    namespace dev{
        class fakeMicrophone;
    }
}

#define DEFAULT_PERIOD 0.01   //s

/**
* \brief `fakeMicrophone` : fake device implementing the IAudioGrabberSound device interface to play sound
*
*/
class yarp::dev::fakeMicrophone :  public DeviceDriver,
                            public yarp::dev::IAudioGrabberSound,
                            public yarp::os::PeriodicThread
{
public:
    fakeMicrophone();
    ~fakeMicrophone();

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    virtual bool getSound(yarp::sig::Sound& sound)  override;
    virtual bool startRecording()  override;
    virtual bool stopRecording()  override;
    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::audio_buffer_size& size)  override;
    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::audio_buffer_size& size)  override;
    virtual bool resetRecordingAudioBuffer() override;

private:
    bool threadInit() override;
    void run() override;

    bool             m_isRecording;
    yarp::os::Mutex  m_mutex;
    yarp::sig::Sound m_audioFile;

    size_t m_cfg_numSamples;
    size_t m_cfg_numChannels;
    size_t m_cfg_frequency;
    size_t m_cfg_bytesPerSample;

    size_t      m_audioFilePointer;
    std::string m_audio_filename;
    size_t      m_bpnt;
    circularAudioBuffer  *m_inputBuffer;
    bool        m_getSoundIsNotBlocking;
};
