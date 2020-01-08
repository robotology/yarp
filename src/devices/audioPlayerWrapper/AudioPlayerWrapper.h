/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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


class AudioPlayerWrapper :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{

    struct scheduled_sound_type
    {
        double scheduled_time;
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
    yarp::os::BufferedPort<yarp::os::Bottle>  m_statusPort;

    yarp::dev::IAudioRender *m_irender;
    yarp::os::Stamp m_lastStateStamp;
    yarp::dev::AudioBufferSize m_current_buffer_size;
    yarp::dev::AudioBufferSize m_max_buffer_size;
    std::queue<scheduled_sound_type> m_sound_buffer;
    double m_period;
    double m_buffer_delay;
    bool   m_isDeviceOwned;
    bool   m_debug_enabled;

    bool initialize_YARP(yarp::os::Searchable &config);
    bool read(yarp::os::ConnectionReader& connection) override;

};

#endif // YARP_DEV_AUDIOPLAYERWRAPPER_H
