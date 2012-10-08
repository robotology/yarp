/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Debug.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <yarp/os/Os.h>


#define RED    "\033[01;31m"
#define GREEN  "\033[01;32m"
#define YELLOW "\033[01;33m"
#define BLUE   "\033[01;34m"
#define CLEAR  "\033[00m"


std::ofstream RobotInterface::Debug::fout;
std::ofstream RobotInterface::Debug::ferr;
std::ofstream RobotInterface::Debug::ftrc;


void RobotInterface::Debug::print_output(MsgType t, const std::ostringstream &s)
{
    switch (t) {
    case TraceType:
        if (ftrc.is_open()) {
            ftrc << s.str() << std::endl;
        } else {
            std::cout << GREEN   << "TRACE"   << CLEAR << ": " << s.str() << std::endl;
        }
        break;
    case DebugType:
        if (fout.is_open()) {
            fout << "DEBUG: " << s.str() << std::endl;
        } else {
            std::cout << BLUE   << "DEBUG"   << CLEAR << ": " << s.str() << std::endl;
        }
        break;
    case WarningType:
        if (ferr.is_open()) {
            ferr << "WARNING: " << s.str() << std::endl;
        }
        std::cerr << YELLOW << "WARNING" << CLEAR << ": " << s.str() << std::endl;
        break;
    case ErrorType:
        if (ferr.is_open()) {
            ferr << "ERROR: " << s.str() << std::endl;
        }
        std::cerr << RED    << "ERROR"   << CLEAR << ": " << s.str() << std::endl;
        break;
    case FatalType:
        if (ferr.is_open()) {
            ferr << "FATAL: " << s.str() << std::endl;
        }
        std::cerr << RED    << "FATAL"   << CLEAR << ": " << s.str() << std::endl;
        yarp::os::exit(-1);
        break;
    }
}

void RobotInterface::Debug::setTraceFile(const std::string& filename)
{
    if(ftrc.is_open()) {
        ftrc.close();
    }
    ftrc.open(filename.c_str());
}


void RobotInterface::Debug::setOutputFile(const std::string& filename)
{
    if(fout.is_open()) {
        fout.close();
    }
    fout.open(filename.c_str());
}

void RobotInterface::Debug::setErrorFile(const std::string& filename)
{
    if(ferr.is_open()) {
        ferr.close();
    }
    ferr.open(filename.c_str());
}
