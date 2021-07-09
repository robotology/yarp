/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <yarp/test/Demo.h>

#include <iostream>


class DemoServer : public yarp::test::Demo
{
public:
    int32_t get_answer() override
    {
        std::cout << "Server:get_answer called" << std::endl;
        return yarp::test::ANSWER;
    }

    int32_t add_one(const int32_t x = 0) override
    {
        std::cout << "Server::add_one called with " << x << std::endl;
        return x + 1;
    }

    int32_t double_down(const int32_t x) override
    {
        std::cout << "Server::double_down called with " << x << std::endl;
        return x * 2;
    }

    yarp::test::PointD add_point(const yarp::test::PointD& x, const yarp::test::PointD& y) override
    {
        std::cout << "Server::add_point called" << std::endl;
        yarp::test::PointD z;
        z.x = x.x + y.x;
        z.y = x.y + y.y;
        z.z = x.z + y.z;
        return z;
    }
};

int main(int argc, char* argv[])
{
    yarp::os::Network yarp;

    DemoServer demoServer;
    yarp::os::Port port;
    demoServer.yarp().attachAsServer(port);
    if (!port.open("/demoServer")) {
        return 1;
    }

    while (true) {
        printf("Server running happily\n");
        yarp::os::Time::delay(10);
    }
    port.close();
    return 0;
}
