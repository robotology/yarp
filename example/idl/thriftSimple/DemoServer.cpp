/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <Demo.h>
#include <iostream>

// ************************************* //
// We implement the Demo interface
// ************************************* //
class DemoServer :
        public Demo
{
    // Class members declaration
    int32_t answer;
    bool isRunning;

public:
    DemoServer();

    //function declarations, copied from Demo.h
    int32_t get_answer() override;
    bool set_answer(int32_t rightAnswer) override;
    int32_t add_one(const int32_t x) override;
    bool start() override;
    bool stop() override;
    bool is_running() override;
};

//implementation of each function:
DemoServer::DemoServer()
{
    // members initialization
    answer = 42;
    isRunning = false;
    std::cout << "I know the answer!\n";
}

int32_t DemoServer::get_answer()
{
    std::cout << "The answer is " << answer << '\n';
    return answer;
}

bool DemoServer::set_answer(int32_t rightAnswer)
{
    std::cout << "OMG are you serious? The answer is " << rightAnswer << "?!?" << '\n';
    answer = rightAnswer;
    return true;
}

int32_t DemoServer::add_one(const int32_t x)
{
    std::cout << "I'm adding one to " << x << ". That's easy :) " << '\n';
    return x + 1;
}

bool DemoServer::start()
{
    std::cout << "Starting!" << '\n';
    isRunning = true;
    return true;
}

bool DemoServer::stop()
{
    std::cout << "Stopping!" << '\n';
    isRunning = false;
    return true;
}

bool DemoServer::is_running()
{
    std::cout << "Indeed I am " << (isRunning ? "" : "not ") << "running" << '\n';
    return isRunning;
}


// Instantiate the server and attach it to a YARP port
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
