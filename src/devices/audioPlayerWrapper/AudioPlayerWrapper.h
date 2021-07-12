/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_AUDIOPLAYERWRAPPER_H
#define YARP_DEV_AUDIOPLAYERWRAPPER_H

 //#include <list>
#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/api.h>
#include <yarp/dev/IPreciselyTimed.h>

/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `AudioPlayerWrapper`: A Wrapper which receives audio streams from a network port and sends it to device for playback
 * \section AudioPlayerWrapper_device_parameters Description of input parameters
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value           | Required                    | Description                                                                | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-----------------------:|:--------------------------: |:--------------------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   /audioPlayerWrapper   | No                          | full name of the port opened by the device                                 | MUST start with a '/' character, xxx/audio:i, xxx/rpc:i, xxx/status:o, ports are opened  |
 * | period         |      -         | int     | ms             |   20                    | No                          | period of the internal thread, in ms                                       | default 20ms |
 * | debug          |      -         | bool    | -              |   -                     | No                          | developers use only                                                        | |
 * | playback_network_buffer_size  | - | float | s              |   5.0                   | No                          | size of the audio buffer in seconds, increasing this value to robustify the real-time audio stream (it will increase latency too) | Audio playback will start when the buffer is full |
 * | start          |      -         | bool    | -              |   false                 | No                          | automatically activates the playback when the device is started            | if false, the playback is enabled via rpc port |
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
*/

class AudioPlayerWrapper :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{

    struct scheduled_sound_type
    {
        double scheduled_time=0;
        yarp::sig::Sound sound_data;
    };

public:
    AudioPlayerWrapper();
    AudioPlayerWrapper(const AudioPlayerWrapper&) = delete;
    AudioPlayerWrapper(AudioPlayerWrapper&&) = delete;
    AudioPlayerWrapper& operator=(const AudioPlayerWrapper&) = delete;
    AudioPlayerWrapper& operator=(AudioPlayerWrapper&&) = delete;

    ~AudioPlayerWrapper() override;

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    /**
     * Specify which sensor this thread has to read from.
     */
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IAudioRender *irend);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    yarp::dev::PolyDriver m_driver;

    std::string m_rpcPortName;
    yarp::os::Port  m_rpcPort;
    std::string  m_audioInPortName;
    yarp::os::BufferedPort<yarp::sig::Sound> m_audioInPort;
    std::string  m_statusPortName;
    yarp::os::Port m_statusPort;

    yarp::dev::IAudioRender *m_irender = nullptr;
    yarp::os::Stamp m_lastStateStamp;
    yarp::dev::AudioBufferSize m_current_buffer_size;
    yarp::dev::AudioBufferSize m_max_buffer_size;
    std::queue<scheduled_sound_type> m_sound_buffer;
    double m_period;
    double m_buffer_delay;
    bool   m_isDeviceOwned = false;
    bool   m_debug_enabled = false;
    bool   m_isPlaying = false;

    bool initialize_YARP(yarp::os::Searchable &config);
    bool read(yarp::os::ConnectionReader& connection) override;

};

#endif // YARP_DEV_AUDIOPLAYERWRAPPER_H
