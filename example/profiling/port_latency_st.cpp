/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// Modified version of port_latency.cpp, this time single-threaded.
// Paul Fitzpatrick May 2008
// Based on code by Lorenzo Natale May 2008

int server(double server_wait)
{
    Port port;
    port.open("/profiling/port");

    int k=0;
    while(true) {
        printf("Sending frame %d\n", k);
        Bottle b;
        b.clear();
        double time=Time::now();
        b.addDouble(time);
        port.write(b);
        //give the CPU some time
        Time::delay(server_wait);
        k++;
    }
    port.close();
    return 0;
}

int client(int nframes)
{
    double delay = 0;
    int count = 0;
    int wait = 10;

    Port port;
    port.open("/profiling/port2");
    while(!Network::connect("/profiling/port","/profiling/port2"))
        {
            fprintf(stderr, "Waiting for connection\n");
            Time::delay(0.5);
        }

    while(count<nframes)
        {
            Bottle datum;
            port.read(datum);
            double t=datum.get(0).asDouble();
            double now=Time::now();
            delay+=(now-t)*1000;
            count++;
            // remove this to spare cpu time
            fprintf(stderr, "%lf\n", (now-t)*1000);
        }

    port.close();

    fprintf(stderr, "Received: %d average latency %.3lf[ms]\n", 
            count, delay/count);
    return 0;
}

int main(int argc, char **argv) {
    Network yarp;
    Property p;
    p.fromCommand(argc, argv);

    if (p.check("server"))
        return server(p.find("period").asDouble());
    else if (p.check("client"))
        return client(p.find("nframes").asInt());
}
