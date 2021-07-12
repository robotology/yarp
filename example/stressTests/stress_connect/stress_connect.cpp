/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

using namespace yarp::os;

#define PORT_NAME1 "/stress/connect/1"
#define PORT_NAME2 "/stress/connect/2"
#define NUM_STRESSORS 10

class Stressor : public Thread {
public:
    virtual void run() {
        for (int i=0; i<100000; i++) {
            Network::connect(PORT_NAME1,PORT_NAME2);
        }
    }
};

int main(int argc, char *argv[]) {
    Network yarp;

    Port p1;
    Port p2;
    p1.open(PORT_NAME1);
    p2.open(PORT_NAME2);

    Stressor ss[NUM_STRESSORS];
    for (int i=0; i<NUM_STRESSORS; i++) {
        ss[i].start();
    }
    for (int i=0; i<NUM_STRESSORS; i++) {
        ss[i].stop();
    }

    return 0;
}
