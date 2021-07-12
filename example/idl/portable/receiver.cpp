/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include <SharedData.h>
#include <iostream>

using namespace std;

int main()
{
    yarp::os::Network network;

    cout << "Starting receiver\n";

    yarp::os::Port port;
    if (!port.open("/receiver")) {
        cerr << "Error opening port, check your yarp network\n";
        return -1;
    }

    while (true) {
        SharedData d;
        port.read(d);

        //access d
    }

    return 0;
}
