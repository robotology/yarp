// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

class Reader : public TypedReaderCallback<Bottle> {
public:
    void onRead(Bottle& datum) {
    }
};

int main() {
    Network yarp;
    yarp.setLocalMode(true);
    Reader reader;

    printf("We really don't recommend you use Bottles for large data structures\n");
    printf("But if you did, what parts gets slow first?\n");
    printf("This is a test program for profiling purposes.\n");
    printf("It doesn't do anything interest by itself.\n");

    BufferedPort<Bottle> port, port2;
    port2.useCallback(reader);
    port.open("/port");
    port2.open("/port2");
    yarp.connect("/port","/port2");

    for (int k=0; k<20; k++) {
        printf("Round %d\n", k);
        Bottle& b = port.prepare();
        b.clear();
        for (int i=0; i<1000; i++) {
            b.add(Value(1));
        }
        port.write();
    }
    port.close();
    port2.close();

    return 0;
}

