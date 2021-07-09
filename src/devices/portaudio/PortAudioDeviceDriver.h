/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef PortAudioDeviceDriverh
#define PortAudioDeviceDriverh

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <portaudio.h>
#include "PortAudioBuffer.h"

#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_DITHER_FLAG     (0)
#define DEFAULT_FRAMES_PER_BUFFER (512)
//#define DEFAULT_FRAMES_PER_BUFFER (1024)


class PortAudioDeviceDriverSettings
{
public:
    int rate;
    int samples;
    int playChannels;
    int recChannels;
    bool wantRead;
    bool wantWrite;
    int deviceNumber;
};

class streamThread :
        public yarp::os::Thread
{
   public:
   bool         something_to_play;
   bool         something_to_record;
   PaStream*    stream;
   void threadRelease() override;
   bool threadInit() override;
   void run() override;

   private:
   PaError      err;
   void handleError();
};

/**
 * @ingroup dev_impl_media
 *
 * \brief `portaudio`:  This device driver has been deprecated!
 * Please use `PortAudioPlayerDeviceDriver` or  `PortAudioRecorderDeviceDriver`
 *
 * Requires the PortAudio library (http://www.portaudio.com), at least v19.
 */
class PortAudioDeviceDriver :
        public yarp::dev::IAudioGrabberSound,
        public yarp::dev::IAudioRender,
        public yarp::dev::DeprecatedDeviceDriver
{
private:
    PaStreamParameters  inputParameters;
    PaStreamParameters  outputParameters;
    PaStream*           stream;
    PaError             err;
    circularDataBuffers dataBuffers;
    size_t              numSamples;
    size_t              numBytes;
    streamThread        pThread;

    PortAudioDeviceDriver(const PortAudioDeviceDriver&);

public:
    PortAudioDeviceDriver();

    virtual ~PortAudioDeviceDriver();

    bool open(yarp::os::Searchable& config) override;

    /**
     * Configures the device.
     *
     * rate: Sample rate to use, in Hertz.  Specify 0 to use a default.
     *
     * samples: Number of samples per call to getSound.  Specify
     * 0 to use a default.
     *
     * channels: Number of channels of input.  Specify
     * 0 to use a default.
     *
     * read: Should allow reading
     *
     * write: Should allow writing
     *
     * @return true on success
     */
    bool open(PortAudioDeviceDriverSettings& config);

    bool close() override;
    bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    bool renderSound(const yarp::sig::Sound& sound) override;
    bool startRecording() override;
    bool stopRecording() override;
    bool startPlayback() override;
    bool stopPlayback() override;
    bool isPlaying(bool& playback_enabled) override;
    bool isRecording(bool& recording_enabled) override;

    bool abortSound();
    bool immediateSound(const yarp::sig::Sound& sound);
    bool appendSound(const yarp::sig::Sound& sound);

    bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    bool resetPlaybackAudioBuffer() override;

    bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    bool resetRecordingAudioBuffer() override;

    bool setHWGain(double gain) override;
    bool setSWGain(double gain) override;

protected:
    void*   m_system_resource;
    size_t  m_numPlaybackChannels;
    size_t  m_numRecordChannels;
    int     m_frequency;
    bool    m_loopBack;
    bool    m_getSoundIsNotBlocking;

    PortAudioDeviceDriverSettings m_driverConfig;
    enum {RENDER_APPEND=0, RENDER_IMMEDIATE=1} renderMode;
    void handleError();
};

#endif
