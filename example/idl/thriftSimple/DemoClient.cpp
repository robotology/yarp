/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <Demo.h>
#include <iostream>

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

    // Let's chat with the server!

    std::cout << "Hey are you up and running?" << std::endl;
    while (!demo.is_running()) {
        std::cout << "No? Well... start!" << std::endl;
        demo.start();
    }

    std::cout << "Wonderful! I have a question for you... so, what's the answer??" << std::endl;
    int32_t answer = demo.get_answer();
    std::cout << "What?? " << answer << "?? Are you kidding??";
    answer = demo.add_one(answer);
    std::cout << " It's definitely " << answer << "!!" << std::endl;
    demo.set_answer(answer);
    std::cout << "Got it? So, repeat after me: the answer is ... " << demo.get_answer() << "! Great!" << std::endl;

    std::cout << "Ok you can relax now, I'll leave you alone" << std::endl;
    demo.stop();

    std::cout << "Bye" << std::endl;

    return 0;
}
