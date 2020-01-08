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


class AudioRecorderWrapper :
        public yarp::dev::DeviceDriver,
        private yarp::os::PeriodicThread,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
private:
    yarp::dev::PolyDriver          m_driver;
    yarp::dev::IAudioGrabberSound* m_mic; //The microphone device
    double                         m_period;
    yarp::os::Port                 m_rpcPort;
    yarp::os::Port                 m_streamingPort;
    yarp::os::Stamp                m_stamp;
    size_t                         m_min_number_of_samples_over_network;
    size_t                         m_max_number_of_samples_over_network;
    double                         m_getSound_timeout;
    bool                           m_isDeviceOwned;
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

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_AUDIORECORDERWRAPPER_H
