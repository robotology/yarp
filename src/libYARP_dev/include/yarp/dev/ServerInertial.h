// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Alexis Maldonado, Radu Bogdan Rusu
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP2_SERVERINERTIAL_
#define _YARP2_SERVERINERTIAL_

#include <stdio.h>

//#include <yarp/XSensMTx.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/PreciselyTimed.h>


namespace yarp
{
    namespace dev
    {
        class ServerInertial;
    }
}


/**
 * @ingroup dev_impl_wrapper
 *
 * Export an inertial sensor.
 * The network interface is a single Port.
 * We will stream bottles with 12 floats:
 * 0  1   2  = Euler orientation data (Kalman filter processed)
 * 3  4   5  = Calibrated 3-axis (X, Y, Z) acceleration data
 * 6  7   8  = Calibrated 3-axis (X, Y, Z) gyroscope data
 * 9 10 11   = Calibrated 3-axis (X, Y, Z) magnetometer data
 *
 * @author Alexis Maldonado, Radu Bogdan Rusu
 */
class yarp::dev::ServerInertial : public DeviceDriver,
            private yarp::os::Thread,
            public yarp::os::PortReader,
            public yarp::dev::IGenericSensor
{
private:
    bool spoke;
    PolyDriver poly;
    IGenericSensor *IMU; //The inertial device
    IPreciselyTimed *iTimed;
    double period;
    yarp::os::Port p;
    yarp::os::PortWriterBuffer<yarp::os::Bottle> writer;
    int prev_timestamp_counter;
    int curr_timestamp_counter;
    int trap;

public:
    /**
     * Constructor.
     */
    ServerInertial()
    {
        IMU = NULL;
        spoke = false;
        iTimed=0;
        period = 0.005;
        prev_timestamp_counter=0;
        curr_timestamp_counter=0;
        trap = 0;
    }

    virtual ~ServerInertial()
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
    virtual bool open(yarp::os::Searchable& config)
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

    virtual bool close()
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


    virtual bool getInertial(yarp::os::Bottle &bot)
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

    virtual void run()
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

    virtual bool read(ConnectionReader& connection)
    {
        yarp::os::Bottle cmd, response;
        cmd.read(connection);
        // printf("command received: %s\n", cmd.toString().c_str());

        // We receive a command but don't do anything with it.
        return true;
    }

    virtual bool read(yarp::sig::Vector &out)
    {
        if (IMU == NULL) { return false; }
        return IMU->read (out);
    }

    virtual bool getChannels(int *nc)
    {
        if (IMU == NULL) { return false; }
        return IMU->getChannels (nc);
    }

    virtual bool calibrate(int ch, double v)
    {
        if (IMU==NULL) {return false;}
        return IMU->calibrate(ch, v);
    }
};

#endif

