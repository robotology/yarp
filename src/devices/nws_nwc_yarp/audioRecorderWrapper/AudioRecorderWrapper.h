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
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Log.h>

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
 * | min_samples_over_network  | -   | int     | samples        |   11250                  | No                          | sends the network packet ifs n samples are collected AND the timeout is expired | the algorithm is implemented in AudioRecorderDeviceBase |
 * | max_samples_over_network  | -   | int     | samples        |   11250                  | No                          | sends the network packet as soon as n samples have been collected          | the algorithm is implemented in AudioRecorderDeviceBase |
 * | max_samples_timeout  |  -       | float   | s              |   1.0                    | No                          | timeout for sample collection                                              | the algorithm is implemented in AudioRecorderDeviceBase |
 * | start          |      -         | bool    | -              |   false                  | No                          | automatically activates the recording when the device is started           | if false, the recording is enabled via rpc port |
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
*/
class AudioRecorderWrapper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
private:
    yarp::dev::PolyDriver          m_driver;
    yarp::dev::IAudioGrabberSound* m_mic = nullptr; //The microphone device
    double                         m_period;
    yarp::os::Port                 m_rpcPort;
    yarp::os::Port                 m_streamingPort;
    yarp::os::Port                 m_statusPort;
    yarp::os::Stamp                m_stamp;
    size_t                         m_min_number_of_samples_over_network;
    size_t                         m_max_number_of_samples_over_network;
    yarp::dev::AudioBufferSize     m_current_buffer_size;
    yarp::dev::AudioBufferSize     m_max_buffer_size;
    double                         m_getSound_timeout;
    bool                           m_isDeviceOwned =false;
    bool                           m_isRecording=false;
    AudioRecorderStatusThread*     m_statusThread = nullptr;
    AudioRecorderDataThread*       m_dataThread =nullptr;
    bool                           m_debug_enabled = false;

private:
    double                         m_debug_last_time=0;

public:
    /**
     * Constructor.
     */
    AudioRecorderWrapper();
    AudioRecorderWrapper(const AudioRecorderWrapper&) = delete;
    AudioRecorderWrapper(AudioRecorderWrapper&&) = delete;
    AudioRecorderWrapper& operator=(const AudioRecorderWrapper&) = delete;
    AudioRecorderWrapper& operator=(AudioRecorderWrapper&&) = delete;

    ~AudioRecorderWrapper() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IAudioGrabberSound *igrab);
    void detach();

    bool read(yarp::os::ConnectionReader& connection) override;
    friend class AudioRecorderStatusThread;
    friend class AudioRecorderDataThread;
};

//----------------------------------------------------------------
class AudioRecorderStatusThread : public yarp::os::PeriodicThread
{
public:
    AudioRecorderWrapper* m_ARW = nullptr;

public:
    AudioRecorderStatusThread(AudioRecorderWrapper* mi) : PeriodicThread(0.010), m_ARW(mi) {}

    bool threadInit() override { return true; }
    void threadRelease() override { return; }
    void run() override;
};

//----------------------------------------------------------------
class AudioRecorderDataThread : public yarp::os::PeriodicThread
{
public:
    AudioRecorderWrapper* m_ARW = nullptr;

public:
    AudioRecorderDataThread(AudioRecorderWrapper* mi) : PeriodicThread(0.010), m_ARW(mi) {}

    bool threadInit() override { return true; }
    void threadRelease() override { return; }
    void run() override;
};

#endif // YARP_DEV_AUDIORECORDERWRAPPER_H
