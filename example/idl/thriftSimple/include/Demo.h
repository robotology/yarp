/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_SERVICE_DEMO_H
#define YARP_THRIFT_GENERATOR_SERVICE_DEMO_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class Demo :
        public yarp::os::Wire
{
public:
    // Constructor
    Demo();

    virtual std::int32_t get_answer();

    virtual bool set_answer(const std::int32_t rightAnswer);

    virtual std::int32_t add_one(const std::int32_t x);

    virtual bool start();

    virtual bool stop();

    virtual bool is_running();

    // help method
    virtual std::vector<std::string> help(const std::string& functionName = "--all");

    // read from ConnectionReader
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_THRIFT_GENERATOR_SERVICE_DEMO_H
