/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>
#include <yarp/dev/PreciselyTimed.h>


namespace yarp
{
    namespace dev
    {
        class audioPlayerWrapper;
    }
}

#define DEFAULT_THREAD_PERIOD 0.02 //s

class yarp::dev::audioPlayerWrapper : public yarp::os::PeriodicThread,
                                      public yarp::dev::DeviceDriver,
                                      public yarp::dev::IMultipleWrapper,
                                      public yarp::os::PortReader
{
public:
    audioPlayerWrapper();
    ~audioPlayerWrapper();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    /**
      * Specify which sensor this thread has to read from.
      */
    bool attachAll(const PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IAudioRender *irend);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
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
    double m_period;
    bool   m_isDeviceOwned;

    bool initialize_YARP(yarp::os::Searchable &config);
    bool read(yarp::os::ConnectionReader& connection) override;


#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif // YARP_DEV_AUDIOPLAYERWRAPPER_H
