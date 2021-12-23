/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>

#include <Demo.h>

#if 0
//! [DemoClient.cpp Old]
int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network yarp;
    yarp::os::RpcClient client_port;

    client_port.open("/demo/client");
    if (!yarp.connect("/demo/client", "/demo/server", "tcp")) {
        yError() << "Could not connect to server /demo/server";
        return -1;
    }

    Demo demo;
    demo.yarp().attachAsClient(client_port);

    int32_t answer = demo.get_answer();
    demo.set_answer(answer + 4);
    answer = demo.get_answer();

    return 0;
}
//! [DemoClient.cpp Old]
#endif

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network yarp;
    yarp::os::RpcClient client_port;

    client_port.open("/demo/client");
//! [DemoClient.cpp New]
    if (!yarp.connect("/demo/client", "/demo/server", "tcp++send.portmonitor+type.dll+file.demo_monitor")) {
//! [DemoClient.cpp New]

# if 0
//! [DemoClient.cpp New2]
    if (!yarp.connect("/demo/client", "/demo/server", "tcp++send.portmonitor+type.dll+file.demo_monitor+monitor./demo/monitor")) {
//! [DemoClient.cpp New2]
#endif

        yError() << "Could not connect to server /demo/server";
        return -1;
    }

    Demo demo;
    demo.yarp().attachAsClient(client_port);

    int32_t answer = demo.get_answer();
    demo.set_answer(answer + 4);
    answer = demo.get_answer();

    return 0;
}
