// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Alex Bernardino & Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef _YARP2_SERVERSERIAL_
#define _YARP2_SERVERSERIAL_

#include <stdio.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/SerialInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


namespace yarp
{
	namespace dev
	{
		class ServerSerial;
        class ImplementCallbackHelper2;
	}
}

/**
 * Callback implementation after buffered input.
 */
class yarp::dev::ImplementCallbackHelper2 : public TypedReaderCallback<Bottle> {
protected:
    ISerialDevice *ser;
    
public:
    /**
     * Constructor.
     * @param x is the instance of the container class using the callback.
     */
    ImplementCallbackHelper2();
    ImplementCallbackHelper2(yarp::dev::ServerSerial *x);

    /**
     * Callback function.
     * @param b is the Bottle being received.
     */
    virtual void onRead(Bottle& b);
};


/**
 * @ingroup dev_impl_wrapper
 *
 * Export a serial sensor.
 * The network interface is composed by two ports.
 * The input port accepts bottles and sends their contents as 
 * text to the serial port.
 * The output port streams out whatever information it gets in the
 * serial port as text bottles.
 *
 * @author Alex Bernardino
 */
class yarp::dev::ServerSerial : public DeviceDriver, public ISerialDevice, 
			                    private Thread
{
private:
    bool verb;
    PolyDriver poly;
    Port toDevice;
    Port fromDevice;

    PortWriterBuffer <Bottle> reply_buffer;
    PortReaderBuffer <Bottle> command_buffer;

    ISerialDevice *serial;
    yarp::dev::ImplementCallbackHelper2 callback_impl;


    bool closeMain() {
        if (Thread::isRunning()) {
            Thread::stop();
        }
        //close the port connection here
        toDevice.close();
        fromDevice.close();
        poly.close();
        return true;
    }
   
public:
    /**
     * Constructor.
     */
    ServerSerial() : callback_impl(this)
    {
        serial = NULL;
        verb = false;
    }

    virtual ~ServerSerial() {
        closeMain();
    }

    virtual bool send(const Bottle& msg)
    {
        if(verb)
            ACE_OS::printf("String to send : %s\n", msg.toString().c_str());
        if(serial != NULL) {
            serial->send(msg);
            return true;
        }
        else
            return false;
    }
    
    virtual bool receive(Bottle& msg)
    {
        if(serial != NULL) {
            serial->receive(msg);
            return true;
        }
        else
            return false;
    }

    /**
    * Default open() method.
    * @return always false since initialization requires certain parameters.
    */

    virtual bool open()  {
        return false;
    }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    virtual bool close() {
        return closeMain();
    }

    /**
     * Open the device driver.
     * 
     * @param prop is a Searchable object that contains the parameters to use.
     * Allowed parameters are:
     * - verbose or v to print diagnostic information while running,
     * - name to specify the prefix of the port names
     * and all parameters requied by the wrapped device driver.
     * @return true iff the object could be configured.
     */
    virtual bool open(Searchable& prop)
    {
        verb = (prop.check("verbose","if present, give detailed output"));
		if (verb)
			ACE_OS::printf("running with verbose output\n");

        Value *name;
        if (prop.check("subdevice",name,"name of specific control device to wrap")) {
            ACE_OS::printf("Subdevice %s\n", name->toString().c_str());
            if (name->isString()) {
                // maybe user isn't doing nested configuration
                Property p;
                p.setMonitor(prop.getMonitor(),
                             "subdevice"); // pass on any monitoring
                p.fromString(prop.toString());
                p.put("device",name->toString());
                poly.open(p);
            } else {
                Bottle subdevice = prop.findGroup("subdevice").tail();
                poly.open(subdevice);
            }
            if (!poly.isValid()) {
                ACE_OS::printf("cannot make <%s>\n", name->toString().c_str());
            }
        } else {
            ACE_OS::printf("\"--subdevice <name>\" not set for server_serial\n");
            return false;
        }

        if (!poly.isValid()) {
            return false;
        }

        String rootName = 
            prop.check("name",Value("/serial"),
                       "prefix for port names").asString().c_str();
        
        command_buffer.attach(toDevice);
        reply_buffer.attach(fromDevice);

        command_buffer.useCallback(callback_impl);
        
        toDevice.open((rootName+"/in").c_str());
        fromDevice.open((rootName+"/out").c_str());



        if (poly.isValid())
            poly.view(serial);

        if(serial != NULL) {
            start();
			return true;
        }

        ACE_OS::printf("subdevice <%s> doesn't look like a serial port (no appropriate interfaces were acquired)\n",
                       name->toString().c_str());
        
        return false;
    }

   /**
     * The thread main loop deals with writing on ports here.
     */
    virtual void run() {
        ACE_OS::printf("Server Serial starting\n");
        double before, now;
        while (!isStopping()) {
            before = Time::now();
            Bottle& b = reply_buffer.get();
            b.clear();
            receive( b ); //blocks
            if(b.size() > 0)
                reply_buffer.write();
            now = Time::now();
            // give other threads the chance to run 
            yarp::os::Time::delay(0.010);
        }
        ACE_OS::printf("Server Serial stopping\n");
    }
};

// ImplementCallbackHelper class.


yarp::dev::ImplementCallbackHelper2::ImplementCallbackHelper2(yarp::dev::ServerSerial *x) {
    ser = dynamic_cast<yarp::dev::ISerialDevice *> (x);
    ACE_ASSERT (ser != 0);
}

void yarp::dev::ImplementCallbackHelper2::onRead(Bottle &b) {
    //ACE_OS::printf("Data received on the control channel of size: %d\n", v.body.size());
    //	int i;
   if (ser) {
        bool ok = ser->send(b);
        if (!ok)
            ACE_OS::printf("Problems while trying to send data\n");
    }
}

#endif

