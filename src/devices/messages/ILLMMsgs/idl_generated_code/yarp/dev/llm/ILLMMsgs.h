/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_SERVICE_ILLMMSGS_H
#define YARP_THRIFT_GENERATOR_SERVICE_ILLMMSGS_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/os/ApplicationNetworkProtocolVersion.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/llm/return_ask.h>
#include <yarp/dev/llm/return_getConversation.h>
#include <yarp/dev/llm/return_readPrompt.h>

namespace yarp::dev::llm {

class ILLMMsgs :
        public yarp::os::Wire
{
public:
    //ProtocolVersion
    virtual yarp::os::ApplicationNetworkProtocolVersion getLocalProtocolVersion();
    virtual yarp::os::ApplicationNetworkProtocolVersion getRemoteProtocolVersion();
    virtual bool checkProtocolVersion();

    // Constructor
    ILLMMsgs();

    //Service methods
    virtual yarp::dev::ReturnValue setPrompt(const std::string& prompt);

    virtual return_readPrompt readPrompt();

    virtual return_ask ask(const std::string& question);

    virtual return_getConversation getConversation();

    virtual yarp::dev::ReturnValue deleteConversation();

    virtual yarp::dev::ReturnValue refreshConversation();

    // help method
    virtual std::vector<std::string> help(const std::string& functionName = "--all");

    // read from ConnectionReader
    bool read(yarp::os::ConnectionReader& connection) override;
};

} // namespace yarp::dev::llm

#endif // YARP_THRIFT_GENERATOR_SERVICE_ILLMMSGS_H
