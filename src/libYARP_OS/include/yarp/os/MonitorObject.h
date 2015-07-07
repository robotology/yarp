// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MONITOROBJECT_
#define _YARP2_MONITOROBJECT_

#include <yarp/os/Property.h>
#include <yarp/os/Things.h>

namespace yarp {
    namespace os {
        class MonitorObject;
    }
}

class YARP_OS_API yarp::os::MonitorObject
{
public:
    virtual ~MonitorObject() {}

    /**
     * This will be called when the dll is properly loaded by the portmonitor carrier
     *
     * @param options A set of useful information of the current connection which
     *        the monitor object is attached.
     * @return Returning false will stop the portmonitor object to procced        
     */
    virtual bool create(const yarp::os::Property& options) { return true; }

    /**
     * This will be called when the portmonitor object destroyes
     */
    virtual void destroy(void) { }

    /**
     * This will be called when the portmonitor carrier parameters are set via Yarp admin port
     *
     * @param params The Property 
     * @return Returns true if paramters are correctly set
     */
    virtual bool setparam(const yarp::os::Property& params) { return false; }

    /**
     * This will be called when the portmonitor carrier parameters are requested via Yarp admin port
     *
     * @param params The Property 
     * @return Returns true of any parameter is available
     */
    virtual bool getparam(yarp::os::Property& params) { return false; }
   
    /**
     * This will be called when one of the peer connections to the same import port receives data
     * \Note this is available only if the portmonitor object attached to the input port
     */
    virtual void trig(void) { }

    /**
     * This will be called when the data reach the portmonitor object
     *
     * @param thing An instance of yarp::os::Thing object which can be used
     *        to typecast the data to the correct type. 
     * @return returning false will avoid delivering data to an input 
     *         port or transmitting through the output port
     */
    virtual bool accept(yarp::os::Things& thing) { return true; }


    /**
     * After data get accpeted in the accept() callback, an instance of that 
     * is given to the update fucntion where the data can be accessed and modified
     *
     * @param thing An instance of yarp::os::Thing object which can be used
     *        to typecast the data to the correct type. 
     * @ return An instance of modified data in form of Thing
     */
    virtual yarp::os::Things& update(yarp::os::Things& thing) { return thing; }


    /**
     * The updateReply makes it possible to modify a reply from a port
     * when the portmonitor object is attached to a two-ways connection (e.g., RPC).
     *
     * @param thing An instance of yarp::os::Thing object which can be used
     *        to typecast the data to the correct type.
     * @ return An instance of modified data in form of Thing
     */
    virtual yarp::os::Things& updateReply(yarp::os::Things& thing) { return thing; }
};

#endif //_YARP2_MONITOROBJECT_

