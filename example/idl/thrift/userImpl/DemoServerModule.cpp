/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <yarp/test/Demo.h>

#include <iostream>


class DemoServerModule :
        public yarp::test::Demo,
        public yarp::os::RFModule
{
public:
    // Thrift Interface Implementation
    int32_t get_answer() override
    {
        std::cout << "Server:get_answer called\n";
        return yarp::test::ANSWER;
    }

    int32_t add_one(const int32_t x = 0) override
    {
        std::cout << "Server::add_one called with " << x << '\n';
        return x + 1;
    }

    int32_t double_down(const int32_t x) override
    {
        std::cout << "Server::double_down called with " << x << '\n';
        return x * 2;
    }

    yarp::test::PointD add_point(const yarp::test::PointD& x, const yarp::test::PointD& y) override
    {
        std::cout << "Server::add_point called\n";
        yarp::test::PointD z;
        z.x = x.x + y.x;
        z.y = x.y + y.y;
        z.z = x.z + y.z;
        return z;
    }

    // RFModule implementation
    yarp::os::Port cmdPort;

    bool attach(yarp::os::Port& source) override
    {
        return this->yarp().attachAsServer(source);
    }

    bool configure(yarp::os::ResourceFinder& rf) override
    {
        std::string moduleName = rf.check("name",
                                          yarp::os::Value("demoServerModule"),
                                          "module name (string)")
                                     .asString()
                                     .c_str();
        setName(moduleName.c_str());

        std::string slash = "/";

        attach(cmdPort);

        std::string cmdPortName = "/";
        cmdPortName += getName();
        cmdPortName += "/cmd";
        if (!cmdPort.open(cmdPortName.c_str())) {
            std::cout << getName() << ": Unable to open port " << cmdPortName << '\n';
            return false;
        }
        return true;
    }

    bool updateModule() override
    {
        return true;
    }

    bool close() override
    {
        cmdPort.close();
        return true;
    }
};

int main(int argc, char* argv[])
{
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        std::cout << "Error: yarp server does not seem available" << '\n';
        return -1;
    }

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    DemoServerModule demoMod;

    if (!demoMod.configure(rf)) {
        return -1;
    }

    return demoMod.runModule();
}
