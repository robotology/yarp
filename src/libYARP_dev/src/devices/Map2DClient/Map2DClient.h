/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_MAP2DCLIENT_H
#define YARP_DEV_MAP2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/RecursiveMutex.h>

namespace yarp {
    namespace dev {
        class Map2DClient;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Map2D_type
{
    private:



};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
* @ingroup dev_impl_wrapper
*
* The client side of any IBattery capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::Map2DClient : public DeviceDriver,
                               public IMap2D
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Port                m_rpcPort;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_remote_name;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

     /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    virtual bool     clear      ();
    virtual bool     remove_map (std::string map_name);
    virtual bool     store_map  (const yarp::dev::MapGrid2D& map);
    virtual bool     get_map    (std::string map_name, yarp::dev::MapGrid2D& map);
    virtual bool     get_map_names(std::vector<std::string>& map_names);
};

#endif // YARP_DEV_MAP2DCLIENT_H
