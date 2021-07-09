/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006 Julio Gomes
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ServerSoundGrabber.h"

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/GenericVocabs.h>


namespace {
YARP_LOG_COMPONENT(SERVERSOUNDGRABBER, "yarp.device.ServerSoundGrabber")
}

ServerSoundGrabber::ServerSoundGrabber()
#ifdef DEBUG_TIME_SPENT
        : last_time(yarp::os::Time::now())
#endif
{
}

ServerSoundGrabber::~ServerSoundGrabber()
{
    if (mic != nullptr) {
        close();
    }
}

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
bool ServerSoundGrabber::open(yarp::os::Searchable& config)
{
    yarp::os::Value* name;
    if (config.check("subdevice", name)) {
        yCDebug(SERVERSOUNDGRABBER, "Subdevice %s", name->toString().c_str());
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.fromString(config.toString());
            p.put("device", name->toString());
            poly.open(p);
        } else {
            poly.open(*name);
        }
    } else {
        yCError(SERVERSOUNDGRABBER, "\"--subdevice <name>\" not set for server_soundgrabber");
        return false;
    }

    if (poly.isValid()) {
        poly.view(mic);
    } else {
        yCError(SERVERSOUNDGRABBER, "cannot make <%s>", name->toString().c_str());
        return false;
    }

    if (mic == nullptr) {
        yCError(SERVERSOUNDGRABBER, "failed to open interface");
        return false;
    }

    //set the streaming port
    std::string portname = "/sound_grabber";
    if (config.check("name", name)) {
        portname = name->asString();
    }
    if (streamingPort.open(portname) == false) {
        yCError(SERVERSOUNDGRABBER) << "Unable to open port" << portname;
        return false;
    }

    //set the RPC port
    if (rpcPort.open(portname + "/rpc") == false) {
        yCError(SERVERSOUNDGRABBER) << "Unable to open port" << portname + "/rpc";
        return false;
    }
    rpcPort.setReader(*this);

    //wait a little and then start
    yarp::os::SystemClock::delaySystem(1);

    //mic->startRecording();
    this->start();

    return true;
}

bool ServerSoundGrabber::close()
{
    if (mic != nullptr) {
        stop();
        mic = nullptr;

        streamingPort.interrupt();
        streamingPort.close();
        rpcPort.interrupt();
        rpcPort.close();

        return true;
    }
    return false;
}

void ServerSoundGrabber::run()
{
    while (!isStopping()) {
#ifdef DEBUG_TIME_SPENT
        double current_time = yarp::os::Time::now();
        yCDebug(SERVERSOUNDGRABBER) << current_time - last_time;
        last_time = current_time;
#endif

        if (mic != nullptr) {
            yarp::sig::Sound snd;
#ifdef PRINT_DEBUG_MESSAGES
            {
                audio_buffer_size buf_max;
                audio_buffer_size buf_cur;
                mic->getRecordingAudioBufferMaxSize(buf_max);
                mic->getRecordingAudioBufferCurrentSize(buf_cur);
                yCDebug(SERVERSOUNDGRABBER) << "BEFORE Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
            }
#endif
            mic->getSound(snd, 44100, 44100, 0.0);
#ifdef PRINT_DEBUG_MESSAGES
            {
                audio_buffer_size buf_max;
                audio_buffer_size buf_cur;
                mic->getRecordingAudioBufferMaxSize(buf_max);
                mic->getRecordingAudioBufferCurrentSize(buf_cur);
                yCDebug(SERVERSOUNDGRABBER) << "AFTER Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
            }
#endif
#ifdef PRINT_DEBUG_MESSAGES
            yCDebug(SERVERSOUNDGRABBER) << "Sound size:" << snd.getSamples() * snd.getChannels() * snd.getBytesPerSample() << " bytes";
            yCDebug(SERVERSOUNDGRABBER);
#endif
            stamp.update();
            streamingPort.setEnvelope(stamp);
            streamingPort.write(snd);
        }
    }
    yCInfo(SERVERSOUNDGRABBER, "Sound grabber stopping");
}

bool ServerSoundGrabber::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok)
        return false;
    reply.clear();

    if (command.get(0).asString() == "start") {
        mic->startRecording();
        reply.addVocab32(VOCAB_OK);
    } else if (command.get(0).asString() == "stop") {
        mic->stopRecording();
        reply.addVocab32(VOCAB_OK);
    } else if (command.get(0).asString() == "help") {
        reply.addVocab32("many");
        reply.addString("start");
        reply.addString("stop");
    } else {
        yCError(SERVERSOUNDGRABBER) << "Invalid command";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr) {
        reply.write(*returnToSender);
    }
    return true;
}
