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
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/api.h>

#include "AudioPlayerWrapper_ParamsParser.h"

/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `AudioPlayerWrapper`: A Wrapper which receives audio streams from a network port and sends it to device for playback
 * \section AudioPlayerWrapper_device_parameters Description of input parameters
 *
 * Parameters required by this device are shown in class: AudioPlayerWrapper_ParamsParser
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
*/

class AudioPlayerWrapper :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public AudioPlayerWrapper_ParamsParser
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
    bool attach(yarp::dev::PolyDriver *driver) override;
    bool detach() override;

    bool threadInit() override;
    void afterStart(bool success) override;
    void threadRelease() override;
    void run() override;

private:
    yarp::os::Port  m_rpcPort;
    yarp::os::BufferedPort<yarp::sig::Sound> m_audioInPort;
    yarp::os::Port m_statusPort;

    yarp::dev::IAudioRender *m_irender = nullptr;
    yarp::os::Stamp m_lastStateStamp;
    yarp::dev::AudioBufferSize m_current_buffer_size;
    yarp::dev::AudioBufferSize m_max_buffer_size;
    std::queue<scheduled_sound_type> m_sound_buffer;

    bool   m_isPlaying = false;

    bool read(yarp::os::ConnectionReader& connection) override;

};

#endif // YARP_DEV_AUDIOPLAYERWRAPPER_H
