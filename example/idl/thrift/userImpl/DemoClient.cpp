/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <yarp/test/Demo.h>

#include <iostream>

using namespace yarp::test;
using namespace yarp::os;

int main(int argc, char* argv[])
{
    Property config;
    config.fromCommand(argc, argv);

    Network yarp;
    Port client_port;

    std::string servername = config.find("server").asString().c_str();
    client_port.open("/demo/client");
    if (!yarp.connect("/demo/client", servername.c_str())) {
        std::cout << "Error! Could not connect to server " << servername << std::endl;
        return -1;
    }

    Demo demo;
    demo.yarp().attachAsClient(client_port);

    PointD point;
    point.x = 0;
    point.y = 0;
    point.z = 0;
    PointD offset;
    offset.x = 1;
    offset.y = 2;
    offset.z = 3;

    std::cout << "== get_answer ==" << std::endl;
    int answer = demo.get_answer();
    std::cout << answer << std::endl;

    std::cout << "== add_one ==" << std::endl;
    answer = demo.add_one(answer);
    std::cout << answer << std::endl;

    std::cout << "== double_down ==" << std::endl;
    answer = demo.double_down(answer);
    std::cout << answer << std::endl;

    std::cout << "== add_point ==" << std::endl;
    point = demo.add_point(point, offset);
    std::cout << ("== done! ==\n");

    return 0;
}
