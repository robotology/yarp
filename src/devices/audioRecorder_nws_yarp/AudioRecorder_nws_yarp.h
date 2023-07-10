/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_AUDIORECORDERWRAPPER_H
#define YARP_DEV_AUDIORECORDERWRAPPER_H

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

#include "AudioRecorderServerImpl.h"

class AudioRecorderStatusThread;
class AudioRecorderDataThread;

/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `AudioRecorderWrapper`: A Wrapper which streams audio over the network, after grabbing it from a device
 * \section AudioRecorderWrapper_device_parameters Description of input parameters
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                                | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:--------------------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   /audioRecorderWrapper  | No                          | full name of the port opened by the device                                 | MUST start with a '/' character, xxx/audio:o and xxx/rpc suffixes are appended   |
 * | period         |      -         | int     | ms             |   20                     | No                          | period of the internal thread, in ms                                       | default 20ms |
 * | debug          |      -         | bool    | -              |   -                      | No                          | developers use only                                                        | |
 * | min_samples_over_network  | -   | int     | samples        |   11250                  | No                          | sends the network packet ifs n samples are collected AND the timeout is expired | the algorithm is implemented in AudioRecorderDeviceBase::getSound() method |
 * | max_samples_over_network  | -   | int     | samples        |   11250                  | No                          | sends the network packet as soon as n samples have been collected          | the algorithm is implemented in AudioRecorderDeviceBase::getSound() method |
 * | max_samples_timeout  |  -       | float   | s              |   1.0                    | No                          | timeout for sample collection                                              | the algorithm is implemented in AudioRecorderDeviceBase::getSound() method |
 * | start          |      -         | bool    | -              |   false                  | No                          | automatically activates the recording when the device is started           | if false, the recording is enabled via rpc port |
 * | send_sound_on_stop  |  -        | bool    | -              |   true                   | No                          | send the sound when the stop rpc is called, even if it does not met network size parameters | it will not send empty sounds |
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
*/
class AudioRecorder_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader
{
private:
    yarp::dev::PolyDriver          m_driver;
    yarp::dev::IAudioGrabberSound* m_mic = nullptr; //The microphone device
    yarp::os::Property             m_config;
    double                         m_period;
    yarp::os::Port                 m_rpcPort;
    yarp::os::Port                 m_streamingPort;
    yarp::os::Port                 m_statusPort;
    yarp::os::Stamp                m_stamp;
    size_t                         m_min_number_of_samples_over_network;
    size_t                         m_max_number_of_samples_over_network;
    double                         m_getSound_timeout;
    AudioRecorderStatusThread*     m_statusThread = nullptr;
    AudioRecorderDataThread*       m_dataThread =nullptr;
    bool                           m_debug_enabled = false;
    yarp::sig::Sound               m_snd;
    bool                           m_send_sound_on_stop = true;

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
class AudioRecorderStatusThread : public yarp::os::PeriodicThread
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
class AudioRecorderDataThread : public yarp::os::PeriodicThread
{
public:
    AudioRecorder_nws_yarp* m_ARW = nullptr;

public:
    AudioRecorderDataThread(AudioRecorder_nws_yarp* mi) : PeriodicThread(0.010), m_ARW(mi) {}

    bool threadInit() override { return true; }
    void threadRelease() override { return; }
    void run() override;

    bool sendSoundAndClear(yarp::sig::Sound& s);
};

#endif // YARP_DEV_AUDIORECORDERWRAPPER_H
