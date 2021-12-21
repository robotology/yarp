/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <Demo.h>

//! [DemoServer.cpp]
class DemoServer :
        public Demo
{
    int32_t mAnswer {42};

public:
    int32_t get_answer() override { return mAnswer; }
    bool set_answer(int32_t rightAnswer) override { mAnswer = rightAnswer; return true; }
};

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network yarp;

    DemoServer demoServer;
    yarp::os::RpcServer port;
    demoServer.yarp().attachAsServer(port);
    if (!port.open("/demo/server")) {
        return 1;
    }

    while (true) {
        yDebug() << "Demo server running happily";
        yarp::os::Time::delay(120);
    }
    port.close();
    return 0;
}
//! [DemoServer.cpp]
