/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Debug.h"

#include <iostream>
#include <sstream>

#include <yarp/os/Os.h>


#define RED    "\033[01;31m"
#define GREEN  "\033[01;32m"
#define YELLOW "\033[01;33m"
#define BLUE   "\033[01;34m"
#define CLEAR  "\033[00m"

void RobotInterface::Debug::print_output(MsgType t, const std::ostringstream &s)
{
    switch (t) {
        case DebugType:
            std::cout << BLUE   << "DEBUG"   << CLEAR << ": " << s.str() << std::endl;
            break;
        case WarningType:
            std::cout << YELLOW << "WARNING" << CLEAR << ": " << s.str() << std::endl;
            break;
        case ErrorType:
            std::cout << RED    << "ERROR"   << CLEAR << ": " << s.str() << std::endl;
            break;
        case FatalType:
            std::cout << RED    << "FATAL"   << CLEAR << ": " << s.str() << std::endl;
            yarp::os::exit(-1);
            break;
    }
}
