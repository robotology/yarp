// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Alexis Maldonado, Radu Bogdan Rusu
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/PreciselyTimed.h>

#include <ServerInertial.h>

using namespace yarp::dev;
using namespace yarp::os;

// needed for the driver factory.
yarp::dev::DriverCreator *createServerInertial()
{
    return new yarp::dev::DriverCreatorOf<yarp::dev::ServerInertial>
            ("inertial", "inertial", "yarp::dev::ServerInertial");
}

/**
 * Constructor.
 */
yarp::dev::ServerInertial::ServerInertial()
{
    IMU = NULL;
    spoke = false;
    iTimed=0;
    period = 0.005;
    prev_timestamp_counter=0;
    curr_timestamp_counter=0;
    trap = 0;
}

yarp::dev::ServerInertial::~ServerInertial()
{
    if (IMU != NULL) close();
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
bool yarp::dev::ServerInertial::open(yarp::os::Searchable& config)
{
    p.setReader(*this);

    period = config.check("period",yarp::os::Value(0.005),"maximum period").asDouble();
    //Look for the device name (serial Port). Usually /dev/ttyUSB0
    yarp::os::Value *name;
    if (config.check("subdevice",name))
    {
        printf("Subdevice %s\n", name->toString().c_str());
        if (name->isString())
        {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.setMonitor(config.getMonitor(),
                         "subdevice"); // pass on any monitoring
            p.fromString(config.toString());
            p.put("device",name->toString());
            poly.open(p);
        }
        else
            poly.open(*name);
        if (!poly.isValid())
            printf("cannot make <%s>\n", name->toString().c_str());
    }
    else
    {
        printf("\"--subdevice <name>\" not set for server_inertial\n");
        return false;
    }
    if (poly.isValid())
        poly.view(IMU);

    if (poly.isValid())
        poly.view(iTimed);

    if (IMU!=NULL)
        writer.attach(p);

    //Look for the portname to register (--name option)
    if (config.check("name",name))
        p.open(name->asString());
    else
        p.open("/inertial");

    //Look for the portname to register (--name option)
    //p.open(config.check("name", Value("/inertial")).asString());

    if (IMU!=NULL)
    {
        start();
        return true;
    }
    else
        return false;
}

bool yarp::dev::ServerInertial::close()
{
    printf("Closing Server Inertial...\n");
    if (IMU != NULL)
    {
        stop();
        IMU = NULL;
        return true;
    }
    return false;
}


bool yarp::dev::ServerInertial::getInertial(yarp::os::Bottle &bot)
{
    if (IMU==NULL)
    {
        return false;
    }
    else
    {
        int nchannels;
        IMU->getChannels (&nchannels);

        yarp::sig::Vector indata(nchannels);
        bool worked(false);

        worked=IMU->read(indata);
        if (worked)
        {
            bot.clear();

            // Euler+accel+gyro+magn orientation values
            for (int i = 0; i < nchannels; i++)
                bot.addDouble (indata[i]);
        }
        else
        {
            bot.clear(); //dummy info.
        }

        return(worked);
    }
}

void yarp::dev::ServerInertial::run()
{
    double before, now;
    printf("Server Inertial starting\n");
    while (!isStopping())
    {
        before = yarp::os::Time::now();
        if (IMU!=NULL)
        {
            yarp::os::Bottle& bot = writer.get();
            bool res = getInertial(bot);
            if (res)
            {
                static yarp::os::Stamp ts;
                if (iTimed)
                    ts=iTimed->getLastInputStamp();
                else
                    ts.update();


                curr_timestamp_counter = ts.getCount();

                if (curr_timestamp_counter!=prev_timestamp_counter)
                {
                    if (!spoke)
                    {
                        printf("Writing an Inertial measurement.\n");
                        spoke = true;
                    }
                    p.setEnvelope(ts);
                    writer.write();
                }
                else
                {
                    trap++;
                }

                prev_timestamp_counter = curr_timestamp_counter;
            }
        }

        /// wait 5 ms.
        now = yarp::os::Time::now();
        if ((now - before) < period) {
            double k = period-(now-before);
            yarp::os::Time::delay(k);
        }
    }
    printf("Server Intertial closed\n");
}

bool yarp::dev::ServerInertial::read(ConnectionReader& connection)
{
    yarp::os::Bottle cmd, response;
    cmd.read(connection);
    // printf("command received: %s\n", cmd.toString().c_str());

    // We receive a command but don't do anything with it.
    return true;
}

bool yarp::dev::ServerInertial::read(yarp::sig::Vector &out)
{
    if (IMU == NULL) { return false; }
    return IMU->read (out);
}

bool yarp::dev::ServerInertial::getChannels(int *nc)
{
    if (IMU == NULL) { return false; }
    return IMU->getChannels (nc);
}

bool yarp::dev::ServerInertial::calibrate(int ch, double v)
{
    if (IMU==NULL) {return false;}
    return IMU->calibrate(ch, v);
}
