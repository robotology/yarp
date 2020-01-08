/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006 Julio Gomes
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_SERVERSOUNDGRABBER_H
#define YARP_DEV_SERVERSOUNDGRABBER_H

#include <cstdio>


#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>

//#define NUM_SAMPLES 8192


/**
 * @ingroup dev_impl_wrapper
 *
 * Export a microphone device

 * streaming sound samples
 *
 * \author (adapted) Júlio Gomes, VisLab ISR/IST - 2006
 *
 */
class ServerSoundGrabber :
        public yarp::dev::DeprecatedDeviceDriver,
        private yarp::os::Thread,
        public yarp::os::PortReader
{
private:
    yarp::os::Stamp stamp;
    yarp::dev::PolyDriver poly;
    yarp::dev::IAudioGrabberSound *mic; //The microphone device
    yarp::os::Port rpcPort;
    yarp::os::Port streamingPort;
#ifdef DEBUG_TIME_SPENT
    double last_time;
#endif
public:
    /**
     * Constructor.
     */
    ServerSoundGrabber()
    {
        mic = nullptr;
#ifdef DEBUG_TIME_SPENT
        last_time = yarp::os::Time::now();
#endif
    }

    virtual ~ServerSoundGrabber()
    {
        if (mic != nullptr)
        {
            close();
        }
    }

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "test_grabber"). </TD></TR>
     * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override
    {
        yarp::os::Value *name;
        if (config.check("subdevice",name))
        {
            yDebug("Subdevice %s\n", name->toString().c_str());
            if (name->isString())
            {
                // maybe user isn't doing nested configuration
                yarp::os::Property p;
                p.fromString(config.toString());
                p.put("device",name->toString());
                poly.open(p);
            }
            else
            {
                poly.open(*name);
            }
        }
        else
        {
            yError("\"--subdevice <name>\" not set for server_soundgrabber");
            return false;
        }

        if (poly.isValid())
        {
            poly.view(mic);
        }
        else
        {
            yError("cannot make <%s>", name->toString().c_str());
            return false;
        }

        if (mic == nullptr)
        {
            yError("failed to open interface");
            return false;
        }

        //set the streaming port
        std::string portname = "/sound_grabber";
        if (config.check("name", name))
        {
            portname= name->asString();
        }
        if (streamingPort.open(portname) == false)
        {
            yError() << "Unable to open port" << portname;
            return false;
        }

        //set the RPC port
        if (rpcPort.open(portname + "/rpc") == false)
        {
            yError() << "Unable to open port" << portname + "/rpc";
            return false;
        }
        rpcPort.setReader(*this);

        //wait a little and then start
        yarp::os::SystemClock::delaySystem(1);

        //mic->startRecording();
        this->start();

        return true;
    }

    bool close() override
    {
        if (mic != nullptr)
        {
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

    void run() override
    {
        while(!isStopping())
        {
#ifdef DEBUG_TIME_SPENT
            double current_time = yarp::os::Time::now();
            yDebug() << current_time - last_time;
            last_time = current_time;
#endif

            if (mic!= nullptr)
            {
                yarp::sig::Sound snd;
#ifdef PRINT_DEBUG_MESSAGES
                {
                    audio_buffer_size buf_max;
                    audio_buffer_size buf_cur;
                    mic->getRecordingAudioBufferMaxSize(buf_max);
                    mic->getRecordingAudioBufferCurrentSize(buf_cur);
                    yDebug() << "BEFORE Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
                }
#endif
                mic->getSound(snd, 44100, 44100, 0.0);
#ifdef PRINT_DEBUG_MESSAGES
                {
                    audio_buffer_size buf_max;
                    audio_buffer_size buf_cur;
                    mic->getRecordingAudioBufferMaxSize(buf_max);
                    mic->getRecordingAudioBufferCurrentSize(buf_cur);
                    yDebug() << "AFTER Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
                }
#endif
#ifdef PRINT_DEBUG_MESSAGES
                yDebug() << "Sound size:" << snd.getSamples()*snd.getChannels()*snd.getBytesPerSample() << " bytes";
                yDebug();
#endif
                stamp.update();
                streamingPort.setEnvelope(stamp);
                streamingPort.write(snd);
            }
        }
        yInfo("Sound grabber stopping\n");
    }

    bool read(yarp::os::ConnectionReader& connection) override
    {
        yarp::os::Bottle command;
        yarp::os::Bottle reply;
        bool ok = command.read(connection);
        if (!ok) return false;
        reply.clear();

        if (command.get(0).asString()=="start")
        {
            mic->startRecording();
            reply.addVocab(VOCAB_OK);
        }
        else if (command.get(0).asString() == "stop")
        {
            mic->stopRecording();
            reply.addVocab(VOCAB_OK);
        }
        else if (command.get(0).asString() == "help")
        {
            reply.addVocab(yarp::os::Vocab::encode("many"));
            reply.addString("start");
            reply.addString("stop");
        }
        else
        {
            yError() << "Invalid command";
            reply.addVocab(VOCAB_ERR);
        }

        yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
        if (returnToSender != nullptr)
        {
            reply.write(*returnToSender);
        }
        return true;
    }

};

#endif // YARP_DEV_SERVERSOUNDGRABBER_H
