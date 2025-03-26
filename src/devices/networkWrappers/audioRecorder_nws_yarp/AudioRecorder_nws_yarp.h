/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_AUDIORECORDER_NWS_YARP_H
#define YARP_DEV_AUDIORECORDER_NWS_YARP_H

#include <cstdio>


#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Log.h>

#include <list>

#include "AudioRecorderServerImpl.h"
#include "AudioRecorder_nws_yarp_ParamsParser.h"

/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `AudioRecorder_nws_yarp`: A Wrapper which streams audio over the network, after grabbing it from a device.
 * \section AudioRecorder_nws_yarp_device_parameters Description of input parameters
 *
 * Parameters required by this device are shown in class: AudioRecorder_nws_yarp_ParamsParser
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
*/
class AudioRecorder_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public AudioRecorder_nws_yarp_ParamsParser
{
private:
    class AudioRecorderStatusThread;
    class AudioRecorderDataThread;

    yarp::dev::PolyDriver          m_driver;
    yarp::dev::IAudioGrabberSound* m_mic = nullptr; //The microphone device
    yarp::os::Property             m_config;
    yarp::os::Port                 m_rpcPort;
    yarp::os::Port                 m_streamingPort;
    yarp::os::Port                 m_statusPort;
    yarp::os::Stamp                m_stamp;
    std::unique_ptr<AudioRecorderStatusThread> m_statusThread;
    std::unique_ptr<AudioRecorderDataThread> m_dataThread;
    const bool                     m_debug_enabled = false;
    std::list <yarp::sig::Sound>   m_listofsnds;

private:
    double                         m_debug_last_time=0;

private:
    //thrift
    IAudioGrabberRPCd              m_RPC;

public:
    /**
     * Constructor.
     */
    AudioRecorder_nws_yarp();
    AudioRecorder_nws_yarp(const AudioRecorder_nws_yarp&) = delete;
    AudioRecorder_nws_yarp(AudioRecorder_nws_yarp&&) = delete;
    AudioRecorder_nws_yarp& operator=(const AudioRecorder_nws_yarp&) = delete;
    AudioRecorder_nws_yarp& operator=(AudioRecorder_nws_yarp&&) = delete;

    ~AudioRecorder_nws_yarp() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;

    bool read(yarp::os::ConnectionReader& connection) override;
    friend class AudioRecorderStatusThread;
    friend class AudioRecorderDataThread;
};

//----------------------------------------------------------------
class AudioRecorder_nws_yarp::AudioRecorderStatusThread : public yarp::os::PeriodicThread
{
public:
    AudioRecorder_nws_yarp* m_ARW = nullptr;

public:
    AudioRecorderStatusThread(AudioRecorder_nws_yarp* mi) : PeriodicThread(0.010), m_ARW(mi) {}

    bool threadInit() override { return true; }
    void threadRelease() override { return; }
    void run() override;
};

//----------------------------------------------------------------
class AudioRecorder_nws_yarp::AudioRecorderDataThread : public yarp::os::PeriodicThread
{
public:
    AudioRecorder_nws_yarp* m_ARW = nullptr;

public:
    AudioRecorderDataThread(AudioRecorder_nws_yarp* mi) : PeriodicThread(0.010), m_ARW(mi) {}

    bool threadInit() override { return true; }
    void threadRelease() override { return; }
    void run() override;

    bool sendSound(yarp::sig::Sound& s);
};

#endif // YARP_DEV_AUDIORECORDER_NWS_YARP_H
