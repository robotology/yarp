// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/PreciselyTimed.h>
#include <string>

namespace yarp{
    namespace dev{
        class fakeIMU;
    }
}

#define DEFAULT_PERIOD 10   //ms

class yarp::dev::fakeIMU :  public DeviceDriver,
                            public IGenericSensor,
                            public yarp::os::RateThread,
                            public yarp::dev::IPreciselyTimed
{
public:
    fakeIMU();
    ~fakeIMU();

    // Device Driver interface
    virtual bool open(yarp::os::Searchable &config);
    virtual bool close();

    // IGenericSensor interface.
    virtual bool read(yarp::sig::Vector &out);
    virtual bool getChannels(int *nc);
    virtual bool calibrate(int ch, double v);

    // IPreciselyTimed interface
    virtual yarp::os::Stamp getLastInputStamp();

private:

    bool threadInit();
    void run();
    unsigned int nchannels;
    int dummy_value;
    yarp::os::Stamp lastStamp;
};
