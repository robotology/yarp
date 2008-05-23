// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// Port latency, basic test.
// Send a sequence of message to a port. Test the 
// time it takes for the message to be received.
// Compute average.

class Reader : public TypedReaderCallback<Bottle> {
public:
    double delay;
    int count;
public:
    Reader()
    {
        delay=0;
        count=0;
    }

    void onRead(Bottle& datum) {
        double t=datum.get(0).asDouble();
        double now=Time::now();
        delay+=(now-t)*1000;
        count++;
    }
};

int main() {
    Network yarp;
    //    yarp.setLocalMode(true);
    Reader reader;

    BufferedPort<Bottle> port, port2;
    port2.useCallback(reader);
    port.open("/profiling/port");
    port2.open("/profiling/port2");
    yarp.connect("/profiling/port","/profiling/port2");

    for (int k=0; k<100; k++) {
        printf("Sending frame %d\n", k);
        Bottle& b = port.prepare();
        b.clear();
        for (int i=0; i<1000; i++) {
            double time=Time::now();
            b.clear();
            b.addDouble(time);
        }
        port.write();
        //give the CPU some time
        Time::delay(0.05);
    }
    port.close();
    port2.close();

    fprintf(stderr, "Average latency %.3lf\n", reader.delay/reader.count);

    return 0;
}

