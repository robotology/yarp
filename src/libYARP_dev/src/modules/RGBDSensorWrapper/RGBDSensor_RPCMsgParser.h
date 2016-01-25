// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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

#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/DeviceDriver.h>

namespace sensor{
    namespace depth{
        class RGBDSensor_RPCMgsParser;
    }
}



class sensor::depth::RGBDSensor_RPCMgsParser: public yarp::dev::DeviceResponder
{
private:
    int verbose;
public:

    RGBDSensor_RPCMgsParser();
    ~RGBDSensor_RPCMgsParser();

  /**
    * Initialization.
    * @param x is the pointer to the instance of the object that uses the RPCMessagesParser.
    * This is required to recover the pointers to the interfaces that implement the responses
    * to the commands.
    */
//     void init(yarp::dev::ControlBoardWrapper *x);

    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);

    // Helper functions
    // Verbose level goes from 0 to getMaxVerbose()
    bool setVerbose(int level);
    int getVerbose();
    int getMaxVerbose();
};
