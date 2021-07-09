/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <yarp/sig/Image.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

using namespace yarp::os;
using namespace yarp::sig;

static bool terminated=false;

BufferedPort<Bottle> *iPort=0;

static void handler (int) {
    static int ct = 0;
    ct++;

    fprintf(stderr, "[try %d of %d] Asking to shut down smoothly\n",ct, 3);
    terminated = true;

    if (iPort!=0)
        iPort->interrupt();

    if (ct>2)
    {
        fprintf(stderr, "[try %d of %d] OK asking to abort...\n", ct, 3);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    Network yarp;

    signal(SIGINT, handler);
    signal(SIGTERM, handler);


    if (argc==1) {
        printf("This program checks the framerate of an output port\n");
        printf("Call as:\n");
        printf("   framerate --remote /port_name --local /local_name --prot protocol\n");
        printf("protocol can be for example tcp,udp,mcast\n");
        exit(0);
    }

    BufferedPort<Bottle> port;
    iPort=&port; //give pointer to port to signal handler (used to interrupt port at shutdown

    // get options
    Property opt;
    opt.fromCommand(argc,argv);

    // name port
    Value *val;
    Value *prot;
    std::string local = "/get_image";
    if (opt.check("local",val)) {
        local = val->asString().c_str();
    }
    port.open(local.c_str());

    // connect port
    if (opt.check("remote", val))
    {
        if (opt.check("prot", prot))
            Network::connect(val->asString(), local.c_str(), prot->asString().c_str());
        else
            Network::connect(val->asString(), local.c_str());
    }
    // read
    double first = Time::now();
    double prev = 0;
    int ct = 0;
    bool spoke = false;
    while (!terminated) {
        Bottle *bot = port.read(true);
        double now = Time::now()-first;
        ct++;
        if (now-prev>=2) {
            double period = (now-prev)/ct;
            printf("Period is %g ms per message, freq is %g (%d mgs in %g secs)\n",
                   period*1000, 1/period, ct, now-prev);
            fflush(stdout);
            ct = 0;
            prev = now;
            spoke = false;
        }
        if (bot!=NULL) {
            if (!spoke) {
                printf("Got something with %d top-level elements\n", bot->size());
                fflush(stdout);
                spoke = true;
            }
        }
    }
    iPort=0; //just in case

    return 0;
}
