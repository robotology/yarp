/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "PortAudioRecorderDeviceDriver.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <yarp/dev/api.h>

#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

#define SLEEP_TIME 0.005f

#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#elif 1
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define SAMPLE_UNSIGNED
#endif

namespace {
YARP_LOG_COMPONENT(PORTAUDIORECORDER, "yarp.devices.portaudioRecorder")
}

#define DEFAULT_FRAMES_PER_BUFFER (512)

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int bufferIOCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    CircularAudioBuffer_16t *recdata = (CircularAudioBuffer_16t*)(userData);
    size_t num_rec_channels     = recdata->getMaxSize().getChannels();
    int finished = paComplete;

    if (1)
    {
        const auto* rptr = (const SAMPLE*)inputBuffer;
        size_t framesToCalc;
        size_t framesLeft = (recdata->getMaxSize().getSamples()* recdata->getMaxSize().getChannels()) -
                            (recdata->size().getSamples()      * recdata->size().getChannels());

        YARP_UNUSED(outputBuffer); // just to prevent unused variable warnings
        YARP_UNUSED(timeInfo);
        YARP_UNUSED(statusFlags);
        YARP_UNUSED(userData);

        if( framesLeft/ num_rec_channels < framesPerBuffer )
        {
            framesToCalc = framesLeft/ num_rec_channels;
#ifdef STOP_REC_ON_EMPTY_BUFFER
            //if we return paComplete, then the callback is not called anymore.
            //method Pa_IsStreamActive() will return 1.
            //user needs to call Pa_StopStream() before starting a new recording session
            finished = paComplete;
#else
            finished = paContinue;
#endif
        }
        else
        {
            framesToCalc = framesPerBuffer;
            //if we return paContinue, then the callback will be invoked again later
            //method Pa_IsStreamActive() will return 0
            finished = paContinue;
        }

        if( inputBuffer == nullptr )
        {
            for( size_t i=0; i<framesToCalc; i++ )
            {
                for (size_t j=0; j < num_rec_channels; j++)
                {
                    recdata->write(0);
                }
            }
        }
        else
        {
            for( size_t i=0; i<framesToCalc; i++ )
            {
                for (size_t j = 0; j < num_rec_channels; j++)
                {
                    recdata->write(*rptr++);
                }
            }
        }
        return finished;
    }

    yCError(PORTAUDIORECORDER, "No write operations requested, aborting");
    return paAbort;
}

PortAudioRecorderDeviceDriver::PortAudioRecorderDeviceDriver() :
    m_stream(nullptr),
    m_err(paNoError),
    m_system_resource(nullptr)
{
    memset(&m_inputParameters, 0, sizeof(PaStreamParameters));
}

PortAudioRecorderDeviceDriver::~PortAudioRecorderDeviceDriver()
{
    close();
}


bool PortAudioRecorderDeviceDriver::open(yarp::os::Searchable& config)
{
    bool b = parseParams(config);
    if (!b) { return false; }

    b = configureRecorderAudioDevice(config.findGroup("AUDIO_BASE"),"portaudioRecorder");
    if (!b) { return false; }

    m_err = Pa_Initialize();
    if(m_err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "portaudio system failed to initialize");
        return false;
    }

    m_inputParameters.device = (m_audio_device_id == -1) ? Pa_GetDefaultInputDevice() : m_audio_device_id;
    m_inputParameters.channelCount = static_cast<int>(m_audiorecorder_cfg.numChannels);
    m_inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    m_inputParameters.hostApiSpecificStreamInfo = nullptr;

    const PaDeviceInfo* devinfo = Pa_GetDeviceInfo(m_inputParameters.device);
    std::string devname = "unknown";
    if (devinfo != nullptr)
    {
        m_inputParameters.suggestedLatency = devinfo->defaultLowInputLatency;
        devname = devinfo->name;
    }
    yCInfo(PORTAUDIORECORDER, "Selected device: number: %d, name: %s", m_inputParameters.device, devname.c_str());

    m_err = Pa_OpenStream(
              &m_stream,
              &m_inputParameters,
              nullptr,
              (double)(m_audiorecorder_cfg.frequency),
              m_driver_frame_size,
              paClipOff,
              bufferIOCallback,
              m_inputBuffer);

    if(m_err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "An error occurred while using the portaudio stream" );
        yCError(PORTAUDIORECORDER, "Error number: %d", m_err );
        yCError(PORTAUDIORECORDER, "Error message: %s", Pa_GetErrorText(m_err ) );
    }

    //start the thread
    bool ret = this->start();
    YARP_UNUSED(ret);

    return (m_err==paNoError);
}

void PortAudioRecorderDeviceDriver::handleError()
{
    //Pa_Terminate();
    m_inputBuffer->clear();

    if(m_err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "An error occurred while using the portaudio stream" );
        yCError(PORTAUDIORECORDER, "Error number: %d", m_err );
        yCError(PORTAUDIORECORDER, "Error message: %s", Pa_GetErrorText(m_err ) );
    }
}

bool PortAudioRecorderDeviceDriver::close()
{
    this->stop();
    if (m_stream != nullptr)
    {
        m_err = Pa_CloseStream(m_stream );
        if(m_err != paNoError )
        {
            yCError(PORTAUDIORECORDER,  "An error occurred while closing the portaudio stream" );
            yCError(PORTAUDIORECORDER,  "Error number: %d", m_err );
            yCError(PORTAUDIORECORDER,  "Error message: %s", Pa_GetErrorText(m_err ) );
        }
    }

    if (this->m_inputBuffer != nullptr)
    {
        delete this->m_inputBuffer;
        this->m_inputBuffer = nullptr;
    }

    return (m_err==paNoError);
}

ReturnValue PortAudioRecorderDeviceDriver::startRecording()
{
    AudioRecorderDeviceBase::startRecording();
    m_err = Pa_StartStream(m_stream );
    if(m_err < 0 ) {handleError(); return ReturnValue::return_code::return_value_error_method_failed;}
    yCInfo(PORTAUDIORECORDER) << "started recording";
    return ReturnValue_ok;
}

ReturnValue PortAudioRecorderDeviceDriver::setHWGain(double gain)
{
    yCInfo(PORTAUDIORECORDER) << "not yet implemented recording";
    return ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

ReturnValue PortAudioRecorderDeviceDriver::stopRecording()
{
    AudioRecorderDeviceBase::stopRecording();
    m_err = Pa_StopStream(m_stream );
    if(m_err < 0 ) {handleError(); return ReturnValue::return_code::return_value_error_method_failed;}
    yCInfo(PORTAUDIORECORDER) << "stopped recording";
    return ReturnValue_ok;
}

void PortAudioRecorderDeviceDriver::threadRelease()
{
}

bool PortAudioRecorderDeviceDriver::threadInit()
{
    return true;
}

void PortAudioRecorderDeviceDriver::run()
{
    while(this->isStopping()==false)
    {
        //The status of the buffer (i.e. the return value of Pa_IsStreamActive() depends on the returned value
        //of the callback function bufferIOCallback() which may return paContinue or paComplete.
        if (m_recording_enabled)
        {
            m_err = Pa_IsStreamActive(m_stream);
            if (m_err < 0)
            {
                handleError();
                yCError(PORTAUDIORECORDER) << "Unhandled error. Calling abortSound()";
                //abortSound();
                continue;
            }
            if (m_err == 1)
            {
                //already doing something
            }
            else if (m_err == 0)
            {
                //the recording is stopped
            }
        }

        yarp::os::Time::delay(SLEEP_TIME);
    }
    return;
}
