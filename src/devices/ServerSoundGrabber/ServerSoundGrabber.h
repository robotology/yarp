/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006 Julio Gomes
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERVERSOUNDGRABBER_H
#define YARP_DEV_SERVERSOUNDGRABBER_H

#include <yarp/os/Port.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>


/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `ServerSoundGrabber`: Export a microphone device streaming sound
 * samples
 */
class ServerSoundGrabber :
        public yarp::dev::DeprecatedDeviceDriver,
        private yarp::os::Thread,
        public yarp::os::PortReader
{
private:
    yarp::os::Stamp stamp;
    yarp::dev::PolyDriver poly;
    yarp::dev::IAudioGrabberSound* mic{nullptr}; //The microphone device
    yarp::os::Port rpcPort;
    yarp::os::Port streamingPort;
#ifdef DEBUG_TIME_SPENT
    double last_time;
#endif
public:
    ServerSoundGrabber();
    ServerSoundGrabber(const ServerSoundGrabber&) = delete;
    ServerSoundGrabber(ServerSoundGrabber&&) = delete;
    ServerSoundGrabber& operator=(const ServerSoundGrabber&) = delete;
    ServerSoundGrabber& operator=(ServerSoundGrabber&&) = delete;

    ~ServerSoundGrabber() override;


    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "fakeFrameGrabber"). </TD></TR>
     * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    void run() override;

    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_SERVERSOUNDGRABBER_H
